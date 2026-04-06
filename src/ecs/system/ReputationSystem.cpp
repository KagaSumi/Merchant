//
// Created by Curry on 2026/04/06.
//
#include "ReputationSystem.h"

#include "Game.h"

int ReputationSystem::getXPForSale(int salePrice, int profitMargin) {
    int xp = 10;

    if (profitMargin > 0 && salePrice > 0) {
        float pct = static_cast<float>(profitMargin) / salePrice;
        xp += static_cast<int>(pct * 50.0f);
    }

    return xp;
}

bool ReputationSystem::onSale(int salePrice,
                              int profitMargin,
                              ShopReputation& rep)
{
    int xpGained = getXPForSale(salePrice, profitMargin);
    Game::gameState.currentRepXP += xpGained;

    bool leveledUp = false;

    while (Game::gameState.currentRepXP >= Game::gameState.xpToNextLevel) {
        Game::gameState.currentRepXP -= Game::gameState.xpToNextLevel;
        Game::gameState.xpToNextLevel =
            static_cast<int>(Game::gameState.xpToNextLevel * 1.5f);

        Game::gameState.shopReputation++;
        rep.reputation = Game::gameState.shopReputation;

        leveledUp = true;
    }

    return leveledUp;
}