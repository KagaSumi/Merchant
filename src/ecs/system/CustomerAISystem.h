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
                auto &pf = entity->getComponent<PathFinding>();
                auto &transform = entity->getComponent<Transform>();
                auto &velocity = entity->getComponent<Velocity>();

                switch (ai.currentState) {
                    case CustomerAIState::Browsing:
                        HandleBrowsing(ai,pf, transform, velocity, deltaTime);
                        break;
                    case CustomerAIState::HeadingToRegister:
                        // If path is empty, request new A* path to register coords
                        HandleHeadingToRegister(ai,pf, transform, velocity);
                        break;
                    case CustomerAIState::LeavingStore:
                        // Set velocity toward the shop exit
                        HandleLeavingStore(*entity,ai,pf ,dayCycleSystem,transform,velocity);
                        break;
                }
            }
        }
    }
 void setRegister(SDL_Point reg) {
        if (reg.x == -1 and reg.y == -1) {
            std::cerr <<"Register Not Initialized" <<std::endl;
            return;
        }
        Register = {reg.x,reg.y};
    }
void setDoor(SDL_Point door) {
        if (door.x == -1 and door.y == -1) {
            std::cerr <<"Door Not Initialized" <<std::endl;
            return;
        }
        Door = {door.x,door.y};
    }

private:
    void HandleHeadingToRegister(CustomerAI& ai, PathFinding& pf, Transform& t, Velocity& v);
    void HandleBrowsing(CustomerAI& ai, PathFinding& pf, Transform& t, Velocity& v, float deltaTime);
    void HandleLeavingStore(Entity& entity, CustomerAI& ai, PathFinding& pf, DayCycleSystem& dcs, Transform& t, Velocity& v);
    void MoveAlongPath(PathFinding& pf, Transform& t, Velocity& v);

    SDL_Point Register{-1,-1};
    SDL_Point Door{-1,-1};
};

#endif
#endif //PROJECT_CUSTOMERAISYSTEM_H