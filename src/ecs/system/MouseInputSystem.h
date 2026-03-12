//
// Created by Curry on 2026-03-11.
//

#ifndef PROJECT_MOUSEINPUTSYSTEM_H
#define PROJECT_MOUSEINPUTSYSTEM_H
#include <SDL3/SDL_events.h>

class World;

class MouseInputSystem {
public:
    void update(World& world, const SDL_Event& event);
};

#endif //PROJECT_MOUSEINPUTSYSTEM_H