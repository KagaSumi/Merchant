//
// Created by Curry on 2026/04/04.
//
#include "HaggleSystem.h"
#include "Entity.h"
#include <iostream>

#include "Components.h"

void HaggleSystem::enqueue(Entity* customerEntity) {
    waitingCustomers.push(customerEntity);
}

void HaggleSystem::update() {
    if (activeCustomer != nullptr) return;
    if (showingFeedback) return;

    if (!waitingCustomers.empty()) {
        activeCustomer = waitingCustomers.front();
        waitingCustomers.pop();
        beginHaggle();
    }
}

void HaggleSystem::submitOffer(int offeredPrice) {
    if (!activeCustomer) return;

    auto& customer = activeCustomer->getComponent<Customer>();
    auto& ai = activeCustomer->getComponent<CustomerAI>();
    ItemDef item = customer.displayStand->item;

    if (willAccept(customer, item, offeredPrice)) {
        // --- SUCCESS ---
        customer.displayStand->quantity--;
        customer.displayStand->reserved_quantity--;

        if (onSaleComplete) {
            int profitMargin = offeredPrice - item.basePrice;
            onSaleComplete(offeredPrice, profitMargin);
        }

        ai.isWaiting = false;
        ai.currentState = CustomerAIState::LeavingStore;
        activeCustomer = nullptr;

        update(); // pull next from queue
    } else {
        // --- FAIL ---
        customer.patience--;
        customer.mood -= 0.2f;
        if (customer.mood < 0.0f) customer.mood = 0.0f;

        if (customer.patience <= 0) {
            customer.displayStand->reserved_quantity--;
            ai.isWaiting = false;
            ai.currentState = CustomerAIState::LeavingStore;
            activeCustomer = nullptr;

            showFeedback("Customer left without buying!");
        } else {
            showFeedback("Too expensive! (" +
                         std::to_string(customer.patience) +
                         " patience left)");
        }
    }
}

void HaggleSystem::dismissFeedback() {
    showingFeedback = false;
    feedbackMessage = "";

    if (activeCustomer) {
        beginHaggle(); // still has patience, reopen haggle
    } else {
        update(); // next in queue
    }
}

void HaggleSystem::beginHaggle() {
    if (!activeCustomer) return;
    auto& customer = activeCustomer->getComponent<Customer>();
    if (onBeginHaggle) onBeginHaggle(customer.displayStand->item);
}

void HaggleSystem::showFeedback(const std::string& msg) {
    showingFeedback = true;
    feedbackMessage = msg;
    if (onShowFeedback) onShowFeedback(msg);
    else std::cout << msg << "\n"; // fallback until dialogue UI exists
}

bool HaggleSystem::willAccept(const Customer& customer, const ItemDef& item, int offeredPrice) {
    return offeredPrice <= getMaxAcceptablePrice(customer, item);
}

int HaggleSystem::getMaxAcceptablePrice(const Customer& customer, const ItemDef& item) {
    float moodMultiplier = 1.0f + (customer.mood * 0.5f);
    float trendMultiplier = getPriceModifier ? getPriceModifier(item) : 1.0f;
    return static_cast<int>(item.basePrice * moodMultiplier * trendMultiplier);
}