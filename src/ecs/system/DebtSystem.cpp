#include "DebtSystem.h"

#include "Game.h"
//
// Created by Curry on 2026-03-25.
//
bool DebtSystem::payDebt(Wallet &wallet, Debt &debt) {
    // 1. Ask our helper function EXACTLY how much is owed right now
    // This uses the switch statement, so it perfectly matches the UI!
    int weeklyPayment = calculatePayment(debt);

    // 2. Check if the player can afford it
    if (wallet.balance >= weeklyPayment) {
        wallet.balance -= weeklyPayment;
        debt.amount -= weeklyPayment;

        std::cout << "Week " << (weeksPassed + 1)
                  << ": Paid " << weeklyPayment
                  << "G. Remaining debt: " << debt.amount << "G\n";

        // 3. Increment the week ONLY after a successful payment
        weeksPassed++;

        return true;
    }
    std::cout << "Game Over! Cannot pay debt.\n";
    return false;
}

int DebtSystem::getNextPayment(const Debt& debt) const {
    // Preview what the payment will be for the current week
    int expectedPayment = 0;

    switch (weeksPassed) {
        case 0: expectedPayment = 500;  break; // Week 1 goal
        case 1: expectedPayment = 1500; break; // Week 2 goal
        case 2: expectedPayment = 3000; break; // Week 3 goal
        default: expectedPayment = 3000; break; // Fallback
    }

    // Never preview a payment larger than the remaining debt
    return std::min(expectedPayment, debt.amount);
}

int DebtSystem::calculatePayment(const Debt& debt) const {
    // Calculate the actual payment due right now
    int expectedPayment = 0;

    switch (weeksPassed) {
        case 0: expectedPayment = 500;  break;
        case 1: expectedPayment = 1500; break;
        case 2: expectedPayment = 3000; break;
        default: expectedPayment = 3000; break;
    }

    // Never charge more than the remaining debt
    return std::min(expectedPayment, debt.amount);
}