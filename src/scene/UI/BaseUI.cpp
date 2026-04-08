//
// Created by Curry on 2026-04-07.
//

#include "BaseUI.h"
#include "../manager/AssetManager.h"

namespace {
    void createSettingsUIComponents(Scene& scene, Entity& overlay) {
        if (!overlay.hasComponent<Children>()) {
            overlay.addComponent<Children>();
        }

        auto &overlayTransform = overlay.getComponent<Transform>();
        auto &overlaySprite = overlay.getComponent<Sprite>();

        float baseX = overlayTransform.position.x;
        float baseY = overlayTransform.position.y;

        auto &closeButton = scene.world.createEntity();
        auto &closeTransform = closeButton.addComponent<Transform>(Vector2D(baseX + overlaySprite.dst.w - 40, baseY + 10),
                                                                   0.0f, 1.0f);

        SDL_Texture *texture = TextureManager::load("../asset/ui/close.png");
        SDL_FRect closeSrc{0, 0, 32, 32};
        SDL_FRect closeDest{closeTransform.position.x, closeTransform.position.y, closeSrc.w, closeSrc.h};
        closeButton.addComponent<Sprite>(texture, closeSrc, closeDest, RenderLayer::UI, false);
        closeButton.addComponent<Collider>("ui", closeDest);

        auto &clickable = closeButton.addComponent<Clickable>();
        clickable.onPressed = [&closeTransform] {
            closeTransform.scale = 0.5f;
        };
        clickable.onReleased = [&scene,&closeTransform] {
            closeTransform.scale = 1.0f;
            scene.world.getUIVisibilityManager().hide("settings");
        };
        clickable.onCancel = [&closeTransform] {
            closeTransform.scale = 1.0f;
        };

        closeButton.addComponent<Parent>(&overlay);
        auto &parentChildren = overlay.getComponent<Children>();
        parentChildren.children.push_back(&closeButton);
    }
}

namespace BaseUI {
    Entity& createSettingsOverlay(Scene& scene, int windowWidth, int windowHeight) {
        auto &overlay(scene.world.createEntity());
        SDL_Texture *overlayTex = TextureManager::load("../asset/ui/settings.jpg");
        SDL_FRect overlaySrc{0, 0, windowWidth * 0.85f, windowHeight * 0.85f};
        SDL_FRect overlayDest{
            (float) windowWidth / 2 - overlaySrc.w / 2, (float) windowHeight / 2 - overlaySrc.h / 2, overlaySrc.w,
            overlaySrc.h
        };
        overlay.addComponent<Transform>(Vector2D{overlayDest.x, overlayDest.y}, 0.0f, 1.0f);
        overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDest, RenderLayer::UI, false);

        createSettingsUIComponents(scene, overlay);
        scene.world.getUIVisibilityManager().registerPanel("settings", &overlay);
        return overlay;
    }

    Entity& createCogButton(Scene& scene, int windowWidth, int windowHeight, Entity& overlay) {
        auto &cog = scene.world.createEntity();
        auto &cogTransform = cog.addComponent<Transform>(Vector2D((float) windowWidth - 50, (float) windowHeight - 50),
                                                         0.0f, 1.0f);

        SDL_Texture *texture = TextureManager::load("../asset/ui/cog.png");
        SDL_FRect cogSrc{0, 0, 32, 32};
        SDL_FRect cogDest{cogTransform.position.x, cogTransform.position.y, cogSrc.w, cogSrc.h};
        cog.addComponent<Sprite>(texture, cogSrc, cogDest, RenderLayer::UI);
        cog.addComponent<Collider>("ui", cogDest);

        auto &clickable = cog.addComponent<Clickable>();
        clickable.onPressed = [&cogTransform] {
            cogTransform.scale = 0.5f;
        };

        clickable.onReleased = [&cogTransform, &scene] {
            cogTransform.scale = 1.0f;
            scene.world.getUIVisibilityManager().toggle("settings");
        };

        clickable.onCancel = [&cogTransform] {
            cogTransform.scale = 1.0f;
        };

        return cog;
    }
    Entity& createLabel(Scene& scene, Entity& parent, const std::string& text, Vector2D pos,
                        SDL_Color color, const std::string& id, bool renderOnBoot) {
        auto& ent = scene.world.createEntity();
        Label lData = {text, AssetManager::getFont("arial"), color, LabelType::Static, id};
        auto& lComp = ent.addComponent<Label>(lData);

        // Start hidden so they don't pop in before the menu logic shows them
        lComp.visible = false;

        if (renderOnBoot) {
            lComp.dirty = true;
            TextureManager::updateLabel(lComp);
        }

        ent.addComponent<Transform>(pos, 0.0f, 1.0f);
        ent.addComponent<Parent>(&parent);
        parent.getComponent<Children>().children.push_back(&ent);
        return ent;
    }

    Entity& createStandardButton(Scene& scene, Entity& parent, Vector2D pos, float width, float height,
                                 SDL_FRect spriteSrc, std::function<void()> onReleased) {
        auto& btnEnt = scene.world.createEntity();

        auto& btnTransform = btnEnt.addComponent<Transform>(pos, 0.0f, 1.0f);
        SDL_Texture* texBtn = TextureManager::load("../asset/ui/Buttons.png");
        SDL_FRect btnDst{pos.x, pos.y, width, height};

        btnEnt.addComponent<Sprite>(texBtn, spriteSrc, btnDst, RenderLayer::UI, false);
        btnEnt.addComponent<Collider>("ui", btnDst).enabled = false;

        auto& clickable = btnEnt.addComponent<Clickable>();
        clickable.onPressed = [&btnTransform] { btnTransform.scale = 0.9f; };
        clickable.onCancel = [&btnTransform] { btnTransform.scale = 1.0f; };

        // Wrap the callback to also reset the scale
        clickable.onReleased = [&btnTransform, onReleased]() {
            btnTransform.scale = 1.0f;
            if (onReleased) onReleased();
        };

        btnEnt.addComponent<Parent>(&parent);
        parent.getComponent<Children>().children.push_back(&btnEnt);

        return btnEnt;
    }

    Entity& createBaseMenuOverlay(Scene& scene, int windowWidth, int windowHeight) {
        auto &overlay(scene.world.createEntity());
        SDL_Texture *overlayTex = TextureManager::load("../asset/ui/UI-Base.png");
        SDL_FRect overlaySrc{0, 0, windowWidth * 0.85f, windowHeight * 0.85f};
        SDL_FRect overlayDest{
            (float) windowWidth / 2 - overlaySrc.w / 2, (float) windowHeight / 2 - overlaySrc.h / 2, overlaySrc.w,
            overlaySrc.h
        };
        overlay.addComponent<Transform>(Vector2D{overlayDest.x, overlayDest.y}, 0.0f, 1.0f);
        overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDest, RenderLayer::UI, false);
        overlay.addComponent<Children>();
        return overlay;
    }
}