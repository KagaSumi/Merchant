//
// Created by Curry on 2026-04-07.
//

#include "DialogueUI.h"
#include "../manager/AssetManager.h"

namespace DialogueUI {
    Entity& create(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIDialogue, std::function<void()>& outSimpleDialogueConfirm) {
        auto& overlay = scene.world.createEntity();

        // Bottom third of screen, like your mockup
        float boxH = windowHeight * 0.30f;
        float boxW = windowWidth * 0.85f;
        float boxX = (windowWidth - boxW) / 2.0f;
        float boxY = windowHeight - boxH - 20.0f;

        SDL_Texture* bgTex = TextureManager::load("../asset/ui/UI-Base.png");
        SDL_FRect src{0, 0, boxW, boxH};
        SDL_FRect dst{boxX, boxY, boxW, boxH};

        overlay.addComponent<Transform>(Vector2D(boxX, boxY), 0.0f, 1.0f);
        overlay.addComponent<Sprite>(bgTex, src, dst, RenderLayer::UI, false);
        overlay.addComponent<Children>();

        auto& session = overlay.addComponent<DialogueSession>();

        // --- MESSAGE LABEL ---
        auto& msgEnt = scene.world.createEntity();
        Label msgData = {
            "...", AssetManager::getFont("arial"),
            {0, 0, 0, 255}, LabelType::Static,  "dialogueMsg", boxW - 40.0f
        };
        auto& msgComp = msgEnt.addComponent<Label>(msgData);
        msgComp.dirty = true;
        msgComp.visible = false;

        // OPTIMIZATION: Removed TextureManager::updateLabel(msgComp); here!
        // We don't need to render "..." because the X position below doesn't rely on text width.

        msgEnt.addComponent<Transform>(Vector2D(boxX + 20.0f, boxY + 20.0f), 0.0f, 1.0f);
        msgEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&msgEnt);
        session.messageLabelRef = &msgEnt;

        // --- CONFIRM BUTTON ---
        auto& btnEnt = scene.world.createEntity();
        float btnW = 120.0f, btnH = 36.0f;
        float btnX = boxX + boxW - btnW - 20.0f;
        float btnY = boxY + boxH - btnH - 20.0f;

        auto& btnTransform = btnEnt.addComponent<Transform>(Vector2D(btnX, btnY), 0.0f, 1.0f);
        SDL_Texture* btnTex = TextureManager::load("../asset/ui/Buttons.png");
        SDL_FRect btnSrc{0, 33, 64, 16};
        SDL_FRect btnDst{btnX, btnY, btnW, btnH};

        btnEnt.addComponent<Sprite>(btnTex, btnSrc, btnDst, RenderLayer::UI, false);
        btnEnt.addComponent<Collider>("ui", btnDst).enabled = false;

        auto& clickable = btnEnt.addComponent<Clickable>();
        clickable.onPressed = [&btnTransform] { btnTransform.scale = 0.9f; };
        clickable.onCancel = [&btnTransform] { btnTransform.scale = 1.0f; };
        clickable.onReleased = [&scene, &btnTransform, &outSimpleDialogueConfirm]() {
            btnTransform.scale = 1.0f;
            scene.world.getUIVisibilityManager().hide("dialogue");
            scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickSoft"));

            // Only restore HUD if no other full-screen UI is open
            auto& ui = scene.world.getUIVisibilityManager();
            bool orderOpen   = ui.isVisible("order");
            bool summaryOpen = ui.isVisible("summary");
            bool haggleOpen  = ui.isVisible("haggle");

            // FIX: Removed the stray semicolon here
            if (!orderOpen && !summaryOpen && !haggleOpen) {
                ui.show("hud");
            }

            auto& haggle = scene.world.getHaggleSystem();
            if (haggle.pendingConfirm) {
                haggle.onDialogueConfirmed();
            } else if (outSimpleDialogueConfirm) {
                auto cb = outSimpleDialogueConfirm;
                outSimpleDialogueConfirm = nullptr;
                if (cb) cb();
            }
        };


        btnEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&btnEnt);
        session.confirmBtnRef = &btnEnt;

        outUIDialogue = &overlay;
        scene.world.getUIVisibilityManager().registerPanel("dialogue", outUIDialogue);
        return overlay;
    }

    Entity& update(Scene& scene, const std::string& message, Entity* UIDialogue) {
        if (!UIDialogue) return *UIDialogue;

        auto& session = UIDialogue->getComponent<DialogueSession>();

        if (session.messageLabelRef) {
            auto& lbl = session.messageLabelRef->getComponent<Label>();
            lbl.text = message;
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);
        }

        scene.world.getUIVisibilityManager().hide("hud");
        scene.world.getUIVisibilityManager().show("dialogue");

        return *UIDialogue;
    }

    void showSimple(Scene& scene, const std::string& message, Entity* UIDialogue, std::function<void()>& outSimpleDialogueConfirm, Entity* playerEntity) {
        if (!UIDialogue) return;

        scene.world.getUIVisibilityManager().hide("hud");

        auto& session = UIDialogue->getComponent<DialogueSession>();

        // Update message
        if (session.messageLabelRef) {
            auto& lbl = session.messageLabelRef->getComponent<Label>();
            lbl.text = message;
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);
        }

        //Lock the player movement
        playerEntity->getComponent<PlayerTag>().movementLocked = true;

        // Store a simple close callback
        outSimpleDialogueConfirm = [&outSimpleDialogueConfirm, playerEntity]() {
            // Nothing to chain, just closed
            outSimpleDialogueConfirm = nullptr;
            playerEntity->getComponent<PlayerTag>().movementLocked = false;
        };

        scene.world.getUIVisibilityManager().show("dialogue");
    }
}