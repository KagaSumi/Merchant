//
// Created by Curry on 2026-04-07.
//

#include "OrderUI.h"
#include "BaseUI.h"
#include "../manager/AssetManager.h"
#include "Game.h"

namespace {
    // ==========================================
    // 1. ORDER UI BUILDERS
    // ==========================================

    void buildOrderHeader(Scene &scene, Entity &overlay, OrderSession &session, float baseX, float baseY,
                          float menuWidth) {
        auto &title = BaseUI::createLabel(scene, overlay, "Market", Vector2D(0, baseY + 30.0f), {0, 0, 0, 255},
                                          "orderTitle", true);
        title.getComponent<Transform>().position.x =
                baseX + (menuWidth / 2.0f) - (title.getComponent<Label>().dst.w / 2.0f);

        session.walletLabelRef = &BaseUI::createLabel(scene, overlay, "Wallet: 0G",
                                                      Vector2D(baseX + 30.0f, baseY + 40.0f), {0, 0, 0, 255},
                                                      "orderWallet", false);
        session.paymentWarningLabelRef = &BaseUI::createLabel(scene, overlay, " ",
                                                              Vector2D(baseX + 30.0f, baseY + 65.0f),
                                                              {211, 47, 47, 255}, "orderWarning", false);
    }

    void buildMarketGrid(Scene &scene, Entity &overlay, OrderSession &session, float baseX, float baseY,
                         float menuWidth, float gridTop, float gridH) {
        int cols = 4, rows = 4;
        float colW = menuWidth / cols;
        float rowH = gridH / rows;
        SDL_Texture *itemsTex = TextureManager::load("asset/items.png");

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                OrderSlotRefs slot;
                int idx = row * cols + col;
                float slotX = baseX + col * colW;
                float slotY = gridTop + row * rowH;
                float iconSize = 36.0f, padding = 6.0f;

                // Icon
                auto &icon = scene.world.createEntity();
                icon.addComponent<Transform>(Vector2D(slotX + padding, slotY + padding), 0.0f, 1.0f);
                icon.addComponent<Sprite>(itemsTex, SDL_FRect{0, 0, 32, 32},
                                          SDL_FRect{slotX + padding, slotY + padding, iconSize, iconSize},
                                          RenderLayer::UI, false);
                icon.addComponent<Parent>(&overlay);
                overlay.getComponent<Children>().children.push_back(&icon);
                slot.icon = &icon;

                // Name & Price Labels
                // 1. Name Label (USE arial-small)
                slot.nameLabel = &BaseUI::createLabel(scene, overlay, "Item",
                    Vector2D(slotX + padding + iconSize + 4.0f, slotY + padding),
                    {0,0,0,255}, "ord_n_" + std::to_string(idx), true);

                auto& nComp = slot.nameLabel->getComponent<Label>();
                nComp.font = AssetManager::getFont("arial-small"); // Force small font
                TextureManager::updateLabel(nComp); // Refresh dimensions


                // 2. Buy Button (Position based on small font height)
                float btnY = slotY + padding + nComp.dst.h + 2.0f;
                slot.buyBtn = &BaseUI::createStandardButton(scene, overlay,
                    Vector2D(slotX + padding + iconSize + 4.0f, btnY), 48.0f, 22.0f, {0,0,32,16}, nullptr);


                // 3. Price Label (USE arial-small)
                slot.priceLabel = &BaseUI::createLabel(scene, overlay, "0G",
                    Vector2D(slotX + padding + iconSize + 56.0f, btnY + 2.0f),
                    {0,0,0,255}, "ord_p_" + std::to_string(idx), false);

                slot.priceLabel->getComponent<Label>().font = AssetManager::getFont("arial-small");

                session.slots.push_back(slot);
            }
        }
    }

    void buildShelfUpgrade(Scene &scene, Entity &overlay, OrderSession &session, float baseX, float menuWidth,
                           float gridBottom) {
        float shelfX = baseX + 40.0f;
        float shelfY = gridBottom + 15.0f;
        SDL_Texture *tilemapTex = TextureManager::load("asset/SpriteSheet.png");

        auto &icon = scene.world.createEntity();
        icon.addComponent<Transform>(Vector2D(shelfX, shelfY), 0.0f, 1.0f);
        icon.addComponent<Sprite>(tilemapTex, SDL_FRect{16 * 32, 8 * 32, 96, 128}, SDL_FRect{-32, -32, 36, 48},
                                  RenderLayer::UI, false);
        icon.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&icon);

        BaseUI::createLabel(scene, overlay, "Additional Shelf", Vector2D(shelfX + 45.0f, shelfY + 8.0f), {0, 0, 0, 255},
                            "shelfName", true);
        session.shelfPriceLabel = &BaseUI::createLabel(scene, overlay, "500G", Vector2D(shelfX + 45.0f, shelfY + 28.0f),
                                                       {0, 0, 0, 255}, "shelfPrice", false);

        session.shelfBuyBtn = &BaseUI::createStandardButton(scene, overlay, Vector2D(shelfX + 160.0f, shelfY + 6.0f),
                                                            64.0f, 24.0f, {0, 0, 32, 16}, [&scene]() {
                                                                auto &s = scene.UIOrderScreen->getComponent<
                                                                    OrderSession>();
                                                                scene.world.getAudioEventQueue().push(
                                                                    std::make_unique<AudioEvent>("clickHard"));

                                                                if (s.onBuyShelf) {
                                                                    s.onBuyShelf();
                                                                }
                                                            });
        session.shelfBuyBtn->getComponent<Collider>().enabled = false;
    }

    // ==========================================
    // 2. UPDATE HELPERS
    // ==========================================

    void updatePriceVisuals(Scene &scene, Entity *priceEnt, int price, float trendMod, bool canAfford) {
        auto &lbl = priceEnt->getComponent<Label>();
        lbl.text = std::to_string(price) + "G";
        if (!canAfford) lbl.color = {180, 50, 50, 255};
        else if (trendMod > 1.05f) lbl.color = {76, 175, 80, 255};
        else if (trendMod < 0.95f) lbl.color = {211, 47, 47, 255};
        else lbl.color = {0, 0, 0, 255};
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);
    }
}

namespace OrderUI {
    Entity &create(Scene &scene, int windowWidth, int windowHeight, Entity *&outUIOrderScreen) {
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;
        mainOverlay.addComponent<Children>();

        auto &session = mainOverlay.addComponent<OrderSession>();
        float baseX = mainOverlay.getComponent<Transform>().position.x;
        float baseY = mainOverlay.getComponent<Transform>().position.y;
        float menuW = mainOverlay.getComponent<Sprite>().dst.w;
        float menuH = mainOverlay.getComponent<Sprite>().dst.h;

        float footerH = 70.0f;
        float gridTop = baseY + 80.0f;
        float gridBottom = baseY + menuH - footerH;

        buildOrderHeader(scene, mainOverlay, session, baseX, baseY, menuW);
        buildMarketGrid(scene, mainOverlay, session, baseX, baseY, menuW, gridTop, gridBottom - gridTop);
        buildShelfUpgrade(scene, mainOverlay, session, baseX, menuW, gridBottom);

        // Continue Button
        BaseUI::createStandardButton(scene, mainOverlay, Vector2D(baseX + menuW - 180.0f, baseY + menuH - 56.0f),
                                     160.0f, 40.0f, {0, 33, 64, 16}, [&scene]() {
                                         scene.world.getUIVisibilityManager().hide("order");
                                         scene.world.getUIVisibilityManager().show("hud");
                                         scene.world.getAudioEventQueue().push(
                                             std::make_unique<AudioEvent>("clickSoft"));
                                         auto &s = scene.UIOrderScreen->getComponent<OrderSession>();
                                         if (s.onContinue) s.onContinue();
                                     });

        outUIOrderScreen = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("order", outUIOrderScreen);
        return mainOverlay;
    }

    Entity &update(Scene &scene, std::vector<ItemDef> availableItems, Wallet &wallet, Inventory &inv,
                   int paymentDueToday, std::function<void()> onContinue, std::function<void()> onBuyShelf,
                   Entity *UIOrderScreen) {
        if (!UIOrderScreen) return *UIOrderScreen;
        auto &session = UIOrderScreen->getComponent<OrderSession>();
        session.walletRef = &wallet.balance;
        session.inventoryRef = &inv;
        session.onContinue = onContinue;
        session.onBuyShelf = onBuyShelf;
        session.currentItems = availableItems;

        scene.world.getUIVisibilityManager().show("order");
        scene.world.getUIVisibilityManager().hide("hud");

        // 1. Header Updates
        auto updateHeaderLbl = [](Entity *ent, const std::string &txt, bool visible = true) {
            if (!ent) return;
            auto &lbl = ent->getComponent<Label>();
            lbl.text = txt;
            lbl.visible = visible;
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);
        };
        updateHeaderLbl(session.walletLabelRef, "Wallet: " + std::to_string(wallet.balance) + "G");
        updateHeaderLbl(session.paymentWarningLabelRef, "Debt Paid Today: " + std::to_string(-paymentDueToday) + "G",
                        paymentDueToday > 0);

        // 2. Grid Slots
        for (int i = 0; i < (int) session.slots.size(); ++i) {
            auto &slot = session.slots[i];
            bool hasItem = i < (int) availableItems.size();
            slot.icon->getComponent<Sprite>().visible = hasItem;
            slot.nameLabel->getComponent<Label>().visible = hasItem;
            slot.priceLabel->getComponent<Label>().visible = hasItem;
            slot.buyBtn->getComponent<Sprite>().visible = hasItem;

            if (hasItem) {
                ItemDef item = availableItems[i];
                slot.icon->getComponent<Sprite>().src = item.src;

                Entity* btnPtr = slot.buyBtn;

                int held = 0;
                for (auto &entry: inv.items) if (entry.item.id == item.id) held = entry.quantity;
                updateHeaderLbl(slot.nameLabel, item.name + " (" + std::to_string(held) + ")");

                float mod = scene.world.getMarketTrendSystem().getModifier(item);
                int tPrice = static_cast<int>(item.basePrice * mod);
                updatePriceVisuals(scene, slot.priceLabel, tPrice, mod, wallet.balance >= tPrice);

                auto &bClick = slot.buyBtn->getComponent<Clickable>();
                bClick.onReleased = [&scene, item, &wallet, &inv, btnPtr]() {
                    btnPtr->getComponent<Transform>().scale = 1.0f;

                    float m = scene.world.getMarketTrendSystem().getModifier(item);
                    int p = static_cast<int>(item.basePrice * m);
                    if (wallet.balance >= p) {
                        wallet.balance -= p;
                        wallet.dailyExpenses += p;
                        inv.addItem(item, 1);
                        scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));
                        // Recursively call update to refresh all numbers
                        auto &s = scene.UIOrderScreen->getComponent<OrderSession>();
                        OrderUI::update(scene, s.currentItems, wallet, inv, 0, s.onContinue, s.onBuyShelf,
                                        scene.UIOrderScreen);
                    }
                };
                slot.buyBtn->getComponent<Collider>().enabled = (wallet.balance >= tPrice);
            } else {
                slot.buyBtn->getComponent<Collider>().enabled = false;
            }
        }

        // 3. Shelf Upgrade
        if (session.shelfBuyBtn) {
            int cost = session.getShelfPrice ? session.getShelfPrice(Game::gameState.displayCasesUnlocked) : 500;
            session.currentShelfPrice = cost;
            bool maxed = Game::gameState.displayCasesUnlocked >= 15;
            bool afford = !maxed && wallet.balance >= cost;

            updateHeaderLbl(session.shelfPriceLabel, maxed ? "Maxed Out" : (std::to_string(cost) + "G"));
            session.shelfPriceLabel->getComponent<Label>().color = afford
                                                                       ? SDL_Color{0, 0, 0, 255}
                                                                       : SDL_Color{180, 50, 50, 255};
            session.shelfBuyBtn->getComponent<Collider>().enabled = afford;

            Entity* shelfBtnPtr = session.shelfBuyBtn;
            session.shelfBuyBtn->getComponent<Clickable>().onReleased = [&scene, &wallet, cost, shelfBtnPtr]() {
                shelfBtnPtr->getComponent<Transform>().scale = 1.0f;

                auto &s = scene.UIOrderScreen->getComponent<OrderSession>();
                if (wallet.balance >= cost && s.onBuyShelf) {
                    s.onBuyShelf();
                    scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));
                    OrderUI::update(scene, s.currentItems, wallet, *s.inventoryRef, 0, s.onContinue, s.onBuyShelf,
                                    scene.UIOrderScreen);
                }
            };
        }

        return *UIOrderScreen;
    }
}
