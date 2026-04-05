//
// Created by Curry on 2026-03-25.
//

#ifndef PROJECT_CUSTOMERSPAWNERSYSTEM_H
#define PROJECT_CUSTOMERSPAWNERSYSTEM_H
#include <memory>
#include <vector>

#include "Components.h"
#include "Entity.h"

class CustomerSpawnerSystem {
public:
    void update(std::vector<std::unique_ptr<Entity> > &entities, float dt) {
        // 1. Find the entities we care about
        Entity *spawnerEntity = nullptr;
        Entity *shopEntity = nullptr;
        Entity *dayEntity = nullptr;

        for (auto &e: entities) {
            if (e->hasComponent<Spawner>()) spawnerEntity = e.get();
            if (e->hasComponent<ShopReputation>()) shopEntity = e.get();
            if (e->hasComponent<DayCycle>()) dayEntity = e.get();
        }

        if (!spawnerEntity || !shopEntity || !dayEntity) return;

        // 2. grab references
        Spawner& spawner = spawnerEntity->getComponent<Spawner>();
        ShopReputation &store = shopEntity->getComponent<ShopReputation>();
        DayPhase currentPhase = dayEntity->getComponent<DayCycle>().currentPhase;
        if (currentPhase != DayPhase::ShopOpen) {
            return;
        }

        // 3. Initialize Daily Customer
        if (spawner.spawnCount == 0 && index == 0) {
            spawner.spawnCount = calculateCustomer(store.reputation);
            customerCount = spawner.spawnCount;
        }

        if (shouldSpawn(spawner,dt)) {
            spawner.spawnCallback();
            index++;
        }
    }

    void resetForNewDay() {
        index=0;
        spawnTimer = 0.0f;
    }

    bool shouldSpawn(Spawner& spawner,float dt);

    int calculateCustomer(float reputation);

private:
    int index = 0;
    int customerCount = 0;
    float spawnTimer = 0.0f;
    std::vector<float> spawnTime = {6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
};

#endif //PROJECT_CUSTOMERSPAWNERSYSTEM_H
