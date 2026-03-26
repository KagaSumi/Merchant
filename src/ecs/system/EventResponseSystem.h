//
// Created by Curry on 2026/03/12.
//

#ifndef PROJECT_EVENTRESPONSESYSTEM_H
#define PROJECT_EVENTRESPONSESYSTEM_H
#include <functional>

#include "event/BaseEvent.h"

class Entity;
class World;

class EventResponseSystem {
public:
    EventResponseSystem(World &world);
private:
    //collisions
    void onCollision(const CollisionEvent& e, const char* otherTag, World& world);
    bool getCollisionEntities(const CollisionEvent& e, const char* otherTag, Entity*& player, Entity*& other);

    //player action
    void onPlayerAction(Entity* player, PlayerAction action,World& world);


    //Mouse Interaction
    void onMouseInteraction(const MouseInteractionEvent& e);
};

#endif //PROJECT_EVENTRESPONSESYSTEM_H