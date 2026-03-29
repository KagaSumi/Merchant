//
// Created by Curry on 2026-01-21.
//

#ifndef PROJECT_WORLD_H
#define PROJECT_WORLD_H
#include <memory>
#include <vector>

#include "AnimationSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "CustomerAISystem.h"
#include "CustomerSpawnerSystem.h"
#include "DayCycleSystem.h"
#include "DestructionSystem.h"
#include "Entity.h"
#include "EventResponseSystem.h"
#include "HUDSystem.h"
#include "Items.h"
#include "event/EventManager.h"
#include "KeyboardInputSystem.h"
#include "MainMenuSystem.h"
#include "Map.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "SpawnTimerSystem.h"
#include "scene/SceneType.h"
#include  "UIRenderSystem.h"
#include  "MouseInputSystem.h"
#include "PreRenderSystem.h"

void printCollision(const CollisionEvent& collision);

class World {
    Map map;
    Items items;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> deferredEntities;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
    KeyboardInputSystem keyboardInputSystem;
    CollisionSystem collisionSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;
    EventManager eventManager;
    SpawnTimerSystem spawnTimerSystem;
    //DestructionSystem destructionSystem;
    EventResponseSystem eventResponseSystem{*this};
    MainMenuSystem mainMenuSystem;
    UIRenderSystem uiRenderSystem;
    MouseInputSystem mouseInputSystem;
    DayCycleSystem  dayCycleSystem;
    CustomerAISystem customerAISystem;
    PathfindingSystem pathfindingSystem;
    HUDSystem hudSystem;
    CustomerSpawnerSystem customerSpawnerSystem;
    PreRenderSystem preRenderSystem;

    public:
    World() = default;
    void update(float dt, const SDL_Event& event, const SceneType sceneType) {
        if (sceneType == SceneType::MainMenu) {
            //Main Menu Scene Update
            mainMenuSystem.update(event);
        }else {
            keyboardInputSystem.update(entities, event,eventManager);
            customerAISystem.update(entities, dt,dayCycleSystem);
            movementSystem.update(entities, dt);
            collisionSystem.update(*this);
            animationSystem.update(entities, dt);
            dayCycleSystem.update(entities);
            cameraSystem.update(entities);
            customerSpawnerSystem.update(entities, dt);
            //destructionSystem.update(entities);
            hudSystem.update(entities);
        }


        mouseInputSystem.update(*this,event);
        preRenderSystem.update(entities);

        synchronizeEntities();
        cleanup();
    }

    void render() {
        for (auto& entity : entities) {
            if (entity->hasComponent<Camera>()) {
                map.draw(entity->getComponent<Camera>());
                break;
            }
        }
        renderSystem.render(entities);
        uiRenderSystem.render(entities);
    }

    Entity& createEntity() {
        //emplace instead of push so no copy is created
        entities.emplace_back(std::make_unique<Entity>());
        return *entities.back();
    }

    Entity& createDeferredEntity() {
        deferredEntities.emplace_back(std::make_unique<Entity>());
        return *deferredEntities.back();

    }

    std::vector<std::unique_ptr<Entity>>& getEntities() {
        return entities;
    }

    void cleanup() {
        //use a lambda predicate to remove all inactive entities
        std::erase_if(
            entities,
            [](std::unique_ptr<Entity>& e) {
                return !e->isActive();
            });
    }

    void synchronizeEntities() {
        if (!deferredEntities.empty()) {
            //push back all deferred entities to the entities vector
            std::move(
                deferredEntities.begin(),
                deferredEntities.end(),
                std::back_inserter(entities)
            );
            //clearing the creation buffer
            deferredEntities.clear();
        };
    }

    EventManager& getEventManager() {return eventManager;}
    Map& getMap(){return map;}
    Items& getItems(){return items;}
};

#endif //PROJECT_WORLD_H