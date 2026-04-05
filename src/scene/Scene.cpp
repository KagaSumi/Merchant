//
// Created by Curry on 2026-02-25.
//

#include "../scene/Scene.h"
#include "DebtSystem.h"
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
    SDL_FRect menuSrc{0, 0, (float) 1200, (float) 896};
    SDL_FRect menuDst{0,0, (float) windowWidth, (float) windowHeight};
    menu.addComponent<Sprite>(texture, menuSrc, menuDst);

    auto &settingsOverlay = createSettingsOverlay(windowWidth, windowHeight);
    createCogButton(windowWidth, windowHeight, settingsOverlay);
}

void Scene::initGameplay(const char *mapPath, int windowWidth, int windowHeight) {
    //load map
    SDL_Texture *tilemapTex = TextureManager::load("../asset/SpriteSheet.png");
    world.getMap().load(mapPath, tilemapTex);
    SDL_Texture *itemsTex = TextureManager::load("../asset/items.png");
    world.getItems().load("../asset/items.xml");

    //Create UI
    createHaggleUI(windowWidth, windowHeight);
    auto &inventoryUIRef = createInventoryUI(windowWidth, windowHeight);
    createQuantityScreen(windowWidth, windowHeight);
    auto &invSession = inventoryUIRef.getComponent<InventorySession>();
    if (invSession.quantityPanelRef) {
        bool hide = false;
        toggleSettingsOverlayVisibility(*invSession.quantityPanelRef, &hide);
    }
    createOrderUI(windowWidth, windowHeight);
    createDialogueUI(windowWidth, windowHeight);

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
    store.addComponent<Wallet>(Game::gameState.walletBalance);
    store.addComponent<Debt>(Game::gameState.debtTotal);
    auto& dayCycle = store.addComponent<DayCycle>();
    dayCycle.date = Game::gameState.dayCount;

    //Needed For Confirm Button
    createDaySummaryUI(windowWidth, windowHeight, dayCycle);

    //Create Player
    auto &player(world.createEntity());
    // player.addComponent<Transform>(Vector2D(door.x * 32, door.y * 32), 1.0f);
    player.addComponent<Transform>(Vector2D(196,481), 1.0f); // Dev Spot
    player.addComponent<Velocity>(Vector2D(0, 0), 150.0f);

    Animation anim = AssetManager::getAnimation("customer");
    player.addComponent<Animation>(anim);

    SDL_Texture *tex = TextureManager::load("../asset/animations/CustomerA.png");
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndicies[0];
    SDL_FRect playerDst{-16, -16, 64, 64};
    player.addComponent<Sprite>(tex, playerSrc, playerDst);

    auto &playerCollider = player.addComponent<Collider>("player");
    playerCollider.rect.w = 16.0f; // Half the width of the tile
    playerCollider.rect.h = 12.0f; // Roughly the bottom third of the sprite
    playerCollider.offsetX = 8.0f; // (32 - 16) / 2 = centers the box horizontally
    playerCollider.offsetY = 20.0f; // Pushes the box down so it only covers the legs/feet

    auto &inv = player.addComponent<Inventory>();
    inv.uiRef = &inventoryUIRef;
    inv.onOpenUI = [this](const std::vector<InventoryEntry> &items) {
        if (!UIInventory) return;

        bool isCurrentlyOpen = UIInventory->getComponent<Sprite>().visible;
        if (isCurrentlyOpen) {
            // Just close it
            toggleSettingsOverlayVisibility(*UIInventory, nullptr);
        } else {
            // Update data and open
            updateInventoryUI(items, InventoryMode::Browse);
        }
    };
    auto &itemDB = world.getItems();
    for (auto &[id,item]: itemDB.items) {
        inv.addItem(item, 0);
    }
    inv.addItem(itemDB.items[1], 5); // Minor Health Potion
    inv.addItem(itemDB.items[7], 2); // Iron Dagger
    inv.addItem(itemDB.items[2], 1); // Phoenix Feather
    inv.addItem(itemDB.items[14], 3);
    player.addComponent<PlayerTag>();

    //Setup DayCycle System callback:
    auto& dayCycleSystem = world.getDayCycleSystem(); // however you access it

    // Setup HaggleSystem callbacks
    auto& haggleSystem = world.getHaggleSystem();
    haggleSystem.onSaleComplete = [&store](int salePrice, int profitMargin) {
        auto& wallet = store.getComponent<Wallet>();
        wallet.balance += salePrice;
        wallet.dailyIncome += salePrice;
        std::cout << "Sale! +" << salePrice << "G (Profit: " << profitMargin << "G)\n";
    };

    haggleSystem.onBeginHaggle = [this](const ItemDef& item) {
        updateHaggleUI(const_cast<ItemDef&>(item));
    };

    haggleSystem.onShowFeedback = [this](const std::string &msg) {
        updateDialogueUI(msg, [this]() {
            world.getHaggleSystem().dismissFeedback();
        });
    };

    // Wire up the phase callbacks
    dayCycleSystem.onMorningStart = [this,&store]() {
        auto& wallet = store.getComponent<Wallet>();
        wallet.dailyIncome = 0; // Reset Daily Income
        wallet.dailyExpenses = 0; // Reset Daily Income
        // Nothing for now - player is free to roam
        std::cout << "Morning started\n";
    };

    dayCycleSystem.onShopOpenStart = [this]() {
        // Spawn customers, lock display cases, etc.
        std::cout << "Shop is open!\n";
        //TODO Lock Player??
    };

    dayCycleSystem.onEveningStart = [this, &player, &store, &dayCycle, tilemapTex]() {
    auto& wallet = store.getComponent<Wallet>();
    auto& debt = store.getComponent<Debt>();

    bool isPaymentDay = (dayCycle.date > 0) && (dayCycle.date % 7 == 0);
    int snapshotDebt = world.getDebtSystem().getNextPayment(debt);

    if (isPaymentDay) {
        world.getDebtSystem().payDebt(wallet, debt);
    }

    // Snapshot AFTER debt payment, BEFORE order screen
    int snapshotIncome = wallet.dailyIncome;
    int snapshotBalanceBeforeOrders = wallet.balance;

    // Sync save state
    Game::gameState.walletBalance = wallet.balance;
    Game::gameState.debtTotal = debt.amount;
    Game::gameState.dayCount = dayCycle.date;

    std::vector<ItemDef> available;
    for (auto& [id, item] : world.getItems().items) {
        if (item.requiredReputation <= Game::gameState.shopReputation)
            available.push_back(item);
    }

    auto& inv = player.getComponent<Inventory>();

    updateOrderUI(available, wallet, inv,
        [this, &store, snapshotIncome,snapshotDebt ,isPaymentDay, &debt]() {
            auto& wallet = store.getComponent<Wallet>();

            DaySummaryData summary;
            summary.grossSales = snapshotIncome;
            summary.orderExpenses = wallet.dailyExpenses; // live - includes orders bought
            summary.currentBalance = wallet.balance;
            summary.weeklyPayment = isPaymentDay ? snapshotDebt : 0;
            summary.weeklyPaymentAmount = snapshotDebt;
            summary.daysUntilPayment = isPaymentDay ? 7 : (7 - (store.getComponent<DayCycle>().date % 7));

            updateDaySummaryUI(summary);
        },
        [this, &store, &dayCycle, &player, tilemapTex]() {
            int nextIdx = Game::gameState.displayCasesUnlocked + 1;
            if (nextIdx > 15) return;

            auto& wallet = store.getComponent<Wallet>();
            int cost = 500;
            if (wallet.balance < cost) return;

            wallet.balance -= cost;
            wallet.dailyExpenses += cost; // track it as an expense

            auto& displayCaseLocations = world.getMap().displayCaseSpawns;
            if (displayCaseLocations.count(nextIdx)) {
                Vector2D loc = displayCaseLocations.at(nextIdx);
                SDL_FRect src = {16 * 32.0f, 8 * 32.0f, 96, 128};
                SDL_FRect dst = {-32.0f, 2 * -32.0f, 96, 128};
                createDisplaycase(loc, tilemapTex, src, dst, dayCycle, &player);
            }

            Game::gameState.displayCasesUnlocked = nextIdx;
            std::cout << "Purchased shelf #" << nextIdx << "\n";
        }
    );
};


    //Customers:
    std::vector<SDL_Texture *> customerTextures = {
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
        e.addComponent<CustomerAI>();
        e.addComponent<Customer>();
        e.addComponent<PathFinding>();

        Animation anim = AssetManager::getAnimation("customer");
        e.addComponent<Animation>(anim);

        // Your texture cycling logic here is excellent.
        // Because the lambda is 'mutable', customerIndexCount will correctly increment across spawns.
        SDL_Texture *tex = customerTextures[customerIndexCount++ % customerTextures.size()];
        SDL_FRect src = anim.clips[anim.currentClip].frameIndicies[0];
        SDL_FRect dst{-16.0f, -16.0f, 64, 64};
        e.addComponent<Sprite>(tex, src, dst);
    });

    //Display Case:
    std::map<int, Vector2D> displayCaseLocations = world.getMap().displayCaseSpawns;
    for (const auto &[order, location]: displayCaseLocations) {
        if (order <= Game::gameState.displayCasesUnlocked) {
            // Spawn owned display case
            SDL_FRect src = {16 * 32.0f, 8 * 32.0f, 96, 128};
            SDL_FRect dst = {-32.0, 2 * -32.0, 96, 128};
            createDisplaycase(location, tilemapTex, src, dst, dayCycle, &player);
        }
    }

    //Cash Register
    auto &cashRegister = world.createEntity();
    cashRegister.addComponent<Transform>(Vector2D(regi.x * 32, (regi.y + 2) * 32));
    auto &c = cashRegister.addComponent<Collider>("wall");
    c.rect = {regi.x * 32.0f - 64.0f, (regi.y + 2) * 32 + 32.0f, 160.0f, 62.0f};
    c.offsetX = -64.0f;
    c.offsetY = -14.0f;
    cashRegister.addComponent<Interaction>([&dayCycle]() {
        if (dayCycle.currentPhase == DayPhase::Morning) {
            std::cout << "Start the day ..." << std::endl;
            dayCycle.phaseSwapReady = true;
        }
    });
    SDL_FRect regSrc = {160.0f, 256.0f, 160.0f, 96.0f};
    SDL_FRect regDst = {2 * -32.0f, -32.0f, 160.0f, 96.0f};
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