//
// Created by Curry on 2026-03-24.
//

#ifndef PROJECT_HUDSYSTEM_H
#define PROJECT_HUDSYSTEM_H
#include <memory>
#include <sstream>
#include <vector>

#include "Components.h"
#include "Entity.h"

class HUDSystem {
    public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities) {
        Entity * playerEntity = nullptr;

        //find player
        for (auto &e : entities) {
            if (e->hasComponent<PlayerTag>()) {
                playerEntity = e.get();
                break;
            }
        }

        if (!playerEntity) {return;}
        auto& playerTransform = playerEntity->getComponent<Transform>();

        for (auto &e : entities) {
            if (e->hasComponent<Label>()) {
                auto& label = e->getComponent<Label>();


                //update player position label
                // if (label.type == LabelType::PlayerPosition) {
                //     std::stringstream ss;
                //     ss << "Player position: " << playerTransform.position;
                //     label.text = ss.str();
                //     label.dirty = true;
                // }
            }
        }


    }
};

#endif //PROJECT_HUDSYSTEM_H