//
// Created by Curry on 2026/03/12.
//

#include "EventResponseSystem.h"

#include "Game.h"
#include "World.h"

EventResponseSystem::EventResponseSystem(World &world) {
    //subscriptions
    world.getEventManager().subscribe([this,&world](const BaseEvent &e) {
            if (e.type != EventType::Collision) return;
            const auto& collision = static_cast<const CollisionEvent &>(e); //cast base type to collision type

            onCollision(collision, "item", world);
            onCollision(collision, "wall", world);
            onCollision(collision, "projectile", world);
        });


    //Player Action subscriptions
    // world.getEventManager().subscribe([this,&world](const BaseEvent &e) {
    //         if (e.type != EventType::PlayerAction) return;
    //         const auto &playerAction = static_cast<const PlayerAction &>(e);
    //         //TODO onPlayerAction
    //     }
    // );


    //Mouse Action subscriptions
    world.getEventManager().subscribe([this,&world](const BaseEvent &e) {
            if (e.type != EventType::MouseInteraction) return;
            const auto &mouseEvent = static_cast<const MouseInteractionEvent &>(e);
            onMouseInteraction(mouseEvent);
        }
    );
}

void EventResponseSystem::onMouseInteraction(const MouseInteractionEvent &e) {

    if (!e.entity->hasComponent<Clickable>()) return;

    auto& clickable = e.entity->getComponent<Clickable>();

    switch (e.state) {
        case MouseInteractionState::Pressed:
            clickable.onPressed();
            break;
        case MouseInteractionState::Released:
            clickable.onReleased();
            break;
        case MouseInteractionState::Cancel:
            clickable.onCancel();
            break;
        default:
            break;
    }
}

void EventResponseSystem::onCollision(const CollisionEvent &e, const char *otherTag, World &world) {
    Entity *player = nullptr;
    Entity *other = nullptr;

    if (!getCollisionEntities(e, otherTag, player, other)) return;

    if (std::string(otherTag) == "item") {
        if (e.state != CollisionState::Enter) return;

        other->destroy();
        //scene state
        for (auto &entity: world.getEntities()) {
            if (!entity->hasComponent<SceneState>()) continue;

            auto &sceneState = entity->getComponent<SceneState>();
            sceneState.coinsCollected++;

            if (sceneState.coinsCollected > 1) {
                Game::onSceneChangeRequest("level2");
            }
        }
    }
    else if (std::string(otherTag) == "wall") {
        if (e.state != CollisionState::Stay) return;

        auto& t = player->getComponent<Transform>();
        t.position = t.oldPosition;
    }
    else if (std::string(otherTag) == "projectile") {
        if (e.state != CollisionState::Enter) return;

        //Simple and Direct
        //But Ideally we would only operate on data in an update function (hinting at transient entities)
        auto& health = player->getComponent<Health>();
        health.currentHealth--;

        Game::gameState.playerHealth = health.currentHealth;

        if (health.currentHealth <= 0) {
            player->destroy();
            //change scenes defer
            Game::onSceneChangeRequest("gameover");
        }
    }

}

bool EventResponseSystem::getCollisionEntities(
    const CollisionEvent &e,
    const char *otherTag,
    Entity *&player,
    Entity *&other) {
    if (e.entityA == nullptr || e.entityB == nullptr) return false;
    if (!(e.entityA->hasComponent<Collider>() && e.entityB->hasComponent<Collider>())) return false;

    auto &colliderA = e.entityA->getComponent<Collider>();
    auto &colliderB = e.entityB->getComponent<Collider>();


    if (colliderA.tag == "player" && colliderB.tag == otherTag) {
        player = e.entityA;
        other = e.entityB;
    } else if (colliderA.tag == otherTag && colliderB.tag == "player") {
        player = e.entityB;
        other = e.entityA;
    }

    return player && other;
}
