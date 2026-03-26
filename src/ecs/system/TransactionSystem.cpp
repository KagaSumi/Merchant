//
// Created by Curry on 2026-03-17.
//
#include "TransactionSystem.h"
#include <iostream>
void TransactionSystem::ProcessSale(std::vector<std::unique_ptr<Entity>>& entities, Entity* targetDisplayStand, int agreedPrice) {

    if (!targetDisplayStand || !targetDisplayStand->hasComponent<DisplayStand>()) {
        std::cerr << "Transaction failed: Invalid display stand!" << std::endl;
        return;
    }

    auto& stand = targetDisplayStand->getComponent<DisplayStand>();

    // Safety check: Ensure the math hasn't broken
    if (stand.quantity <= 0 || stand.reserved_quantity <= 0) {
        std::cerr << "Transaction failed: Item stock or reservation count is invalid!" << std::endl;
        return;
    }

    // Find the Player's Wallet
    Wallet* playerWallet = nullptr;
    for (auto& entity : entities) {
        if (entity->hasComponent<Wallet>()) {
            playerWallet = &entity->getComponent<Wallet>();
            break;
        }
    }

    if (!playerWallet) {
        std::cerr << "Transaction failed: Could not find player wallet!" << std::endl;
        return;
    }

    // --- EXECUTE THE TRANSACTION ---

    // 1. Add money to the player
    playerWallet->balance += agreedPrice;

    // 2. Remove the item from physical stock
    stand.quantity -= 1;

    // 3. Clear the AI's reservation
    stand.reserved_quantity -= 1;

    // 4. (Optional) Clear the stand's name if it's completely empty
    if (stand.quantity == 0) {
        stand.name = "Empty";
    }

    std::cout << "Sold 1 " << stand.name << " for " << agreedPrice << "G. New Balance: " << playerWallet->balance << "G" << std::endl;
}