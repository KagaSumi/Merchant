//
// Created by Curry on 2026-02-04.
//

#include <iostream>

#include "CollisionSystem.h"
#include "Collision.h"
#include "World.h"

//has two important functions
// 1. first is position the collider with the transform
// 2. checking for collisions
void CollisionSystem::update(World &world) {
    //get a list of entities that have colliders and transforms
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());

    //update all collider positions first
    for (auto entity: collidables) {
        auto& t = entity->getComponent<Transform>();
        auto& c = entity->getComponent<Collider>();
        c.rect.x =t.position.x + c.offsetX;
        c.rect.y =t.position.y + c.offsetY;
    }

    std::set<CollisionKey> currentCollisions;


    //outer loop
    for (size_t i = 0; i < collidables.size(); ++i) {
        //update the collider position
        auto entityA = collidables[i];
        auto& colliderA = entityA->getComponent<Collider>();

        //Check for collision
        //inner loop
        for (size_t j = i + 1 ; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto& colliderB = entityB->getComponent<Collider>();

            if (Collision::AABB(colliderA.rect, colliderB.rect)) {
                CollisionKey key = makeKey(entityA,entityB);
                currentCollisions.insert(key);
                if (!activeCollisions.contains(key)) {
                    world.getEventManager().emit(CollisionEvent{entityA,entityB, CollisionState::Enter});
                }
                world.getEventManager().emit(CollisionEvent{entityA,entityB, CollisionState::Stay});
            }
        }
    }
    for (auto& key: activeCollisions) {
        if (!currentCollisions.contains(key)) {
            world.getEventManager().emit(CollisionEvent{key.first,key.second, CollisionState::Exit});
        }
    }

    activeCollisions = std::move(currentCollisions); //Updates with Current Collisions
}

std::vector<Entity*> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities) {
    std::vector<Entity*> collidables;
    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            collidables.push_back(e.get());
        }
    }
    return collidables;
}