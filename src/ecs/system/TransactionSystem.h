//
// Created by Curry on 2026-03-17.
//

#ifndef PROJECT_TRANSACTIONSYSTEM_H
#define PROJECT_TRANSACTIONSYSTEM_H
#include <memory>

#include "Components.h"

class TransactionSystem {
public:
    // Takes the full entity list to find the Player's wallet,
    // and takes the specific DisplayStand entity being bought from.
    void ProcessSale(std::vector<std::unique_ptr<Entity>>& entities, Entity* targetDisplayStand, int agreedPrice);
};
#endif //PROJECT_TRANSACTIONSYSTEM_H