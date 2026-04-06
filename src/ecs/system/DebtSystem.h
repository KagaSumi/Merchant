//
// Created by Curry on 2026-03-25.
//

#ifndef PROJECT_DEBTSYSTEM_H
#define PROJECT_DEBTSYSTEM_H
#include "Components.h"

class DebtSystem {
public:
    DebtSystem() : basePayment(500), weeklyIncrement(100), weeksPassed(0) {}
    DebtSystem(int basePayment, int increment) : basePayment(basePayment), weeklyIncrement(increment), weeksPassed(0) {}

    bool payDebt(Wallet &wallet, Debt &debt);
    int calculatePayment(const Debt& debt) const;
    int getNextPayment(const Debt& debt) const;
private:
    int basePayment;       // starting weekly payment
    int weeklyIncrement;   // how much payment grows per week
    int weeksPassed;       // tracks how many weeks have passed
};
#endif //PROJECT_DEBTSYSTEM_H