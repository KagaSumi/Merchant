//
// Created by Curry on 2026-02-25.
//

#include "../scene/Scene.h"
#include "../manager/AssetManager.h"
#include "Game.h"

Scene::Scene(SceneType sceneType, const char *sceneName, const char *mapPath, int windowWidth,
             int windowHeight) : name(sceneName), type(sceneType) {
    if (sceneType == SceneType::MainMenu) {
        initMainMenu(windowWidth, windowHeight);
        return;
    }
    initGameplay(mapPath, windowWidth, windowHeight);
}


void Scene::initMainMenu(int windowWidth, int windowHeight) {
    //camera
    auto &cam = world.createEntity();
    cam.addComponent<Camera>();
    //menu
    auto &menu(world.createEntity());
    auto menuTransform = menu.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);

    SDL_Texture *texture = TextureManager::load("../asset/menu.png");
    SDL_FRect menuSrc{0, 0, (float) windowWidth, (float) windowHeight};
    SDL_FRect menuDst{menuTransform.position.x, menuTransform.position.y, menuSrc.w, menuSrc.h};
    menu.addComponent<Sprite>(texture, menuSrc, menuDst);

    auto &settingsOverlay = createSettingsOverlay(windowWidth, windowHeight);
    // createCogButton(windowWidth, windowHeight, settingsOverlay);
    ItemDef test;
    test.basePrice = 100;
    test.id = 1;
    test.name ="Longsword";
    test.src = {0,0,32,32};
    test.requiredReputation = 1;

    auto &haggleOverlay = createHaggleUI(windowWidth, windowHeight, test);
    //createHaggleUIComponents(haggleOverlay,test.basePrice);
    createCogButton(windowWidth,windowHeight,haggleOverlay);
}

void Scene::initGameplay(const char *mapPath, int windowWidth, int windowHeight) {
    //load map
    SDL_Texture *tilemapTex = TextureManager::load("../asset/SpriteSheet.png");
    world.getMap().load(mapPath, tilemapTex);
    SDL_Texture *itemsTex = TextureManager::load("../asset/items.png");
    world.getItems().load("../asset/items.xml");

    for (auto &collider: world.getMap().colliders) {
        auto &e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, 1.0f);
        auto &c = e.addComponent<Collider>("wall");

        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;
    }

    //PathFinding:
    //Find Non-Walkable layer
    PathfindingSystem::InitMap(25, 19, 32, world.getMap().AIWalkable);

    //Customer AI Setup Door and Register
    SDL_Point door = world.getMap().Door;
    SDL_Point regi = world.getMap().Register;

    world.getCustomerAISystem().setDoor(door);
    world.getCustomerAISystem().setRegister(regi);


    //Create Camera:
    auto &cam = world.createEntity();
    SDL_FRect camView{};
    camView.w = windowWidth;
    camView.h = windowHeight;
    cam.addComponent<Camera>(camView, static_cast<float>(world.getMap().width * 32),
                             static_cast<float>(world.getMap().height * 32));

    //Store:
    auto &store(world.createEntity());
    store.addComponent<ShopReputation>(Game::gameState.shopReputation);
    store.addComponent<Wallet>(Game::gameState.Wallet);
    store.addComponent<Debt>(Game::gameState.Debt);
    auto &dayCycle = store.addComponent<DayCycle>();

    //Create Player
    auto &player(world.createEntity());
    auto &playerTransform = player.addComponent<Transform>(Vector2D(12 * 32, 14 * 32), 1.0f);
    player.addComponent<Velocity>(Vector2D(0, 0), 120.0f);

    Animation anim = AssetManager::getAnimation("customer");
    player.addComponent<Animation>(anim);

    SDL_Texture *tex =TextureManager::load("../asset/animations/CustomerA.png");
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndicies[0];
    SDL_FRect playerDst{-16,-16, 64, 64};
    player.addComponent<Sprite>(tex, playerSrc, playerDst);

    auto &playerCollider = player.addComponent<Collider>("player");
    playerCollider.rect.w = 16.0f; // Half the width of the tile
    playerCollider.rect.h = 12.0f; // Roughly the bottom third of the sprite
    playerCollider.offsetX = 8.0f; // (32 - 16) / 2 = centers the box horizontally
    playerCollider.offsetY = 20.0f; // Pushes the box down so it only covers the legs/feet

    player.addComponent<PlayerTag>();


    //Customers:
    std::vector<SDL_Texture *> customerTextures = {
        //TextureManager::load("../asset/animations/CustomerA.png"),
        TextureManager::load("../asset/animations/CustomerF.png"),
        TextureManager::load("../asset/animations/CustomerM.png")
    };
    int customerIndexCount = 0; // Index on which Texture ^

    auto &spawner(world.createEntity());
    Transform t = spawner.addComponent<Transform>(Vector2D(door.x * 32, door.y * 32), 1.0f);
    spawner.addComponent<Spawner>([this,t,customerTextures,customerIndexCount]() mutable {
        auto &e = world.createDeferredEntity();
        e.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 1.0f);
        e.addComponent<Velocity>(Vector2D(0, 0), 100.0f);
        e.addComponent<CustomerAI>(); // The AI takes over from here!
        e.addComponent<Customer>();

        Animation anim = AssetManager::getAnimation("customer");
        e.addComponent<Animation>(anim);

        // Your texture cycling logic here is excellent.
        // Because the lambda is 'mutable', customerIndexCount will correctly increment across spawns.
        SDL_Texture *tex = customerTextures[customerIndexCount++ % customerTextures.size()];
        SDL_FRect src = anim.clips[anim.currentClip].frameIndicies[0];
        SDL_FRect dst{- 16.0f, - 16.0f, 64, 64};
        e.addComponent<Sprite>(tex, src, dst);
    });
    //Display Case: (Test)
    SDL_FRect src = {64, 32, 32, 32};
    SDL_FRect dst = {0, 0, 32, 32};
    // createDisplaycase(Vector2D(21 * 32, 15 * 32), tilemapTex, {64, 32, 32, 32}, {0, 0, 32, 32});
    // createDisplaycase(Vector2D(22 * 32, 15 * 32), tilemapTex, {64, 32, 32, 32}, {0, 0, 32, 32});

    //Cash Register
     auto &cashRegister = world.createEntity();
     auto& cashTransform = cashRegister.addComponent<Transform>(Vector2D(regi.x * 32, (regi.y + 2) * 32));
     auto &c = cashRegister.addComponent<Collider>("wall");
     c.rect = { regi.x*32.0f - 64.0f, (regi.y+2)*32 + 32.0f, 160.0f, 62.0f };
     c.offsetX = -64.0f;
     c.offsetY = -14.0f;
     cashRegister.addComponent<Interaction>([&dayCycle]() {
         if (dayCycle.currentPhase == DayPhase::Morning) {
             std::cout << "Start the day ..." << std::endl;
             dayCycle.phaseSwapReady = true;
         }
     });
    SDL_FRect regSrc = { 160.0f, 256.0f, 160.0f, 96.0f };
    SDL_FRect regDst = {2*-32.0f , -32.0f, 160.0f, 96.0f };
    cashRegister.addComponent<Sprite>(tilemapTex, regSrc, regDst);

    // 5. Interaction
    cashRegister.addComponent<Interaction>([&dayCycle]() {
        if (dayCycle.currentPhase == DayPhase::Morning) {
            std::cout << "Start the day ..." << std::endl;
            dayCycle.phaseSwapReady = true;
        }
    });


    //Message Board
    auto &bulletinBoard = world.createEntity();
    bulletinBoard.addComponent<Transform>(Vector2D(17 * 32, 5 * 32), 1.0f);
    bulletinBoard.addComponent<Interaction>([]() {
        std::cout << "Display Market Conditions ..." << std::endl;
    });


    //add scene state
    auto &state(world.createEntity());
    state.addComponent<SceneState>();

    //Add Label
    createPlayerPosLabel();
}

Entity &Scene::createDisplaycase(Vector2D location, SDL_Texture *texture, SDL_FRect src, SDL_FRect dst) {
    auto &displayCase(world.createEntity());
    displayCase.addComponent<Transform>(location, 0.0f, 1.0f);
    displayCase.addComponent<DisplayStand>();
    auto &c = displayCase.addComponent<Collider>("wall");
    c.rect.w = 32;
    c.rect.h = 32;
    c.rect.x = location.x;
    c.rect.y = location.y;
    displayCase.addComponent<Sprite>(texture, src, dst);
    displayCase.addComponent<Interaction>([&displayCase]() {
        auto &dc = displayCase.getComponent<DisplayStand>();

        if (dc.quantity > 0 && dc.item.id != -1) {
            std::cout << "Display case has an " << dc.item.name << "! Opening modification UI...\n";
        } else {
            std::cout << "Display case is empty. Opening inventory UI to place item...\n";
        }
    });
    return displayCase;
}

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
Entity& Scene::createItemHaggleDisplay(Entity& parent) {
    auto& session = parent.getComponent<HaggleSession>();
    auto& parentS = parent.getComponent<Sprite>();
    auto& parentT = parent.getComponent<Transform>();

    float parentCenterX = parentS.dst.w / 2.0f;

    auto& subOverlay = world.createEntity();
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

    SDL_Texture* subTex = TextureManager::load("../asset/ui/UI-Sub.png");
    SDL_FRect subSrc{0, 0, 21.0f, 21.0f};

    // 3. ZERO OUT the local x/y offset so the renderer doesn't push it twice
    SDL_FRect subDst{0.0f, 0.0f, subWidth, subHeight};

    subOverlay.addComponent<Sprite>(subTex, subSrc, subDst, RenderLayer::UI, false);
    subOverlay.addComponent<Children>();

    // --- ITEM NAME ---
    auto& nameLabel = world.createEntity();
    Label nameData = {session.currentItem.name, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "itemName"};
    auto& nameLabelComp = nameLabel.addComponent<Label>(nameData);
    nameLabelComp.visible = false;
    nameLabelComp.dirty = true;
    TextureManager::updateLabel(nameLabelComp);

    // Your nameLabel math was already perfectly centered!
    nameLabel.addComponent<Transform>(Vector2D(parentT.position.x + parentCenterX - (nameLabelComp.dst.w / 2.0f), parentT.position.y + 40), 0.0f, 1.0f);
    nameLabel.addComponent<Parent>(&subOverlay);
    subOverlay.getComponent<Children>().children.push_back(&nameLabel);

    session.itemNameRef = &nameLabel; // Cache reference

    // --- ITEM ICON ---
    auto& itemIcon = world.createEntity();
    float iconSize = 64.0f;
    itemIcon.addComponent<Transform>(Vector2D(parentT.position.x + parentCenterX - (iconSize / 2.0f), parentT.position.y + 80), 0.0f, 1.0f);

    SDL_Texture* itemsTex = TextureManager::get("items");
    if (!itemsTex) itemsTex = TextureManager::load("../asset/items.png");

    SDL_FRect itemDst{0, 0, iconSize, iconSize};
    itemIcon.addComponent<Sprite>(itemsTex, session.currentItem.src, itemDst, RenderLayer::UI, false);
    itemIcon.addComponent<Parent>(&subOverlay);
    subOverlay.getComponent<Children>().children.push_back(&itemIcon);

    session.itemIconRef = &itemIcon; // Cache reference

    // --- BASE VALUE ---
    auto& baseValLabel = world.createEntity();
    std::string baseValText = "Base Value: " + std::to_string(session.currentItem.basePrice) + "G";
    Label valData = {baseValText, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "itemBaseVal"};
    auto& valLabelComp = baseValLabel.addComponent<Label>(valData);
    valLabelComp.visible = false;
    valLabelComp.dirty = true;
    TextureManager::updateLabel(valLabelComp);

    baseValLabel.addComponent<Transform>(Vector2D(parentT.position.x + parentCenterX - (valLabelComp.dst.w / 2.0f), parentT.position.y + 155), 0.0f, 1.0f);
    baseValLabel.addComponent<Parent>(&subOverlay);
    subOverlay.getComponent<Children>().children.push_back(&baseValLabel);

    session.itemBaseValRef = &baseValLabel; // Cache reference

    parent.getComponent<Children>().children.push_back(&subOverlay);

    return subOverlay;
}

Entity& Scene::createHaggleUI(int windowWidth, int windowHeight, ItemDef& item) {
    auto& mainOverlay = createBaseMenuOverlay(windowWidth, windowHeight);
    mainOverlay.getComponent<Sprite>().visible = false;

    if (!mainOverlay.hasComponent<Children>()) {
        mainOverlay.addComponent<Children>();
    }

    // 1. Initialize the Shared State
    auto& session = mainOverlay.addComponent<HaggleSession>();
    session.currentItem = item;

    // Set initial digits based on base price
    int tempPrice = item.basePrice;
    for (int i = 4; i >= 0; --i) {
        session.digits[i] = tempPrice % 10;
        tempPrice /= 10;
    }

    // 2. Build the Modular Parts (passing the mainOverlay)
    createItemHaggleDisplay(mainOverlay);
    createPriceSelection(windowWidth, windowHeight, mainOverlay);
    createHaggleButton(windowWidth, windowHeight, mainOverlay);

    // Set your global pointer so updateHaggleUI can find it later
    UIMenu = &mainOverlay;

    return mainOverlay;
}

Entity& Scene::createPriceSelection(int windowWidth, int windowHeight, Entity& overlay) {
    auto& session = overlay.getComponent<HaggleSession>();
    auto& overlayTransform = overlay.getComponent<Transform>();
    auto& overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

    // --- SIZING & SPACING MATH ---
    // Increase button size (1.5x scale of the original 32x16)
    float btnWidth = 48.0f;
    float btnHeight = 24.0f;

    // Increase gap between columns to fit the larger buttons
    float spacingX = 56.0f;

    // Calculate the X start point so all 5 columns perfectly center
    // 5 columns means we go -2, -1, 0, 1, 2 widths from the center line.
    float startX = centerLineX - (2.0f * spacingX);

    // Y Offsets
    float startY = baseY + 240.0f; // Top of the UP buttons
    float downBtnY = startY + 80.0f; // Top of the DOWN buttons (increases vertical gap for digits)
    float textCenterY = startY + btnHeight + ((downBtnY - (startY + btnHeight)) / 2.0f); // Perfect middle for text


    // --- 1. TITLE LABEL ("How much should I sell it for?") ---
    auto& titleLabelEntity = world.createEntity();
    Label tLabel = {"How much should I sell it for?", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "haggleTitle"};
    auto& tLabelComp = titleLabelEntity.addComponent<Label>(tLabel);
    tLabelComp.dirty = true;
    tLabelComp.visible = false;
    TextureManager::updateLabel(tLabelComp);

    // Centered above the buttons (40px up)
    titleLabelEntity.addComponent<Transform>(Vector2D(centerLineX - (tLabelComp.dst.w / 2.0f), startY - 40.0f), 0.0f, 1.0f);
    titleLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&titleLabelEntity);


    // --- 2. BASE PRICE LABEL ("Base Price:") ---
    auto& basePriceLabelEntity = world.createEntity();
    Label bpLabel = {"Base Price:", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "haggleBasePriceTxt"};
    auto& bpLabelComp = basePriceLabelEntity.addComponent<Label>(bpLabel);
    bpLabelComp.dirty = true;
    bpLabelComp.visible = false;
    TextureManager::updateLabel(bpLabelComp);

    // Aligned to the left, below the down buttons
    basePriceLabelEntity.addComponent<Transform>(Vector2D(startX - 20.0f, downBtnY + 40.0f), 0.0f, 1.0f);
    basePriceLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&basePriceLabelEntity);


    // --- 3. THE PERCENTAGE LABEL ---
    auto& percentLabelEntity = world.createEntity();
    Label pLabel = {"100%", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "hagglePercent"};
    auto& pLabelComp = percentLabelEntity.addComponent<Label>(pLabel);
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
    auto& currencyLabelEntity = world.createEntity();
    Label gLabel = {"G", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "haggleCurrencyTxt"};
    auto& gLabelComp = currencyLabelEntity.addComponent<Label>(gLabel);
    gLabelComp.dirty = true;
    gLabelComp.visible = false;
    TextureManager::updateLabel(gLabelComp);

    // Placed to the right of the last column, vertically centered with the digits
    currencyLabelEntity.addComponent<Transform>(Vector2D(startX + (4 * spacingX) + btnWidth + 15.0f, textCenterY - (gLabelComp.dst.h / 2.0f)), 0.0f, 1.0f);
    currencyLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&currencyLabelEntity);


    // --- 5. GENERATE THE 5 COLUMNS ---
    for (int i = 0; i < 5; ++i) {
        float colX = startX + (i * spacingX);
        float columnCenter = colX + (btnWidth / 2.0f);

        // --- DIGIT LABEL ---
        auto& digitEntity = world.createEntity();
        Label dLabel = {std::to_string(session.digits[i]), AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "digit_" + std::to_string(i)};
        auto& dLabelComp = digitEntity.addComponent<Label>(dLabel);
        dLabelComp.visible = false;
        dLabelComp.dirty = true;
        TextureManager::updateLabel(dLabelComp);

        float centeredX = columnCenter - (dLabelComp.dst.w / 2.0f);
        float centeredY = textCenterY - (dLabelComp.dst.h / 2.0f);

        digitEntity.addComponent<Transform>(Vector2D(centeredX, centeredY), 0.0f, 1.0f);
        digitEntity.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&digitEntity);

        Entity* digitPtr = &digitEntity;
        session.digitRefs[i] = digitPtr;

        // --- UP BUTTON (+) ---
        auto& btnUp = world.createEntity();
        auto& btnUpTransform = btnUp.addComponent<Transform>(Vector2D(colX, startY), 0.0f, 1.0f);
        SDL_Texture* texButtons = TextureManager::load("../asset/ui/Buttons.png");

        SDL_FRect srcUp{32, 0, 32, 16};
        SDL_FRect destUp{colX, startY, btnWidth, btnHeight}; // Rendered larger!

        btnUp.addComponent<Sprite>(texButtons, srcUp, destUp, RenderLayer::UI, false);
        btnUp.addComponent<Collider>("ui", destUp);

        auto& clickUp = btnUp.addComponent<Clickable>();
        clickUp.onPressed = [&btnUpTransform] { btnUpTransform.scale = 0.8f; };
        clickUp.onCancel = [&btnUpTransform] { btnUpTransform.scale = 1.0f; };
        clickUp.onReleased = [&overlay, i, digitPtr, columnCenter, &btnUpTransform]() {
            btnUpTransform.scale = 1.0f;
            auto& currentSession = overlay.getComponent<HaggleSession>();

            currentSession.digits[i] = (currentSession.digits[i] + 1) % 10;

            auto& dLabelComp = digitPtr->getComponent<Label>();
            dLabelComp.text = std::to_string(currentSession.digits[i]);
            dLabelComp.dirty = true;
            TextureManager::updateLabel(dLabelComp);

            auto& dTransform = digitPtr->getComponent<Transform>();
            dTransform.position.x = columnCenter - (dLabelComp.dst.w / 2.0f);

            if (currentSession.percentLabelRef) {
                auto& pLabelComp = currentSession.percentLabelRef->getComponent<Label>();
                pLabelComp.text = std::to_string(currentSession.getPercentage()) + "%";
                pLabelComp.dirty = true;
                TextureManager::updateLabel(pLabelComp);
            }
        };
        btnUp.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&btnUp);

        // --- DOWN BUTTON (-) ---
        auto& btnDown = world.createEntity();
        auto& btnDownTransform = btnDown.addComponent<Transform>(Vector2D(colX, downBtnY), 0.0f, 1.0f);

        SDL_FRect srcDown{32, 16, 32, 16};
        SDL_FRect destDown{colX, downBtnY, btnWidth, btnHeight}; // Rendered larger!

        btnDown.addComponent<Sprite>(texButtons, srcDown, destDown, RenderLayer::UI, false);
        btnDown.addComponent<Collider>("ui", destDown);

        auto& clickDown = btnDown.addComponent<Clickable>();
        clickDown.onPressed = [&btnDownTransform] { btnDownTransform.scale = 0.8f; };
        clickDown.onCancel = [&btnDownTransform] { btnDownTransform.scale = 1.0f; };
        clickDown.onReleased = [&overlay, i, digitPtr, columnCenter, &btnDownTransform]() {
            btnDownTransform.scale = 1.0f;
            auto& currentSession = overlay.getComponent<HaggleSession>();

            currentSession.digits[i] = currentSession.digits[i] - 1;
            if (currentSession.digits[i] < 0) currentSession.digits[i] = 9;

            auto& dLabelComp = digitPtr->getComponent<Label>();
            dLabelComp.text = std::to_string(currentSession.digits[i]);
            dLabelComp.dirty = true;
            TextureManager::updateLabel(dLabelComp);

            auto& dTransform = digitPtr->getComponent<Transform>();
            dTransform.position.x = columnCenter - (dLabelComp.dst.w / 2.0f);

            if (currentSession.percentLabelRef) {
                auto& pLabelComp = currentSession.percentLabelRef->getComponent<Label>();
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

Entity& Scene::createHaggleButton(int windowWidth, int windowHeight, Entity& overlay) {
    auto& overlayTransform = overlay.getComponent<Transform>();
    auto& overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

    auto& haggleButton = world.createEntity();
    float displaySize = 128.0f;
    float scaleX = centerLineX - (displaySize / 2.0f);
    float scaleY = (baseY + overlaySprite.dst.h) - (displaySize  );

    auto& haggleTransform = haggleButton.addComponent<Transform>(Vector2D(scaleX, scaleY), 0.0f, 1.0f);
    SDL_Texture* haggleTex = TextureManager::load("../asset/ui/haggleButton.png");

    SDL_FRect haggleSrc{0, 0, 256, 256};
    SDL_FRect haggleDest{scaleX, scaleY, displaySize, displaySize};

    haggleButton.addComponent<Sprite>(haggleTex, haggleSrc, haggleDest, RenderLayer::UI, false);
    haggleButton.addComponent<Collider>("ui", haggleDest);

    auto& clickable = haggleButton.addComponent<Clickable>();
    clickable.onPressed = [&haggleTransform] { haggleTransform.scale = 0.9f; };
    clickable.onCancel = [&haggleTransform] { haggleTransform.scale = 1.0f; };
    clickable.onReleased = [this, &overlay, &haggleTransform]() {
        haggleTransform.scale = 1.0f;

        // Read the final state from our shared component
        auto& session = overlay.getComponent<HaggleSession>();
        int finalPrice = session.getProposedPrice();

        std::cout << "Transaction Confirmed! Sold " << session.currentItem.name
                  << " for: " << finalPrice << "G" << std::endl;

        // TODO: Fire off transaction event to the wallet/reputation system here.

        toggleSettingsOverlayVisibility(overlay, nullptr);
    };

    haggleButton.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&haggleButton);

    return haggleButton;
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
        toggleSettingsOverlayVisibility(overlay,nullptr);
    };

    clickable.onCancel = [&cogTransform] {
        cogTransform.scale = 1.0f;
    };

    return cog;
}

void Scene::createHaggleUIComponents(Entity &overlay, int basePrice) {
    if (!overlay.hasComponent<Children>()) {
        overlay.addComponent<Children>();
    }

    auto &overlayTransform = overlay.getComponent<Transform>();
    auto &overlaySprite = overlay.getComponent<Sprite>();
    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;
    float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

    // 1. THE SHARED STATE
    auto digits = std::make_shared<std::vector<int>>(5, 0);
    int tempPrice = basePrice;
    for (int i = 4; i >= 0; --i) {
        (*digits)[i] = tempPrice % 10;
        tempPrice /= 10;
    }

    // 2. THE PERCENTAGE LABEL
    auto &percentLabelEntity = world.createEntity();
    Label pLabel = {"100%", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "hagglePercent"};
    auto& pLabelComp = percentLabelEntity.addComponent<Label>(pLabel);
    pLabelComp.dirty = true;
    pLabelComp.visible = false;
    TextureManager::updateLabel(pLabelComp);


    // Positioned at the bottom-right of the lock area
    percentLabelEntity.addComponent<Transform>(Vector2D(centerLineX + 80, baseY + 360), 0.0f, 1.0f);
    percentLabelEntity.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&percentLabelEntity);

    Entity* percentPtr = &percentLabelEntity;

    // 3. THE RECALCULATION LAMBDA
    auto updateHagglePercentage = [digits, percentPtr, basePrice]() {
        int totalProposedPrice = 0;
        int multiplier = 10000;
        for (int i = 0; i < 5; ++i) {
            totalProposedPrice += (*digits)[i] * multiplier;
            multiplier /= 10;
        }

        if (basePrice > 0) {
            int percentage = (int)(((float)totalProposedPrice / (float)basePrice) * 100.0f);

            // Grab the reference to the label component
            auto& pLabelComp = percentPtr->getComponent<Label>();

            // Update data

            pLabelComp.text = std::to_string(percentage) + "%";
            pLabelComp.dirty = true;

            // Force the rebuild immediately! (Bypasses loadLabel)
            TextureManager::updateLabel(pLabelComp);
        }
    };

    // 4. GENERATE THE 5 COLUMNS (Mathematically Centered!)
    float spacingX = 48.0f; // Gap between columns
    // Total width of 5 columns is roughly (4 * 48) + 32 = 224 pixels.
    // To center it, we subtract half of that (112) from the center line.
    float startX = centerLineX - 112.0f;
    float startY = baseY + 240.0f; // Pushed down to the bottom half of the panel

    for (int i = 0; i < 5; ++i) {
        float colX = startX + (i * spacingX);
        float columnCenter = colX + (32.0f / 2.0f); // 32 is the width of your +/- buttons

        // --- DIGIT LABEL (Middle) ---
        auto &digitEntity = world.createEntity();
        Label dLabel = {std::to_string((*digits)[i]), AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "digit_" + std::to_string(i)};

        // 1. Add and Update first to get the width/height
        auto& dLabelComp = digitEntity.addComponent<Label>(dLabel);
        dLabelComp.visible = false;
        dLabelComp.dirty = true;
        TextureManager::updateLabel(dLabelComp);

        // 2. MATH: (Center of Column) - (Half of the Text Width)
        float centeredX = columnCenter - (dLabelComp.dst.w / 2.0f);
        // Do the same for Y if you want it perfectly between the buttons
        float centeredY = (startY + 16.0f) + ( (72.0f - 16.0f) / 2.0f ) - (dLabelComp.dst.h / 2.0f);

        digitEntity.addComponent<Transform>(Vector2D(centeredX, centeredY), 0.0f, 1.0f);
        digitEntity.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&digitEntity);

        Entity* digitPtr = &digitEntity;

        // --- UP BUTTON (+) ---
        auto &btnUp = world.createEntity();
        auto &btnUpTransform = btnUp.addComponent<Transform>(Vector2D(colX, startY), 0.0f, 1.0f);
        SDL_Texture *texButtons = TextureManager::load("../asset/ui/Buttons.png");
        SDL_FRect srcUp{32, 0, 32, 16};
        SDL_FRect destUp{colX, startY, 32, 16};
        btnUp.addComponent<Sprite>(texButtons, srcUp, destUp, RenderLayer::UI, false);
        btnUp.addComponent<Collider>("ui", destUp);

        auto &clickUp = btnUp.addComponent<Clickable>();
        clickUp.onPressed = [&btnUpTransform] { btnUpTransform.scale = 0.8f; };
        clickUp.onCancel = [&btnUpTransform] { btnUpTransform.scale = 1.0f; };
        clickUp.onReleased = [digits, i, digitPtr, updateHagglePercentage,columnCenter ,&btnUpTransform]() {
            btnUpTransform.scale = 1.0f;
            (*digits)[i] = ((*digits)[i] + 1) % 10;
            auto& dLabelComp = digitPtr->getComponent<Label>();
            dLabelComp.text = std::to_string((*digits)[i]);
            dLabelComp.dirty = true;
            TextureManager::updateLabel(dLabelComp);

            // RE-CENTER AFTER TEXT CHANGE
            auto& dTransform = digitPtr->getComponent<Transform>();
            dTransform.position.x = columnCenter - (dLabelComp.dst.w / 2.0f);

            updateHagglePercentage();
        };
        btnUp.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&btnUp);

        // --- DOWN BUTTON (-) ---
        auto &btnDown = world.createEntity();
        auto &btnDownTransform = btnDown.addComponent<Transform>(Vector2D(colX, startY + 72), 0.0f, 1.0f);
        SDL_FRect srcDown{32, 16, 32, 16};
        SDL_FRect destDown{colX, startY + 72, 32, 16};
        btnDown.addComponent<Sprite>(texButtons, srcDown, destDown, RenderLayer::UI, false);
        btnDown.addComponent<Collider>("ui", destDown);

        auto &clickDown = btnDown.addComponent<Clickable>();
        clickDown.onPressed = [&btnDownTransform] { btnDownTransform.scale = 0.8f; };
        clickDown.onCancel = [&btnDownTransform] { btnDownTransform.scale = 1.0f; };
        clickDown.onReleased = [digits, i, digitPtr, updateHagglePercentage,columnCenter ,&btnDownTransform]() {
            btnDownTransform.scale = 1.0f;
            (*digits)[i] = ((*digits)[i] + 1) % 10;

            auto &dLabelComp = digitPtr->getComponent<Label>();
            dLabelComp.text = std::to_string((*digits)[i]);
            dLabelComp.dirty = true;
            TextureManager::updateLabel(dLabelComp);

            // RE-CENTER AFTER TEXT CHANGE
            auto &dTransform = digitPtr->getComponent<Transform>();
            dTransform.position.x = columnCenter - (dLabelComp.dst.w / 2.0f);

            updateHagglePercentage();

        };
        btnDown.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&btnDown);
    }

    // 5. THE SELL / CONFIRM BUTTON (The Scales)
    auto &haggleButton = world.createEntity();

    // Our new desired UI size
    float displaySize = 128.0f;

    // Calculate centering: (Panel Center) - (Half of Display Size)
    // Position it 40px up from the very bottom of the parchment
    float scaleX = centerLineX - (displaySize / 2.0f);
    float scaleY = (baseY + overlaySprite.dst.h) - (displaySize + 40.0f);

    auto &haggleTransform = haggleButton.addComponent<Transform>(Vector2D(scaleX, scaleY), 0.0f, 1.0f);

    SDL_Texture *haggleTex = TextureManager::load("../asset/ui/haggleButton.png");

    // Source is the FULL file size
    SDL_FRect haggleSrc{0, 0, 256, 256};
    // Destination is our SHRUNK display size
    SDL_FRect haggleDest{scaleX, scaleY, displaySize, displaySize};

    // Important: Set visible to false so it follows your toggle logic!
    haggleButton.addComponent<Sprite>(haggleTex, haggleSrc, haggleDest, RenderLayer::UI, false);

    // --- The Collider ---
    // Make the clickable area match the 128x128 visual size
    haggleButton.addComponent<Collider>("ui", haggleDest);

    auto& clickable = haggleButton.addComponent<Clickable>();
    clickable.onPressed = [&haggleTransform] { haggleTransform.scale = 0.9f; };
    clickable.onCancel = [&haggleTransform] { haggleTransform.scale = 1.0f; };
    clickable.onReleased = [this, &overlay, &haggleTransform, digits]() {
        haggleTransform.scale = 1.0f;
        int finalPrice = 0;
        int multiplier = 10000;
        for (int i = 0; i < 5; ++i) {
            finalPrice += (*digits)[i] * multiplier;
            multiplier /= 10;
        }
        std::cout << "Transaction Confirmed! Sold for: " << finalPrice << "G" << std::endl;
        toggleSettingsOverlayVisibility(overlay,nullptr);
    };

    haggleButton.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&haggleButton);
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
        toggleSettingsOverlayVisibility(overlay,nullptr);
    };
    clickable.onCancel = [&closeTransform] {
        closeTransform.scale = 1.0f;
    };

    closeButton.addComponent<Parent>(&overlay);
    auto &parentChildren = overlay.getComponent<Children>();
    parentChildren.children.push_back(&closeButton);
}

void Scene::toggleSettingsOverlayVisibility(Entity &overlay, bool* forceState) {
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
        for (auto &child : c.children) {
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
