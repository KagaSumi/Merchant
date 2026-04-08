#include "CustomerSpawnerSystem.h"
//
// Created by Curry on 2026-03-25.
//
int CustomerSpawnerSystem::calculateCustomer(float reputation, int day) {
    int baseCustomers = 5;
    float repMultiplier = 1.5f;
    int dayBonus = day / 2;

    return baseCustomers +
           static_cast<int>(reputation * repMultiplier) +
           dayBonus;

}

bool CustomerSpawnerSystem::shouldSpawn(Spawner& spawner,float dt) {
    // 1. Stop spawning if we've hit today's total customer quota
    if (spawner.isFinished) return false; // Spawner is finished for the day

    if (index >= spawner.maxSpawns) {
        spawner.isFinished = true;
        return false;
    }
    // 2. Accumulate time
    spawnTimer += dt;

    // 3. Find our current delay target using modulo
    // If index is 5, 5 % 4 = 1. It will use spawnTime[1] (which is 6).
    int cycleIndex = index % spawnTime.size();

    // 4. Check if we've waited long enough for the current customer
    if (spawnTimer >= spawnTime[cycleIndex]) {
        spawnTimer = 0.0f; // Reset timer for the next customer
        return true;
    }

    return false;
}

void CustomerSpawnerSystem::resetForNewDay()
{
    auto& spawner = spawnerEntity->getComponent<Spawner>();
    auto& rep = shopEntity->getComponent<ShopReputation>();
    auto& day = shopEntity->getComponent<DayCycle>();

    index = 0;
    spawnTimer = 0.0f;

    spawner.isFinished = false;

    spawner.maxSpawns = calculateCustomer(rep.reputation, day.date);

    customerCount = spawner.maxSpawns;
}