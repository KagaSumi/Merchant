//
// Created by Curry on 2026-04-07.
//

#include "OrderUI.h"
#include "BaseUI.h"
#include "../manager/AssetManager.h"
#include "Game.h"

namespace OrderUI {
    Entity& create(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIOrderScreen) {
        auto& mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
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
        auto& titleEnt = scene.world.createEntity();
        Label tData = {"Market", AssetManager::getFont("arial"), {0,0,0,255}, LabelType::Static, "orderTitle"};
        auto& tComp = titleEnt.addComponent<Label>(tData);
        tComp.dirty = true; tComp.visible = false;

        // STATIC: Render once for layout centering
        TextureManager::updateLabel(tComp);
        titleEnt.addComponent<Transform>(Vector2D(centerX - tComp.dst.w / 2.0f, baseY + 30.0f), 0.0f, 1.0f);
        titleEnt.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&titleEnt);

        // --- WALLET LABEL (top left) ---
        auto& walletEnt = scene.world.createEntity();
        Label wData = {"Wallet: 0G", AssetManager::getFont("arial"), {0,0,0,255}, LabelType::Static, "orderWallet"};
        auto& wComp = walletEnt.addComponent<Label>(wData);
        wComp.dirty = true; wComp.visible = false;

        walletEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, baseY + 40.0f), 0.0f, 1.0f);
        walletEnt.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&walletEnt);
        session.walletLabelRef = &walletEnt;

        // --- PAYMENT WARNING LABEL (below wallet) ---
        auto& warningEnt = scene.world.createEntity();
        Label warnData = {" ", AssetManager::getFont("arial-small"), {211, 47, 47, 255}, LabelType::Static, "orderWarning"};
        auto& warnComp = warningEnt.addComponent<Label>(warnData);
        warnComp.dirty = true;
        warnComp.visible = false;

        warningEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, baseY + 65.0f), 0.0f, 1.0f);
        warningEnt.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&warningEnt);
        session.paymentWarningLabelRef = &warningEnt;

        // --- GRID: 4 cols x 4 rows = 16 slots ---
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
                auto& iconEnt = scene.world.createEntity();
                iconEnt.addComponent<Transform>(Vector2D(slotX + padding, slotY + padding), 0.0f, 1.0f);
                SDL_FRect iconDst = {slotX + padding, slotY + padding, iconSize, iconSize};
                iconEnt.addComponent<Sprite>(itemsTex, SDL_FRect{0,0,32,32}, iconDst, RenderLayer::UI, false);
                iconEnt.addComponent<Parent>(&mainOverlay);
                mainOverlay.getComponent<Children>().children.push_back(&iconEnt);
                slot.icon = &iconEnt;

                // Name label
                auto& nameEnt = scene.world.createEntity();
                Label nData = {"Item", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static,
                               "ord_name_" + std::to_string(row) + "_" + std::to_string(col)};
                auto& nComp = nameEnt.addComponent<Label>(nData);
                nComp.dirty = true; nComp.visible = false;

                // REQUIRED: Render this once to get the font height (dst.h) for the button below
                TextureManager::updateLabel(nComp);

                nameEnt.addComponent<Transform>(
                    Vector2D(slotX + padding + iconSize + 4.0f, slotY + padding), 0.0f, 1.0f);
                nameEnt.addComponent<Parent>(&mainOverlay);
                mainOverlay.getComponent<Children>().children.push_back(&nameEnt);
                slot.nameLabel = &nameEnt;

                // Price label
                float btnY = slotY + padding + nComp.dst.h + 4.0f; // Uses nComp.dst.h!
                float btnW = 48.0f, btnH = 22.0f;
                auto& priceEnt = scene.world.createEntity();
                Label pData = {"0G", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static,
                               "ord_price_" + std::to_string(row) + "_" + std::to_string(col)};
                auto& pComp = priceEnt.addComponent<Label>(pData);
                pComp.dirty = true; pComp.visible = false;

                priceEnt.addComponent<Transform>(
                    Vector2D(slotX + padding + iconSize + btnW + 4.0f, btnY + 2.0f), 0.0f, 1.0f);
                priceEnt.addComponent<Parent>(&mainOverlay);
                mainOverlay.getComponent<Children>().children.push_back(&priceEnt);
                slot.priceLabel = &priceEnt;

                // Buy button
                auto& btnEnt = scene.world.createEntity();
                float btnX = slotX + padding + iconSize + 4.0f;

                btnEnt.addComponent<Transform>(Vector2D(btnX, btnY), 0.0f, 1.0f);
                SDL_FRect btnDst = {btnX, btnY, btnW, btnH};
                btnEnt.addComponent<Sprite>(btnTex, SDL_FRect{0,0,32,16}, btnDst, RenderLayer::UI, false);
                auto& btnCol = btnEnt.addComponent<Collider>("ui", btnDst);
                btnCol.enabled = false;
                slot.buyBtn = &btnEnt;

                int slotIdx = row * cols + col;
                auto& bClick = btnEnt.addComponent<Clickable>();

                Entity* btnPtr = &btnEnt;
                bClick.onPressed = [btnPtr]{ btnPtr->getComponent<Transform>().scale = 0.9f; };
                bClick.onCancel = [btnPtr]{ btnPtr->getComponent<Transform>().scale = 1.0f; };
                bClick.onReleased = [&scene, slotIdx, btnPtr]() {
                    btnPtr->getComponent<Transform>().scale = 1.0f;
                    if (!scene.UIOrderScreen) return;
                    scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));

                    auto& s = scene.UIOrderScreen->getComponent<OrderSession>();
                    if (!s.inventoryRef || !s.walletRef) return;
                    if (slotIdx >= s.slots.size()) return;
                };

                btnEnt.addComponent<Parent>(&mainOverlay);
                mainOverlay.getComponent<Children>().children.push_back(&btnEnt);

                session.slots.push_back(slot);
            }
        }

        // --- ADDITIONAL SHELF UPGRADE (Footer) ---
        float shelfX = baseX + 40.0f;
        float shelfY = gridBottom + 15.0f;

        // Shelf Icon
        auto& shelfIcon = scene.world.createEntity();
        shelfIcon.addComponent<Transform>(Vector2D(shelfX, shelfY), 0.0f, 1.0f);
        shelfIcon.addComponent<Sprite>(tilemapTex, SDL_FRect{16* 32.0f, 8*32.0f, 96, 128}, SDL_FRect{-32.0f, -32.0, 36.0f, 48.0f}, RenderLayer::UI, false);
        shelfIcon.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&shelfIcon);

        // Shelf Name Label
        auto& shelfName = scene.world.createEntity();
        Label sNameData = {"Additional Shelf", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static, "shelfName"};
        auto& sNameComp = shelfName.addComponent<Label>(sNameData);
        sNameComp.dirty = true; sNameComp.visible = false;

        shelfName.addComponent<Transform>(Vector2D(shelfX + 45.0f, shelfY + 8.0f), 0.0f, 1.0f);
        shelfName.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&shelfName);

        // Shelf Price Label
        auto& shelfPrice = scene.world.createEntity();
        Label sPriceData = {"500G", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static, "shelfPrice"};
        auto& sPriceComp = shelfPrice.addComponent<Label>(sPriceData);
        sPriceComp.dirty = true; sPriceComp.visible = false;
        // OPTIMIZED: Removed updateLabel here

        shelfPrice.addComponent<Transform>(Vector2D(shelfX + 45.0f, shelfY + 28.0f), 0.0f, 1.0f);
        shelfPrice.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&shelfPrice);
        session.shelfPriceLabel = &shelfPrice;

        // Shelf Buy Button
        auto& sBtnEnt = scene.world.createEntity();
        float sBtnW = 64.0f, sBtnH = 24.0f;
        float sBtnX = shelfX + 160.0f;
        sBtnEnt.addComponent<Transform>(Vector2D(sBtnX, shelfY + 6.0f), 0.0f, 1.0f);
        sBtnEnt.addComponent<Sprite>(btnTex, SDL_FRect{0,0,32,16}, SDL_FRect{sBtnX, shelfY + 6.0f, sBtnW, sBtnH}, RenderLayer::UI, false);
        sBtnEnt.addComponent<Collider>("ui", SDL_FRect{sBtnX, shelfY + 6.0f, sBtnW, sBtnH}).enabled = false;

        auto& sClick = sBtnEnt.addComponent<Clickable>();
        Entity* sBtnPtr = &sBtnEnt;
        sClick.onPressed = [sBtnPtr]{ sBtnPtr->getComponent<Transform>().scale = 0.9f; };
        sClick.onCancel = [sBtnPtr]{ sBtnPtr->getComponent<Transform>().scale = 1.0f; };
        sClick.onReleased = [sBtnPtr]{ sBtnPtr->getComponent<Transform>().scale = 1.0f; };

        sBtnEnt.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&sBtnEnt);
        session.shelfBuyBtn = &sBtnEnt;

        // --- CONTINUE BUTTON ---
        auto& contBtn = scene.world.createEntity();
        float cBtnW = 160.0f, cBtnH = 40.0f;
        float cBtnX = baseX + menuWidth - cBtnW - 20.0f;
        float cBtnY = baseY + menuHeight - cBtnH - 16.0f;
        contBtn.addComponent<Transform>(Vector2D(cBtnX, cBtnY), 0.0f, 1.0f);
        contBtn.addComponent<Sprite>(btnTex, SDL_FRect{0,33,64,16},
                                      SDL_FRect{cBtnX,cBtnY,cBtnW,cBtnH}, RenderLayer::UI, false);
        contBtn.addComponent<Collider>("ui", SDL_FRect{cBtnX,cBtnY,cBtnW,cBtnH}).enabled = false;

        auto& cClick = contBtn.addComponent<Clickable>();
        Entity* cBtnPtr = &contBtn;
        cClick.onPressed = [cBtnPtr]{ cBtnPtr->getComponent<Transform>().scale = 0.9f; };
        cClick.onCancel = [cBtnPtr]{ cBtnPtr->getComponent<Transform>().scale = 1.0f; };
        cClick.onReleased = [&scene, cBtnPtr]() {
            cBtnPtr->getComponent<Transform>().scale = 1.0f;
            scene.world.getUIVisibilityManager().hide("order");
            scene.world.getUIVisibilityManager().show("hud");
            scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickSoft"));

            auto& s = scene.UIOrderScreen->getComponent<OrderSession>();
            if (s.onContinue) s.onContinue();
        };
        contBtn.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&contBtn);

        outUIOrderScreen = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("order", outUIOrderScreen);
        return mainOverlay;
    }

    Entity& update(Scene& scene, std::vector<ItemDef> availableItems, Wallet& wallet, Inventory& inv, int paymentDueToday, std::function<void()> onContinue, std::function<void()> onBuyShelf, Entity* UIOrderScreen) {
        if (!UIOrderScreen) return *UIOrderScreen;

        auto& session = UIOrderScreen->getComponent<OrderSession>();
        session.walletRef = &wallet.balance;
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

        // --- UPDATE PAYMENT WARNING ---
        if (session.paymentWarningLabelRef) {
            auto& warnLbl = session.paymentWarningLabelRef->getComponent<Label>();

            if (paymentDueToday > 0) {
                warnLbl.text = "Debt Paid Today: " + std::to_string(-paymentDueToday) + "G";
                warnLbl.visible = true;
            } else {
                warnLbl.text = " ";
                warnLbl.visible = false;
            }

            warnLbl.dirty = true;
            TextureManager::updateLabel(warnLbl);
        }

        // Populate slots
        for (int i = 0; i < (int)session.slots.size(); ++i) {
            auto& slot = session.slots[i];
            bool hasItem = i < (int)availableItems.size();

            slot.icon->getComponent<Sprite>().visible = hasItem;
            if (hasItem) slot.icon->getComponent<Sprite>().src = availableItems[i].src;

            auto& nameLbl = slot.nameLabel->getComponent<Label>();
            nameLbl.visible = hasItem;
            if (hasItem) {
                int held = 0;
                for (auto& entry : inv.items) {
                    if (entry.item.id == availableItems[i].id) {
                        held = entry.quantity;
                        break;
                    }
                }
                nameLbl.text = availableItems[i].name;
                nameLbl.text += " (" + std::to_string(held) + ")";
                nameLbl.dirty = true;
                TextureManager::updateLabel(nameLbl);
            }

            auto& priceLbl = slot.priceLabel->getComponent<Label>();
            priceLbl.visible = hasItem;
            if (hasItem) {
                float trendMod = scene.world.getMarketTrendSystem().getModifier(availableItems[i]);
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

                priceLbl.dirty = true;
                TextureManager::updateLabel(priceLbl);
            }

            slot.buyBtn->getComponent<Sprite>().visible = hasItem;

            if (hasItem) {
                ItemDef item = availableItems[i];
                auto& bClick = slot.buyBtn->getComponent<Clickable>();

                Entity* btnPtr = slot.buyBtn;
                Wallet* walletPtr = &wallet;
                Inventory* invPtr = &inv;

                bClick.onReleased = [&scene, item, btnPtr, walletPtr, invPtr]() {
                    btnPtr->getComponent<Transform>().scale = 1.0f;
                    scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));

                    float trendMod = scene.world.getMarketTrendSystem().getModifier(item);
                    int trendPrice = static_cast<int>(item.basePrice * trendMod);

                    if (walletPtr->balance < trendPrice) return;

                    walletPtr->balance -= trendPrice;
                    walletPtr->dailyExpenses += trendPrice;

                    invPtr->addItem(item, 1);

                    auto& s = scene.UIOrderScreen->getComponent<OrderSession>();

                    if (s.walletLabelRef) {
                        auto& lbl = s.walletLabelRef->getComponent<Label>();
                        lbl.text = "Wallet: " + std::to_string(walletPtr->balance) + "G";
                        lbl.dirty = true;
                        TextureManager::updateLabel(lbl);
                    }

                    for (int j = 0; j < (int)s.slots.size() && j < (int)s.currentItems.size(); ++j) {
                        int heldNow = 0;
                        for (auto& entry : invPtr->items) {
                            if (entry.item.id == s.currentItems[j].id) { heldNow = entry.quantity; break; }
                        }
                        auto& nLbl = s.slots[j].nameLabel->getComponent<Label>();
                        nLbl.text = s.currentItems[j].name + " (" + std::to_string(heldNow) + ")";
                        nLbl.dirty = true;
                        TextureManager::updateLabel(nLbl);
                    }

                    for (int j = 0; j < (int)s.slots.size() && j < (int)s.currentItems.size(); ++j) {
                        float jTrendMod = scene.world.getMarketTrendSystem().getModifier(s.currentItems[j]);
                        int jTrendPrice = static_cast<int>(s.currentItems[j].basePrice * jTrendMod);
                        bool afford = walletPtr->balance >= jTrendPrice;

                        s.slots[j].buyBtn->getComponent<Collider>().enabled = afford;

                        auto& pLbl = s.slots[j].priceLabel->getComponent<Label>();
                        pLbl.text = std::to_string(jTrendPrice) + "G";

                        if (!afford) {
                            pLbl.color = {180, 50, 50, 255};
                        } else if (jTrendMod > 1.0f) {
                            pLbl.color = {76, 175, 80, 255};
                        } else if (jTrendMod < 1.0f) {
                            pLbl.color = {211, 47, 47, 255};
                        } else {
                            pLbl.color = {0, 0, 0, 255};
                        }
                        pLbl.dirty = true;
                        TextureManager::updateLabel(pLbl);
                    }

                    if (s.shelfBuyBtn && s.shelfPriceLabel) {
                        bool nowMaxed = Game::gameState.displayCasesUnlocked >= 15;
                        bool canStillAfford = !nowMaxed && (walletPtr->balance >= s.currentShelfPrice);

                        s.shelfBuyBtn->getComponent<Collider>().enabled = canStillAfford;

                        auto& shelfLbl = s.shelfPriceLabel->getComponent<Label>();
                        shelfLbl.text = nowMaxed ? "Maxed Out" : (std::to_string(s.currentShelfPrice) + "G");
                        shelfLbl.color = canStillAfford ? SDL_Color{0,0,0,255} : SDL_Color{180,50,50,255};
                        shelfLbl.dirty = true;
                        TextureManager::updateLabel(shelfLbl);
                    }
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

            Entity* sBtnPtr = session.shelfBuyBtn;

            sClick.onReleased = [&scene, sBtnPtr]() {
                auto& s = scene.UIOrderScreen->getComponent<OrderSession>();
                sBtnPtr->getComponent<Transform>().scale = 1.0f;
                scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));

                if (s.onBuyShelf) s.onBuyShelf();

                if (s.getShelfPrice) {
                    s.currentShelfPrice = s.getShelfPrice(Game::gameState.displayCasesUnlocked);
                }

                if (s.walletLabelRef && s.walletRef) {
                    auto& lbl = s.walletLabelRef->getComponent<Label>();
                    lbl.text = "Wallet: " + std::to_string(*s.walletRef) + "G";
                    lbl.dirty = true;
                    TextureManager::updateLabel(lbl);
                }

                int currentWallet = s.walletRef ? *s.walletRef : 0;

                for (int j = 0; j < (int)s.slots.size() && j < (int)s.currentItems.size(); ++j) {
                    float jTrendMod = scene.world.getMarketTrendSystem().getModifier(s.currentItems[j]);
                    int jTrendPrice = static_cast<int>(s.currentItems[j].basePrice * jTrendMod);
                    bool afford = currentWallet >= jTrendPrice;

                    s.slots[j].buyBtn->getComponent<Collider>().enabled = afford;

                    auto& pLbl = s.slots[j].priceLabel->getComponent<Label>();
                    pLbl.text = std::to_string(jTrendPrice) + "G";

                    if (!afford) {
                        pLbl.color = {180, 50, 50, 255};
                    } else if (jTrendMod > 1.0f) {
                        pLbl.color = {76, 175, 80, 255};
                    } else if (jTrendMod < 1.0f) {
                        pLbl.color = {211, 47, 47, 255};
                    } else {
                        pLbl.color = {0, 0, 0, 255};
                    }
                    pLbl.dirty = true;
                    TextureManager::updateLabel(pLbl);
                }

                if (s.shelfBuyBtn && s.shelfPriceLabel) {
                    bool nowMaxed = Game::gameState.displayCasesUnlocked >= 15;
                    bool canStillAfford = !nowMaxed && (currentWallet >= s.currentShelfPrice);

                    s.shelfBuyBtn->getComponent<Collider>().enabled = canStillAfford;

                    auto& pLbl = s.shelfPriceLabel->getComponent<Label>();
                    pLbl.text = nowMaxed ? "Maxed Out" : (std::to_string(s.currentShelfPrice) + "G");
                    pLbl.color = canStillAfford ? SDL_Color{0,0,0,255} : SDL_Color{180,50,50,255};
                    pLbl.dirty = true;
                    TextureManager::updateLabel(pLbl);
                }
            };
        }


        scene.world.getUIVisibilityManager().show("order");
        scene.world.getUIVisibilityManager().hide("hud");

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
}