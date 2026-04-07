//
// Created by Curry on 2026-04-07.
//

#include "InventoryUI.h"
#include "BaseUI.h"
#include "../manager/AssetManager.h"

namespace InventoryUI {
    Entity &createGrid(Scene &scene, int windowWidth, int windowHeight, Entity *&outUIInventory) {
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;

        auto &session = mainOverlay.addComponent<InventorySession>();
        auto &overlayTransform = mainOverlay.getComponent<Transform>();
        auto &overlaySprite = mainOverlay.getComponent<Sprite>();

        float baseX = overlayTransform.position.x;
        float baseY = overlayTransform.position.y;
        float menuWidth = overlaySprite.dst.w;

        auto &gridContainer = scene.world.createEntity();
        gridContainer.addComponent<Transform>(Vector2D(baseX, baseY), 0.0f, 1.0f);
        gridContainer.addComponent<Children>();
        gridContainer.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&gridContainer);
        session.gridContainerRef = &gridContainer;

        // --- 1. TITLE ---
        auto &titleLabel = scene.world.createEntity();
        Label tData = {
            "Stockroom Inventory", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "invTitle"
        };
        tData.dirty = true;
        tData.visible = false;
        TextureManager::updateLabel(titleLabel.addComponent<Label>(tData));
        titleLabel.addComponent<Transform>(
            Vector2D(baseX + (menuWidth / 2) - (titleLabel.getComponent<Label>().dst.w / 2), baseY + 30.0f), 0.0f,
            1.0f);
        titleLabel.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&titleLabel);

        // --- 1.5 NEW: RETURN BUTTON (Top Left) ---
        // 1. Load your specific sprite (or your main sprite sheet if it's packed in there)
        SDL_Texture *buttonTex = TextureManager::load("../asset/ui/Buttons.png");

        auto &returnBtn = scene.world.createEntity();

        // 2. Adjust these to match the actual pixel dimensions of your sprite
        float rBtnW = 128.0f; // Width of your button on screen
        float rBtnH = 32.0f; // Height of your button on screen
        float rBtnX = baseX + 20.0f;
        float rBtnY = baseY + 20.0f;

        auto &rBtnTransform = returnBtn.addComponent<Transform>(Vector2D(rBtnX, rBtnY), 0.0f, 1.0f);

        // 3. Set the source rect to match where the graphic is on the texture
        SDL_FRect returnSrc{0, 48, 64, 16}; // Update to your sprite's source coordinates
        SDL_FRect returnDst{rBtnX, rBtnY, rBtnW, rBtnH};

        returnBtn.addComponent<Sprite>(buttonTex, returnSrc, returnDst, RenderLayer::UI, false);
        returnBtn.addComponent<Collider>("ui", returnDst).enabled = false;

        // NOTE: Removed the Label entity entirely!

        auto &rClick = returnBtn.addComponent<Clickable>();
        Entity *rBtnPtr = &returnBtn;
        rClick.onPressed = [rBtnPtr] { rBtnPtr->getComponent<Transform>().scale = 0.9f; };
        rClick.onCancel = [rBtnPtr] { rBtnPtr->getComponent<Transform>().scale = 1.0f; };
        rClick.onReleased = [&scene, rBtnPtr]() {
            rBtnPtr->getComponent<Transform>().scale = 1.0f;
            scene.world.getUIVisibilityManager().hide("inventory");
            scene.world.getUIVisibilityManager().show("hud");
        };

        returnBtn.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&returnBtn);

        // --- Stock Indicator ---
        float stockX = baseX + menuWidth - 160.0f;
        float stockY = baseY + 24.0f;

        auto &stockIcon = scene.world.createEntity();
        stockIcon.addComponent<Transform>(Vector2D(stockX, stockY), 0.0f, 1.0f);
        SDL_Texture *itemsTex = TextureManager::load("../asset/items.png");
        stockIcon.addComponent<Sprite>(itemsTex, SDL_FRect{0, 0, 32, 32}, SDL_FRect{stockX, stockY, 32, 32},
                                       RenderLayer::UI, false).visible = false;
        stockIcon.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&stockIcon);
        session.targetStockIconRef = &stockIcon;

        auto &stockLabel = scene.world.createEntity();
        Label stkData = {
            "Empty (0/4)", AssetManager::getFont("arial-small"), {0, 0, 0, 255}, LabelType::Static, "invStockLbl"
        };
        auto &stkComp = stockLabel.addComponent<Label>(stkData);
        stkComp.visible = false;
        TextureManager::updateLabel(stkComp);

        stockLabel.addComponent<Transform>(Vector2D(stockX + 36.0f, stockY + 6.0f), 0.0f, 1.0f);
        stockLabel.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&stockLabel);
        session.targetStockLabelRef = &stockLabel;

        // --- 1.6 RETRIEVE ALL BUTTON (-) ---
        auto &retrieveBtn = scene.world.createEntity();

        // 2. Adjust dimensions
        float retBtnW = 64.0f;
        float retBtnH = 32.0f;
        float retBtnX = stockX - retBtnW - 15.0f;

        auto &retBtnTransform = retrieveBtn.addComponent<Transform>(Vector2D(retBtnX, stockY), 0.0f, 1.0f);

        // 3. Set the source rect
        SDL_FRect retrieveSrc{32, 16, 32, 16}; // Update to your sprite's source coordinates
        SDL_FRect retrieveDst{retBtnX, stockY, retBtnW, retBtnH};

        retrieveBtn.addComponent<Sprite>(buttonTex, retrieveSrc, retrieveDst, RenderLayer::UI, false);
        retrieveBtn.addComponent<Collider>("ui", retrieveDst).enabled = false;

        auto &retClick = retrieveBtn.addComponent<Clickable>();
        Entity *retBtnPtr = &retrieveBtn;
        retClick.onPressed = [retBtnPtr] { retBtnPtr->getComponent<Transform>().scale = 0.9f; };
        retClick.onCancel = [retBtnPtr] { retBtnPtr->getComponent<Transform>().scale = 1.0f; };
        retClick.onReleased = [&scene, retBtnPtr]() {
            retBtnPtr->getComponent<Transform>().scale = 1.0f;
            scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));
            auto &s = scene.UIInventory->getComponent<InventorySession>();

            if (s.mode == InventoryMode::PlaceItem && s.currentStand && s.currentStand->quantity > 0) {
                // 1. Get the player's inventory
                auto &playerInv = scene.playerEntity->getComponent<Inventory>();

                // 2. Add the items from the display stand back to the player
                playerInv.addItem(s.currentStand->item, s.currentStand->quantity);

                //3. Clear the stand
                s.currentStand->quantity = 0;

                //Update Sprite
                if (s.currentStand->owner) {
                    auto &standSprite = s.currentStand->owner->getComponent<Sprite>();
                    standSprite.src = s.currentStand->emptySrc;
                }
                //Sfx for picking up items
                scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("place"));
                scene.world.getUIVisibilityManager().hide("inventory");
                scene.world.getUIVisibilityManager().show("hud");
            }
        };

        retrieveBtn.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&retrieveBtn);
        session.retrieveAllBtnRef = &retrieveBtn;

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
                auto &container = scene.world.createEntity();
                container.addComponent<Transform>(Vector2D(slotX, slotY), 0.0f, 1.0f);
                container.addComponent<Children>();
                slotRefs.container = &container;

                // Icon

                auto &iconEnt = scene.world.createEntity();
                auto &iconTransform = iconEnt.addComponent<Transform>(
                    Vector2D(slotX + iconOffsetX, slotY + iconOffsetY),
                    0.0f, 1.0f);
                SDL_FRect iconDst = {slotX + iconOffsetX, slotY + iconOffsetY, iconSize, iconSize};
                iconEnt.addComponent<Sprite>(itemsTex, SDL_FRect{0, 0, 32, 32}, iconDst, RenderLayer::UI, false).visible
                        =
                        false;
                slotRefs.icon = &iconEnt;
                iconEnt.addComponent<Parent>(&container);
                container.getComponent<Children>().children.push_back(&iconEnt);

                // Label (Vertical stacking: Text sits below the icon for 4x4)
                auto &labelEnt = scene.world.createEntity();
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

                iconClick.onPressed = [&scene, iconPtr, idx]() {
                    auto &s = scene.UIInventory->getComponent<InventorySession>();
                    if (s.mode != InventoryMode::PlaceItem) return;

                    // 2. Only shrink if the item exists and is in stock
                    if (idx < s.cachedInventory.size() && s.cachedInventory[idx].quantity > 0) {
                        iconPtr->getComponent<Transform>().scale = 0.9f;
                    }
                };

                iconClick.onCancel = [&scene, iconPtr, idx]() {
                    auto &s = scene.UIInventory->getComponent<InventorySession>();
                    if (s.mode != InventoryMode::PlaceItem) return;

                    // 3. Only grow back if it was allowed to shrink in the first place
                    if (idx < s.cachedInventory.size() && s.cachedInventory[idx].quantity > 0) {
                        iconPtr->getComponent<Transform>().scale = 1.0f;
                    }
                };

                iconClick.onReleased = [&scene, iconPtr, idx]() {
                    auto &s = scene.UIInventory->getComponent<InventorySession>();
                    scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickSoft"));

                    if (s.mode != InventoryMode::PlaceItem) return;

                    if (idx < s.cachedInventory.size()) {
                        auto &entry = s.cachedInventory[idx];

                        if (entry.quantity > 0) {
                            // Reset the scale back to normal before changing screens
                            iconPtr->getComponent<Transform>().scale = 1.0f;

                            // 1. Hide the Inventory Screen
                            scene.world.getUIVisibilityManager().hide("inventory");

                            int maxToPlace = std::min(entry.quantity, 4);

                            // 2. Open the completely separate Quantity Screen
                            InventoryUI::openQuantityScreen(
                                scene,
                                entry,
                                maxToPlace,
                                s.quantitySession.onConfirm,
                                [&scene]() {
                                    auto &session = scene.UIInventory->getComponent<InventorySession>();
                                    InventoryUI::updateGrid(scene, session.cachedInventory, session.mode,
                                                            session.currentStand, scene.UIInventory);
                                },
                                scene.UIQuantityScreen
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

        outUIInventory = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("inventory", outUIInventory);
        return mainOverlay;
    }

    Entity &updateGrid(Scene &scene, const std::vector<InventoryEntry> &inventoryData, InventoryMode mode,
                       DisplayStand *targetStand, Entity *UIInventory) {
        if (!UIInventory) return *UIInventory;
        auto &session = UIInventory->getComponent<InventorySession>();
        session.currentStand = targetStand;
        session.mode = mode;

        // 1. FORCE OPEN FIRST! Let the manager make everything visible.
        // (Make sure you register this panel as "inventory" in createInventoryUI!)
        scene.world.getUIVisibilityManager().show("inventory");
        scene.world.getUIVisibilityManager().hide("hud");

        // 2. Sort the entire 16-item list
        auto sortedInv = inventoryData;
        std::sort(sortedInv.begin(), sortedInv.end(), [](const InventoryEntry &a, const InventoryEntry &b) {
            if (a.item.type != b.item.type) return static_cast<int>(a.item.type) < static_cast<int>(b.item.type);
            return a.item.basePrice > b.item.basePrice;
        });
        session.cachedInventory = sortedInv;

        // 3. Map to the 16 slots (This will now safely hide empty slots)
        for (size_t i = 0; i < session.slots.size(); ++i) {
            auto &slot = session.slots[i];
            if (i < sortedInv.size()) {
                const auto &entry = sortedInv[i];
                auto &label = slot.label->getComponent<Label>();
                auto &iconSprite = slot.icon->getComponent<Sprite>();

                iconSprite.src = entry.item.src;
                iconSprite.visible = true;
                label.visible = true;
                label.text = entry.item.name + " x" + std::to_string(entry.quantity);
                label.color = (entry.quantity <= 0) ? SDL_Color{120, 120, 120, 255} : SDL_Color{0, 0, 0, 255};
                label.dirty = true;
                TextureManager::updateLabel(label);

                float iconX = slot.icon->getComponent<Transform>().position.x;
                label.dst.x = (iconX + 24.0f) - (label.dst.w / 2.0f);

                // Set Clickables
                bool isClickable = (entry.quantity > 0 && session.mode == InventoryMode::PlaceItem);
                if (slot.icon->hasComponent<Collider>()) {
                    slot.icon->getComponent<Collider>().enabled = isClickable;
                }
            } else {
                // Hide the extra slots
                slot.icon->getComponent<Sprite>().visible = false;
                slot.label->getComponent<Label>().visible = false;
                if (slot.icon->hasComponent<Collider>()) slot.icon->getComponent<Collider>().enabled = false;
            }
        }

        // 4. APPLY VISIBILITY OVERRIDES (This will now safely hide the button!)
        if (session.targetStockIconRef && session.targetStockLabelRef) {
            auto &iconSprite = session.targetStockIconRef->getComponent<Sprite>();
            auto &labelComp = session.targetStockLabelRef->getComponent<Label>();

            bool shelfHasItems = (mode == InventoryMode::PlaceItem && targetStand != nullptr && targetStand->quantity >
                                  0);

            if (mode == InventoryMode::PlaceItem && targetStand != nullptr) {
                labelComp.visible = true;

                if (shelfHasItems) {
                    iconSprite.src = targetStand->item.src;
                    iconSprite.visible = true;
                    labelComp.text = "x" + std::to_string(targetStand->quantity) + " / 4";
                } else {
                    iconSprite.visible = false;
                    labelComp.text = "Empty (0/4)";
                }

                labelComp.dirty = true;
                TextureManager::updateLabel(labelComp);
            } else {
                iconSprite.visible = false;
                labelComp.visible = false;
            }

            // Apply Retrieve Button State
            if (session.retrieveAllBtnRef) {
                session.retrieveAllBtnRef->getComponent<Sprite>().visible = shelfHasItems;
                if (session.retrieveAllBtnRef->hasComponent<Collider>()) {
                    session.retrieveAllBtnRef->getComponent<Collider>().enabled = shelfHasItems;
                }
            }
        }

        return *UIInventory;
    }

    Entity &createQuantityScreen(Scene &scene, int windowWidth, int windowHeight, Entity *&outUIQuantityScreen) {
        // 1. Create a fresh base menu overlay
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;

        auto &session = mainOverlay.addComponent<QuantityScreenSession>();
        auto &overlaySprite = mainOverlay.getComponent<Sprite>();
        auto &overlayTransform = mainOverlay.getComponent<Transform>();

        float baseX = overlayTransform.position.x;
        float baseY = overlayTransform.position.y;
        float centerX = baseX + (overlaySprite.dst.w / 2.0f);

        // --- HEADER (Icon & Name) ---
        auto &iconEnt = scene.world.createEntity();
        iconEnt.addComponent<Transform>(Vector2D(centerX - 80.0f, baseY + 80.0f), 0.0f, 1.0f);
        SDL_Texture *itemsTex = TextureManager::load("../asset/items.png");
        iconEnt.addComponent<Sprite>(itemsTex, SDL_FRect{0, 0, 32, 32}, SDL_FRect{0, 0, 48, 48}, RenderLayer::UI,
                                     false);
        iconEnt.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&iconEnt);
        session.itemIconRef = &iconEnt;

        auto &nameEnt = scene.world.createEntity();
        Label nameData = {
            "Item Name", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "qtyScreenName"
        };
        TextureManager::updateLabel(nameEnt.addComponent<Label>(nameData));
        nameEnt.addComponent<Transform>(Vector2D(centerX - 20.0f, baseY + 90.0f), 0.0f, 1.0f);
        nameEnt.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&nameEnt);
        session.itemNameRef = &nameEnt;

        // --- PROMPT ---
        auto &promptEnt = scene.world.createEntity();
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
            auto &btn = scene.world.createEntity();
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
            bClick.onReleased = [&scene, q, btnPtr]() {
                btnPtr->getComponent<Transform>().scale = 1.0f;
                auto &s = scene.UIQuantityScreen->getComponent<QuantityScreenSession>();

                scene.world.getUIVisibilityManager().hide("quantity");

                if (s.onConfirm) {
                    s.onConfirm(s.selectedItem, q);
                    scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("place"));
                }

                scene.world.getUIVisibilityManager().show("hud");
            };

            btn.addComponent<Parent>(&mainOverlay);
            mainOverlay.getComponent<Children>().children.push_back(&btn);
            session.quantityButtonRefs.push_back(&btn);
        }

        // --- BACK/CANCEL BUTTON ---
        auto &returnBtn = scene.world.createEntity();
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
        rClick.onReleased = [&scene, rBtnPtr]() {
            rBtnPtr->getComponent<Transform>().scale = 1.0f;
            scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickSoft"));

            // Hide this screen
            scene.world.getUIVisibilityManager().hide("quantity");

            // Trigger cancel callback (which will reopen the inventory)
            auto &s = scene.UIQuantityScreen->getComponent<QuantityScreenSession>();
            if (s.onCancel) s.onCancel();
        };

        returnBtn.addComponent<Parent>(&mainOverlay);
        mainOverlay.getComponent<Children>().children.push_back(&returnBtn);

        outUIQuantityScreen = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("quantity", outUIQuantityScreen);
        return mainOverlay;
    }

    void openQuantityScreen(Scene &scene, const InventoryEntry &item, int maxQty,
                            std::function<void(InventoryEntry, int)> onConfirm, std::function<void()> onCancel,
                            Entity *UIQuantityScreen) {
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
        scene.world.getUIVisibilityManager().show("quantity");
        scene.world.getUIVisibilityManager().hide("hud");

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
}
