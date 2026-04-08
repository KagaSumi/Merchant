//
// Created by Curry on 2026-04-07.
//

#include "InventoryUI.h"
#include "BaseUI.h"
#include "../manager/AssetManager.h"
#include <algorithm>

namespace {
    // ==========================================
    // 1. PRIVATE LAYOUT BUILDERS
    // ==========================================

    void buildInventoryHeader(Scene& scene, Entity& overlay, float baseX, float baseY, float menuWidth) {
        // Title - Standard Arial
        auto& title = BaseUI::createLabel(scene, overlay, "Stockroom Inventory", Vector2D(0, baseY + 30.0f), {0,0,0,255}, "invTitle", true);
        title.getComponent<Transform>().position.x = baseX + (menuWidth / 2.0f) - (title.getComponent<Label>().dst.w / 2.0f);

        // Return Button
        BaseUI::createStandardButton(scene, overlay, Vector2D(baseX + 20.0f, baseY + 20.0f), 128.0f, 32.0f, {0, 48, 64, 16}, [&scene]() {
            scene.world.getUIVisibilityManager().hide("inventory");
            scene.world.getUIVisibilityManager().show("hud");
        });
    }
    void buildQuantityHeader(Scene& scene, Entity& overlay, QuantityScreenSession& session, float centerX, float baseY) {
        SDL_Texture *itemsTex = TextureManager::load("../asset/items.png");

        // --- ITEM ICON (32x32) ---
        auto& icon = scene.world.createEntity();
        icon.addComponent<Transform>(Vector2D(centerX - 80.0f, baseY + 80.0f), 0.0f, 1.0f);
        // Using 48x48 for the "Preview" icon so it stands out, but keeping the 32x32 source
        icon.addComponent<Sprite>(itemsTex, SDL_FRect{0,0,32,32}, SDL_FRect{0,0,64,64}, RenderLayer::UI, false);
        icon.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&icon);
        session.itemIconRef = &icon;

        // --- ITEM NAME ---
        session.itemNameRef = &BaseUI::createLabel(scene, overlay, "Item Name",
            Vector2D(centerX - 20.0f, baseY + 100.0f), {0,0,0,255}, "qtyName", true);

        // --- PROMPT ---
        auto& prompt = BaseUI::createLabel(scene, overlay, "How many to place?",
            Vector2D(0, baseY + 200.0f), {0,0,0,255}, "qtyPrompt", true);

        prompt.getComponent<Label>().font = AssetManager::getFont("arial-small");
        prompt.getComponent<Transform>().position.x = centerX - (prompt.getComponent<Label>().dst.w / 2.0f);
    }

    void buildStockIndicator(Scene& scene, Entity& overlay, InventorySession& session, float baseX, float baseY, float menuWidth) {
        float stockX = baseX + menuWidth - 160.0f;
        float stockY = baseY + 24.0f;

        // Shelf Icon (32x32)
        auto &stockIcon = scene.world.createEntity();
        stockIcon.addComponent<Transform>(Vector2D(stockX, stockY), 0.0f, 1.0f);
        SDL_Texture *itemsTex = TextureManager::load("../asset/items.png");
        stockIcon.addComponent<Sprite>(itemsTex, SDL_FRect{0, 0, 32, 32}, SDL_FRect{stockX, stockY, 32, 32}, RenderLayer::UI, false).visible = false;
        stockIcon.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&stockIcon);
        session.targetStockIconRef = &stockIcon;

        // Shelf Label - Small Font
        session.targetStockLabelRef = &BaseUI::createLabel(scene, overlay, "Empty (0/4)", Vector2D(stockX + 36.0f, stockY + 6.0f), {0,0,0,255}, "invStockLbl", true);
        session.targetStockLabelRef->getComponent<Label>().font = AssetManager::getFont("arial-small");

        // Retrieve Button
        session.retrieveAllBtnRef = &BaseUI::createStandardButton(scene, overlay, Vector2D(stockX - 79.0f, stockY), 64.0f, 32.0f, {32, 16, 32, 16}, [&scene]() {
            scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));
            auto &s = scene.UIInventory->getComponent<InventorySession>();
            if (s.mode == InventoryMode::PlaceItem && s.currentStand && s.currentStand->quantity > 0) {
                scene.playerEntity->getComponent<Inventory>().addItem(s.currentStand->item, s.currentStand->quantity);
                s.currentStand->quantity = 0;
                if (s.currentStand->owner) s.currentStand->owner->getComponent<Sprite>().src = s.currentStand->emptySrc;
                scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("place"));
                scene.world.getUIVisibilityManager().hide("inventory");
                scene.world.getUIVisibilityManager().show("hud");
            }
        });
    }

    void buildSlotGrid(Scene& scene, Entity& overlay, InventorySession& session, float baseX, float baseY, float menuWidth, float menuHeight) {
        float padding = 40.0f;
        float gridStartX = baseX + padding;
        float gridStartY = baseY + 100.0f; // More spacing for title
        float colSpacing = (menuWidth - (padding * 2.0f)) / 4.0f;
        float rowSpacing = (menuHeight - (gridStartY - baseY) - padding) / 4.0f;
        SDL_Texture *itemsTex = TextureManager::load("../asset/items.png");

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                int idx = row * 4 + col;
                float slotX = gridStartX + (col * colSpacing);
                float slotY = gridStartY + (row * rowSpacing);

                float iconSize = 64.0f;
                float iconOffX = (colSpacing - iconSize) / 2.0f;

                InventorySlotRefs slotRefs;
                auto &container = scene.world.createEntity();
                container.addComponent<Transform>(Vector2D(slotX, slotY), 0.0f, 1.0f);
                container.addComponent<Children>();
                container.addComponent<Parent>(&overlay);
                overlay.getComponent<Children>().children.push_back(&container);
                slotRefs.container = &container;

                // Icon / Clickable
                auto &iconEnt = scene.world.createEntity();
                auto &iconTrans = iconEnt.addComponent<Transform>(Vector2D(slotX + iconOffX, slotY), 0.0f, 1.0f);

                SDL_FRect iconDst = {slotX + iconOffX, slotY, iconSize, iconSize};
                iconEnt.addComponent<Sprite>(itemsTex, SDL_FRect{0,0,32,32}, iconDst, RenderLayer::UI, false).visible = false;
                iconEnt.addComponent<Collider>("ui", iconDst).enabled = false;

                auto &iconClick = iconEnt.addComponent<Clickable>();
                iconClick.onPressed = [&scene, &iconTrans, idx]() {
                    auto &s = scene.UIInventory->getComponent<InventorySession>();
                    if (s.mode == InventoryMode::PlaceItem && idx < s.cachedInventory.size() && s.cachedInventory[idx].quantity > 0)
                        iconTrans.scale = 0.9f;
                };
                iconClick.onCancel = [&iconTrans] { iconTrans.scale = 1.0f; };
                iconClick.onReleased = [&scene, &iconTrans, idx]() {
                    iconTrans.scale = 1.0f;
                    scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickSoft"));
                    auto &s = scene.UIInventory->getComponent<InventorySession>();
                    if (s.mode == InventoryMode::PlaceItem && idx < s.cachedInventory.size() && s.cachedInventory[idx].quantity > 0) {
                        scene.world.getUIVisibilityManager().hide("inventory");
                        InventoryUI::openQuantityScreen(scene, s.cachedInventory[idx], std::min(s.cachedInventory[idx].quantity, 4), s.quantitySession.onConfirm, [&scene]() {
                            auto &session = scene.UIInventory->getComponent<InventorySession>();
                            InventoryUI::updateGrid(scene, session.cachedInventory, session.mode, session.currentStand, scene.UIInventory);
                        }, scene.UIQuantityScreen);
                    }
                };

                iconEnt.addComponent<Parent>(&container);
                container.getComponent<Children>().children.push_back(&iconEnt);
                slotRefs.icon = &iconEnt;

                // Label - Uses arial-small
                slotRefs.label = &BaseUI::createLabel(scene, container, "Empty", Vector2D(slotX, slotY + iconSize + 8.0f), {0,0,0,255}, "inv_" + std::to_string(idx), false);
                slotRefs.label->getComponent<Label>().font = AssetManager::getFont("arial-small");

                session.slots.push_back(slotRefs);
            }
        }
    }
}

namespace InventoryUI {
    Entity &createGrid(Scene &scene, int windowWidth, int windowHeight, Entity *&outUIInventory) {
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;
        mainOverlay.addComponent<Children>();

        auto &session = mainOverlay.addComponent<InventorySession>();
        float baseX = mainOverlay.getComponent<Transform>().position.x;
        float baseY = mainOverlay.getComponent<Transform>().position.y;
        float menuW = mainOverlay.getComponent<Sprite>().dst.w;
        float menuH = mainOverlay.getComponent<Sprite>().dst.h;

        buildInventoryHeader(scene, mainOverlay, baseX, baseY, menuW);
        buildStockIndicator(scene, mainOverlay, session, baseX, baseY, menuW);
        buildSlotGrid(scene, mainOverlay, session, baseX, baseY, menuW, menuH);

        outUIInventory = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("inventory", outUIInventory);
        return mainOverlay;
    }

    Entity &updateGrid(Scene &scene, const std::vector<InventoryEntry> &inventoryData, InventoryMode mode, DisplayStand *targetStand, Entity *UIInventory) {
        if (!UIInventory) return *UIInventory;
        auto &session = UIInventory->getComponent<InventorySession>();
        session.currentStand = targetStand;
        session.mode = mode;

        scene.world.getUIVisibilityManager().show("inventory");
        scene.world.getUIVisibilityManager().hide("hud");

        auto sortedInv = inventoryData;
        std::sort(sortedInv.begin(), sortedInv.end(), [](const InventoryEntry &a, const InventoryEntry &b) {
            if (a.item.type != b.item.type) return static_cast<int>(a.item.type) < static_cast<int>(b.item.type);
            return a.item.basePrice > b.item.basePrice;
        });
        session.cachedInventory = sortedInv;

        float menuW = UIInventory->getComponent<Sprite>().dst.w;
        float padding = 40.0f;
        float colSpacing = (menuW - (padding * 2.0f)) / 4.0f;

        for (size_t i = 0; i < session.slots.size(); ++i) {
            auto &slot = session.slots[i];
            bool hasData = i < sortedInv.size();
            auto &iconSprite = slot.icon->getComponent<Sprite>();
            auto &label = slot.label->getComponent<Label>();

            iconSprite.visible = hasData;
            label.visible = hasData;

            if (hasData) {
                const auto &entry = sortedInv[i];
                iconSprite.src = entry.item.src;
                label.text = entry.item.name + " x" + std::to_string(entry.quantity);
                label.color = (entry.quantity <= 0) ? SDL_Color{120, 120, 120, 255} : SDL_Color{0, 0, 0, 255};
                label.dirty = true;
                TextureManager::updateLabel(label);

                // Precise Centering: Column Center - Half Text Width
                float colX = slot.container->getComponent<Transform>().position.x;
                float colCenter = colX + (colSpacing / 2.0f);
                slot.label->getComponent<Transform>().position.x = colCenter - (label.dst.w / 2.0f);

                slot.icon->getComponent<Collider>().enabled = (entry.quantity > 0 && mode == InventoryMode::PlaceItem);
            } else {
                iconSprite.src = {0,0,0,0};
            }
        }

        // Shelf UI Logic (Indicator + Retrieve Button)
        if (session.targetStockLabelRef) {
            bool isPlacingMode = (mode == InventoryMode::PlaceItem && targetStand != nullptr);
            session.targetStockLabelRef->getComponent<Label>().visible = isPlacingMode;

            bool shelfHasItems = isPlacingMode && targetStand->quantity > 0;
            session.targetStockIconRef->getComponent<Sprite>().visible = shelfHasItems;

            if (session.retrieveAllBtnRef) {
                session.retrieveAllBtnRef->getComponent<Sprite>().visible = shelfHasItems;
                session.retrieveAllBtnRef->getComponent<Collider>().enabled = shelfHasItems;
            }

            if (isPlacingMode) {
                auto& lbl = session.targetStockLabelRef->getComponent<Label>();
                if (shelfHasItems) {
                    session.targetStockIconRef->getComponent<Sprite>().src = targetStand->item.src;
                    lbl.text = "x" + std::to_string(targetStand->quantity) + " / 4";
                } else {
                    lbl.text = "Empty (0/4)";
                }
                lbl.dirty = true;
                TextureManager::updateLabel(lbl);
            }
        }

        return *UIInventory;
    }

    Entity &createQuantityScreen(Scene &scene, int windowWidth, int windowHeight, Entity *&outUIQuantityScreen) {
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;
        mainOverlay.addComponent<Children>();

        auto &session = mainOverlay.addComponent<QuantityScreenSession>();
        float baseX = mainOverlay.getComponent<Transform>().position.x;
        float baseY = mainOverlay.getComponent<Transform>().position.y;
        float centerX = baseX + (mainOverlay.getComponent<Sprite>().dst.w / 2.0f);

        buildQuantityHeader(scene, mainOverlay, session, centerX, baseY);

        // Buttons pool
        for (int q = 1; q <= 4; ++q) {
            float srcX = (q % 2 == 1) ? 0.0f : 33.0f;
            float srcY = (q <= 2) ? 64.0f : 80.0f;
            auto& btn = BaseUI::createStandardButton(scene, mainOverlay, {0,0}, 96.0f, 48.0f, {srcX, srcY, 32, 16}, [&scene, q]() {
                auto &s = scene.UIQuantityScreen->getComponent<QuantityScreenSession>();
                scene.world.getUIVisibilityManager().hide("quantity");
                if (s.onConfirm) s.onConfirm(s.selectedItem, q);
                scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("place"));
                scene.world.getUIVisibilityManager().show("hud");
            });
            btn.getComponent<Sprite>().visible = false;
            session.quantityButtonRefs.push_back(&btn);
        }

        BaseUI::createStandardButton(scene, mainOverlay, Vector2D(centerX - 60.0f, baseY + mainOverlay.getComponent<Sprite>().dst.h - 80.0f), 120.0f, 36.0f, {0, 49, 64, 16}, [&scene]() {
            scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickSoft"));
            scene.world.getUIVisibilityManager().hide("quantity");
            auto &s = scene.UIQuantityScreen->getComponent<QuantityScreenSession>();
            if (s.onCancel) s.onCancel();
        });

        outUIQuantityScreen = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("quantity", outUIQuantityScreen);
        return mainOverlay;
    }
    void openQuantityScreen(Scene &scene, const InventoryEntry &item, int maxQty, std::function<void(InventoryEntry, int)> onConfirm, std::function<void()> onCancel, Entity *UIQuantityScreen) {
        if (!UIQuantityScreen) return;
        auto &session = UIQuantityScreen->getComponent<QuantityScreenSession>();
        session.selectedItem = item;
        session.onConfirm = onConfirm;
        session.onCancel = onCancel;

        if (session.itemIconRef) session.itemIconRef->getComponent<Sprite>().src = item.item.src;
        auto &lbl = session.itemNameRef->getComponent<Label>();
        lbl.text = item.item.name + " (Max: " + std::to_string(maxQty) + ")";
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);

        scene.world.getUIVisibilityManager().show("quantity");
        scene.world.getUIVisibilityManager().hide("hud");

        float centerX = UIQuantityScreen->getComponent<Transform>().position.x + (UIQuantityScreen->getComponent<Sprite>().dst.w / 2.0f);
        float containerY = UIQuantityScreen->getComponent<Transform>().position.y + 240.0f;
        int count = std::min(maxQty, 4);
        float spacing = 110.0f;
        float startX = centerX - ((count * spacing - (spacing - 96.0f)) / 2.0f);

        for (int i = 0; i < 4; ++i) {
            auto& btn = *session.quantityButtonRefs[i];
            bool active = i < count;
            btn.getComponent<Sprite>().visible = active;
            btn.getComponent<Collider>().enabled = active;
            if (active) {
                btn.getComponent<Transform>().position = Vector2D(startX + (i * spacing), containerY);
                btn.getComponent<Collider>().rect.x = startX + (i * spacing);
                btn.getComponent<Collider>().rect.y = containerY;
            }
        }
    }
}