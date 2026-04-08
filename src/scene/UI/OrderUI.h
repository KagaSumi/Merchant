//
// Created by Curry on 2026-04-07.
//

#ifndef PROJECT_ORDERUI_H
#define PROJECT_ORDERUI_H

#pragma once
#include <vector>
#include <functional>
#include "../World.h"
#include "../scene/Scene.h"

struct OrderSlotRefs {
    Entity* icon = nullptr;
    Entity* nameLabel = nullptr;
    Entity* priceLabel = nullptr;
    Entity* buyBtn = nullptr;
    Entity* buyLabel = nullptr;
    Entity* buyBtnCollider = nullptr;
};

struct OrderSession {
    std::vector<OrderSlotRefs> slots;
    std::vector<ItemDef> currentItems;
    Entity* walletLabelRef = nullptr;
    int* walletRef = nullptr;
    Inventory* inventoryRef = nullptr;
    std::function<void()> onContinue;
    Entity* shelfBuyBtn = nullptr;
    Entity* shelfPriceLabel = nullptr;
    Entity* paymentWarningLabelRef = nullptr;
    int currentShelfPrice = 500;
    std::function<int(int currentCount)> getShelfPrice;
    std::function<void()> onBuyShelf;
};

namespace OrderUI {
    Entity& create(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIOrderScreen);
    Entity& update(Scene& scene, std::vector<ItemDef> availableItems, Wallet& wallet, Inventory& inv, int paymentDueToday, std::function<void()> onContinue, std::function<void()> onBuyShelf, Entity* UIOrderScreen);
}

#endif //PROJECT_ORDERUI_H
