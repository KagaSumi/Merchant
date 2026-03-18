//
// Created by Curry on 2026-01-21.
//

#ifndef PROJECT_COMPONENTS_H
#define PROJECT_COMPONENTS_H
#include <functional>
#include <queue>
#include <SDL3/SDL_render.h>

#include "Vector2D.h"
#include <string>
#include <unordered_map>

#include "AnimationClip.h"
#include "Entity.h"

struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    Vector2D oldPosition{};

};

//direction and speed
struct Velocity {
    Vector2D direction{};
    float speed{};
};

struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

enum class RenderLayer {
    World,
    UI
};

struct Sprite {
    SDL_Texture* Texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
    RenderLayer renderLayer = RenderLayer::World;
    bool visible = true;
};

struct Collider {
    std::string tag;
    SDL_FRect rect{};
    bool enabled = true;
};

struct Animation {
    std::unordered_map<std::string, AnimationClip> clips{};
    std::string currentClip{};
    float time{}; //time is accumulated for the current frame
    int currentFrame{}; //index of current frame in the clip
    float speed = 0.1f; //Time per frame
};

struct Camera {
    SDL_FRect view;
    float worldWidth;
    float worldHeight;
};

struct TimedSpawner {
    float spawnInterval{};
    std::function<void()> spawnCallback{};
    float timer{};
};

//Game State, scene because might have multiple scenes
struct SceneState {
    int coinsCollected = 0;
};

struct Health {
    int currentHealth{};
};

struct Clickable {
    std::function<void()> onPressed{};
    std::function<void()> onReleased{};
    std::function<void()> onCancel{};
    bool pressed = false;
};

struct Parent {
    Entity* parent = nullptr;
};

struct Children {
    std::vector<Entity*> children{};
};

struct PlayerTag{};
struct ProjectileTag{};

struct DayCycle {
    enum TimeofDay {Morning, Shop_Open, Evening};
};

struct MarketTrend {
    std::unordered_map<std::string,float> Trend;
};

struct Wallet {
    int wallet;
};

struct Debt {
    int debt;
};

struct DisplayStand {
    std::string name = "Empty";
    int quantity = 0 ;
    int reserved_quantity = 0; //incremented to prevent more than 1 customer buying the same item.
};

struct Customer {
    DisplayStand* DisplayStand{};
    int budget{};
    float mood{};
    int patience = 3;

};

struct CustomerAIState {
    enum state {
        Browsing,
        HeadingToRegister,
        LeavingStore
    };
};

struct CustomerAI {
    CustomerAIState::state currentState = CustomerAIState::Browsing;
    // Pathfinding data
    std::vector<SDL_Point> path; // List of grid coordinates from A*
    int pathIndex = 0;                  // Which node we are currently walking toward

    // Logic Timers
    float stateTimer = 0.0f;           // Stay in "Browsing" for 5 seconds
    SDL_Point targetGridpos; // Current heading
};

struct Inventory {
    //Iron Sword : 100g
    std::unordered_map<std::string,int> inventory;
};

struct ShopReputation {
    float Reputation = 1;
};

struct Interaction {
};

struct PathFinding {
    std::queue<Vector2D> path;
};

struct Dialogue {
    std::string text;
};

struct UIElement {
    int width;
    int height;
};


#endif //PROJECT_COMPONENTS_H