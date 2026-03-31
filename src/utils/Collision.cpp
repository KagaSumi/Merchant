//
// Created by Curry on 2026-02-04.
//

#include <SDL3/SDL_rect.h>

#include "Collision.h"


//pass in rect
//Axis-aligned bounding box
bool Collision::AABB(const SDL_FRect rectA, const SDL_FRect rectB) {
    //1. is the right edge of RectA >= to left edge of rectB
    //2. Is the right edge of rect B >= to the left edge of Rect A
    //3. Is the bottom edge of rect A >= to teh top edge of RectB
    //4. Is the bottom edge of rect B >= to teh top edge of RectA
    //If all above then collision

    if (rectA.x + rectA.w >= rectB.x &&
        rectB.x + rectB.w >= rectA.x &&
        rectA.y + rectA.h >= rectB.y &&
        rectB.y + rectB.h >= rectA.y
        ) {return true;}
    return false;
}

//Pass in collider
bool Collision::AABB(const Collider &colA, const Collider &colB) {
    if (AABB(colA.rect, colB.rect)) {return true;}
    return false;
}
