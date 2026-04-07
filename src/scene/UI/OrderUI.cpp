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
    TextureManager::updateLabel(tComp);
    titleEnt.addComponent<Transform>(Vector2D(centerX - tComp.dst.w / 2.0f, baseY + 30.0f), 0.0f, 1.0f);
    titleEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&titleEnt);

    // --- WALLET LABEL (top left) ---
    auto& walletEnt = scene.world.createEntity();
    Label wData = {"Wallet: 0G", AssetManager::getFont("arial"), {0,0,0,255}, LabelType::Static, "orderWallet"};
    auto& wComp = walletEnt.addComponent<Label>(wData);
    wComp.dirty = true; wComp.visible = false;
    TextureManager::updateLabel(wComp);
    walletEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, baseY + 40.0f), 0.0f, 1.0f);
    walletEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&walletEnt);
    session.walletLabelRef = &walletEnt;

    // --- PAYMENT WARNING LABEL (below wallet) ---
    auto& warningEnt = scene.world.createEntity();
    Label warnData = {" ", AssetManager::getFont("arial-small"), {211, 47, 47, 255}, LabelType::Static, "orderWarning"};
    auto& warnComp = warningEnt.addComponent<Label>(warnData);
    warnComp.dirty = true;
    warnComp.visible = false; // Hidden by default!
    TextureManager::updateLabel(warnComp);

    // Placed slightly lower on the Y-axis than the wallet label
    warningEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, baseY + 65.0f), 0.0f, 1.0f);
    warningEnt.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&warningEnt);
    session.paymentWarningLabelRef = &warningEnt;

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
            TextureManager::updateLabel(nComp);
            nameEnt.addComponent<Transform>(
                Vector2D(slotX + padding + iconSize + 4.0f, slotY + padding), 0.0f, 1.0f);
            nameEnt.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&nameEnt);
            slot.nameLabel = &nameEnt;

            // Price label
            float btnY = slotY + padding + nComp.dst.h + 4.0f;
            float btnW = 48.0f, btnH = 22.0f;
            auto& priceEnt = scene.world.createEntity();
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
            auto& btnEnt = scene.world.createEntity();
            float btnX = slotX + padding + iconSize + 4.0f;

            auto& btnTransform = btnEnt.addComponent<Transform>(Vector2D(btnX, btnY), 0.0f, 1.0f);
            SDL_FRect btnDst = {btnX, btnY, btnW, btnH};
            btnEnt.addComponent<Sprite>(btnTex, SDL_FRect{0,0,32,16}, btnDst, RenderLayer::UI, false);
            auto& btnCol = btnEnt.addComponent<Collider>("ui", btnDst);
            btnCol.enabled = false;
            slot.buyBtn = &btnEnt;

            int slotIdx = row * cols + col;
            auto& bClick = btnEnt.addComponent<Clickable>();
            bClick.onPressed = [&btnTransform]{ btnTransform.scale = 0.9f; };
            bClick.onCancel = [&btnTransform]{ btnTransform.scale = 1.0f; };
            bClick.onReleased = [&scene, slotIdx, &btnTransform]() {
                btnTransform.scale = 1.0f;
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
    float shelfX = baseX + 40.0f; // Put it on the bottom left
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
    TextureManager::updateLabel(sNameComp);
    shelfName.addComponent<Transform>(Vector2D(shelfX + 45.0f, shelfY + 8.0f), 0.0f, 1.0f);
    shelfName.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&shelfName);

    // Shelf Price Label
    auto& shelfPrice = scene.world.createEntity();
    Label sPriceData = {"500G", AssetManager::getFont("arial-small"), {0,0,0,255}, LabelType::Static, "shelfPrice"};
    auto& sPriceComp = shelfPrice.addComponent<Label>(sPriceData);
    sPriceComp.dirty = true; sPriceComp.visible = false;
    TextureManager::updateLabel(sPriceComp);
    shelfPrice.addComponent<Transform>(Vector2D(shelfX + 45.0f, shelfY + 28.0f), 0.0f, 1.0f);
    shelfPrice.addComponent<Parent>(&mainOverlay);
    mainOverlay.getComponent<Children>().children.push_back(&shelfPrice);
    session.shelfPriceLabel = &shelfPrice;

    // Shelf Buy Button
    auto& sBtnEnt = scene.world.createEntity();
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
    auto& contBtn = scene.world.createEntity();
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
    cClick.onReleased = [&scene, &cTransform]() {
        cTransform.scale = 1.0f;
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

    // --- UPDATE PAYMENT WARNING ---
    if (session.paymentWarningLabelRef) {
        auto& warnLbl = session.paymentWarningLabelRef->getComponent<Label>();

        if (paymentDueToday > 0) {
            // It's payday! Show the warning.
            warnLbl.text = "Debt Paid Today: " + std::to_string(-paymentDueToday) + "G";
            warnLbl.visible = true;
        } else {
            // Not payday, hide the warning.
            warnLbl.text = " "; // <--- ADD THIS LINE TO PHYSICALLY CLEAR IT
            warnLbl.visible = false;
        }

        warnLbl.dirty = true;
        TextureManager::updateLabel(warnLbl);
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

            // Also use trendPrice for affordability check, not basePrice
            priceLbl.dirty = true;
            TextureManager::updateLabel(priceLbl);
        }


        slot.buyBtn->getComponent<Sprite>().visible = hasItem;

        // Wire the buy click with live item data
        if (hasItem) {
            ItemDef item = availableItems[i];
            auto& bClick = slot.buyBtn->getComponent<Clickable>();
            auto &bTransform = slot.buyBtn->getComponent<Transform>();
            bClick.onReleased = [&scene, item, &bTransform, &wallet, &inv]() {
                bTransform.scale = 1.0f;
                scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));
                float trendMod = scene.world.getMarketTrendSystem().getModifier(item);
                int trendPrice = static_cast<int>(item.basePrice * trendMod);

                if (wallet.balance < trendPrice) return;

                // Deduct wallet
                wallet.balance -= trendPrice;
                wallet.dailyExpenses += trendPrice;

                // Add to inventory
                inv.addItem(item, 1);

                auto& s = scene.UIOrderScreen->getComponent<OrderSession>();

                // Refresh wallet label
                if (s.walletLabelRef) {
                    auto& lbl = s.walletLabelRef->getComponent<Label>();
                    lbl.text = "Wallet: " + std::to_string(wallet.balance) + "G";
                    lbl.dirty = true;
                    TextureManager::updateLabel(lbl);
                }

                // Update x1 -> x2 item count
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

                // --- REFRESH EVERYTHING (Items + Shelf) ---
                // 1. Refresh Items
                for (int j = 0; j < (int)s.slots.size() && j < (int)s.currentItems.size(); ++j) {
                    float jTrendMod = scene.world.getMarketTrendSystem().getModifier(s.currentItems[j]);
                    int jTrendPrice = static_cast<int>(s.currentItems[j].basePrice * jTrendMod);
                    bool afford = wallet.balance >= jTrendPrice;

                    s.slots[j].buyBtn->getComponent<Collider>().enabled = afford;

                    auto& pLbl = s.slots[j].priceLabel->getComponent<Label>();
                    pLbl.text = std::to_string(jTrendPrice) + "G";

                    if (!afford) {
                        pLbl.color = {180, 50, 50, 255};   // can't afford — red
                    } else if (jTrendMod > 1.0f) {
                        pLbl.color = {76, 175, 80, 255};   // demand up — green
                    } else if (jTrendMod < 1.0f) {
                        pLbl.color = {211, 47, 47, 255};   // demand down — muted red
                    } else {
                        pLbl.color = {0, 0, 0, 255};       // normal
                    }
                    pLbl.dirty = true;
                    TextureManager::updateLabel(pLbl);
                }

                // 2. Refresh Shelf Upgrade State
                if (s.shelfBuyBtn && s.shelfPriceLabel) {
                    bool nowMaxed = Game::gameState.displayCasesUnlocked >= 15;
                    bool canStillAfford = !nowMaxed && (wallet.balance >= s.currentShelfPrice);

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
        auto &sBtnTransform = session.shelfBuyBtn->getComponent<Transform>();
        sClick.onReleased = [&scene]() {
            auto& s = scene.UIOrderScreen->getComponent<OrderSession>();
            auto& sBtnTransform = s.shelfBuyBtn->getComponent<Transform>();
            sBtnTransform.scale = 1.0f;
            scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));

            if (s.onBuyShelf) s.onBuyShelf();

            // Recalculate price for NEXT shelf after purchase
            if (s.getShelfPrice) {
                s.currentShelfPrice = s.getShelfPrice(Game::gameState.displayCasesUnlocked);
            }

            // Refresh wallet label
            if (s.walletLabelRef && s.walletRef) {
                auto& lbl = s.walletLabelRef->getComponent<Label>();
                lbl.text = "Wallet: " + std::to_string(*s.walletRef) + "G";
                lbl.dirty = true;
                TextureManager::updateLabel(lbl);
            }

            // --- REFRESH EVERYTHING (Items + Shelf) ---
            int currentWallet = s.walletRef ? *s.walletRef : 0;

            // 1. Refresh Items
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

            // 2. Refresh Shelf Upgrade State
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
}