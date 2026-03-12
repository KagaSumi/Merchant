//
// Created by Curry on 2026-02-04.
//

#ifndef PROJECT_COLLISIONSYSTEM_H
#define PROJECT_COLLISIONSYSTEM_H
#include <algorithm>
#include <vector>
#include <memory>
#include <set>

#include "Entity.h"

using CollisionKey = std::pair<Entity*, Entity*>;

//forward declaration
class World;

class CollisionSystem {
public:
    void update(World& world);
    std::set<CollisionKey> activeCollisions;
    private:
    std::vector<Entity*> queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities);

    CollisionKey makeKey(Entity* entityA, Entity* entityB) {
        return std::minmax(entityA,entityB); //Automatically orders our pair so smaller number (memory address) is ordered first
    }

};

#endif //PROJECT_COLLISIONSYSTEM_H