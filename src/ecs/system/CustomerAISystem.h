//
// Created by Curry on 2026-03-17.
//

#ifndef PROJECT_CUSTOMERAISYSTEM_H
#define PROJECT_CUSTOMERAISYSTEM_H
#ifndef CUSTOMER_AI_SYSTEM_H
#define CUSTOMER_AI_SYSTEM_H

#include <memory>
#include <vector>

#include "Components.h"
#include "PathfindingSystem.h"

class CustomerAISystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float deltaTime) {
        for (auto &entity: entities) {
            if (entity->hasComponent<CustomerAI>() && entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
                auto &ai = entity->getComponent<CustomerAI>();
                auto &transform = entity->getComponent<Transform>();
                auto &velocity = entity->getComponent<Velocity>();

                switch (ai.currentState) {
                    case CustomerAIState::Browsing:
                        HandleBrowsing(ai, transform, velocity);
                        break;
                    case CustomerAIState::HeadingToRegister:
                        // If path is empty, request new A* path to register coords
                        HandleHeadingToRegister(ai, transform, velocity);
                        break;
                    case CustomerAIState::LeavingStore:
                        // Set velocity toward the shop exit
                        HandleLeavingStore(ai,transform,velocity);
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
    void HandleBrowsing(CustomerAI &ai, Transform &t, Velocity &v);

    // Removed EntityAdmin, added Velocity
    void HandleHeadingToRegister(CustomerAI &ai, Transform &t, Velocity &v);
    void HandleLeavingStore(CustomerAI &ai, Transform &t, Velocity &v);

    void MoveAlongPath(CustomerAI &ai, Transform &t, Velocity &v);

    SDL_Point Register{};
    SDL_Point Door{};
};

#endif
#endif //PROJECT_CUSTOMERAISYSTEM_H