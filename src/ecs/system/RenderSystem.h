
//
// Created by Curry on 2026-01-21.
//

#ifndef PROJECT_RENDERSYSTEM_H
#define PROJECT_RENDERSYSTEM_H
#include <memory>
#include <vector>
#include <algorithm> // Required for std::sort
#include <Entity.h>
#include "Components.h"
#include "TextureManager.h"

class RenderSystem {
public:
    void render(std::vector<std::unique_ptr<Entity>> &entities) {
        Entity *cameraEntity = nullptr;

        // 1. Find Camera
        for (auto &e : entities) {
            if (e->hasComponent<Camera>()) {
                cameraEntity = e.get();
                break;
            }
        }
        if (!cameraEntity) return;
        auto &cam = cameraEntity->getComponent<Camera>();

        // 2. Collect World-Layer Renderables
        std::vector<Entity*> renderables;
        for (auto &entity : entities) {
            if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                if (entity->getComponent<Sprite>().renderLayer == RenderLayer::World) {
                    renderables.push_back(entity.get());
                }
            }
        }

        // 3. Y-Sort (The Depth Magic)
        // We sort based on where the object touches the ground (Pivot Point)
        std::sort(renderables.begin(), renderables.end(), [](Entity* a, Entity* b) {
            auto &tA = a->getComponent<Transform>();
            auto &tB = b->getComponent<Transform>();
            auto &sA = a->getComponent<Sprite>();
            auto &sB = b->getComponent<Sprite>();

            // Depth = Transform Y + Local Offset Y + Sprite Height
            // This finds the absolute bottom edge of the sprite in the world
            float depthA = tA.position.y + sA.dst.y + sA.dst.h;
            float depthB = tB.position.y + sB.dst.y + sB.dst.h;

            return depthA < depthB;
        });

        // 4. Draw Sorted Entities
        for (auto *entity : renderables) {
            auto &t = entity->getComponent<Transform>();
            auto &sprite = entity->getComponent<Sprite>();

            // --- THE FIX: USE A TEMPORARY DRAW RECT ---
            // This prevents overwriting the original sprite.dst offsets
            SDL_FRect drawRect;
            drawRect.w = sprite.dst.w;
            drawRect.h = sprite.dst.h;

            // Screen Pos = (World Position) + (Permanent Offset) - (Camera Position)
            drawRect.x = t.position.x + sprite.dst.x - cam.view.x;
            drawRect.y = t.position.y + sprite.dst.y - cam.view.y;

            // Handle Animation
            if (entity->hasComponent<Animation>()) {
                auto &anim = entity->getComponent<Animation>();
                sprite.src = anim.clips[anim.currentClip].frameIndicies[anim.currentFrame];
            }

            // Draw using the temporary rect
            TextureManager::draw(sprite.Texture, &sprite.src, &drawRect);
        }
    }
};
#endif //PROJECT_RENDERSYSTEM_H