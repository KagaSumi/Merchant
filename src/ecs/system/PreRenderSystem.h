//
// Created by Curry on 2026-03-24.
//

#ifndef PROJECT_PRERENDERSYSTEM_H
#define PROJECT_PRERENDERSYSTEM_H
#include <memory>
#include <vector>

#include "Components.h"
#include "Entity.h"
#include "TextureManager.h"

class PreRenderSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        for (auto& entity: entities) {
            if (entity->hasComponent<Label>() && entity->getComponent<Label>().dirty) {
                //update label texture
                TextureManager::updateLabel(entity->getComponent<Label>());
            }
        }
    }
};

#endif //PROJECT_PRERENDERSYSTEM_H