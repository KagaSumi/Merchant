//
// Created by Curry on 2026/04/04.
//
#include "Scene.h"
#include "../manager/AssetManager.h"

Entity &Scene::createDisplaycase(Vector2D location, SDL_Texture *texture, SDL_FRect src, SDL_FRect dst,
                                 DayCycle &dayCycle, Entity *playerRef) {
    auto &displayCase(world.createEntity());
    displayCase.addComponent<Transform>(location, 0.0f, 1.0f);
    displayCase.addComponent<DisplayStand>();
    auto &c = displayCase.addComponent<Collider>("wall");
    c.rect.w = 96;
    c.rect.h = 48;
    c.rect.x = location.x;
    c.rect.y = location.y;
    c.offsetX = -32.0f;
    c.offsetY = 16.0f;
    displayCase.addComponent<Sprite>(texture, src, dst);

    Entity *casePtr = &displayCase;
    DayCycle *cyclePtr = &dayCycle;

    //Add Current location of DisplayCase to browse points in pathfinding..
    PathfindingSystem::AddBrowsePoint({
        static_cast<int>(location.x / 32),
        static_cast<int>(location.y / 32)
    }, &displayCase);

    // 1. CAPTURE THE EMPTY SPRITE SOURCE
    SDL_FRect emptySrc = src;

    // 2. PASS `emptySrc` INTO THIS LAMBDA
    displayCase.addComponent<Interaction>([casePtr, cyclePtr, playerRef, emptySrc, this]() {
        auto &dc = casePtr->getComponent<DisplayStand>();

        if (cyclePtr->currentPhase != DayPhase::Morning) return;
        if (!playerRef || !playerRef->hasComponent<Inventory>()) return;

        auto &inv = playerRef->getComponent<Inventory>();
        auto &session = UIInventory->getComponent<InventorySession>();

        // 3. PASS `emptySrc` INTO THE CONFIRM LAMBDA
        session.quantitySession.onConfirm = [casePtr, playerRef, emptySrc](InventoryEntry entry, int qty) {
            auto &stand = casePtr->getComponent<DisplayStand>();
            auto &sprite = casePtr->getComponent<Sprite>(); // Grab the physical sprite component!
            auto &playerInv = playerRef->getComponent<Inventory>().items;

            // REFUND OLD ITEMS (If the stand isn't empty)
            if (stand.quantity > 0) {
                for (auto &e: playerInv) {
                    if (e.item.name == stand.item.name) {
                        e.quantity += stand.quantity;
                        std::cout << "Auto-Retrieved " << stand.quantity << "x " << stand.item.name << "\n";
                        break;
                    }
                }
            }

            // SET NEW ITEMS IN STAND
            stand.item = entry.item;
            stand.quantity = qty;

            // DEDUCT NEW ITEMS FROM PLAYER
            for (auto &e: playerInv) {
                if (e.item.name == entry.item.name) {
                    e.quantity -= qty;
                    break;
                }
            }

            // --- 4. UPDATE THE DISPLAY CASE SPRITE GRAPHICS ---
            if (qty > 0) {
                int variant = (rand() % 2) + 1; // Randomly rolls a 1 or a 2
                sprite.src = emptySrc;
                // Shift the source rect to the right by 1 or 2 full display-case widths
                sprite.src.x += (variant * emptySrc.w);
            } else {
                // If quantity is 0, revert back to the completely empty shelf
                sprite.src = emptySrc;
            }

            std::cout << "Placed " << qty << "x " << entry.item.name << " in display case\n";
        };

        // Open the inventory menu and pass the Display Stand so the Top-Right UI updates!
        updateInventoryUI(inv.items, InventoryMode::PlaceItem, &dc);
    });
    return displayCase;
}
