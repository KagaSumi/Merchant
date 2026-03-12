//
// Created by Curry on 2026-01-21.
//

#ifndef PROJECT_RENDERSYSTEM_H
#define PROJECT_RENDERSYSTEM_H
#include <memory>
#include <vector>

#include <Entity.h>
#include "Components.h"
#include "TextureManager.h"

class RenderSystem {
    public:
    void render(std::vector<std::unique_ptr<Entity>> & entities) {
        Entity* cameraEntity = nullptr;

        //find Camera
        for (auto& e : entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }

        if (!cameraEntity) {return;}

        auto & cam = cameraEntity->getComponent<Camera>();

        for (auto& entity: entities) {
            if (entity-> hasComponent<Transform>() && entity->hasComponent<Sprite>()) {

                auto& t = entity->getComponent<Transform>();
                auto& sprite = entity->getComponent<Sprite>();

                if (sprite.renderLayer != RenderLayer::World) continue;

                //Converting from world space to screen space
                sprite.dst.x = t.position.x - cam.view.x;
                sprite.dst.y = t.position.y - cam.view.y;

                //if entity has animation, update source rect
                if (entity->hasComponent<Animation>()) {
                    auto anim = entity->getComponent<Animation>();
                    sprite.src = anim.clips[anim.currentClip].frameIndicies[anim.currentFrame];
                }

                TextureManager::draw(sprite.Texture,sprite.src,sprite.dst);
            }
        }
    }
};
#endif //PROJECT_RENDERSYSTEM_H