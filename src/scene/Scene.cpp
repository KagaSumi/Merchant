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
    if (sceneType == SceneType::Victory) {
        initWin(windowWidth, windowHeight);
        return;
    }
    if (sceneType == SceneType::GameOver) {
        initLose(windowWidth, windowHeight);
        return;
    }
    if (sceneType == SceneType::Gameplay) {
        initGameplay(mapPath, windowWidth, windowHeight);
    }
}

void Scene::initMainMenu(int windowWidth, int windowHeight) {
    //camera
    auto &cam = world.createEntity();
    cam.addComponent<Camera>();
    //menu
    auto &menu(world.createEntity());
    menu.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);

    SDL_Texture *texture = TextureManager::load("../asset/menu.png");
    SDL_FRect menuSrc{0, 0, (float) 2400, (float) 1792};
    SDL_FRect menuDst{0, 0, (float) windowWidth, (float) windowHeight};
    menu.addComponent<Sprite>(texture, menuSrc, menuDst);

    // auto &settingsOverlay = createSettingsOverlay(windowWidth, windowHeight);
    // createCogButton(windowWidth, windowHeight, settingsOverlay);
}

void Scene::initLose(int windowWidth, int windowHeight) {
    auto &cam = world.createEntity();
    cam.addComponent<Camera>();
    auto &screen(world.createEntity());
    screen.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);
    SDL_Texture *texture = TextureManager::load("../asset/Lose.png");
    SDL_FRect menuSrc{0, 0, (float) 4800, (float) 3584};
    SDL_FRect menuDst{0, 0, (float) windowWidth, (float) windowHeight};
    screen.addComponent<Sprite>(texture, menuSrc, menuDst);
}

void Scene::initWin(int windowWidth, int windowHeight) {
    auto &cam = world.createEntity();
    cam.addComponent<Camera>();
    auto &screen(world.createEntity());
    screen.addComponent<Transform>(Vector2D(0, 0), 0.0f, 1.0f);
    SDL_Texture *texture = TextureManager::load("../asset/Win.png");
    SDL_FRect menuSrc{0, 0, (float) 2400, (float) 1792};
    SDL_FRect menuDst{0, 0, (float) windowWidth, (float) windowHeight};
    screen.addComponent<Sprite>(texture, menuSrc, menuDst);
}

void Scene::initGameplay(const char *mapPath, int windowWidth, int windowHeight) {
    initAssets(mapPath);
    initUI(windowWidth, windowHeight);
    initWorld(windowWidth, windowHeight);
    initPlayer(windowWidth, windowHeight);
    initSystems();
    initEntities(windowWidth, windowHeight);
}

void Scene::initAssets(const char *mapPath) {
    tilemapTex = TextureManager::load("../asset/SpriteSheet.png");
    world.getMap().load(mapPath, tilemapTex);
    TextureManager::load("../asset/items.png");
    world.getItems().load("../asset/items.xml");
    world.getMarketTrendSystem().load("../asset/market_trends.xml");
    world.getMarketTrendSystem().rollDailyTrend();
    world.getCustomerDialogueSystem().load("../asset/customer_dialogue.xml");
}

void Scene::initUI(int windowWidth, int windowHeight) {
    createHaggleUI(windowWidth, windowHeight);

    auto &inventoryUIRef = createInventoryUI(windowWidth, windowHeight);
    createQuantityScreen(windowWidth, windowHeight);
    auto &invSession = inventoryUIRef.getComponent<InventorySession>();
    if (invSession.quantityPanelRef) {
        bool hide = false;
        toggleSettingsOverlayVisibility(*invSession.quantityPanelRef, &hide);
    }

    createOrderUI(windowWidth, windowHeight);
    auto &orderSession = UIOrderScreen->getComponent<OrderSession>();
    orderSession.getShelfPrice = [](int currentCount) -> int {
        switch (currentCount) {
            case 3: return 500;
            case 4: return 1000;
            case 5: return 1750;
            case 6: return 2750;
            case 7: return 4000;
            case 8: return 5500;
            case 9: return 7500;
            case 10: return 10000;
            case 11: return 13000;
            case 12: return 17000;
            case 13: return 22000;
            case 14: return 28000;
            default: return 999999;
        }
    };
    orderSession.currentShelfPrice = orderSession.getShelfPrice(Game::gameState.displayCasesUnlocked);

    createDialogueUI(windowWidth, windowHeight);
    createHUD(windowWidth, windowHeight);
}

void Scene::initWorld(int windowWidth, int windowHeight) {
    // Colliders
    for (auto &collider: world.getMap().colliders) {
        auto &e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, 1.0f);
        auto &c = e.addComponent<Collider>("wall");
        c.rect = {collider.rect.x, collider.rect.y, collider.rect.w, collider.rect.h};
    }

    // Pathfinding
    PathfindingSystem::InitMap(25, 19, 32, world.getMap().AIWalkable);
    SDL_Point door = world.getMap().Door;
    SDL_Point regi = world.getMap().Register;
    world.getCustomerAISystem().setDoor(door);
    world.getCustomerAISystem().setRegister(regi);

    // Camera
    auto &cam = world.createEntity();
    SDL_FRect camView{0, 0, (float) windowWidth, (float) windowHeight};
    cam.addComponent<Camera>(camView,
                             static_cast<float>(world.getMap().width * 32),
                             static_cast<float>(world.getMap().height * 32));

    // Store entity — holds wallet, debt, reputation, day cycle
    storeEntity = &world.createEntity();
    storeEntity->addComponent<ShopReputation>(Game::gameState.shopReputation);
    auto &wallet = storeEntity->addComponent<Wallet>(Game::gameState.walletBalance);
    storeEntity->addComponent<Debt>(Game::gameState.debtTotal);
    auto &dayCycle = storeEntity->addComponent<DayCycle>();
    dayCycle.date = Game::gameState.dayCount;

    // Day summary needs dayCycle ref at create time
    createDaySummaryUI(windowWidth, windowHeight, dayCycle);

    updateHUD(wallet, dayCycle);
}

void Scene::initPlayer(int windowWidth, int windowHeight) {
    SDL_Point door = world.getMap().Door;
    auto &inv_ui = *UIInventory;

    auto &player = world.createEntity();
    playerEntity = &player;

    player.addComponent<Transform>(Vector2D(door.x * 32, door.y * 32), 1.0f);
    player.addComponent<Velocity>(Vector2D(0, 0), 150.0f);
    player.addComponent<PlayerTag>();

    Animation anim = AssetManager::getAnimation("customer");
    player.addComponent<Animation>(anim);

    SDL_Texture *tex = TextureManager::load("../asset/animations/CustomerA.png");
    SDL_FRect playerDst{-16, -16, 64, 64};
    player.addComponent<Sprite>(tex, anim.clips[anim.currentClip].frameIndicies[0], playerDst);

    auto &col = player.addComponent<Collider>("player");
    col.rect.w = 16.0f;
    col.rect.h = 12.0f;
    col.offsetX = 8.0f;
    col.offsetY = 20.0f;

    auto &inv = player.addComponent<Inventory>();
    inv.uiRef = &inv_ui;
    inv.onOpenUI = [this](const std::vector<InventoryEntry> &items) {
        if (!UIInventory) return;
        bool isOpen = UIInventory->getComponent<Sprite>().visible;
        if (isOpen) toggleSettingsOverlayVisibility(*UIInventory, nullptr);
        else updateInventoryUI(items, InventoryMode::Browse);
    };

    // Seed inventory
    auto &itemDB = world.getItems();
    for (auto &[id, item]: itemDB.items) inv.addItem(item, 0);
    inv.addItem(itemDB.items[1], 5); // Minor Health Potion
    inv.addItem(itemDB.items[7], 2); // Iron Dagger
    inv.addItem(itemDB.items[2], 1); // Phoenix Feather
    inv.addItem(itemDB.items[14], 3);
}

void Scene::initSystems() {
    initHaggleSystem();
    initDayCycleCallbacks();
}

void Scene::initHaggleSystem() {
    auto &haggleSystem = world.getHaggleSystem();
    auto &dialogue = world.getCustomerDialogueSystem();

    // Scene provides the actual UI calls
    haggleSystem.onShowDialogue = [this](const std::string &msg) {
        if (UIHud) { //Hide HUD
            bool hide = false;
            toggleSettingsOverlayVisibility(*UIHud, &hide);
        }
        updateDialogueUI(msg);
    };
    haggleSystem.onShowHaggleUI = [this](const ItemDef &item) {
        ItemDef copy = item;
        updateHaggleUI(copy);
    };

    // CustomerDialogueSystem provides the lines
    haggleSystem.onGetOpeningLine = [&dialogue](float mood) { return dialogue.getOpeningLine(mood); };
    haggleSystem.onGetRejectionLine = [&dialogue](int patience) { return dialogue.getRejectionLine(patience); };
    haggleSystem.onGetWalkawayLine = [&dialogue]() { return dialogue.getWalkawayLine(); };
    haggleSystem.onGetSuccessLine = [&dialogue](int sale, int base) { return dialogue.getSuccessLine(sale, base); };

    haggleSystem.onSaleComplete = [this, &dialogue](int salePrice, int profitMargin) {
        auto &wallet = storeEntity->getComponent<Wallet>();
        auto &rep = storeEntity->getComponent<ShopReputation>();
        auto &dc = storeEntity->getComponent<DayCycle>();

        wallet.balance += salePrice;
        wallet.dailyIncome += salePrice;

        int xpGained = 10;
        if (profitMargin > 0) {
            float pct = static_cast<float>(profitMargin) / salePrice;
            xpGained += static_cast<int>(pct * 50.0f);
        }
        Game::gameState.currentRepXP += xpGained;

        bool leveledUp = false;
        while (Game::gameState.currentRepXP >= Game::gameState.xpToNextLevel) {
            Game::gameState.currentRepXP -= Game::gameState.xpToNextLevel;
            Game::gameState.xpToNextLevel = static_cast<int>(Game::gameState.xpToNextLevel * 1.5f);
            Game::gameState.shopReputation++;
            rep.reputation = Game::gameState.shopReputation;
            leveledUp = true;
        }
        updateHUD(wallet, dc);

        // Use pushDialogue so it queues behind any pending dialogue
        int basePrice = salePrice - profitMargin;
        std::string successLine = dialogue.getSuccessLine(salePrice, basePrice);

        if (leveledUp) {
            world.getHaggleSystem().pushDialogue(successLine, [this]() {
                world.getHaggleSystem().pushDialogue(
                    "Reputation increased to " +
                    std::to_string(Game::gameState.shopReputation) +
                    "! New items now available.",
                    [this]() { world.getHaggleSystem().resumeQueue(); }
                );
            });
        } else {
            world.getHaggleSystem().pushDialogue(successLine, [this]() {
                world.getHaggleSystem().resumeQueue();
            });
        }
    };

    haggleSystem.getPriceModifier = [this](const ItemDef &item) {
        return world.getMarketTrendSystem().getModifier(item);
    };
}

void Scene::initDayCycleCallbacks() {
    auto &dayCycleSystem = world.getDayCycleSystem();

    dayCycleSystem.onMorningStart = [this]() {
        auto &wallet = storeEntity->getComponent<Wallet>();
        auto &dayCycle = storeEntity->getComponent<DayCycle>();
        if (playerEntity && playerEntity->hasComponent<PlayerTag>()) {
            playerEntity->getComponent<PlayerTag>().movementLocked = false;
        }
        wallet.dailyIncome = 0;
        wallet.dailyExpenses = 0;
        world.getMarketTrendSystem().rollDailyTrend();
        updateHUD(wallet, dayCycle);
    };

    dayCycleSystem.onShopOpenStart = [this]() {
        auto &wallet = storeEntity->getComponent<Wallet>();
        auto &dayCycle = storeEntity->getComponent<DayCycle>();
        if (playerEntity && playerEntity->hasComponent<PlayerTag>()) {
            playerEntity->getComponent<PlayerTag>().movementLocked = true;
        }
        updateHUD(wallet, dayCycle);

        // Reset spawner for new day
        auto &spawner = spawnerEntity->getComponent<Spawner>();
        spawner.isFinished = false;
        spawner.spawnCount = 0;

        world.getCustomerSpawnerSystem().resetForNewDay();
    };

    dayCycleSystem.onEveningStart = [this]() {
        auto &wallet = storeEntity->getComponent<Wallet>();
        auto &dayCycle = storeEntity->getComponent<DayCycle>();
        auto &debt = storeEntity->getComponent<Debt>();
        auto &rep = storeEntity->getComponent<ShopReputation>();
        updateHUD(wallet, dayCycle);

        bool isPaymentDay = (dayCycle.date > 0) && (dayCycle.date % 7 == 0);
        int snapshotDebt = world.getDebtSystem().getNextPayment(debt);

        bool bankrupt = false;
        if (isPaymentDay) {
            bankrupt = !world.getDebtSystem().payDebt(wallet, debt);
        }
        bool gameWon = (debt.amount <= 0);

        int snapshotIncome = wallet.dailyIncome;


        Game::gameState.walletBalance = wallet.balance;
        Game::gameState.debtTotal = debt.amount;
        Game::gameState.dayCount = dayCycle.date;

        std::vector<ItemDef> available;
        for (auto &[id, item]: world.getItems().items) {
            if (item.requiredReputation <= rep.reputation)
                available.push_back(item);
        }

        auto &inv = playerEntity->getComponent<Inventory>();

        updateOrderUI(available, wallet, inv,
                      // onContinue
                      [this, snapshotIncome, snapshotDebt, isPaymentDay, debt, bankrupt, gameWon]() {
                          auto &wallet = storeEntity->getComponent<Wallet>();
                          auto &dayCycle = storeEntity->getComponent<DayCycle>();

                          DaySummaryData summary;
                          summary.grossSales = snapshotIncome;
                          summary.orderExpenses = wallet.dailyExpenses;
                          summary.currentBalance = wallet.balance;
                          summary.weeklyPayment = isPaymentDay ? snapshotDebt : 0;
                          summary.weeklyPaymentAmount = snapshotDebt;
                          summary.totalDebt = debt.amount;
                          summary.daysUntilPayment = isPaymentDay ? 0 : (7 - (dayCycle.date % 7));
                          summary.isBankrupt = bankrupt;
                          summary.isGameWon = gameWon;
                          updateDaySummaryUI(summary);
                      },
                      // onBuyShelf
                      [this]() {
                          int nextIdx = Game::gameState.displayCasesUnlocked + 1;
                          if (nextIdx > 15) return;

                          auto &wallet = storeEntity->getComponent<Wallet>();
                          auto &dayCycle = storeEntity->getComponent<DayCycle>();
                          auto &s = UIOrderScreen->getComponent<OrderSession>();
                          int cost = s.getShelfPrice
                                         ? s.getShelfPrice(Game::gameState.displayCasesUnlocked)
                                         : 500;
                          if (wallet.balance < cost) return;

                          wallet.balance -= cost;
                          wallet.dailyExpenses += cost;

                          auto &locs = world.getMap().displayCaseSpawns;
                          if (locs.count(nextIdx)) {
                              Vector2D loc = locs.at(nextIdx);
                              SDL_FRect src = {16 * 32.0f, 8 * 32.0f, 96, 128};
                              SDL_FRect dst = {-32.0f, 2 * -32.0f, 96, 128};
                              createDisplaycase(loc, tilemapTex, src, dst, dayCycle, playerEntity);
                          }
                          Game::gameState.displayCasesUnlocked = nextIdx;
                          std::cout << "Purchased shelf #" << nextIdx << "\n";
                      }
        );
    };

    dayCycleSystem.onHudVisibilityChange = [this](bool visible) {
        if (!UIHud) return;
        bool v = visible;
        toggleSettingsOverlayVisibility(*UIHud, &v);

        if (visible) {
            auto &wallet = storeEntity->getComponent<Wallet>();
            auto &dayCycle = storeEntity->getComponent<DayCycle>();
            updateHUD(wallet, dayCycle);
        }
    };
}

void Scene::initEntities(int windowWidth, int windowHeight) {
    SDL_Point door = world.getMap().Door;
    SDL_Point regi = world.getMap().Register;
    auto &dayCycle = storeEntity->getComponent<DayCycle>();

    // Customer spawner
    std::vector<SDL_Texture *> customerTextures = {
        TextureManager::load("../asset/animations/CustomerF.png"),
        TextureManager::load("../asset/animations/CustomerM.png")
    };
    int customerIndexCount = 0;

    auto &spawner = world.createEntity();
    spawnerEntity = &spawner;
    spawner.addComponent<Transform>(Vector2D(door.x * 32, door.y * 32), 1.0f);
    spawner.addComponent<Spawner>([this, door, customerTextures, customerIndexCount]() mutable {
        auto &e = world.createDeferredEntity();
        e.addComponent<Transform>(Vector2D(door.x * 32, door.y * 32), 1.0f);
        e.addComponent<Velocity>(Vector2D(0, 0), 100.0f);
        e.addComponent<CustomerAI>();
        e.addComponent<Customer>();
        e.addComponent<PathFinding>();

        Animation anim = AssetManager::getAnimation("customer");
        e.addComponent<Animation>(anim);

        SDL_Texture *tex = customerTextures[customerIndexCount++ % customerTextures.size()];
        e.addComponent<Sprite>(tex, anim.clips[anim.currentClip].frameIndicies[0],
                               SDL_FRect{-16.0f, -16.0f, 64, 64});
    });

    // Display cases
    for (const auto &[order, location]: world.getMap().displayCaseSpawns) {
        if (order <= Game::gameState.displayCasesUnlocked) {
            SDL_FRect src = {16 * 32.0f, 8 * 32.0f, 96, 128};
            SDL_FRect dst = {-32.0f, 2 * -32.0f, 96, 128};
            createDisplaycase(location, tilemapTex, src, dst, dayCycle, playerEntity);
        }
    }

    // Cash register
    auto &cashRegister = world.createEntity();
    cashRegister.addComponent<Transform>(Vector2D(regi.x * 32, (regi.y + 2) * 32));
    auto &c = cashRegister.addComponent<Collider>("wall");
    c.rect = {regi.x * 32.0f - 64.0f, (regi.y + 2) * 32 + 32.0f, 160.0f, 62.0f};
    c.offsetX = -64.0f;
    c.offsetY = -14.0f;
    cashRegister.addComponent<Sprite>(tilemapTex,
                                      SDL_FRect{160.0f, 256.0f, 160.0f, 96.0f},
                                      SDL_FRect{2 * -32.0f, -32.0f, 160.0f, 96.0f});
    cashRegister.addComponent<Interaction>([&dayCycle, &cashRegister, this]() {
        if (dayCycle.currentPhase != DayPhase::Morning) return;

        // Only allow interaction if player is below the register
        if (playerEntity) {
            auto &playerT = playerEntity->getComponent<Transform>();
            auto &playerA = playerEntity->getComponent<Animation>();
            auto &registerT = cashRegister.getComponent<Transform>();

            // Player must be below the register (higher Y value)
            if (playerA.direction != 1||playerT.position.y <= registerT.position.y) return;
        }

        dayCycle.phaseSwapReady = true;
    });

    // Bulletin board
    auto &bulletinBoard = world.createEntity();
    bulletinBoard.addComponent<Transform>(Vector2D(17 * 32, 5 * 32), 1.0f);
    bulletinBoard.addComponent<Interaction>([this]() {
        showSimpleDialogue(world.getMarketTrendSystem().getActiveTrend().blurb);
    });

    // Scene state
    world.createEntity().addComponent<SceneState>();
}
