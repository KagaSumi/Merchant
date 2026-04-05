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
        const auto &collision = static_cast<const CollisionEvent &>(e); //cast base type to collision type

        onCollision(collision, "item", world);
        onCollision(collision, "wall", world);
        onCollision(collision, "projectile", world);
    });


    //Player Action subscriptions
    world.getEventManager().subscribe([this,&world](const BaseEvent &e) {
        if (e.type != EventType::PlayerAction) return;

        // 1. Cast the event to the correct struct
        const auto &actionEvent = static_cast<const PlayerActionEvent &>(e);

        // 2. Unpack the data and pass it to your existing function!
        onPlayerAction(actionEvent.player, actionEvent.action,world);
    });

    //Mouse Action subscriptions
    world.getEventManager().subscribe([this,&world](const BaseEvent &e) {
            if (e.type != EventType::MouseInteraction) return;
            const auto &mouseEvent = static_cast<const MouseInteractionEvent &>(e);
            onMouseInteraction(mouseEvent);
        }
    );
}

void EventResponseSystem::onPlayerAction(Entity* player, PlayerAction action, World& world) {
    switch (action) {
        case PlayerAction::Interact: {
            // 1. Safety check
            if (!player || !player->hasComponent<Transform>() || !player->hasComponent<Animation>()) {
                break;
            }

            auto& pTransform = player->getComponent<Transform>();
            auto& pAnim = player->getComponent<Animation>();

            // 2. Calculate target point
            float reachDistance = 32.0f; // Tweak this for your grid!
            Vector2D targetPoint = pTransform.position;

            switch (pAnim.direction) {
                case 1: targetPoint.y -= reachDistance; break; // Up
                case 2: targetPoint.y += reachDistance; break; // Down
                case 3: targetPoint.x -= reachDistance; break; // Left
                case 4: targetPoint.x += reachDistance; break; // Right
            }

            // 3. Find the closest interactable entity
            // 3. Find the closest interactable entity
            float interactionRadius = 32.0f;

            // Track the winner
            Entity* closestEntity = nullptr;
            float shortestDistance = interactionRadius; // Start with the max allowed distance

            for (auto& e : world.getEntities()) {
                if (e->hasComponent<Interaction>() && e->hasComponent<Transform>()) {
                    auto& targetTransform = e->getComponent<Transform>();

                    float dx = targetPoint.x - targetTransform.position.x;
                    float dy = targetPoint.y - targetTransform.position.y;
                    float distance = std::sqrt(dx * dx + dy * dy);

                    // If it's within radius AND closer than anything else we've found
                    if (distance <= shortestDistance) {
                        shortestDistance = distance;
                        closestEntity = e.get(); // Save this as the new best target
                    }
                }
            }

            // 4. Trigger the winner (if we found one)
            if (closestEntity) {
                std::cout << "[EventResponseSystem] HIT! Triggering callback!\n";
                closestEntity->getComponent<Interaction>().onInteract();
            } else {
                std::cout << "[EventResponseSystem] Swished at the air. Nothing in range.\n";
            }
            break; // Break out of the switch case
        }
        case PlayerAction::Inventory: {
            if (player && player->hasComponent<Inventory>()) {
                auto& inv = player->getComponent<Inventory>();
                if (inv.uiRef) {
                    player->getComponent<Inventory>().openUI();
                }
            }
            break;
        }

    }
}

void EventResponseSystem::onMouseInteraction(const MouseInteractionEvent &e) {
    if (!e.entity->hasComponent<Clickable>()) return;

    auto &clickable = e.entity->getComponent<Clickable>();

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
    } else if (std::string(otherTag) == "wall") {
        if (e.state != CollisionState::Stay) return;

        auto &t = player->getComponent<Transform>();
        t.position = t.oldPosition;
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
