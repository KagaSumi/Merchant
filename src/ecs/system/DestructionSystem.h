//
// Created by Curry on 2026-02-25.
//

#ifndef PROJECT_DESTRUCTIONSYSTEM_H
#define PROJECT_DESTRUCTIONSYSTEM_H
#include <memory>
#include <vector>

#include "Components.h"
#include "Entity.h"

class DestructionSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity* cameraEntity = nullptr;

        //find Camera
        for (auto& entity : entities) {
            if (entity->hasComponent<Camera>()) {
                cameraEntity = entity.get();
                break;
            }
        }

        if (!cameraEntity) return;

        auto& cam = cameraEntity->getComponent<Camera>();
        // for (auto& entity : entities) {
        //     if (entity->hasComponent<Transform>() && entity->hasComponent<ProjectileTag>()) {
        //         auto& t = entity->getComponent<Transform>();
        //
        //         // destroy entity if out of view
        //
        //         if (
        //             t.position.x > cam.view.x + cam.view.w || // Right edge of view
        //             t.position.x < cam.view.x || //left edge
        //             t.position.y > cam.view.y + cam.view.h || //Bottom
        //             t.position.y < cam.view.y) { //Top
        //
        //             entity->destroy();
        //         }
        //     }
        // }
    }
};

#endif //PROJECT_DESTRUCTIONSYSTEM_H