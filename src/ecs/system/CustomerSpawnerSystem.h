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
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt)
    {
        spawnerEntity = nullptr;
        shopEntity = nullptr;
        dayEntity = nullptr;

        for (auto& e : entities) {
            if (e->hasComponent<Spawner>()) spawnerEntity = e.get();
            if (e->hasComponent<ShopReputation>()) shopEntity = e.get();
            if (e->hasComponent<DayCycle>()) dayEntity = e.get();
        }

        if (!spawnerEntity || !shopEntity || !dayEntity) return;

        Spawner& spawner = spawnerEntity->getComponent<Spawner>();
        DayPhase currentPhase = dayEntity->getComponent<DayCycle>().currentPhase;

        if (currentPhase != DayPhase::ShopOpen) return;

        if (shouldSpawn(spawner, dt)) {
            spawner.spawnCallback();
            index++;
        }
    }

    void resetForNewDay();

    bool shouldSpawn(Spawner& spawner,float dt);

    int calculateCustomer(float reputation,int day);

private:
    Entity *spawnerEntity = nullptr;
    Entity *shopEntity = nullptr;
    Entity *dayEntity = nullptr;
    int index = 0;
    int customerCount = 0;
    float spawnTimer = 0.0f;
    std::vector<float> spawnTime = {3.0f, 5.0f, 4.0f, 6.0f};
};

#endif //PROJECT_CUSTOMERSPAWNERSYSTEM_H
