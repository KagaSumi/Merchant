//
// Created by Curry on 2026-03-25.
//

#ifndef PROJECT_DEBTSYSTEM_H
#define PROJECT_DEBTSYSTEM_H
#include "Components.h"

class DebtSystem {
public:
    // Simply initialize weeksPassed to 0
    DebtSystem() : weeksPassed(0) {}

    bool payDebt(Wallet &wallet, Debt &debt);
    int calculatePayment(const Debt& debt) const;
    int getNextPayment(const Debt& debt) const;

private:
    int weeksPassed; // Tracks how many weeks have successfully been paid
};

#endif //PROJECT_DEBTSYSTEM_H