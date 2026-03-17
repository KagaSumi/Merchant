//
// Created by Curry on 2026-03-17.
//

#ifndef PROJECT_DAYCYCLESYSTEM_H
#define PROJECT_DAYCYCLESYSTEM_H
#include <memory>
#include <vector>

#include "Components.h"
#include "Entity.h"

class DayCycleSystem {
public:
    void ApplyEveningShade(std::vector<std::unique_ptr<Entity> > &entities) {
        for (auto &entity: entities) {
            if (entity->hasComponent<Sprite>()) {
                auto &Texture = entity->getComponent<Sprite>().Texture;
                SDL_SetTextureColorMod(Texture,
                                       255,
                                       180,
                                       120);
            }
        }
    }
    void RemoveEveningShade(std::vector<std::unique_ptr<Entity> > &entities) {
        for (auto &entity: entities) {
            if (entity->hasComponent<Sprite>()) {
                auto &Texture = entity->getComponent<Sprite>().Texture;
                SDL_SetTextureColorMod(Texture,
                                       255,
                                       255,
                                       255);
            }
        }
    }
};
#endif //PROJECT_DAYCYCLESYSTEM_H
