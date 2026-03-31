#include "CustomerSpawnerSystem.h"
//
// Created by Curry on 2026-03-25.
//
int CustomerSpawnerSystem::calculateCustomer(float reputation) {
    int baseCustomers = 3;
    float repMultiplier = 1.5f;

    // e.g., Reputation 5 = 3 + (5 * 1.5) = 10 customers
    return baseCustomers + static_cast<int>(reputation * repMultiplier);
}

bool CustomerSpawnerSystem::shouldSpawn(Spawner& spawner,float dt) {
    // 1. Stop spawning if we've hit today's total customer quota
    if (index >= spawner.spawnCount) {
        spawner.isFinished = true;
        return false; // Spawner is finished for the day
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