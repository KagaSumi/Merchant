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
    void render(std::vector<std::unique_ptr<Entity>> &entities) {
        Entity *cameraEntity = nullptr;

        // 1. Find Camera
        for (auto &e: entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }

        if (!cameraEntity) { return; }

        auto &cam = cameraEntity->getComponent<Camera>();

        // 2. Collect Renderable Entities
        // We store raw pointers here because we are just referencing existing entities for this frame.
        std::vector<Entity*> renderables;
        for (auto &entity: entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                if (entity->getComponent<Sprite>().renderLayer == RenderLayer::World) {
                    renderables.push_back(entity.get());
                }
            }
        }

        // 3. Y-Sort the Entities (The Depth Magic)
        std::sort(renderables.begin(), renderables.end(), [](Entity* a, Entity* b) {
            auto &tA = a->getComponent<Transform>();
            auto &tB = b->getComponent<Transform>();
            auto &spriteA = a->getComponent<Sprite>();
            auto &spriteB = b->getComponent<Sprite>();

            // Sort based on the BOTTOM of the sprite (feet), not the top (head).
            // We add the destination height to the Y position to find where the object touches the floor.
            float bottomA = tA.position.y + spriteA.dst.h;
            float bottomB = tB.position.y + spriteB.dst.h;

            return bottomA < bottomB;
        });

        // 4. Update and Draw Sorted Entities
        for (auto *entity : renderables) {
            auto &t = entity->getComponent<Transform>();
            auto &sprite = entity->getComponent<Sprite>();

            // Converting from world space to screen space
            sprite.dst.x = t.position.x - cam.view.x;
            sprite.dst.y = t.position.y - cam.view.y;

            // If entity has animation, update source rect
            if (entity->hasComponent<Animation>()) {
                auto anim = entity->getComponent<Animation>();
                sprite.src = anim.clips[anim.currentClip].frameIndicies[anim.currentFrame];
            }

            TextureManager::draw(sprite.Texture, &sprite.src, &sprite.dst);
        }
    }
};
#endif //PROJECT_RENDERSYSTEM_H
