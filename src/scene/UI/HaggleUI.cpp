//
// Created by Curry on 2026-04-07.
//

#include "HaggleUI.h"
#include "BaseUI.h"
#include "../manager/AssetManager.h"
#include <algorithm>

// Define the struct methods
int HaggleSession::getProposedPrice() const {
    int finalPrice = 0;
    int multiplier = 10000;
    for (int i = 0; i < 5; ++i) {
        finalPrice += digits[i] * multiplier;
        multiplier /= 10;
    }
    return finalPrice;
}

int HaggleSession::getPercentage() const {
    if (currentItem.basePrice <= 0) return 0;
    return static_cast<int>(((float) getProposedPrice() / currentItem.basePrice) * 100.0f);
}

namespace {
    void propagateCarry(HaggleSession &s) {
        // 1. First, check if the total value would be negative
        if (s.getProposedPrice() < 0) {
            // Reset to absolute zero and stop
            for (int i = 0; i < 5; ++i) s.digits[i] = 0;
        } else {
            // 2. Resolve carries (prevent massive 99999 wrap-around)
            for (int i = 4; i >= 0; --i) {
                if (s.digits[i] > 9) {
                    s.digits[i] = 0;
                    if (i > 0) s.digits[i - 1]++;
                } else if (s.digits[i] < 0) {
                    // Only borrow if there is a higher digit to borrow from
                    bool hasHigherDigit = false;
                    for (int j = 0; j < i; ++j) {
                        if (s.digits[j] > 0) {
                            hasHigherDigit = true;
                            break;
                        }
                    }

                    if (hasHigherDigit) {
                        s.digits[i] = 9;
                        if (i > 0) s.digits[i - 1]--;
                    } else {
                        s.digits[i] = 0;
                    }
                }
            }
        }

        // Clamp the 10,000s place
        s.digits[0] = std::max(0, std::min(9, s.digits[0]));

        // 3. Update ALL digit labels and positions
        for (int i = 0; i < 5; ++i) {
            if (!s.digitRefs[i]) continue;
            auto &lbl = s.digitRefs[i]->getComponent<Label>();
            lbl.text = std::to_string(s.digits[i]);
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);

            s.digitRefs[i]->getComponent<Transform>().position.x =
                    s.columnCenters[i] - (lbl.dst.w / 2.0f);
        }
    }

    void createItemHaggleDisplay(Scene &scene, Entity &parent) {
        auto &session = parent.getComponent<HaggleSession>();
        auto &parentT = parent.getComponent<Transform>();
        auto &parentS = parent.getComponent<Sprite>();

        float subWidth = 180.0f;
        float subHeight = 160.0f;
        float parentCenterX = parentS.dst.w / 2.0f;

        session.absoluteX = parentT.position.x + parentCenterX - (subWidth / 2.0f);
        float absoluteY = parentT.position.y + 40.0f;

        auto &subOverlay = scene.world.createEntity();
        subOverlay.addComponent<Transform>(Vector2D(session.absoluteX, absoluteY), 0.0f, 1.0f);
        subOverlay.addComponent<Parent>(&parent);
        subOverlay.addComponent<Children>();
        parent.getComponent<Children>().children.push_back(&subOverlay);

        SDL_Texture *subTex = TextureManager::load("asset/ui/UI-Sub.png");
        subOverlay.addComponent<Sprite>(subTex, SDL_FRect{0, 0, 21, 21}, SDL_FRect{0, 0, subWidth, subHeight},
                                        RenderLayer::UI, false);

        // --- ITEM NAME ---
        session.itemNameRef = &BaseUI::createLabel(scene, subOverlay, session.currentItem.name,
            Vector2D(0, absoluteY + 15.0f), {0, 0, 0, 255}, "itemName", true);

        auto& nameLbl = session.itemNameRef->getComponent<Label>();
        session.itemNameRef->getComponent<Transform>().position.x =  session.absoluteX + (subWidth / 2.0f) - (nameLbl.dst.w / 2.0f);

        // --- ITEM ICON ---
        auto &itemIcon = scene.world.createEntity();
        float iconSize = 64.0f;
        itemIcon.addComponent<Transform>(Vector2D(session.absoluteX + (subWidth / 2.0f) - (iconSize / 2.0f), absoluteY + 45.0f), 0.0f, 1.0f);

        SDL_Texture *itemsTex = TextureManager::load("asset/items.png");
        itemIcon.addComponent<Sprite>(itemsTex, session.currentItem.src, SDL_FRect{0, 0, iconSize, iconSize}, RenderLayer::UI, false);
        itemIcon.addComponent<Parent>(&subOverlay);
        subOverlay.getComponent<Children>().children.push_back(&itemIcon);
        session.itemIconRef = &itemIcon;

        // --- BASE VALUE ---
        session.itemBaseValRef = &BaseUI::createLabel(scene, subOverlay, "Base Value: " + std::to_string(session.currentItem.basePrice) + "G",
            Vector2D(0, absoluteY + subHeight - 30.0f), {0, 0, 0, 255}, "itemBaseVal", true);

        auto& valLbl = session.itemBaseValRef->getComponent<Label>();
        session.itemBaseValRef->getComponent<Transform>().position.x = session.absoluteX + (subWidth / 2.0f) - (valLbl.dst.w / 2.0f);
    }

    void createPriceSelection(Scene &scene, Entity &overlay) {
        auto &session = overlay.getComponent<HaggleSession>();
        auto &overlayTransform = overlay.getComponent<Transform>();
        auto &overlaySprite = overlay.getComponent<Sprite>();

        float baseX = overlayTransform.position.x;
        float baseY = overlayTransform.position.y;
        float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

        float btnWidth = 48.0f, btnHeight = 24.0f, spacingX = 56.0f;
        float startX = centerLineX - (2.0f * spacingX) - (btnWidth / 2.0f);
        float startY = baseY + 240.0f;
        float downBtnY = startY + 80.0f;
        float textCenterY = startY + btnHeight + ((downBtnY - (startY + btnHeight)) / 2.0f);

        // --- TITLES & LABELS ---
        auto &title = BaseUI::createLabel(scene, overlay, "How much should I sell it for?", Vector2D(0, startY - 40.0f), {0, 0, 0, 255}, "haggleTitle", true);
        title.getComponent<Transform>().position.x = centerLineX - (title.getComponent<Label>().dst.w / 2.0f);

        BaseUI::createLabel(scene, overlay, "Base Price:", Vector2D(startX - 20.0f, downBtnY + 40.0f), {0, 0, 0, 255}, "haggleBasePriceTxt", true);

        session.percentLabelRef = &BaseUI::createLabel(scene, overlay, "100%", Vector2D(0, downBtnY + 40.0f), {0, 0, 0, 255}, "hagglePercent", true);
        session.percentLabelRef->getComponent<Transform>().position.x = startX + (4 * spacingX) + btnWidth - session.percentLabelRef->getComponent<Label>().dst.w;

        // --- COLUMNS ---
        for (int i = 0; i < 5; ++i) {
            float colX = startX + (i * spacingX);
            float columnCenter = colX + (btnWidth / 2.0f);

            session.digitRefs[i] = &BaseUI::createLabel(scene, overlay, std::to_string(session.digits[i]), Vector2D(colX, textCenterY), {0, 0, 0, 255}, "digit_" + std::to_string(i), false);
            session.columnCenters[i] = columnCenter;

            BaseUI::createStandardButton(scene, overlay, Vector2D(colX, startY), btnWidth, btnHeight, {32, 0, 32, 16}, [&scene, &overlay, i]() {
                scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickSoft"));
                auto &s = overlay.getComponent<HaggleSession>();
                s.digits[i]++;
                propagateCarry(s);
                if (s.percentLabelRef) {
                    auto &pLbl = s.percentLabelRef->getComponent<Label>();
                    pLbl.text = std::to_string(s.getPercentage()) + "%";
                    pLbl.dirty = true;
                    TextureManager::updateLabel(pLbl);
                }
            });

            BaseUI::createStandardButton(scene, overlay, Vector2D(colX, downBtnY), btnWidth, btnHeight, {32, 16, 32, 16}, [&scene, &overlay, i]() {
                scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickSoft"));
                auto &s = overlay.getComponent<HaggleSession>();
                s.digits[i]--;
                propagateCarry(s);
                if (s.percentLabelRef) {
                    auto &pLbl = s.percentLabelRef->getComponent<Label>();
                    pLbl.text = std::to_string(s.getPercentage()) + "%";
                    pLbl.dirty = true;
                    TextureManager::updateLabel(pLbl);
                }
            });
        }

        // --- CURRENCY LABEL ("G") ---
        auto& gLabel = BaseUI::createLabel(scene, overlay, "G", Vector2D(0, 0), {0,0,0,255}, "haggleCurrencyTxt", true);
        auto& gComp = gLabel.getComponent<Label>();
        TextureManager::updateLabel(gComp);
        gLabel.getComponent<Transform>().position = Vector2D(
            startX + (4 * spacingX) + btnWidth + 5.0f,
            textCenterY
        );

    }

    void createHaggleButton(Scene &scene, Entity &overlay) {
        auto &overlayT = overlay.getComponent<Transform>();
        auto &overlayS = overlay.getComponent<Sprite>();
        float centerLineX = overlayT.position.x + (overlayS.dst.w / 2.0f);
        float displaySize = 128.0f;
        float scaleX = centerLineX - (displaySize / 2.0f);
        float scaleY = (overlayT.position.y + overlayS.dst.h) - (displaySize);

        auto &haggleButton = scene.world.createEntity();
        auto &haggleTransform = haggleButton.addComponent<Transform>(Vector2D(scaleX, scaleY), 0.0f, 1.0f);
        SDL_Texture *haggleTex = TextureManager::load("asset/ui/haggleButton.png");
        haggleButton.addComponent<Sprite>(haggleTex, SDL_FRect{0, 0, 256, 256}, SDL_FRect{scaleX, scaleY, displaySize, displaySize}, RenderLayer::UI, false);
        haggleButton.addComponent<Collider>("ui", SDL_FRect{scaleX, scaleY, displaySize, displaySize}).enabled = false;

        auto &clickable = haggleButton.addComponent<Clickable>();
        clickable.onPressed = [&haggleTransform] { haggleTransform.scale = 0.9f; };
        clickable.onCancel = [&haggleTransform] { haggleTransform.scale = 1.0f; };
        clickable.onReleased = [&scene, &overlay, &haggleTransform]() {
            haggleTransform.scale = 1.0f;
            auto &session = overlay.getComponent<HaggleSession>();
            scene.world.getUIVisibilityManager().hide("haggle");
            scene.world.getHaggleSystem().submitOffer(session.getProposedPrice());
        };

        haggleButton.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&haggleButton);
    }
}

namespace HaggleUI {
    Entity &create(Scene &scene, int windowWidth, int windowHeight, Entity *&outUIMenu) {
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;
        if (!mainOverlay.hasComponent<Children>()) mainOverlay.addComponent<Children>();

        auto &session = mainOverlay.addComponent<HaggleSession>();
        session.digits = {0, 0, 0, 0, 0};

        createItemHaggleDisplay(scene, mainOverlay);
        createPriceSelection(scene, mainOverlay);
        createHaggleButton(scene, mainOverlay);

        outUIMenu = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("haggle", outUIMenu);
        return mainOverlay;
    }

    Entity &update(Scene &scene, ItemDef &item, Entity *UIMenu) {
        if (!UIMenu) return *UIMenu;
        auto &session = UIMenu->getComponent<HaggleSession>();
        session.currentItem = item;

        int tempPrice = item.basePrice;
        for (int i = 4; i >= 0; --i) {
            session.digits[i] = tempPrice % 10;
            tempPrice /= 10;
        }

        if (session.itemNameRef) {
            auto &lbl = session.itemNameRef->getComponent<Label>();
            lbl.text = item.name; lbl.dirty = true;
            TextureManager::updateLabel(lbl);
            session.itemNameRef->getComponent<Transform>().position.x = session.absoluteX + ( session.subWidth / 2.0f) - (lbl.dst.w / 2.0f);
        }

        if (session.itemIconRef) {
            session.itemIconRef->getComponent<Sprite>().src = item.src;
        }

        if (session.itemBaseValRef) {
            auto &lbl = session.itemBaseValRef->getComponent<Label>();
            lbl.text = "Base Value: " + std::to_string(item.basePrice) + "G"; lbl.dirty = true;
            TextureManager::updateLabel(lbl);
            session.itemBaseValRef->getComponent<Transform>().position.x =session.absoluteX + (session.subWidth / 2.0f) - (lbl.dst.w / 2.0f);
        }

        propagateCarry(session);

        if (session.percentLabelRef) {
            auto &lbl = session.percentLabelRef->getComponent<Label>();
            lbl.text = std::to_string(session.getPercentage()) + "%"; lbl.dirty = true;
            TextureManager::updateLabel(lbl);
        }

        scene.world.getUIVisibilityManager().show("haggle");
        scene.world.getUIVisibilityManager().hide("hud");

        return *UIMenu;
    }
}