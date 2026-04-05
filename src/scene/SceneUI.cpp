//
// Created by Curry on 2026/04/04.
//

#include "Game.h"
#include "Scene.h"
#include "../manager/AssetManager.h"

Entity &Scene::createSettingsOverlay(int windowWidth, int windowHeight) {
    auto &overlay(world.createEntity());
    SDL_Texture *overlayTex = TextureManager::load("../asset/ui/settings.jpg");
    SDL_FRect overlaySrc{0, 0, windowWidth * 0.85f, windowHeight * 0.85f};
    SDL_FRect overlayDest{
        (float) windowWidth / 2 - overlaySrc.w / 2, (float) windowHeight / 2 - overlaySrc.h / 2, overlaySrc.w,
        overlaySrc.h
    };
    overlay.addComponent<Transform>(Vector2D{overlayDest.x, overlayDest.y}, 0.0f, 1.0f);
    overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDest, RenderLayer::UI, false);

    createSettingsUIComponents(overlay);
    return overlay;
}

Entity &Scene::createCogButton(int windowWidth, int windowHeight, Entity &overlay) {
    auto &cog = world.createEntity();
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

    clickable.onReleased = [&cogTransform, this ,&overlay] {
        cogTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay, nullptr);
    };

    clickable.onCancel = [&cogTransform] {
        cogTransform.scale = 1.0f;
    };

    return cog;
}

void Scene::createSettingsUIComponents(Entity &overlay) {
    if (!overlay.hasComponent<Children>()) {
        overlay.addComponent<Children>();
    }

    auto &overlayTransform = overlay.getComponent<Transform>();
    auto &overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;

    auto &closeButton = world.createEntity();
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
    clickable.onReleased = [this,&overlay,&closeTransform] {
        closeTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay, nullptr);
    };
    clickable.onCancel = [&closeTransform] {
        closeTransform.scale = 1.0f;
    };

    closeButton.addComponent<Parent>(&overlay);
    auto &parentChildren = overlay.getComponent<Children>();
    parentChildren.children.push_back(&closeButton);
}

Entity &Scene::createBaseMenuOverlay(int windowWidth, int windowHeight) {
    auto &overlay(world.createEntity());
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

//Haggle UI
Entity &Scene::createItemHaggleDisplay(Entity &parent) {
    auto &session = parent.getComponent<HaggleSession>();
    auto &parentS = parent.getComponent<Sprite>();
    auto &parentT = parent.getComponent<Transform>();

    float parentCenterX = parentS.dst.w / 2.0f;

    auto &subOverlay = world.createEntity();
    subOverlay.addComponent<Parent>(&parent);

    float subWidth = 180.0f;
    float subHeight = 160.0f;

    // 1. Calculate the exact, absolute screen position
    // Parent Left + Half Parent Width - Half Sub Width
    float absoluteX = parentT.position.x + parentCenterX - (subWidth / 2.0f);
    // Parent Top + 10px down
    float absoluteY = parentT.position.y + 30.0f;

    // 2. Assign the absolute position to the Transform
    subOverlay.addComponent<Transform>(Vector2D(absoluteX, absoluteY), 0.0f, 1.0f);

    SDL_Texture *subTex = TextureManager::load("../asset/ui/UI-Sub.png");
    SDL_FRect subSrc{0, 0, 21.0f, 21.0f};

    // 3. ZERO OUT the local x/y offset so the renderer doesn't push it twice
    SDL_FRect subDst{0.0f, 0.0f, subWidth, subHeight};

    subOverlay.addComponent<Sprite>(subTex, subSrc, subDst, RenderLayer::UI, false);
    subOverlay.addComponent<Children>();

    // --- ITEM NAME ---
    auto &nameLabel = world.createEntity();
    Label nameData = {
        session.currentItem.name, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "itemName"
    };
    auto &nameLabelComp = nameLabel.addComponent<Label>(nameData);
    nameLabelComp.visible = false;
    nameLabelComp.dirty = true;
    TextureManager::updateLabel(nameLabelComp);

    // Your nameLabel math was already perfectly centered!
    nameLabel.addComponent<Transform>(Vector2D(parentT.position.x + parentCenterX - (nameLabelComp.dst.w / 2.0f),
                                               parentT.position.y + 40), 0.0f, 1.0f);
    nameLabel.addComponent<Parent>(&subOverlay);
    subOverlay.getComponent<Children>().children.push_back(&nameLabel);

    session.itemNameRef = &nameLabel; // Cache reference

    // --- ITEM ICON ---
    auto &itemIcon = world.createEntity();
    float iconSize = 64.0f;
    itemIcon.addComponent<Transform>(
        Vector2D(parentT.position.x + parentCenterX - (iconSize / 2.0f), parentT.position.y + 80), 0.0f, 1.0f);

    SDL_Texture *itemsTex = TextureManager::get("items");
    if (!itemsTex) itemsTex = TextureManager::load("../asset/items.png");

    SDL_FRect itemDst{0, 0, iconSize, iconSize};
    itemIcon.addComponent<Sprite>(itemsTex, session.currentItem.src, itemDst, RenderLayer::UI, false);
    itemIcon.addComponent<Parent>(&subOverlay);
    subOverlay.getComponent<Children>().children.push_back(&itemIcon);

    session.itemIconRef = &itemIcon; // Cache reference

    // --- BASE VALUE ---
    auto &baseValLabel = world.createEntity();
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

    session.itemBaseValRef = &baseValLabel; // Cache reference

    parent.getComponent<Children>().children.push_back(&subOverlay);

    return subOverlay;
}

Entity &Scene::createHaggleUI(int windowWidth, int windowHeight) {
    auto &mainOverlay = createBaseMenuOverlay(windowWidth, windowHeight);
    mainOverlay.getComponent<Sprite>().visible = false;

    if (!mainOverlay.hasComponent<Children>()) {
        mainOverlay.addComponent<Children>();
    }

    auto &session = mainOverlay.addComponent<HaggleSession>();
    // Set a safe default state so the initial labels don't crash
    session.currentItem.name = "Loading...";
    session.currentItem.basePrice = 0;
    session.currentItem.src = {0, 0, 32, 32};
    session.digits = {0, 0, 0, 0, 0};

    // Build the modular parts
    createItemHaggleDisplay(mainOverlay);
    createPriceSelection(windowWidth, windowHeight, mainOverlay);
    createHaggleButton(windowWidth, windowHeight, mainOverlay);

    UIMenu = &mainOverlay;
    return mainOverlay;
}

Entity &Scene::createPriceSelection(int windowWidth, int windowHeight, Entity &overlay) {
    auto &session = overlay.getComponent<HaggleSession>();
    auto &overlayTransform = overlay.getComponent<Transform>();
    auto &overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

    // --- SIZING & SPACING MATH ---
    // Increase button size (1.5x scale of the original 32x16)
    float btnWidth = 48.0f;
    float btnHeight = 24.0f;
    float spacingX = 56.0f;

    // Increase gap between columns to fit the larger buttons

    // Calculate the X start point so all 5 columns perfectly center
    // 5 columns means we go -2, -1, 0, 1, 2 widths from the center line.
    float startX = centerLineX - (2.0f * spacingX) - (btnWidth / 2.0f);

    // Y Offsets
    float startY = baseY + 240.0f;
    float downBtnY = startY + 80.0f;
    float textCenterY = startY + btnHeight + ((downBtnY - (startY + btnHeight)) / 2.0f);

    // --- 1. TITLE LABEL ("How much should I sell it for?") ---
    auto &titleLabelEntity = world.createEntity();
    Label tLabel = {
        "How much should I sell it for?", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static,
        "haggleTitle"
    };
    auto &tLabelComp = titleLabelEntity.addComponent<Label>(tLabel);
    tLabelComp.dirty = true;
    tLabelComp.visible = false;
    TextureManager::updateLabel(tLabelComp);

    // Centered above the buttons (40px up)
    titleLabelEntity.addComponent<Transform>(Vector2D(centerLineX - (tLabelComp.dst.w / 2.0f), startY - 40.0f), 0.0f,
                                             1.0f);
    titleLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&titleLabelEntity);


    // --- 2. BASE PRICE LABEL ("Base Price:") ---
    auto &basePriceLabelEntity = world.createEntity();
    Label bpLabel = {
        "Base Price:", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "haggleBasePriceTxt"
    };
    auto &bpLabelComp = basePriceLabelEntity.addComponent<Label>(bpLabel);
    bpLabelComp.dirty = true;
    bpLabelComp.visible = false;
    TextureManager::updateLabel(bpLabelComp);

    // Aligned to the left, below the down buttons
    basePriceLabelEntity.addComponent<Transform>(Vector2D(startX - 20.0f, downBtnY + 40.0f), 0.0f, 1.0f);
    basePriceLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&basePriceLabelEntity);


    // --- 3. THE PERCENTAGE LABEL ---
    auto &percentLabelEntity = world.createEntity();
    Label pLabel = {"100%", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "hagglePercent"};
    auto &pLabelComp = percentLabelEntity.addComponent<Label>(pLabel);
    pLabelComp.dirty = true;
    pLabelComp.visible = false;
    TextureManager::updateLabel(pLabelComp);

    // Aligned to the right, matching the Base Price Y coordinate
    float rightAlignX = startX + (4 * spacingX) + btnWidth - pLabelComp.dst.w;
    percentLabelEntity.addComponent<Transform>(Vector2D(rightAlignX, downBtnY + 40.0f), 0.0f, 1.0f);
    percentLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&percentLabelEntity);

    session.percentLabelRef = &percentLabelEntity; // Cache reference


    // --- 4. CURRENCY LABEL ("G") ---
    auto &currencyLabelEntity = world.createEntity();
    Label gLabel = {"G", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "haggleCurrencyTxt"};
    auto &gLabelComp = currencyLabelEntity.addComponent<Label>(gLabel);
    gLabelComp.dirty = true;
    gLabelComp.visible = false;
    TextureManager::updateLabel(gLabelComp);

    // Placed to the right of the last column, vertically centered with the digits
    currencyLabelEntity.addComponent<Transform>(
        Vector2D(startX + (4 * spacingX) + btnWidth + 15.0f, textCenterY - (gLabelComp.dst.h / 2.0f)), 0.0f, 1.0f);
    currencyLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&currencyLabelEntity);


    // --- 5. GENERATE THE 5 COLUMNS ---
    for (int i = 0; i < 5; ++i) {
        float colX = startX + (i * spacingX);
        float columnCenter = colX + (btnWidth / 2.0f);

        // --- DIGIT LABEL ---
        auto &digitEntity = world.createEntity();
        Label dLabel = {
            std::to_string(session.digits[i]), AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static,
            "digit_" + std::to_string(i)
        };
        auto &dLabelComp = digitEntity.addComponent<Label>(dLabel);
        dLabelComp.visible = false;
        dLabelComp.dirty = true;
        TextureManager::updateLabel(dLabelComp);

        float centeredX = columnCenter - (dLabelComp.dst.w / 2.0f);
        float centeredY = textCenterY - (dLabelComp.dst.h / 2.0f);

        digitEntity.addComponent<Transform>(Vector2D(centeredX, centeredY), 0.0f, 1.0f);
        digitEntity.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&digitEntity);

        Entity *digitPtr = &digitEntity;
        session.digitRefs[i] = digitPtr;

        // --- UP BUTTON (+) ---
        auto &btnUp = world.createEntity();
        auto &btnUpTransform = btnUp.addComponent<Transform>(Vector2D(colX, startY), 0.0f, 1.0f);
        SDL_Texture *texButtons = TextureManager::load("../asset/ui/Buttons.png");

        SDL_FRect srcUp{32, 0, 32, 16};
        SDL_FRect destUp{colX, startY, btnWidth, btnHeight}; // Rendered larger!

        btnUp.addComponent<Sprite>(texButtons, srcUp, destUp, RenderLayer::UI, false);
        btnUp.addComponent<Collider>("ui", destUp).enabled = false;

        auto &clickUp = btnUp.addComponent<Clickable>();
        clickUp.onPressed = [&btnUpTransform] { btnUpTransform.scale = 0.8f; };
        clickUp.onCancel = [&btnUpTransform] { btnUpTransform.scale = 1.0f; };
        clickUp.onReleased = [&overlay, i, digitPtr, columnCenter, &btnUpTransform]() {
            btnUpTransform.scale = 1.0f;
            auto &currentSession = overlay.getComponent<HaggleSession>();

            currentSession.digits[i] = (currentSession.digits[i] + 1) % 10;

            auto &dLabelComp = digitPtr->getComponent<Label>();
            dLabelComp.text = std::to_string(currentSession.digits[i]);
            dLabelComp.dirty = true;
            TextureManager::updateLabel(dLabelComp);

            auto &dTransform = digitPtr->getComponent<Transform>();
            dTransform.position.x = columnCenter - (dLabelComp.dst.w / 2.0f);

            if (currentSession.percentLabelRef) {
                auto &pLabelComp = currentSession.percentLabelRef->getComponent<Label>();
                pLabelComp.text = std::to_string(currentSession.getPercentage()) + "%";
                pLabelComp.dirty = true;
                TextureManager::updateLabel(pLabelComp);
            }
        };
        btnUp.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&btnUp);

        // --- DOWN BUTTON (-) ---
        auto &btnDown = world.createEntity();
        auto &btnDownTransform = btnDown.addComponent<Transform>(Vector2D(colX, downBtnY), 0.0f, 1.0f);

        SDL_FRect srcDown{32, 16, 32, 16};
        SDL_FRect destDown{colX, downBtnY, btnWidth, btnHeight}; // Rendered larger!

        btnDown.addComponent<Sprite>(texButtons, srcDown, destDown, RenderLayer::UI, false);
        btnDown.addComponent<Collider>("ui", destDown).enabled = false;

        auto &clickDown = btnDown.addComponent<Clickable>();
        clickDown.onPressed = [&btnDownTransform] { btnDownTransform.scale = 0.8f; };
        clickDown.onCancel = [&btnDownTransform] { btnDownTransform.scale = 1.0f; };
        clickDown.onReleased = [&overlay, i, digitPtr, columnCenter, &btnDownTransform]() {
            btnDownTransform.scale = 1.0f;
            auto &currentSession = overlay.getComponent<HaggleSession>();

            currentSession.digits[i] = currentSession.digits[i] - 1;
            if (currentSession.digits[i] < 0) currentSession.digits[i] = 9;

            auto &dLabelComp = digitPtr->getComponent<Label>();
            dLabelComp.text = std::to_string(currentSession.digits[i]);
            dLabelComp.dirty = true;
            TextureManager::updateLabel(dLabelComp);

            auto &dTransform = digitPtr->getComponent<Transform>();
            dTransform.position.x = columnCenter - (dLabelComp.dst.w / 2.0f);

            if (currentSession.percentLabelRef) {
                auto &pLabelComp = currentSession.percentLabelRef->getComponent<Label>();
                pLabelComp.text = std::to_string(currentSession.getPercentage()) + "%";
                pLabelComp.dirty = true;
                TextureManager::updateLabel(pLabelComp);
            }
        };
        btnDown.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&btnDown);
    }

    return overlay;
}

Entity &Scene::createHaggleButton(int windowWidth, int windowHeight, Entity &overlay) {
    auto &overlayTransform = overlay.getComponent<Transform>();
    auto &overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

    auto &haggleButton = world.createEntity();
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
    clickable.onReleased = [this, &overlay, &haggleTransform]() {
        haggleTransform.scale = 1.0f;
        auto& session = overlay.getComponent<HaggleSession>();
        int finalPrice = session.getProposedPrice();
        toggleSettingsOverlayVisibility(overlay, nullptr);
        world.getHaggleSystem().submitOffer(finalPrice);
    };

    haggleButton.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&haggleButton);

    return haggleButton;
}

Entity &Scene::updateHaggleUI(ItemDef &item) {
    if (!UIMenu) {
        std::cerr << "Error: UIMenu not initialized!" << std::endl;
        return *UIMenu; // Return early to prevent crash
    }

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
        iconSprite.src = item.src; // Update the source rect for the new item icon
    }

    if (session.itemBaseValRef) {
        auto &baseValLabel = session.itemBaseValRef->getComponent<Label>();
        baseValLabel.text = "Base Value: " + std::to_string(item.basePrice) + "G";
        baseValLabel.dirty = true;
        TextureManager::updateLabel(baseValLabel);
    }

    // 3. Update Digit Spinners
    for (int i = 0; i < 5; ++i) {
        if (session.digitRefs[i]) {
            auto &dLabel = session.digitRefs[i]->getComponent<Label>();
            dLabel.text = std::to_string(session.digits[i]);
            dLabel.dirty = true;
            TextureManager::updateLabel(dLabel);
        }
    }

    // 4. Update Percentage (will default to 100% since digits == basePrice)
    if (session.percentLabelRef) {
        auto &pLabel = session.percentLabelRef->getComponent<Label>();
        pLabel.text = std::to_string(session.getPercentage()) + "%";
        pLabel.dirty = true;
        TextureManager::updateLabel(pLabel);
    }

    // 5. Open the menu
    bool forceOpen = true;
    toggleSettingsOverlayVisibility(*UIMenu, &forceOpen);

    return *UIMenu;
}

//Summary UI
Entity &Scene::createDaySummaryUI(int windowWidth, int windowHeight, DayCycle& dayCycle) {
    auto &mainOverlay = createBaseMenuOverlay(windowWidth, windowHeight);
    mainOverlay.getComponent<Sprite>().visible = false;

    if (!mainOverlay.hasComponent<Children>()) {
        mainOverlay.addComponent<Children>();
    }

    // Create a local, zeroed-out data struct just for the initial layout calculations
    DaySummaryData defaultData;

    // Attach the session
    auto &session = mainOverlay.addComponent<DaySummarySession>();
    session.currentData = defaultData;

    // Pass the zeroed-out data to build the hidden layout
    createDaySummaryContent(mainOverlay, defaultData);
    createDaySummaryFooter(mainOverlay, defaultData,dayCycle);

    UIDaySummary = &mainOverlay;
    return mainOverlay;
}

void Scene::createDaySummaryContent(Entity &overlay, const DaySummaryData &data) {
    auto &overlayTransform = overlay.getComponent<Transform>();
    auto &overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

    // --- 1. TITLE ---
    auto &titleLabel = world.createEntity();
    Label tData = {"Today's Report", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "reportTitle"};
    tData.dirty = true;
    tData.visible = false;
    auto &tComp = titleLabel.addComponent<Label>(tData);
    TextureManager::updateLabel(tComp);

    titleLabel.addComponent<Transform>(Vector2D(centerLineX - (tComp.dst.w / 2.0f), baseY + 30.0f), 0.0f, 1.0f);
    titleLabel.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&titleLabel);

    // --- 2. LEDGER CONFIG ---
    float leftColX = baseX + 60.0f;
    float rightColX = baseX + overlaySprite.dst.w - 120.0f; // Anchor for right-aligned text
    float startY = baseY + 120.0f;
    float rowSpacing = 45.0f;

    SDL_Color colorPositive = {76, 175, 80, 255}; // Green
    SDL_Color colorNegative = {211, 47, 47, 255}; // Red
    SDL_Color colorNeutral = {0, 0, 0, 255}; // Black

    // --- HELPER LAMBDA FOR ROWS ---
    auto createRow = [&](const std::string &labelText, int value, float yPos,
                         SDL_Color valColor) -> std::pair<Entity *, Entity *> {
        // Left Label
        auto &textEnt = world.createEntity();
        Label lData = {labelText, AssetManager::getFont("arial"), colorNeutral, LabelType::Static, labelText};
        lData.dirty = true;
        lData.visible = false;
        TextureManager::updateLabel(textEnt.addComponent<Label>(lData));
        textEnt.addComponent<Transform>(Vector2D(leftColX, yPos), 0.0f, 1.0f);
        textEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&textEnt);

        // Right Value
        auto &valEnt = world.createEntity();
        std::string valString = std::to_string(value) + "g";
        // If it's negative, std::to_string handles the minus sign. For positive > 0, we can add a plus if desired.

        Label vData = {valString, AssetManager::getFont("arial"), valColor, LabelType::Static, labelText + "_val"};
        auto &vComp = valEnt.addComponent<Label>(vData);
        vComp.dirty = true;
        vComp.visible = false;
        TextureManager::updateLabel(vComp);

        // Right-align math: Anchor X minus the width of the generated text
        valEnt.addComponent<Transform>(Vector2D(rightColX - vComp.dst.w, yPos), 0.0f, 1.0f);
        valEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&valEnt);

        return {&textEnt, &valEnt};
    };
    auto &session = overlay.getComponent<DaySummarySession>();

    // --- 3. GENERATE ROWS ---
    session.grossSalesValRef = createRow("Gross Sales", data.grossSales, startY,
                                         data.grossSales > 0 ? colorPositive : colorNeutral).second;
    session.customerPurchasesValRef = createRow("Stock Orders", data.orderExpenses, startY + rowSpacing,
                                                data.orderExpenses < 0 ? colorNegative : colorNeutral).second;
    auto wpRow = createRow("Weekly Payment", data.weeklyPaymentAmount, startY + rowSpacing * 2, colorNegative);
    session.weeklyPaymentLabelRef = wpRow.first;
    session.weeklyPaymentValRef = wpRow.second;

    int profit = data.getGrossProfit();
    SDL_Color profitColor = profit > 0 ? colorPositive : (profit < 0 ? colorNegative : colorNeutral);
    session.grossProfitValRef = createRow("Gross Profit", profit, startY + (rowSpacing * 2.5f), profitColor).second;
    // Extra gap for the total
}

void Scene::createDaySummaryFooter(Entity &overlay, const DaySummaryData &data,DayCycle& dayCycle) {
    auto &session = overlay.getComponent<DaySummarySession>();
    auto &overlayTransform = overlay.getComponent<Transform>();
    auto &overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;

    float footerHeight = 140.0f;
    float footerY = baseY + overlaySprite.dst.h - footerHeight;

    // --- 1. DIVIDER LINE ---
    // Hack: Stretch a tiny UI texture to act as a 2-pixel tall separator line
    auto &lineEnt = world.createEntity();
    SDL_Texture *texUI = TextureManager::load("../asset/ui/UI-Sub.png");
    SDL_FRect lineSrc{0, 0, 8, 8};
    SDL_FRect lineDst{baseX + 10.0f, footerY, overlaySprite.dst.w - 20.0f, 2.0f};

    lineEnt.addComponent<Transform>(Vector2D(lineDst.x, lineDst.y), 0.0f, 1.0f);
    lineEnt.addComponent<Sprite>(texUI, lineSrc, SDL_FRect{0, 0, lineDst.w, lineDst.h}, RenderLayer::UI, false);
    lineEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&lineEnt);

    // --- 2. WEEKLY PAYMENT TEXT ---
    // Title
    auto &debtTitleEnt = world.createEntity();
    Label dtData = {
        "Weekly Payment", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "debtTitleText"
    };
    dtData.dirty = true;
    dtData.visible = false;
    TextureManager::updateLabel(debtTitleEnt.addComponent<Label>(dtData));

    // Position the title near the top of the footer
    debtTitleEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, footerY + 15.0f), 0.0f, 1.0f);
    debtTitleEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&debtTitleEnt);

    // Subtitle (The actual days and amount)
    auto &debtSubEnt = world.createEntity();
    session.debtSubTextRef = &debtSubEnt;
    std::string debtSubText = "In " + std::to_string(data.daysUntilPayment) + " days: " + std::to_string(
                                  data.weeklyPaymentAmount) + "g";
    Label dsData = {debtSubText, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "debtSubText"};
    dsData.dirty = true;
    dsData.visible = false;
    TextureManager::updateLabel(debtSubEnt.addComponent<Label>(dsData));

    // Position the subtitle directly below the title (Y offset + 35.0f)
    debtSubEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, footerY + 50.0f), 0.0f, 1.0f);
    debtSubEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&debtSubEnt);

    // --- 3. CURRENT BALANCE TEXT ---
    auto &balEnt = world.createEntity();
    session.balanceTextRef = &balEnt;
    std::string balText = "Current Balance: " + std::to_string(data.currentBalance) + "g";
    Label bData = {balText, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "balText"};
    bData.dirty = true;
    bData.visible = false;
    TextureManager::updateLabel(balEnt.addComponent<Label>(bData));

    balEnt.addComponent<Transform>(Vector2D(baseX + (overlaySprite.dst.w * 0.45f), footerY + 20.0f), 0.0f, 1.0f);
    balEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&balEnt);

    // --- 3.5. TOTAL DEBT TEXT (NEW) ---
    auto &totalDebtEnt = world.createEntity();
    session.totalDebtTextRef = &totalDebtEnt;
    std::string tdText = "Total Debt: " + std::to_string(data.totalDebt) + "g";
    // Using your UI's red color {211, 47, 47, 255} so it looks like a debt
    Label tdData = {tdText, AssetManager::getFont("arial"), {211, 47, 47, 255}, LabelType::Static, "totalDebtText"};
    tdData.dirty = true;
    tdData.visible = false;
    TextureManager::updateLabel(totalDebtEnt.addComponent<Label>(tdData));

    // Place it exactly below the Current Balance text
    totalDebtEnt.addComponent<Transform>(Vector2D(baseX + (overlaySprite.dst.w * 0.45f), footerY + 50.0f), 0.0f, 1.0f);
    totalDebtEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&totalDebtEnt);

    // --- 4. CONFIRM BUTTON ---
    auto &btnEnt = world.createEntity();
    float btnWidth = 160.0f;
    float btnHeight = 40.0f;
    float btnX = baseX + overlaySprite.dst.w - btnWidth - 30.0f;
    float btnY = footerY + (footerHeight / 2.0f) - (btnHeight / 2.0f);

    auto &btnTransform = btnEnt.addComponent<Transform>(Vector2D(btnX, btnY), 0.0f, 1.0f);

    // Assuming you have a standard button sprite texture
    SDL_Texture *texBtn = TextureManager::load("../asset/ui/Buttons.png");
    SDL_FRect btnSrc{0, 33, 64, 16}; // Adjust to your actual button source rect
    SDL_FRect btnDst{btnX, btnY, btnWidth, btnHeight};

    btnEnt.addComponent<Sprite>(texBtn, btnSrc, btnDst, RenderLayer::UI, false);
    btnEnt.addComponent<Collider>("ui", btnDst).enabled = false;

    // Click Logic
    auto &clickable = btnEnt.addComponent<Clickable>();
    clickable.onPressed = [&btnTransform] { btnTransform.scale = 0.9f; };
    clickable.onCancel = [&btnTransform] { btnTransform.scale = 1.0f; };
    clickable.onReleased = [&overlay, &btnTransform, this, &dayCycle]() {
        btnTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay, nullptr);

        // 1. Grab the session data to see if we won or lost
        auto &session = overlay.getComponent<DaySummarySession>();

        if (session.currentData.isBankrupt) {
            // ❌ LOSE CONDITION
            initLose();
        }
        else if (session.currentData.isGameWon) {
            // ✅ WIN CONDITION
            initWin();
        }
        else {
            // ⏩ NORMAL PLAY (Next Day)
            std::cout << "Starting next day cycle..." << std::endl;
            dayCycle.phaseSwapReady = true;
        }
    };

    btnEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&btnEnt);
}

Entity &Scene::updateDaySummaryUI(const DaySummaryData &data) {
    if (!UIDaySummary) {
        std::cerr << "Error: UIDaySummary not initialized!" << std::endl;
        return *UIDaySummary;
    }

    auto &session = UIDaySummary->getComponent<DaySummarySession>();
    session.currentData = data;

    auto &overlayTransform = UIDaySummary->getComponent<Transform>();
    auto &overlaySprite = UIDaySummary->getComponent<Sprite>();

    // We need to recalculate the right anchor for our right-aligned text
    float rightColX = overlayTransform.position.x + overlaySprite.dst.w - 120.0f;

    SDL_Color colorPositive = {76, 175, 80, 255};
    SDL_Color colorNegative = {211, 47, 47, 255};
    SDL_Color colorNeutral = {0, 0, 0, 255};

    // --- HELPER LAMBDA FOR UPDATING RIGHT-ALIGNED LEDGER VALUES ---
    auto updateLedgerValue = [&](Entity *ent, int value, SDL_Color color) {
        if (!ent) return;
        auto &label = ent->getComponent<Label>();
        label.text = std::to_string(value) + "g";
        label.color = color;
        label.dirty = true;
        TextureManager::updateLabel(label); // This updates label.dst.w

        // Shift the X position based on the new text width to keep it right-aligned!
        auto &transform = ent->getComponent<Transform>();
        transform.position.x = rightColX - label.dst.w;
    };

    // 1. Update Ledger Rows
    updateLedgerValue(session.grossSalesValRef, data.grossSales, data.grossSales > 0 ? colorPositive : colorNeutral);
    updateLedgerValue(session.customerPurchasesValRef, -data.orderExpenses,
                      data.orderExpenses < 0 ? colorNegative : colorNeutral);

    int profit = data.getGrossProfit();
    updateLedgerValue(session.grossProfitValRef, profit,
                      profit > 0 ? colorPositive : (profit < 0 ? colorNegative : colorNeutral));

    bool isPaymentDay = (data.daysUntilPayment == 0);

    if (session.weeklyPaymentLabelRef && session.weeklyPaymentValRef) {
        // Toggle visibility for both the left and right side of the row
        session.weeklyPaymentLabelRef->getComponent<Label>().visible = isPaymentDay;
        session.weeklyPaymentValRef->getComponent<Label>().visible = isPaymentDay;

        // If it is visible, update the number and right-align it
        if (isPaymentDay) {
            updateLedgerValue(session.weeklyPaymentValRef, -data.weeklyPayment, colorNegative);
        }
    }

    // 2. Update Footer Subtext (Days Remaining & Amount)
    if (session.debtSubTextRef) {
        auto &subLabel = session.debtSubTextRef->getComponent<Label>();
        subLabel.text = "In " + std::to_string(data.daysUntilPayment) + " days: " + std::to_string(
                            data.weeklyPaymentAmount) + "g";
        subLabel.dirty = true;
        TextureManager::updateLabel(subLabel);
    }

    // 3. Update Balance Text
    if (session.balanceTextRef) {
        auto &balLabel = session.balanceTextRef->getComponent<Label>();
        balLabel.text = "Current Balance: " + std::to_string(data.currentBalance) + "g";
        balLabel.dirty = true;
        TextureManager::updateLabel(balLabel);
    }


    // 4. Open the menu
    bool forceOpen = true;
    toggleSettingsOverlayVisibility(*UIDaySummary, &forceOpen);


    //Hide Weekly Payment if none was made
    if (session.weeklyPaymentLabelRef && session.weeklyPaymentValRef) {
        bool isPaymentDay = (data.daysUntilPayment == 0);
        session.weeklyPaymentLabelRef->getComponent<Label>().visible = isPaymentDay;
        session.weeklyPaymentValRef->getComponent<Label>().visible = isPaymentDay;
    }


    return *UIDaySummary;
}

//Inventory UI
Entity &Scene::createInventoryUI(int windowWidth, int windowHeight) {
    auto &mainOverlay = createBaseMenuOverlay(windowWidth, windowHeight);
    mainOverlay.getComponent<Sprite>().visible = false;


    auto &session = mainOverlay.addComponent<InventorySession>();
    auto &overlayTransform = mainOverlay.getComponent<Transform>();
    auto &overlaySprite = mainOverlay.getComponent<Sprite>();


    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float menuWidth = overlaySprite.dst.w;

    auto &gridContainer = world.createEntity();
    gridContainer.addComponent<Transform>(Vector2D(baseX, baseY), 0.0f, 1.0f);
    gridContainer.addComponent<Children>();
    gridContainer.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&gridContainer);
    session.gridContainerRef = &gridContainer;

    // --- 1. TITLE ---
    auto &titleLabel = world.createEntity();
    Label tData = {
        "Stockroom Inventory", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "invTitle"
    };
    tData.dirty = true;
    tData.visible = false;
    TextureManager::updateLabel(titleLabel.addComponent<Label>(tData));
    titleLabel.addComponent<Transform>(
        Vector2D(baseX + (menuWidth / 2) - (titleLabel.getComponent<Label>().dst.w / 2), baseY + 30.0f), 0.0f, 1.0f);
    titleLabel.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&titleLabel);

    //Stock Indicator
    float stockX = baseX + menuWidth - 160.0f; // Anchor to the right side
    float stockY = baseY + 24.0f;

    auto& stockIcon = world.createEntity();
    stockIcon.addComponent<Transform>(Vector2D(stockX, stockY), 0.0f, 1.0f);
    SDL_Texture* itemsTex = TextureManager::load("../asset/items.png");
    stockIcon.addComponent<Sprite>(itemsTex, SDL_FRect{0,0,32,32}, SDL_FRect{stockX, stockY, 32, 32}, RenderLayer::UI, false).visible = false;
    stockIcon.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&stockIcon);
    session.targetStockIconRef = &stockIcon;

    // The Text Label (e.g. "x2 / 4")
    auto& stockLabel = world.createEntity();
    Label stkData = {"Empty (0/4)", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static, "invStockLbl"};
    auto& stkComp = stockLabel.addComponent<Label>(stkData);
    stkComp.visible = false;
    TextureManager::updateLabel(stkComp);

    // Position text next to the icon
    stockLabel.addComponent<Transform>(Vector2D(stockX + 36.0f, stockY + 6.0f), 0.0f, 1.0f);
    stockLabel.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&stockLabel);
    session.targetStockLabelRef = &stockLabel;

    // --- 2. 4x4 GRID SETUP ---
    float padding = 40.0f;
    float gridStartX = baseX + padding;
    float gridStartY = baseY + 80.0f; // just below title

    float menuHeight = overlaySprite.dst.h;

    // Divide the full available width and height across 4 cols/rows
    float colSpacing = (menuWidth - (padding * 2.0f)) / 4.0f;
    float rowSpacing = (menuHeight - gridStartY + baseY - padding) / 4.0f;

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            InventorySlotRefs slotRefs;
            float iconSize = 48.0f;
            float iconOffsetX = (colSpacing - iconSize) / 2.0f;
            float iconOffsetY = (rowSpacing * 0.2f); // sits in upper portion of cell

            float slotX = gridStartX + (col * colSpacing);
            float slotY = gridStartY + (row * rowSpacing);

            // Container
            auto &container = world.createEntity();
            container.addComponent<Transform>(Vector2D(slotX, slotY), 0.0f, 1.0f);
            container.addComponent<Children>();
            slotRefs.container = &container;

            // Icon

            auto &iconEnt = world.createEntity();
            auto &iconTransform = iconEnt.addComponent<Transform>(Vector2D(slotX + iconOffsetX, slotY + iconOffsetY),
                                                                  0.0f, 1.0f);
            SDL_FRect iconDst = {slotX + iconOffsetX, slotY + iconOffsetY, iconSize, iconSize};
            iconEnt.addComponent<Sprite>(itemsTex, SDL_FRect{0, 0, 32, 32}, iconDst, RenderLayer::UI, false).visible =
                    false;
            slotRefs.icon = &iconEnt;
            iconEnt.addComponent<Parent>(&container);
            container.getComponent<Children>().children.push_back(&iconEnt);

            // Label (Vertical stacking: Text sits below the icon for 4x4)
            auto &labelEnt = world.createEntity();
            labelEnt.addComponent<Parent>(&container);
            container.getComponent<Children>().children.push_back(&labelEnt);
            Label lData = {
                "Empty", AssetManager::getFont("arial-small"), {0, 0, 0, 255}, LabelType::Static,
                "inv_" + std::to_string(row) + "_" + std::to_string(col)
            };
            lData.dirty = true;
            lData.visible = false;
            TextureManager::updateLabel(labelEnt.addComponent<Label>(lData));
            // Offset label below icon
            labelEnt.addComponent<Transform>(Vector2D(slotX, slotY + iconOffsetY + iconSize + 6.0f), 0.0f, 1.0f);
            slotRefs.label = &labelEnt;

            //Clickable
            int idx = row * 4 + col;
            Entity *iconPtr = &iconEnt;
            auto &iconClick = iconEnt.addComponent<Clickable>();

            iconClick.onPressed = [this, iconPtr, idx]() {
                auto &s = UIInventory->getComponent<InventorySession>();
                if (s.mode != InventoryMode::PlaceItem) return;

                // 2. Only shrink if the item exists and is in stock
                if (idx < s.cachedInventory.size() && s.cachedInventory[idx].quantity > 0) {
                    iconPtr->getComponent<Transform>().scale = 0.9f;
                }
            };

            iconClick.onCancel = [this, iconPtr, idx]() {
                auto &s = UIInventory->getComponent<InventorySession>();
                if (s.mode != InventoryMode::PlaceItem) return;

                // 3. Only grow back if it was allowed to shrink in the first place
                if (idx < s.cachedInventory.size() && s.cachedInventory[idx].quantity > 0) {
                    iconPtr->getComponent<Transform>().scale = 1.0f;
                }
            };

            iconClick.onReleased = [this, iconPtr, idx]() {
                auto &s = UIInventory->getComponent<InventorySession>();

                if (s.mode != InventoryMode::PlaceItem) return;

                if (idx < s.cachedInventory.size()) {
                    auto &entry = s.cachedInventory[idx];

                    if (entry.quantity > 0) {
                        // Reset the scale back to normal before changing screens
                        iconPtr->getComponent<Transform>().scale = 1.0f;

                        // 1. Hide the Inventory Screen
                        bool close = false;
                        toggleSettingsOverlayVisibility(*UIInventory, &close);

                        int maxToPlace = std::min(entry.quantity, 4);

                        // 2. Open the completely separate Quantity Screen
                        openQuantityScreen(
                            entry,
                            maxToPlace,
                            s.quantitySession.onConfirm,
                            [this]() {
                                auto &session = UIInventory->getComponent<InventorySession>();
                                // Safely reopen the inventory by passing the data back through our main pipeline!
                                updateInventoryUI(session.cachedInventory, session.mode, session.currentStand);
                            }
                        );
                    }
                }
            };

            auto &iconCol = iconEnt.addComponent<Collider>("ui", iconDst);
            iconCol.enabled = false;

            iconEnt.addComponent<Parent>(&container);
            labelEnt.addComponent<Parent>(&container);
            container.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&container);

            session.slots.push_back(slotRefs);
            container.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&container);
        }
    }

    UIInventory = &mainOverlay;
    return mainOverlay;
}

Entity &Scene::updateInventoryUI(const std::vector<InventoryEntry> &inventoryData,
                                 InventoryMode mode, DisplayStand* targetStand) {
    if (!UIInventory) return *UIInventory;
    auto &session = UIInventory->getComponent<InventorySession>();
    session.currentStand = targetStand;
    session.mode = mode;

    //Update Stock If there is something
    if (session.targetStockIconRef && session.targetStockLabelRef) {
        auto& iconSprite = session.targetStockIconRef->getComponent<Sprite>();
        auto& labelComp = session.targetStockLabelRef->getComponent<Label>();

        if (mode == InventoryMode::PlaceItem && targetStand != nullptr) {
            if (targetStand->quantity > 0) {
                // Shelf has items: Show sprite and count
                iconSprite.src = targetStand->item.src;
                iconSprite.visible = true;
                labelComp.text = "x" + std::to_string(targetStand->quantity) + " / 4";
                labelComp.visible = true;
            } else {
                // Shelf is empty
                iconSprite.visible = false;
                labelComp.text = "Empty (0/4)";
                labelComp.visible = true;
            }
            labelComp.dirty = true;
            TextureManager::updateLabel(labelComp);
        } else {
            // Browsing mode: Hide the indicator entirely
            iconSprite.visible = false;
            labelComp.visible = false;
        }
    }


    // 1. Sort the entire 16-item list so it's always in the same order
    auto sortedInv = inventoryData;
    std::sort(sortedInv.begin(), sortedInv.end(), [](const InventoryEntry &a, const InventoryEntry &b) {
        if (a.item.type != b.item.type) return static_cast<int>(a.item.type) < static_cast<int>(b.item.type);
        return a.item.basePrice > b.item.basePrice;
    });
    session.cachedInventory = sortedInv;

    // 2. Map to the 16 slots
    for (size_t i = 0; i < session.slots.size(); ++i) {
        auto &slot = session.slots[i];

        if (i < sortedInv.size()) {
            const auto &entry = sortedInv[i];
            auto &label = slot.label->getComponent<Label>();
            auto &iconSprite = slot.icon->getComponent<Sprite>();

            // Always show the icon and the name
            iconSprite.src = entry.item.src;
            iconSprite.visible = true;
            label.visible = true;
            label.text = entry.item.name + " x" + std::to_string(entry.quantity);

            // --- THE GRAY OUT LOGIC ---
            if (entry.quantity <= 0) {
                // Out of Stock: Light Gray text
                label.color = {120, 120, 120, 255};
            } else {
                // In Stock: Black text
                label.color = {0, 0, 0, 255};
            }

            label.dirty = true;
            TextureManager::updateLabel(label);

            // Re-center
            float iconX = slot.icon->getComponent<Transform>().position.x;
            label.dst.x = (iconX + 24.0f) - (label.dst.w / 2.0f);
        } else {
            // If the XML has fewer than 16 items, hide the extra slots
            slot.icon->getComponent<Sprite>().visible = true;
            slot.label->getComponent<Label>().visible = true;
        }
    }

    bool forceOpen = true;
    toggleSettingsOverlayVisibility(*UIInventory, &forceOpen);
    for (size_t i = 0; i < session.slots.size(); ++i) {
        auto& slot = session.slots[i];
        bool isClickable = false;

        // As long as they have the item in stock, let them click it!
        // The seamless swap logic handles the rest.
        if (i < sortedInv.size() && sortedInv[i].quantity > 0 && session.mode == InventoryMode::PlaceItem) {
            isClickable = true;
        }

        // Force the icon's collider to obey the rules
        if (slot.icon->hasComponent<Collider>()) {
            slot.icon->getComponent<Collider>().enabled = isClickable;
        }
    }

    if (session.targetStockIconRef && session.targetStockLabelRef) {
        auto& iconSprite = session.targetStockIconRef->getComponent<Sprite>();
        auto& labelComp = session.targetStockLabelRef->getComponent<Label>();

        if (mode == InventoryMode::PlaceItem && targetStand != nullptr) {
            labelComp.visible = true; // Label is always visible in Place Mode

            if (targetStand->quantity > 0) {
                // Shelf has items: Show sprite and count
                iconSprite.src = targetStand->item.src;
                iconSprite.visible = true; // Sledgehammer turned this on, we keep it on
                labelComp.text = "x" + std::to_string(targetStand->quantity) + " / 4";
            } else {
                // Shelf is empty: Force the sprite back OFF
                iconSprite.visible = false;
                labelComp.text = "Empty (0/4)";
            }

            labelComp.dirty = true;
            TextureManager::updateLabel(labelComp);
        } else {
            // Browsing mode: Force hide the indicator entirely
            iconSprite.visible = false;
            labelComp.visible = false;
        }
    }
    return *UIInventory;
}

//Select UI
Entity &Scene::createQuantityScreen(int windowWidth, int windowHeight) {
    // 1. Create a fresh base menu overlay
    auto &mainOverlay = createBaseMenuOverlay(windowWidth, windowHeight);
    mainOverlay.getComponent<Sprite>().visible = false;

    auto &session = mainOverlay.addComponent<QuantityScreenSession>();
    auto &overlaySprite = mainOverlay.getComponent<Sprite>();
    auto &overlayTransform = mainOverlay.getComponent<Transform>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float centerX = baseX + (overlaySprite.dst.w / 2.0f);

    // --- HEADER (Icon & Name) ---
    auto &iconEnt = world.createEntity();
    iconEnt.addComponent<Transform>(Vector2D(centerX - 80.0f, baseY + 80.0f), 0.0f, 1.0f);
    SDL_Texture *itemsTex = TextureManager::load("../asset/items.png");
    iconEnt.addComponent<Sprite>(itemsTex, SDL_FRect{0, 0, 32, 32}, SDL_FRect{0, 0, 48, 48}, RenderLayer::UI, false);
    iconEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&iconEnt);
    session.itemIconRef = &iconEnt;

    auto &nameEnt = world.createEntity();
    Label nameData = {"Item Name", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "qtyScreenName"};
    TextureManager::updateLabel(nameEnt.addComponent<Label>(nameData));
    nameEnt.addComponent<Transform>(Vector2D(centerX - 20.0f, baseY + 90.0f), 0.0f, 1.0f);
    nameEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&nameEnt);
    session.itemNameRef = &nameEnt;

    // --- PROMPT ---
    auto &promptEnt = world.createEntity();
    Label promptData = {
        "How many to place?", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "qtyScreenPrompt"
    };
    auto &promptComp = promptEnt.addComponent<Label>(promptData);
    TextureManager::updateLabel(promptComp);
    promptEnt.addComponent<Transform>(Vector2D(centerX - (promptComp.dst.w / 2.0f), baseY + 180.0f), 0.0f, 1.0f);
    promptEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&promptEnt);
    // --- THE 4 BUTTON POOL ---
    SDL_Texture *btnTex = TextureManager::load("../asset/ui/Buttons.png");

    // Scale up the 32x16 source image so it's easy to click
    float btnSizeW = 96.0f;
    float btnSizeH = 48.0f;

    for (int q = 1; q <= 4; ++q) {
        auto &btn = world.createEntity();
        btn.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);

        // Determine exact X and Y on the sprite sheet based on the button number
        float srcX = (q == 1 || q == 3) ? 0.0f : 33.0f;
        float srcY = (q == 1 || q == 2) ? 64.0f : 80.0f;

        SDL_FRect srcRect{srcX, srcY, 32.0f, 16.0f};
        SDL_FRect dstRect{0, 0, btnSizeW, btnSizeH};

        btn.addComponent<Sprite>(btnTex, srcRect, dstRect, RenderLayer::UI, false).visible = false;
        btn.addComponent<Collider>("ui", dstRect).enabled = false;

        // (We completely removed the numEnt Label creation here!)

        // Click Logic
        auto &bClick = btn.addComponent<Clickable>();
        Entity *btnPtr = &btn;
        bClick.onPressed = [btnPtr] { btnPtr->getComponent<Transform>().scale = 0.9f; };
        bClick.onCancel = [btnPtr] { btnPtr->getComponent<Transform>().scale = 1.0f; };
        bClick.onReleased = [this, q, btnPtr]() {
            btnPtr->getComponent<Transform>().scale = 1.0f;
            auto &s = UIQuantityScreen->getComponent<QuantityScreenSession>();

            bool close = false;
            toggleSettingsOverlayVisibility(*UIQuantityScreen, &close);

            if (s.onConfirm) {
                s.onConfirm(s.selectedItem, q);
            }
        };

        btn.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&btn);
        session.quantityButtonRefs.push_back(&btn);
    }

    // --- BACK/CANCEL BUTTON ---
    auto &returnBtn = world.createEntity();
    float rBtnW = 120.0f, rBtnH = 36.0f;
    float rBtnX = centerX - (rBtnW / 2.0f);
    float rBtnY = baseY + overlaySprite.dst.h - 80.0f;

    returnBtn.addComponent<Transform>(Vector2D(rBtnX, rBtnY), 0.0f, 1.0f);
    returnBtn.addComponent<Sprite>(btnTex, SDL_FRect{0, 49, 64, 16}, SDL_FRect{rBtnX, rBtnY, rBtnW, rBtnH},
                                   RenderLayer::UI, false);
    returnBtn.addComponent<Collider>("ui", SDL_FRect{rBtnX, rBtnY, rBtnW, rBtnH}).enabled = false;

    auto &rClick = returnBtn.addComponent<Clickable>();
    Entity *rBtnPtr = &returnBtn;
    rClick.onPressed = [rBtnPtr] { rBtnPtr->getComponent<Transform>().scale = 0.9f; };
    rClick.onCancel = [rBtnPtr] { rBtnPtr->getComponent<Transform>().scale = 1.0f; };
    rClick.onReleased = [this, rBtnPtr]() {
        rBtnPtr->getComponent<Transform>().scale = 1.0f;

        // Hide this screen
        bool close = false;
        toggleSettingsOverlayVisibility(*UIQuantityScreen, &close);

        // Trigger cancel callback (which will reopen the inventory)
        auto &s = UIQuantityScreen->getComponent<QuantityScreenSession>();
        if (s.onCancel) s.onCancel();
    };

    returnBtn.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&returnBtn);

    UIQuantityScreen = &mainOverlay;
    return mainOverlay;
}

void Scene::openQuantityScreen(const InventoryEntry &item, int maxQty,
                               std::function<void(InventoryEntry, int)> onConfirm,
                               std::function<void()> onCancel) {
    if (!UIQuantityScreen) return;
    auto &session = UIQuantityScreen->getComponent<QuantityScreenSession>();

    session.selectedItem = item;
    session.onConfirm = onConfirm;
    session.onCancel = onCancel;

    if (session.itemIconRef) {
        session.itemIconRef->getComponent<Sprite>().src = item.item.src;
    }
    if (session.itemNameRef) {
        auto &lbl = session.itemNameRef->getComponent<Label>();
        lbl.text = item.item.name + " (Max: " + std::to_string(maxQty) + ")";
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);
    }

    // 1. Force the menu on
    bool forceOpen = true;
    toggleSettingsOverlayVisibility(*UIQuantityScreen, &forceOpen);

    // 2. Layout the buttons
    auto &overlaySprite = UIQuantityScreen->getComponent<Sprite>();
    auto &overlayTransform = UIQuantityScreen->getComponent<Transform>();
    float centerX = overlayTransform.position.x + overlaySprite.dst.w / 2.0f;
    float containerY = overlayTransform.position.y + 240.0f;

    int count = std::min(maxQty, 4); // Capped at 4!

    // Adjust spacing to account for the wider 96x48 buttons
    float btnSizeW = 96.0f;
    float btnSpacing = 110.0f;
    float totalWidth = count * btnSpacing - (btnSpacing - btnSizeW);
    float startX = centerX - totalWidth / 2.0f;

    for (int i = 0; i < 4; ++i) {
        Entity *btn = session.quantityButtonRefs[i];
        if (!btn) continue;

        auto &sprite = btn->getComponent<Sprite>();
        auto &col = btn->getComponent<Collider>();

        if (i < count) {
            float bx = startX + (i * btnSpacing);

            btn->getComponent<Transform>().position.x = bx;
            btn->getComponent<Transform>().position.y = containerY;

            sprite.dst.x = bx;
            sprite.dst.y = containerY;
            col.rect.x = bx;
            col.rect.y = containerY;

            sprite.visible = true;
            col.enabled = true;
        } else {
            sprite.visible = false;
            col.enabled = false;
        }
    }
}

//Order UI
Entity& Scene::createOrderUI(int windowWidth, int windowHeight) {
    auto& mainOverlay = createBaseMenuOverlay(windowWidth, windowHeight);
    mainOverlay.getComponent<Sprite>().visible = false;

    auto& session = mainOverlay.addComponent<OrderSession>();
    auto& overlayTransform = mainOverlay.getComponent<Transform>();
    auto& overlaySprite = mainOverlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float menuWidth = overlaySprite.dst.w;
    float menuHeight = overlaySprite.dst.h;
    float centerX = baseX + menuWidth / 2.0f;

    // --- TITLE ---
    auto& titleEnt = world.createEntity();
    Label tData = {"Market", AssetManager::getFont("arial"), {0,0,0,255}, LabelType::Static, "orderTitle"};
    auto& tComp = titleEnt.addComponent<Label>(tData);
    tComp.dirty = true; tComp.visible = false;
    TextureManager::updateLabel(tComp);
    titleEnt.addComponent<Transform>(Vector2D(centerX - tComp.dst.w / 2.0f, baseY + 30.0f), 0.0f, 1.0f);
    titleEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&titleEnt);

    // --- WALLET LABEL (top left) ---
    auto& walletEnt = world.createEntity();
    Label wData = {"Wallet: 0G", AssetManager::getFont("arial"), {0,0,0,255}, LabelType::Static, "orderWallet"};
    auto& wComp = walletEnt.addComponent<Label>(wData);
    wComp.dirty = true; wComp.visible = false;
    TextureManager::updateLabel(wComp);
    walletEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, baseY + 40.0f), 0.0f, 1.0f);
    walletEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&walletEnt);
    session.walletLabelRef = &walletEnt;

    // --- GRID: 4 cols x 4 rows = 16 slots ---
    // Reserve bottom strip for Continue button
    float footerH = 70.0f;
    float gridTop = baseY + 80.0f;
    float gridBottom = baseY + menuHeight - footerH;
    float gridH = gridBottom - gridTop;

    int cols = 4;
    int rows = 4;
    float colW = menuWidth / cols;
    float rowH = gridH / rows;

    SDL_Texture* tilemapTex = TextureManager::load("../asset/SpriteSheet.png");
    SDL_Texture* itemsTex = TextureManager::load("../asset/items.png");
    SDL_Texture* btnTex = TextureManager::load("../asset/ui/Buttons.png");

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            OrderSlotRefs slot;
            float slotX = baseX + col * colW;
            float slotY = gridTop + row * rowH;

            float iconSize = 36.0f;
            float padding = 6.0f;

            // Icon
            auto& iconEnt = world.createEntity();
            iconEnt.addComponent<Transform>(Vector2D(slotX + padding, slotY + padding), 0.0f, 1.0f);
            SDL_FRect iconDst = {slotX + padding, slotY + padding, iconSize, iconSize};
            iconEnt.addComponent<Sprite>(itemsTex, SDL_FRect{0,0,32,32}, iconDst, RenderLayer::UI, false);
            iconEnt.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&iconEnt);
            slot.icon = &iconEnt;

            // Name label
            auto& nameEnt = world.createEntity();
            Label nData = {"Item", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static,
                           "ord_name_" + std::to_string(row) + "_" + std::to_string(col)};
            auto& nComp = nameEnt.addComponent<Label>(nData);
            nComp.dirty = true; nComp.visible = false;
            TextureManager::updateLabel(nComp);
            nameEnt.addComponent<Transform>(
                Vector2D(slotX + padding + iconSize + 4.0f, slotY + padding), 0.0f, 1.0f);
            nameEnt.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&nameEnt);
            slot.nameLabel = &nameEnt;

            // Price label
            float btnY = slotY + padding + nComp.dst.h + 4.0f;
            float btnW = 48.0f, btnH = 22.0f;
            auto& priceEnt = world.createEntity();
            Label pData = {"0G", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static,
                           "ord_price_" + std::to_string(row) + "_" + std::to_string(col)};
            auto& pComp = priceEnt.addComponent<Label>(pData);
            pComp.dirty = true; pComp.visible = false;
            TextureManager::updateLabel(pComp);
            priceEnt.addComponent<Transform>(
                Vector2D(slotX + padding + iconSize + btnW + 4.0f, btnY + 2.0f), 0.0f, 1.0f);
            priceEnt.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&priceEnt);
            slot.priceLabel = &priceEnt;

            // Buy button
            auto& btnEnt = world.createEntity();
            float btnX = slotX + padding + iconSize + 4.0f;

            auto& btnTransform = btnEnt.addComponent<Transform>(Vector2D(btnX, btnY), 0.0f, 1.0f);
            SDL_FRect btnDst = {btnX, btnY, btnW, btnH};
            btnEnt.addComponent<Sprite>(btnTex, SDL_FRect{0,0,32,16}, btnDst, RenderLayer::UI, false);
            auto& btnCol = btnEnt.addComponent<Collider>("ui", btnDst);
            btnCol.enabled = false;
            slot.buyBtn = &btnEnt;

            // Buy label "Buy"
            auto& buyLblEnt = world.createEntity();
            Label buyLblData = {"Buy", AssetManager::getFont("arial-small"), {0,0,0,255},
                                LabelType::Static, "ord_buy_lbl_" + std::to_string(row*cols+col)};
            auto& buyLblComp = buyLblEnt.addComponent<Label>(buyLblData);
            buyLblComp.dirty = true; buyLblComp.visible = false;
            TextureManager::updateLabel(buyLblComp);
            buyLblEnt.addComponent<Transform>(
                Vector2D(btnX + (btnW/2) - (buyLblComp.dst.w/2),
                         btnY + (btnH/2) - (buyLblComp.dst.h/2)), 0.0f, 1.0f);
            buyLblEnt.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&buyLblEnt);
            slot.buyLabel = &buyLblEnt;

            int slotIdx = row * cols + col;
            auto& bClick = btnEnt.addComponent<Clickable>();
            bClick.onPressed = [&btnTransform]{ btnTransform.scale = 0.9f; };
            bClick.onCancel = [&btnTransform]{ btnTransform.scale = 1.0f; };
            bClick.onReleased = [this, slotIdx, &btnTransform]() {
                btnTransform.scale = 1.0f;
                if (!UIOrderScreen) return;

                auto& s = UIOrderScreen->getComponent<OrderSession>();
                if (!s.inventoryRef || !s.walletRef) return;
                if (slotIdx >= s.slots.size()) return;

                // Find the item for this slot from the label text
                // (we store item data in the slot directly via updateOrderUI)
                // Deduct wallet and add to inventory handled in updateOrderUI's click capture
            };

            btnEnt.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&btnEnt);

            session.slots.push_back(slot);
        }
    }
    // --- ADDITIONAL SHELF UPGRADE (Footer) ---
    float shelfX = baseX + 40.0f; // Put it on the bottom left
    float shelfY = gridBottom + 15.0f;

    // Shelf Icon
    auto& shelfIcon = world.createEntity();
    shelfIcon.addComponent<Transform>(Vector2D(shelfX, shelfY), 0.0f, 1.0f);
    shelfIcon.addComponent<Sprite>(tilemapTex, SDL_FRect{16* 32.0f, 8*32.0f, 96, 128}, SDL_FRect{-32.0f, -32.0, 36.0f, 48.0f}, RenderLayer::UI, false);
    shelfIcon.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&shelfIcon);

    // Shelf Name Label
    auto& shelfName = world.createEntity();
    Label sNameData = {"Additional Shelf", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static, "shelfName"};
    auto& sNameComp = shelfName.addComponent<Label>(sNameData);
    sNameComp.dirty = true; sNameComp.visible = false;
    TextureManager::updateLabel(sNameComp);
    shelfName.addComponent<Transform>(Vector2D(shelfX + 45.0f, shelfY + 8.0f), 0.0f, 1.0f);
    shelfName.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&shelfName);

    // Shelf Price Label
    auto& shelfPrice = world.createEntity();
    Label sPriceData = {"500G", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static, "shelfPrice"};
    auto& sPriceComp = shelfPrice.addComponent<Label>(sPriceData);
    sPriceComp.dirty = true; sPriceComp.visible = false;
    TextureManager::updateLabel(sPriceComp);
    shelfPrice.addComponent<Transform>(Vector2D(shelfX + 45.0f, shelfY + 28.0f), 0.0f, 1.0f);
    shelfPrice.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&shelfPrice);
    session.shelfPriceLabel = &shelfPrice;

    // Shelf Buy Button
    auto& sBtnEnt = world.createEntity();
    float sBtnW = 64.0f, sBtnH = 24.0f;
    float sBtnX = shelfX + 160.0f;
    auto& sBtnTransform = sBtnEnt.addComponent<Transform>(Vector2D(sBtnX, shelfY + 6.0f), 0.0f, 1.0f);
    sBtnEnt.addComponent<Sprite>(btnTex, SDL_FRect{0,0,32,16}, SDL_FRect{sBtnX, shelfY + 6.0f, sBtnW, sBtnH}, RenderLayer::UI, false);
    sBtnEnt.addComponent<Collider>("ui", SDL_FRect{sBtnX, shelfY + 6.0f, sBtnW, sBtnH}).enabled = false;

    // Add Clickable (Logic will be wired in updateOrderUI)
    auto& sClick = sBtnEnt.addComponent<Clickable>();
    sClick.onPressed = [&sBtnTransform]{ sBtnTransform.scale = 0.9f; };
    sClick.onCancel = [&sBtnTransform]{ sBtnTransform.scale = 1.0f; };
    sClick.onReleased = [&sBtnTransform]{ sBtnTransform.scale = 1.0f; }; // To prevent crash

    sBtnEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&sBtnEnt);
    session.shelfBuyBtn = &sBtnEnt;

    // --- CONTINUE BUTTON ---
    auto& contBtn = world.createEntity();
    float cBtnW = 160.0f, cBtnH = 40.0f;
    float cBtnX = baseX + menuWidth - cBtnW - 20.0f;
    float cBtnY = baseY + menuHeight - cBtnH - 16.0f;
    auto& cTransform = contBtn.addComponent<Transform>(Vector2D(cBtnX, cBtnY), 0.0f, 1.0f);
    contBtn.addComponent<Sprite>(btnTex, SDL_FRect{0,33,64,16},
                                  SDL_FRect{cBtnX,cBtnY,cBtnW,cBtnH}, RenderLayer::UI, false);
    contBtn.addComponent<Collider>("ui", SDL_FRect{cBtnX,cBtnY,cBtnW,cBtnH}).enabled = false;

    auto& cClick = contBtn.addComponent<Clickable>();
    cClick.onPressed = [&cTransform]{ cTransform.scale = 0.9f; };
    cClick.onCancel = [&cTransform]{ cTransform.scale = 1.0f; };
    cClick.onReleased = [this, &cTransform]() {
        cTransform.scale = 1.0f;
        bool close = false;
        toggleSettingsOverlayVisibility(*UIOrderScreen, &close);

        if (UIHud) {
            bool showHud = true;
            toggleSettingsOverlayVisibility(*UIHud, &showHud);
        }

        auto& s = UIOrderScreen->getComponent<OrderSession>();
        if (s.onContinue) s.onContinue();
    };
    contBtn.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&contBtn);

    UIOrderScreen = &mainOverlay;
    return mainOverlay;
}

Entity& Scene::updateOrderUI(std::vector<ItemDef> availableItems,
                              Wallet& wallet, Inventory& inv,
                              std::function<void()> onContinue,
                              std::function<void()> onBuyShelf
                              ) {
    if (!UIOrderScreen) return *UIOrderScreen;

    auto& session = UIOrderScreen->getComponent<OrderSession>();
    session.walletRef = &wallet.balance;       // assumes Wallet has int amount
    session.inventoryRef = &inv;
    session.onContinue = onContinue;
    session.currentItems = availableItems;

    // Update wallet label
    if (session.walletLabelRef) {
        auto& lbl = session.walletLabelRef->getComponent<Label>();
        lbl.text = "Wallet: " + std::to_string(wallet.balance) + "G";
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);
    }

    // Populate slots
    for (int i = 0; i < (int)session.slots.size(); ++i) {
        auto& slot = session.slots[i];
        bool hasItem = i < (int)availableItems.size();

        // Icon
        slot.icon->getComponent<Sprite>().visible = hasItem;
        if (hasItem) slot.icon->getComponent<Sprite>().src = availableItems[i].src;

        // Name label
        auto& nameLbl = slot.nameLabel->getComponent<Label>();
        nameLbl.visible = hasItem;
        if (hasItem) {
            // Find how many the player holds
            int held = 0;
            for (auto& entry : inv.items) {
                if (entry.item.id == availableItems[i].id) {
                    held = entry.quantity;
                    break;
                }
            }
            nameLbl.text = availableItems[i].name;
            // Append stock count in a muted way
            nameLbl.text += " (" + std::to_string(held) + ")";
            nameLbl.dirty = true;
            TextureManager::updateLabel(nameLbl);
        }

        // Price label
        auto& priceLbl = slot.priceLabel->getComponent<Label>();
        priceLbl.visible = hasItem;
        if (hasItem) {
            float trendMod = world.getMarketTrendSystem().getModifier(availableItems[i]);
            int trendPrice = static_cast<int>(availableItems[i].basePrice * trendMod);
            bool canAfford = wallet.balance >= trendPrice;

            priceLbl.text = std::to_string(trendPrice) + "G";

            if (!canAfford) {
                priceLbl.color = {180, 50, 50, 255};
            } else if (trendMod > 1.0f) {
                priceLbl.color = {76, 175, 80, 255};
            } else if (trendMod < 1.0f) {
                priceLbl.color = {211, 47, 47, 255};
            } else {
                priceLbl.color = {0, 0, 0, 255};
            }

            // Also use trendPrice for affordability check, not basePrice
            bool canAffordTrend = wallet.balance >= trendPrice;
            slot.buyBtn->getComponent<Collider>().enabled = canAffordTrend;
            priceLbl.dirty = true;
            TextureManager::updateLabel(priceLbl);
        }


        slot.buyBtn->getComponent<Sprite>().visible = hasItem;

        // Wire the buy click with live item data
        if (hasItem) {
            ItemDef item = availableItems[i];
            auto& bClick = slot.buyBtn->getComponent<Clickable>();
            bClick.onReleased = [this, item, &wallet, &inv]() {
                float trendMod = world.getMarketTrendSystem().getModifier(item);
                int trendPrice = static_cast<int>(item.basePrice * trendMod);

                if (wallet.balance < trendPrice) return; // was checking basePrice

                // Deduct wallet
                wallet.balance -= trendPrice;
                wallet.dailyExpenses += trendPrice;

                // Add to inventory
                inv.addItem(item, 1);


                // Refresh wallet label
                auto& s = UIOrderScreen->getComponent<OrderSession>();
                if (s.walletLabelRef) {
                    auto& lbl = s.walletLabelRef->getComponent<Label>();
                    lbl.text = "Wallet: " + std::to_string(wallet.balance) + "G";
                    lbl.dirty = true;
                    TextureManager::updateLabel(lbl);
                }

                //Update x1 ->x2 item count
                for (int j = 0; j < (int)s.slots.size() && j < (int)s.currentItems.size(); ++j) {
                    int heldNow = 0;
                    for (auto& entry : inv.items) {
                        if (entry.item.id == s.currentItems[j].id) { heldNow = entry.quantity; break; }
                    }
                    auto& nLbl = s.slots[j].nameLabel->getComponent<Label>();
                    nLbl.text = s.currentItems[j].name + " (" + std::to_string(heldNow) + ")";
                    nLbl.dirty = true;
                    TextureManager::updateLabel(nLbl);
                }

                // Refresh all buy button states (some may now be unaffordable)
                for (int j = 0; j < (int)s.slots.size() && j < (int)s.currentItems.size(); ++j) {
                    bool afford = wallet.balance >= s.currentItems[j].basePrice;
                    s.slots[j].buyBtn->getComponent<Collider>().enabled = afford;

                    auto& pLbl = s.slots[j].priceLabel->getComponent<Label>();

                    // Recalculate trend color for each item
                    float trendMod = world.getMarketTrendSystem().getModifier(s.currentItems[j]);
                    int trendPrice = static_cast<int>(s.currentItems[j].basePrice * trendMod);
                    pLbl.text = std::to_string(trendPrice) + "G";

                    if (!afford) {
                        pLbl.color = {180, 50, 50, 255};   // can't afford — red override
                    } else if (trendMod > 1.0f) {
                        pLbl.color = {76, 175, 80, 255};   // demand up — green
                    } else if (trendMod < 1.0f) {
                        pLbl.color = {211, 47, 47, 255};   // demand down — muted red
                    } else {
                        pLbl.color = {0, 0, 0, 255};       // normal
                    }

                    pLbl.dirty = true;
                    TextureManager::updateLabel(pLbl);
                }

                std::cout << "Bought " << item.name << " for " << item.basePrice
                          << "G. Wallet: " << wallet.balance << "G\n";
            };
        }
        else {
            slot.icon->getComponent<Sprite>().visible = false;
            slot.nameLabel->getComponent<Label>().visible = false;
            slot.priceLabel->getComponent<Label>().visible = false;
            slot.buyBtn->getComponent<Sprite>().visible = false;
            slot.buyBtn->getComponent<Collider>().enabled = false;
            if (slot.buyLabel)
                slot.buyLabel->getComponent<Label>().visible = false;
        }
    }
    if (session.shelfBuyBtn && session.shelfPriceLabel) {
        int cost = session.getShelfPrice
                       ? session.getShelfPrice(Game::gameState.displayCasesUnlocked)
                       : 500;
        session.currentShelfPrice = cost;
        bool maxedOut = Game::gameState.displayCasesUnlocked >= 15;
        bool canAfford = !maxedOut && wallet.balance >= cost;

        auto &pLbl = session.shelfPriceLabel->getComponent<Label>();
        pLbl.text = maxedOut ? "Maxed Out" : (std::to_string(cost) + "G");
        pLbl.color = canAfford ? SDL_Color{0, 0, 0, 255} : SDL_Color{180, 50, 50, 255};
        pLbl.dirty = true;
        TextureManager::updateLabel(pLbl);

        session.shelfBuyBtn->getComponent<Collider>().enabled = canAfford;
        session.onBuyShelf = onBuyShelf;

        auto &sClick = session.shelfBuyBtn->getComponent<Clickable>();
        auto &sBtnTransform = session.shelfBuyBtn->getComponent<Transform>();
        sClick.onReleased = [this, &sBtnTransform]() {
            auto& s = UIOrderScreen->getComponent<OrderSession>();
            sBtnTransform.scale = 1.0f;

            if (s.onBuyShelf) s.onBuyShelf(); // deducts wallet internally

            // Refresh wallet label
            if (s.walletLabelRef && s.walletRef) {
                auto& lbl = s.walletLabelRef->getComponent<Label>();
                lbl.text = "Wallet: " + std::to_string(*s.walletRef) + "G";
                lbl.dirty = true;
                TextureManager::updateLabel(lbl);
            }

            // Re-evaluate shelf button for NEXT purchase
            bool nowMaxed = Game::gameState.displayCasesUnlocked >= 15;
            bool canStillAfford = !nowMaxed && s.walletRef && (*s.walletRef >= s.currentShelfPrice);

            s.shelfBuyBtn->getComponent<Collider>().enabled = canStillAfford;

            auto& pLbl = s.shelfPriceLabel->getComponent<Label>();
            pLbl.text = nowMaxed ? "Maxed Out" : (std::to_string(s.currentShelfPrice) + "G");
            pLbl.color = canStillAfford ? SDL_Color{0,0,0,255} : SDL_Color{180,50,50,255};
            pLbl.dirty = true;
            TextureManager::updateLabel(pLbl);
        };
    }

    bool forceOpen = true;
    toggleSettingsOverlayVisibility(*UIOrderScreen, &forceOpen);

    if (UIHud) {
        bool hideHud = false;
        toggleSettingsOverlayVisibility(*UIHud, &hideHud);
    }

    //apply hidden on items you can't buy yet
    for (int i = 0; i < (int)session.slots.size(); ++i) {
        auto& slot = session.slots[i];
        bool hasItem = i < (int)availableItems.size();

        slot.icon->getComponent<Sprite>().visible = hasItem;
        slot.nameLabel->getComponent<Label>().visible = hasItem;
        slot.priceLabel->getComponent<Label>().visible = hasItem;
        slot.buyBtn->getComponent<Sprite>().visible = hasItem;

        if (slot.buyLabel)
            slot.buyLabel->getComponent<Label>().visible = hasItem;

        if (!hasItem) {
            slot.buyBtn->getComponent<Collider>().enabled = false;
        }
    }
    return *UIOrderScreen;
}

//Dialogue UI
Entity& Scene::createDialogueUI(int windowWidth, int windowHeight) {
    auto& overlay = world.createEntity();

    // Bottom third of screen, like your mockup
    float boxH = windowHeight * 0.30f;
    float boxW = windowWidth * 0.85f;
    float boxX = (windowWidth - boxW) / 2.0f;
    float boxY = windowHeight - boxH - 20.0f;

    SDL_Texture* bgTex = TextureManager::load("../asset/ui/settings.jpg");
    SDL_FRect src{0, 0, boxW, boxH};
    SDL_FRect dst{boxX, boxY, boxW, boxH};

    overlay.addComponent<Transform>(Vector2D(boxX, boxY), 0.0f, 1.0f);
    overlay.addComponent<Sprite>(bgTex, src, dst, RenderLayer::UI, false);
    overlay.addComponent<Children>();

    auto& session = overlay.addComponent<DialogueSession>();

    // --- MESSAGE LABEL ---
    auto& msgEnt = world.createEntity();
    Label msgData = {
        "...", AssetManager::getFont("arial"),
        {0, 0, 0, 255}, LabelType::Static, "dialogueMsg"
    };
    auto& msgComp = msgEnt.addComponent<Label>(msgData);
    msgComp.dirty = true;
    msgComp.visible = false;
    TextureManager::updateLabel(msgComp);

    msgEnt.addComponent<Transform>(Vector2D(boxX + 20.0f, boxY + 20.0f), 0.0f, 1.0f);
    msgEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&msgEnt);
    session.messageLabelRef = &msgEnt;

    // --- CONFIRM BUTTON ---
    auto& btnEnt = world.createEntity();
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
    clickable.onReleased = [this, &btnTransform]() {
        btnTransform.scale = 1.0f;
        bool close = false;
        toggleSettingsOverlayVisibility(*UIDialogue, &close);

        if (UIHud) {
            bool showHud = true;
            toggleSettingsOverlayVisibility(*UIHud, &showHud);
        }

        auto& s = UIDialogue->getComponent<DialogueSession>();
        if (s.onConfirm) s.onConfirm();
        s.onConfirm = nullptr; // clear so it doesn't fire twice
    };

    btnEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&btnEnt);
    session.confirmBtnRef = &btnEnt;

    UIDialogue = &overlay;
    return overlay;
}

Entity& Scene::updateDialogueUI(const std::string& message, std::function<void()> onConfirm) {
    if (!UIDialogue) return *UIDialogue;

    auto& session = UIDialogue->getComponent<DialogueSession>();
    session.onConfirm = onConfirm;

    // Update message text
    if (session.messageLabelRef) {
        auto& lbl = session.messageLabelRef->getComponent<Label>();
        lbl.text = message;
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);
    }

    bool forceOpen = true;
    toggleSettingsOverlayVisibility(*UIDialogue, &forceOpen);

    if (UIHud) {
        bool hideHud = false;
        toggleSettingsOverlayVisibility(*UIHud, &hideHud);
    }

    return *UIDialogue;
}

//HUD
Entity& Scene::createHUD(int windowWidth, int windowHeight) {
    auto& overlay = world.createEntity();

    // Top-right bar — thin strip like your mockup
    float barW = 280.0f;
    float barH = 40.0f;
    float barX = windowWidth - barW - 10.0f;
    float barY = 10.0f;

    SDL_Texture* bgTex = TextureManager::load("../asset/ui/settings.jpg");
    SDL_FRect src{0, 0, barW, barH};
    SDL_FRect dst{barX, barY, barW, barH};

    overlay.addComponent<Transform>(Vector2D(barX, barY), 0.0f, 1.0f);
    overlay.addComponent<Sprite>(bgTex, src, dst, RenderLayer::UI, true);
    overlay.addComponent<Children>();

    auto& session = overlay.addComponent<HUDSession>();

    // --- DAY LABEL (left side of bar) ---
    auto& dayEnt = world.createEntity();
    Label dayData = {
        "Day 1", AssetManager::getFont("arial"),
        {0, 0, 0, 255}, LabelType::Static, "hudDay"
    };
    auto& dayComp = dayEnt.addComponent<Label>(dayData);
    dayComp.dirty = true;
    dayComp.visible = true;
    TextureManager::updateLabel(dayComp);
    dayEnt.addComponent<Transform>(
        Vector2D(barX + 15.0f, barY + (barH / 2) - (dayComp.dst.h / 2)),
        0.0f, 1.0f);
    dayEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&dayEnt);
    session.dayLabelRef = &dayEnt;

    // --- PHASE LABEL (right side of bar) ---
    auto& phaseEnt = world.createEntity();
    Label phaseData = {
        "Morning", AssetManager::getFont("arial"),
        {0, 0, 0, 255}, LabelType::Static, "hudPhase"
    };
    auto& phaseComp = phaseEnt.addComponent<Label>(phaseData);
    phaseComp.dirty = true;
    phaseComp.visible = true;
    TextureManager::updateLabel(phaseComp);
    phaseEnt.addComponent<Transform>(
        Vector2D(barX + barW - phaseComp.dst.w - 15.0f,
                 barY + (barH / 2) - (phaseComp.dst.h / 2)),
        0.0f, 1.0f);
    phaseEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&phaseEnt);
    session.phaseLabelRef = &phaseEnt;

    // --- WALLET ICON (bottom right, coin sprite) ---
    float iconSize = 40.0f;
    float iconX = windowWidth - iconSize - 10.0f;
    float iconY = windowHeight - iconSize - 10.0f;

    auto& iconEnt = world.createEntity();
    SDL_Texture* coinTex = TextureManager::load("../asset/ui/Gold.png");
    iconEnt.addComponent<Transform>(Vector2D(iconX, iconY), 0.0f, 1.0f);
    iconEnt.addComponent<Sprite>(
        coinTex,
        SDL_FRect{0, 0, 256, 256},
        SDL_FRect{0, 0, iconSize, iconSize},
        RenderLayer::UI, true);
    iconEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&iconEnt);
    session.walletIconRef = &iconEnt;

    // --- WALLET LABEL (left of coin icon) ---
    auto& walletEnt = world.createEntity();
    Label walletData = {
        "1000G", AssetManager::getFont("arial"),
        {255, 215, 0, 255}, LabelType::Static, "hudWallet"
    };
    auto& walletComp = walletEnt.addComponent<Label>(walletData);
    walletComp.dirty = true;
    walletComp.visible = true;
    TextureManager::updateLabel(walletComp);
    walletEnt.addComponent<Transform>(
    Vector2D(iconX - walletComp.dst.w - 8.0f,
             iconY + (iconSize / 2) - (walletComp.dst.h / 2)),
    0.0f, 1.0f);
    walletEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&walletEnt);
    session.walletLabelRef = &walletEnt;

    UIHud = &overlay;
    return overlay;
}

void Scene::updateHUD(const Wallet& wallet, const DayCycle& dayCycle) {
    if (!UIHud) return;
    auto& session = UIHud->getComponent<HUDSession>();

    // Find store entity for wallet + daycycle
    // (You can also pass these in as params if preferred)

    // Update day label
    if (session.dayLabelRef) {
        auto& lbl = session.dayLabelRef->getComponent<Label>();
        lbl.text = "Day " + std::to_string(dayCycle.date);
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);
    }

    // Update phase label
    if (session.phaseLabelRef) {
        auto& lbl = session.phaseLabelRef->getComponent<Label>();
        switch (dayCycle.currentPhase) {
            case DayPhase::Morning:      lbl.text = "Morning"; break;
            case DayPhase::ShopOpen:     lbl.text = "Open";    break;
            case DayPhase::Evening:      lbl.text = "Evening"; break;
            case DayPhase::FadeToBlack:  lbl.text = "...";     break;
            case DayPhase::GameOver:     lbl.text = "Game Over"; break;
        }
        // Right-align phase label inside the bar
        auto& barSprite = UIHud->getComponent<Sprite>();
        auto& barTransform = UIHud->getComponent<Transform>();
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);
        auto& t = session.phaseLabelRef->getComponent<Transform>();
        t.position.x = barTransform.position.x + barSprite.dst.w - lbl.dst.w - 15.0f;
    }

    // Update wallet label and re-right-align it
    if (session.walletLabelRef && session.walletIconRef) {
        auto& lbl = session.walletLabelRef->getComponent<Label>();
        lbl.text = std::to_string(wallet.balance) + "G";
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);

        // Use transform position, not sprite dst
        auto& iconT = session.walletIconRef->getComponent<Transform>();
        auto& lblT = session.walletLabelRef->getComponent<Transform>();
        lblT.position.x = iconT.position.x - lbl.dst.w - 8.0f;
        lblT.position.y = iconT.position.y + (40.0f / 2) - (lbl.dst.h / 2);
    }
}

//UI Utils
void Scene::toggleSettingsOverlayVisibility(Entity &overlay, bool *forceState) {
    // 1. Determine the new state
    // If forceState is null, we just flip the current visibility.
    // If we are recursing, we pass the parent's state down.
    bool newVisibility = forceState ? *forceState : !overlay.getComponent<Sprite>().visible;

    // 2. Toggle the Parent's own components
    if (overlay.hasComponent<Sprite>()) {
        overlay.getComponent<Sprite>().visible = newVisibility;
    }
    if (overlay.hasComponent<Label>()) {
        overlay.getComponent<Label>().visible = newVisibility;
    }
    if (overlay.hasComponent<Collider>()) {
        overlay.getComponent<Collider>().enabled = newVisibility;
    }

    // 3. CASCADE: Recurse through all children
    if (overlay.hasComponent<Children>()) {
        auto &c = overlay.getComponent<Children>();
        for (auto &child: c.children) {
            if (child) {
                // We pass the newVisibility down to the child
                toggleSettingsOverlayVisibility(*child, &newVisibility);
            }
        }
    }
}

Entity &Scene::createPlayerPosLabel() {
    auto &playerPosLabel(world.createEntity());
    Label label = {
        "Text String",
        AssetManager::getFont("arial"),
        {255, 255, 255, 255},
        LabelType::PlayerPosition,
        "playerPos"
    };
    TextureManager::loadLabel(label);
    playerPosLabel.addComponent<Label>(label);
    playerPosLabel.addComponent<Transform>(Vector2D(10, 10), 0.0f, 1.0f);
    return playerPosLabel;
}
