//
// Created by Curry on 2026-04-07.
//

#ifndef PROJECT_INVENTORYUI_H
#define PROJECT_INVENTORYUI_H

#pragma once
#include <vector>
#include <functional>
#include "../World.h"
#include "../scene/Scene.h"

enum class InventoryUIState { Grid, QuantitySelect };
enum class InventoryMode { Browse, PlaceItem };

struct InventorySlotRefs {
    Entity* container = nullptr;
    Entity* icon = nullptr;
    Entity* label = nullptr;
};

struct QuantitySession {
    InventoryEntry selectedItem;
    int maxQuantity = 0;

    Entity* itemIconRef = nullptr;
    Entity* itemNameRef = nullptr;
    Entity* quantityButtonContainer = nullptr;
    std::vector<Entity*> quantityButtonRefs;

    std::function<void(InventoryEntry, int)> onConfirm;
};

struct QuantityScreenSession {
    InventoryEntry selectedItem;
    std::function<void(InventoryEntry, int)> onConfirm;
    std::function<void()> onCancel;

    Entity* itemIconRef = nullptr;
    Entity* itemNameRef = nullptr;
    std::vector<Entity*> quantityButtonRefs;
};

struct InventorySession {
    std::vector<InventoryEntry> cachedInventory;
    std::vector<InventorySlotRefs> slots;
    InventoryMode mode = InventoryMode::Browse;

    InventoryUIState state = InventoryUIState::Grid;
    QuantitySession quantitySession;

    Entity* targetStockIconRef = nullptr;
    Entity* targetStockLabelRef = nullptr;
    DisplayStand* currentStand = nullptr;

    Entity* gridContainerRef = nullptr;
    Entity* quantityPanelRef = nullptr;
    Entity* retrieveAllBtnRef = nullptr;
};

namespace InventoryUI {
    Entity& createGrid(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIInventory);
    Entity& updateGrid(Scene& scene, const std::vector<InventoryEntry>& inventoryData, InventoryMode mode, DisplayStand* targetStand, Entity* UIInventory);

    Entity& createQuantityScreen(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIQuantityScreen);
    void openQuantityScreen(Scene& scene, const InventoryEntry& item, int maxQty, std::function<void(InventoryEntry, int)> onConfirm, std::function<void()> onCancel, Entity* UIQuantityScreen);
}

#endif //PROJECT_INVENTORYUI_H
