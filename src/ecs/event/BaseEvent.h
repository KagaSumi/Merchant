//
// Created by Curry on 2026/03/12.
//

#ifndef PROJECT_BASEEVENT_H
#define PROJECT_BASEEVENT_H


class Entity;

enum class EventType {
    Collision,
    PlayerAction,
    MouseInteraction
};


struct BaseEvent {
    EventType type{};
    virtual  ~BaseEvent() = default; //in C++ always make the destructor virtual from a class you are deriving from
};

enum class CollisionState {
    Enter,
    Stay,
    Exit
};

struct CollisionEvent : BaseEvent {
    Entity* entityA = nullptr;
    Entity* entityB = nullptr;
    CollisionState state{};
    CollisionEvent(Entity* entityA, Entity* entityB, CollisionState state) : entityA(entityA), entityB(entityB), state(state) {
        type = EventType::Collision;
    }
};

enum class PlayerAction {
    Interact,
};

struct PlayerActionEvent : BaseEvent {
    Entity* player = nullptr;
    PlayerAction action{};
    PlayerActionEvent(Entity* player, PlayerAction action) : player(player), action(action) {
        type = EventType::PlayerAction;
    }
};

enum class MouseInteractionState {
    Pressed,
    Released,
    Cancel
};

struct MouseInteractionEvent : BaseEvent {
    Entity* entity = nullptr;
    MouseInteractionState state{};
    MouseInteractionEvent(Entity* entity, MouseInteractionState state) : entity(entity), state(state) {
        type = EventType::MouseInteraction;
    }
};

#endif //PROJECT_BASEEVENT_H