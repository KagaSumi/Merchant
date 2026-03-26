//
// Created by Curry on 2026-03-17.
//

#ifndef PROJECT_CUSTOMERAISYSTEM_H
#define PROJECT_CUSTOMERAISYSTEM_H
#ifndef CUSTOMER_AI_SYSTEM_H
#define CUSTOMER_AI_SYSTEM_H

#include <iostream>
#include <memory>
#include <vector>

#include "Components.h"
#include "DayCycleSystem.h"
#include "PathfindingSystem.h"

class CustomerAISystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities,float deltaTime, DayCycleSystem& dayCycleSystem) {
        for (auto &entity: entities) {
            if (entity->hasComponent<CustomerAI>() && entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
                auto &ai = entity->getComponent<CustomerAI>();
                auto &transform = entity->getComponent<Transform>();
                auto &velocity = entity->getComponent<Velocity>();

                switch (ai.currentState) {
                    case CustomerAIState::Browsing:
                        HandleBrowsing(ai, transform, velocity, deltaTime);
                        break;
                    case CustomerAIState::HeadingToRegister:
                        // If path is empty, request new A* path to register coords
                        HandleHeadingToRegister(ai, transform, velocity);
                        break;
                    case CustomerAIState::LeavingStore:
                        // Set velocity toward the shop exit
                        HandleLeavingStore(*entity,ai,dayCycleSystem,transform,velocity);
                        break;
                }
            }
        }
    }

    void SetDoorLocation(int gridX, int gridY) {
        Door = {gridX, gridY};
    }
    void SetRegisterLocation(int gridX, int gridY) {
        Register = {gridX, gridY};
    }

private:
    void HandleBrowsing(CustomerAI &ai, Transform &t, Velocity &v,float deltaTime);

    // Removed EntityAdmin, added Velocity
    void HandleHeadingToRegister(CustomerAI &ai, Transform &t, Velocity &v);
    void HandleLeavingStore(Entity& entity, CustomerAI &ai,DayCycleSystem& dayCycleSystem, Transform &t, Velocity &v);

    void MoveAlongPath(CustomerAI &ai, Transform &t, Velocity &v);

    SDL_Point Register{20,16};
    SDL_Point Door{20,10};
};

#endif
#endif //PROJECT_CUSTOMERAISYSTEM_H