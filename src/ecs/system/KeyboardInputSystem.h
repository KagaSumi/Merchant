//
// Created by Curry on 2026-01-28.
//

#ifndef PROJECT_KEYBOARDINPUTSYSTEM_H
#define PROJECT_KEYBOARDINPUTSYSTEM_H

#include <memory>
#include <vector>
#include <SDL3/SDL_events.h>

#include "Components.h"
#include "Entity.h"

class KeyboardInputSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event,EventManager& eventManager) {
        for (auto& e : entities) {
            if (e-> hasComponent<Velocity>() && e-> hasComponent<PlayerTag>()) {
                auto& v = e->getComponent<Velocity>();
                auto& p = e->getComponent<PlayerTag>();
                if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                        case SDLK_W:
                            v.direction.y = -1;
                            break;
                        case SDLK_S:
                            v.direction.y = 1;
                            break;
                        case SDLK_A:
                            v.direction.x = -1;
                            break;
                        case SDLK_D:
                            v.direction.x = 1;
                            break;
                        case SDLK_E:
                            PlayerActionEvent interactEvent(e.get(), PlayerAction::Interact);
                            // Send it to your Event Bus / Event Manager to distribute to systems!
                            eventManager.emit(interactEvent);

                            break;
                    }
                }

                if (event.type == SDL_EVENT_KEY_UP) {
                    switch (event.key.key) {
                        case SDLK_W:
                            v.direction.y = 0;
                            break;
                        case SDLK_S:
                            v.direction.y = 0;
                            break;
                        case SDLK_A:
                            v.direction.x = 0;
                            break;
                        case SDLK_D:
                            v.direction.x = 0;
                            break;
                        default:
                            break;
                    }
                }
            }
        }

    }
};

#endif //PROJECT_KEYBOARDINPUTSYSTEM_H