//
// Created by Curry on 2026-03-25.
//

#ifndef PROJECT_DEBTSYSTEM_H
#define PROJECT_DEBTSYSTEM_H
#include "Components.h"
#include "Game.h"

class DebtSystem {
public:
    DebtSystem(int basePayment, int increment): basePayment(basePayment), weeklyIncrement(increment), weeksPassed(0) {}

    void payDebt(Wallet& wallet, Debt& debt);

private:
    int basePayment;       // starting weekly payment
    int weeklyIncrement;   // how much payment grows per week
    int weeksPassed;       // tracks how many weeks have passed
};
#endif //PROJECT_DEBTSYSTEM_H