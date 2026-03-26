#include "DebtSystem.h"

#include "Game.h"
//
// Created by Curry on 2026-03-25.
//

void DebtSystem::payDebt(Wallet &wallet, Debt &debt) {
    weeksPassed++;

    // Payment grows each week
    int weeklyPayment = basePayment + (weeklyIncrement * (weeksPassed - 1));
    if (weeklyPayment > debt.amount) weeklyPayment = debt.amount; // cap to remaining debt

    if (wallet.balance >= weeklyPayment) {
        wallet.balance -= weeklyPayment;
        debt.amount -= weeklyPayment;
        std::cout << "Week " << weeksPassed
                << ": Paid " << weeklyPayment
                << ". Remaining debt: " << debt.amount << "\n";
    } else {
        std::cout << "Game Over! Cannot pay debt.\n";
        // trigger game over
    }
}
