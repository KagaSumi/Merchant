//
// Created by Curry on 2026-02-04.
//

#ifndef PROJECT_COLLISSION_H
#define PROJECT_COLLISSION_H
#include <SDL3/SDL_rect.h>

#include "Components.h"

class Collision {
    public:
    //Axis Aligned Bounding Box
    static bool AABB(const SDL_FRect rectA, const SDL_FRect rectB);
    static bool AABB(const Collider& colA, const Collider& colB);
};

#endif //PROJECT_COLLISSION_H