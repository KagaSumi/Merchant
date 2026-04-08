//
// Created by Curry on 2026/04/06.
//

#ifndef PROJECT_REPUTATIONSYSTEM_H
#define PROJECT_REPUTATIONSYSTEM_H
#include "Components.h"

class ReputationSystem {
public:
    // Returns true if leveled up
    bool onSale(int salePrice, int profitMargin, ShopReputation& rep);

    static int getXPForSale(int salePrice, int profitMargin);
};
#endif //PROJECT_REPUTATIONSYSTEM_H
