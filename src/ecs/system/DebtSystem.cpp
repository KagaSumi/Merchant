#include "DebtSystem.h"

#include "Game.h"
//
// Created by Curry on 2026-03-25.
//

bool DebtSystem::payDebt(Wallet &wallet, Debt &debt) {
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
        return true;
    } else {
        std::cout << "Game Over! Cannot pay debt.\n";
        return false;
    }
}
int DebtSystem::getNextPayment(const Debt& debt) const {
    // Preview what NEXT week's payment will be
    int nextPayment = basePayment + (weeklyIncrement * weeksPassed);

    return std::min(nextPayment, debt.amount);
}

int DebtSystem::calculatePayment(const Debt& debt) const {
    // Calculate the expected payment based on how many weeks have passed
    int expectedPayment = basePayment + (weeklyIncrement * weeksPassed);

    // Get the total remaining debt (assuming your Debt class has a getter like this)
    int remainingDebt = debt.amount;

    // If the expected payment is larger than the remaining debt,
    // just return the remaining debt so the player doesn't overpay.
    if (expectedPayment > remainingDebt) {
        return remainingDebt;
    }

    return expectedPayment;
}