//
// Created by Curry on 2026-04-07.
//

#include "HaggleUI.h"
#include "BaseUI.h"
#include "../manager/AssetManager.h"
#include <algorithm> // For std::max, std::min

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
        // Resolve carries left
        for (int i = 4; i >= 0; --i) {
            if (s.digits[i] > 9) {
                s.digits[i] = 0;
                if (i > 0) s.digits[i - 1]++;
            } else if (s.digits[i] < 0) {
                s.digits[i] = 9;
                if (i > 0) s.digits[i - 1]--;
            }
        }
        s.digits[0] = std::max(0, std::min(9, s.digits[0]));

        // Update ALL digit labels and positions
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

    Entity &createItemHaggleDisplay(Scene &scene, Entity &parent) {
        auto &session = parent.getComponent<HaggleSession>();
        auto &parentS = parent.getComponent<Sprite>();
        auto &parentT = parent.getComponent<Transform>();

        float parentCenterX = parentS.dst.w / 2.0f;

        auto &subOverlay = scene.world.createEntity();
        subOverlay.addComponent<Parent>(&parent);

        float subWidth = 180.0f;
        float subHeight = 160.0f;

        float absoluteX = parentT.position.x + parentCenterX - (subWidth / 2.0f);
        float absoluteY = parentT.position.y + 30.0f;

        subOverlay.addComponent<Transform>(Vector2D(absoluteX, absoluteY), 0.0f, 1.0f);

        SDL_Texture *subTex = TextureManager::load("../asset/ui/UI-Sub.png");
        SDL_FRect subSrc{0, 0, 21.0f, 21.0f};
        SDL_FRect subDst{0.0f, 0.0f, subWidth, subHeight};

        subOverlay.addComponent<Sprite>(subTex, subSrc, subDst, RenderLayer::UI, false);
        subOverlay.addComponent<Children>();

        // --- ITEM NAME ---
        auto &nameLabel = scene.world.createEntity();
        Label nameData = {
            session.currentItem.name, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "itemName",
            subWidth - 20.0f
        };
        auto &nameLabelComp = nameLabel.addComponent<Label>(nameData);
        nameLabelComp.visible = false;
        nameLabelComp.dirty = true;
        TextureManager::updateLabel(nameLabelComp);

        nameLabel.addComponent<Transform>(
            Vector2D(absoluteX + (subWidth / 2.0f) - (nameLabelComp.dst.w / 2.0f),
                     absoluteY + 10.0f), 0.0f, 1.0f);
        nameLabel.addComponent<Parent>(&subOverlay);
        subOverlay.getComponent<Children>().children.push_back(&nameLabel);

        session.itemNameRef = &nameLabel;

        // --- ITEM ICON ---
        auto &itemIcon = scene.world.createEntity();
        float iconSize = 64.0f;
        itemIcon.addComponent<Transform>(
            Vector2D(parentT.position.x + parentCenterX - (iconSize / 2.0f), parentT.position.y + 80), 0.0f, 1.0f);

        SDL_Texture *itemsTex = TextureManager::get("items");
        if (!itemsTex) itemsTex = TextureManager::load("../asset/items.png");

        SDL_FRect itemDst{0, 0, iconSize, iconSize};
        itemIcon.addComponent<Sprite>(itemsTex, session.currentItem.src, itemDst, RenderLayer::UI, false);
        itemIcon.addComponent<Parent>(&subOverlay);
        subOverlay.getComponent<Children>().children.push_back(&itemIcon);

        session.itemIconRef = &itemIcon;

        // --- BASE VALUE ---
        auto &baseValLabel = scene.world.createEntity();
        std::string baseValText = "Base Value: " + std::to_string(session.currentItem.basePrice) + "G";
        Label valData = {baseValText, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "itemBaseVal"};
        auto &valLabelComp = baseValLabel.addComponent<Label>(valData);
        valLabelComp.visible = false;
        valLabelComp.dirty = true;
        TextureManager::updateLabel(valLabelComp);

        baseValLabel.addComponent<Transform>(Vector2D(parentT.position.x + parentCenterX - (valLabelComp.dst.w / 2.0f),
                                                      parentT.position.y + 155), 0.0f, 1.0f);
        baseValLabel.addComponent<Parent>(&subOverlay);
        subOverlay.getComponent<Children>().children.push_back(&baseValLabel);

        session.itemBaseValRef = &baseValLabel;

        parent.getComponent<Children>().children.push_back(&subOverlay);

        return subOverlay;
    }

    Entity &createPriceSelection(Scene &scene, Entity &overlay) {
        auto &session = overlay.getComponent<HaggleSession>();
        auto &overlayTransform = overlay.getComponent<Transform>();
        auto &overlaySprite = overlay.getComponent<Sprite>();

        float baseX = overlayTransform.position.x;
        float baseY = overlayTransform.position.y;
        float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

        float btnWidth = 48.0f;
        float btnHeight = 24.0f;
        float spacingX = 56.0f;

        float startX = centerLineX - (2.0f * spacingX) - (btnWidth / 2.0f);
        float startY = baseY + 240.0f;
        float downBtnY = startY + 80.0f;
        float textCenterY = startY + btnHeight + ((downBtnY - (startY + btnHeight)) / 2.0f);

        // --- 1. TITLE LABEL ---
        auto &titleLabelEntity = scene.world.createEntity();
        Label tLabel = {
            "How much should I sell it for?", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static,
            "haggleTitle"
        };
        auto &tLabelComp = titleLabelEntity.addComponent<Label>(tLabel);
        tLabelComp.dirty = true;
        tLabelComp.visible = false;
        TextureManager::updateLabel(tLabelComp);

        titleLabelEntity.addComponent<Transform>(Vector2D(centerLineX - (tLabelComp.dst.w / 2.0f), startY - 40.0f), 0.0f, 1.0f);
        titleLabelEntity.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&titleLabelEntity);

        // --- 2. BASE PRICE LABEL ---
        auto &basePriceLabelEntity = scene.world.createEntity();
        Label bpLabel = {
            "Base Price:", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "haggleBasePriceTxt"
        };
        auto &bpLabelComp = basePriceLabelEntity.addComponent<Label>(bpLabel);
        bpLabelComp.dirty = true;
        bpLabelComp.visible = false;
        TextureManager::updateLabel(bpLabelComp);

        basePriceLabelEntity.addComponent<Transform>(Vector2D(startX - 20.0f, downBtnY + 40.0f), 0.0f, 1.0f);
        basePriceLabelEntity.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&basePriceLabelEntity);

        // --- 3. THE PERCENTAGE LABEL ---
        auto &percentLabelEntity = scene.world.createEntity();
        Label pLabel = {"100%", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "hagglePercent"};
        auto &pLabelComp = percentLabelEntity.addComponent<Label>(pLabel);
        pLabelComp.dirty = true;
        pLabelComp.visible = false;
        TextureManager::updateLabel(pLabelComp);

        float rightAlignX = startX + (4 * spacingX) + btnWidth - pLabelComp.dst.w;
        percentLabelEntity.addComponent<Transform>(Vector2D(rightAlignX, downBtnY + 40.0f), 0.0f, 1.0f);
        percentLabelEntity.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&percentLabelEntity);

        session.percentLabelRef = &percentLabelEntity;

        // --- 4. CURRENCY LABEL ("G") ---
        auto &currencyLabelEntity = scene.world.createEntity();
        Label gLabel = {"G", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "haggleCurrencyTxt"};
        auto &gLabelComp = currencyLabelEntity.addComponent<Label>(gLabel);
        gLabelComp.dirty = true;
        gLabelComp.visible = false;
        TextureManager::updateLabel(gLabelComp);

        currencyLabelEntity.addComponent<Transform>(
            Vector2D(startX + (4 * spacingX) + btnWidth + 15.0f, textCenterY - (gLabelComp.dst.h / 2.0f)), 0.0f, 1.0f);
        currencyLabelEntity.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&currencyLabelEntity);

        // --- 5. GENERATE THE 5 COLUMNS ---
        for (int i = 0; i < 5; ++i) {
            float colX = startX + (i * spacingX);
            float columnCenter = colX + (btnWidth / 2.0f);

            // --- DIGIT LABEL ---
            auto &digitEntity = scene.world.createEntity();
            Label dLabel = {
                std::to_string(session.digits[i]), AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static,
                "digit_" + std::to_string(i)
            };
            auto &dLabelComp = digitEntity.addComponent<Label>(dLabel);
            dLabelComp.visible = false;
            dLabelComp.dirty = true;


            digitEntity.addComponent<Transform>(Vector2D(colX, textCenterY), 0.0f, 1.0f);
            digitEntity.addComponent<Parent>(&overlay);
            overlay.getComponent<Children>().children.push_back(&digitEntity);

            Entity *digitPtr = &digitEntity;
            session.digitRefs[i] = digitPtr;
            session.columnCenters[i] = columnCenter;

            // --- UP BUTTON (+) ---
            auto &btnUp = scene.world.createEntity();
            auto &btnUpTransform = btnUp.addComponent<Transform>(Vector2D(colX, startY), 0.0f, 1.0f);
            SDL_Texture *texButtons = TextureManager::load("../asset/ui/Buttons.png");

            SDL_FRect srcUp{32, 0, 32, 16};
            SDL_FRect destUp{colX, startY, btnWidth, btnHeight};

            btnUp.addComponent<Sprite>(texButtons, srcUp, destUp, RenderLayer::UI, false);
            btnUp.addComponent<Collider>("ui", destUp).enabled = false;

            auto &clickUp = btnUp.addComponent<Clickable>();
            clickUp.onPressed = [&btnUpTransform] { btnUpTransform.scale = 0.8f; };
            clickUp.onCancel = [&btnUpTransform] { btnUpTransform.scale = 1.0f; };
            clickUp.onReleased = [&overlay, i, &btnUpTransform, &scene]() {
                btnUpTransform.scale = 1.0f;
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
            };
            btnUp.addComponent<Parent>(&overlay);
            overlay.getComponent<Children>().children.push_back(&btnUp);

            // --- DOWN BUTTON (-) ---
            auto &btnDown = scene.world.createEntity();
            auto &btnDownTransform = btnDown.addComponent<Transform>(Vector2D(colX, downBtnY), 0.0f, 1.0f);

            SDL_FRect srcDown{32, 16, 32, 16};
            SDL_FRect destDown{colX, downBtnY, btnWidth, btnHeight};

            btnDown.addComponent<Sprite>(texButtons, srcDown, destDown, RenderLayer::UI, false);
            btnDown.addComponent<Collider>("ui", destDown).enabled = false;

            auto &clickDown = btnDown.addComponent<Clickable>();
            clickDown.onPressed = [&btnDownTransform] { btnDownTransform.scale = 0.8f; };
            clickDown.onCancel = [&btnDownTransform] { btnDownTransform.scale = 1.0f; };

            clickDown.onReleased = [&overlay, i, &btnDownTransform, &scene]() {
                btnDownTransform.scale = 1.0f;
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
            };
            btnDown.addComponent<Parent>(&overlay);
            overlay.getComponent<Children>().children.push_back(&btnDown);
        }

        return overlay;
    }

    Entity &createHaggleButton(Scene &scene, Entity &overlay) {
        auto &overlayTransform = overlay.getComponent<Transform>();
        auto &overlaySprite = overlay.getComponent<Sprite>();

        float baseX = overlayTransform.position.x;
        float baseY = overlayTransform.position.y;
        float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

        auto &haggleButton = scene.world.createEntity();
        float displaySize = 128.0f;
        float scaleX = centerLineX - (displaySize / 2.0f);
        float scaleY = (baseY + overlaySprite.dst.h) - (displaySize);

        auto &haggleTransform = haggleButton.addComponent<Transform>(Vector2D(scaleX, scaleY), 0.0f, 1.0f);
        SDL_Texture *haggleTex = TextureManager::load("../asset/ui/haggleButton.png");

        SDL_FRect haggleSrc{0, 0, 256, 256};
        SDL_FRect haggleDest{scaleX, scaleY, displaySize, displaySize};

        haggleButton.addComponent<Sprite>(haggleTex, haggleSrc, haggleDest, RenderLayer::UI, false);
        haggleButton.addComponent<Collider>("ui", haggleDest).enabled = false;


        auto &clickable = haggleButton.addComponent<Clickable>();
        clickable.onPressed = [&haggleTransform] { haggleTransform.scale = 0.9f; };
        clickable.onCancel = [&haggleTransform] { haggleTransform.scale = 1.0f; };
        clickable.onReleased = [&scene, &overlay, &haggleTransform]() {
            haggleTransform.scale = 1.0f;
            auto &session = overlay.getComponent<HaggleSession>();
            int finalPrice = session.getProposedPrice();
            scene.world.getUIVisibilityManager().hide("haggle");
            scene.world.getHaggleSystem().submitOffer(finalPrice);
        };

        haggleButton.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&haggleButton);

        return haggleButton;
    }
}

// --- PUBLIC NAMESPACE FUNCTIONS ---
namespace HaggleUI {
    Entity &create(Scene &scene, int windowWidth, int windowHeight, Entity *&outUIMenu) {
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;

        if (!mainOverlay.hasComponent<Children>()) {
            mainOverlay.addComponent<Children>();
        }

        auto &session = mainOverlay.addComponent<HaggleSession>();
        session.currentItem.name = "Loading...";
        session.currentItem.basePrice = 0;
        session.currentItem.src = {0, 0, 32, 32};
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

        // 1. Reset digits based on new item's base price
        int tempPrice = item.basePrice;
        for (int i = 4; i >= 0; --i) {
            session.digits[i] = tempPrice % 10;
            tempPrice /= 10;
        }

        // 2. Update Item Display Details using cached refs
        if (session.itemNameRef) {
            auto &nameLabel = session.itemNameRef->getComponent<Label>();
            nameLabel.text = item.name;
            nameLabel.dirty = true;
            TextureManager::updateLabel(nameLabel);
        }

        if (session.itemIconRef) {
            auto &iconSprite = session.itemIconRef->getComponent<Sprite>();
            iconSprite.src = item.src;
        }

        if (session.itemBaseValRef) {
            auto &baseValLabel = session.itemBaseValRef->getComponent<Label>();
            baseValLabel.text = "Base Value: " + std::to_string(item.basePrice) + "G";
            baseValLabel.dirty = true;
            TextureManager::updateLabel(baseValLabel);
        }

        // 3. Update Digit Spinners (Using propagateCarry here ensures they are centered perfectly)
        propagateCarry(session);

        // 4. Update Percentage
        if (session.percentLabelRef) {
            auto &pLabel = session.percentLabelRef->getComponent<Label>();
            pLabel.text = std::to_string(session.getPercentage()) + "%";
            pLabel.dirty = true;
            TextureManager::updateLabel(pLabel);
        }

        // 5. Open the menu
        scene.world.getUIVisibilityManager().show("haggle");
        scene.world.getUIVisibilityManager().hide("hud");

        return *UIMenu;
    }
}