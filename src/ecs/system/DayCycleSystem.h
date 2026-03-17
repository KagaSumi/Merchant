//
// Created by Curry on 2026-03-17.
//

#ifndef PROJECT_DAYCYCLESYSTEM_H
#define PROJECT_DAYCYCLESYSTEM_H
#include <memory>
#include <vector>

#include "Components.h"
#include "Entity.h"

struct rgba {
    int r, g, b, a;
};

class DayCycleSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>> &entities);

    void applyTint(const std::vector<std::unique_ptr<Entity> > &entities, const rgba rgba) {
        for (auto &entity: entities) {
            if (entity->hasComponent<Sprite>()) {
                if (entity->getComponent<Sprite>().renderLayer == RenderLayer::World){
                    auto &Texture = entity->getComponent<Sprite>().Texture;
                    SDL_SetTextureColorMod(Texture,
                                           rgba.r,
                                           rgba.g,
                                           rgba.b);
                    SDL_SetTextureAlphaMod(Texture, rgba.a);
                }
            }
        }
    }
private:
    rgba evening_target = {255,180,120,220};
    rgba morning_target = {255,255,255,255};
    rgba steps = {};
    int* customerCount = nullptr;
};
#endif //PROJECT_DAYCYCLESYSTEM_H
