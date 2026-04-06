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
    if (dialogueBusy) return;

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
        // 1. Update stand inventory
        customer.displayStand->quantity--;
        customer.displayStand->reserved_quantity--;

        //Update Sprite if stand is now empty
        if (customer.displayStand->quantity <= 0 && customer.displayStandEntity) {
            if (customer.displayStandEntity->hasComponent<Sprite>()) {
                customer.displayStandEntity->getComponent<Sprite>().src =
                    customer.displayStand->emptySrc;
            }
        }

        // 2. Clean up AI state
        ai.isWaiting = false;
        ai.currentState = CustomerAIState::LeavingStore;

        // 3. Clear active customer BEFORE firing callback
        //    so pauseQueue inside onSaleComplete works cleanly
        activeCustomer = nullptr;

        // 4. Fire sale callback once — this pauses queue and shows dialogue
        if (onSaleComplete) {
            int profitMargin = offeredPrice - item.basePrice;
            onSaleComplete(offeredPrice, profitMargin);
        }

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
            showFeedback("walkaway"); // triggers onShowFeedback
        } else {
            showFeedback("rejection");
        }
    }
}

void HaggleSystem::dismissFeedback() {
    feedbackMessage = "";

    if (activeCustomer) {
        // Customer still has patience — reopen haggle UI directly, no opening dialogue
        auto& customer = activeCustomer->getComponent<Customer>();
        if (onShowHaggleUI) onShowHaggleUI(customer.displayStand->item);
    } else {
        // Customer walked away — pull next from queue
        update();
    }
}

void HaggleSystem::pushDialogue(const std::string& msg, std::function<void()> onConfirm) {
    if (!dialogueBusy) {
        dialogueBusy = true;
        pendingConfirm = onConfirm; //Don't get overwritten by
        if (onShowDialogue) {
            onShowDialogue(msg);
        }
    } else {
        dialogueQueue.push({msg, onConfirm});
    }
}

void HaggleSystem::onDialogueConfirmed() {
    auto confirm = pendingConfirm;  // grab it
    pendingConfirm = nullptr;       // clear it
    dialogueBusy = false;           // unblock

    if (confirm) confirm();         // fire — may set pendingConfirm again via pushDialogue

    // Only drain queue if confirm didn't push a new dialogue
    if (!dialogueBusy) processDialogueQueue();
}

void HaggleSystem::processDialogueQueue() {
    if (dialogueQueue.empty()) return;
    auto [msg, cb] = dialogueQueue.front();
    dialogueQueue.pop();
    pushDialogue(msg, cb);
}

void HaggleSystem::beginHaggle() {
    if (!activeCustomer) return;
    auto& customer = activeCustomer->getComponent<Customer>();
    ItemDef item = customer.displayStand->item;

    if (onGetOpeningLine) {
        std::string opening = onGetOpeningLine(customer.mood);
        pushDialogue(opening, [this, item]() mutable {
            if (onShowHaggleUI) onShowHaggleUI(item);
        });
    } else {
        if (onShowHaggleUI) onShowHaggleUI(item);
    }
}

void HaggleSystem::showFeedback(const std::string&) {
    std::string line;
    if (activeCustomer) {
        auto& c = activeCustomer->getComponent<Customer>();
        line = c.patience <= 0
            ? (onGetWalkawayLine ? onGetWalkawayLine() : "Fine, I'm leaving.")
            : (onGetRejectionLine ? onGetRejectionLine(c.patience) : "Too expensive!");
    } else {
        line = onGetWalkawayLine ? onGetWalkawayLine() : "Fine, I'm leaving.";
    }

    pushDialogue(line, [this]() {
        dismissFeedback();
    });
}

bool HaggleSystem::willAccept(const Customer& customer, const ItemDef& item, int offeredPrice) {
    return offeredPrice <= getMaxAcceptablePrice(customer, item);
}

int HaggleSystem::getMaxAcceptablePrice(const Customer& customer, const ItemDef& item) {
    // Base: customers always accept up to 120% of base price
    // Mood bonus: up to additional 20% (so 140% at max mood)
    // Trend bonus: market modifier on top of everything
    float baseLeniency = 1.20f;
    float moodBonus = customer.mood * 0.20f;  // 0.0 mood = +0%, 1.0 mood = +20%
    float moodMultiplier = baseLeniency + moodBonus;
    float trendMultiplier = getPriceModifier ? getPriceModifier(item) : 1.0f;
    return static_cast<int>(item.basePrice * moodMultiplier * trendMultiplier);
}