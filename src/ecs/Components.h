//
// Created by Curry on 2026-01-21.
//

#ifndef PROJECT_COMPONENTS_H
#define PROJECT_COMPONENTS_H
#include <functional>
#include <queue>
#include <SDL3/SDL_render.h>
#include "SDL3_ttf/SDL_ttf.h"

#include "Vector2D.h"
#include <string>
#include <unordered_map>

#include "AnimationClip.h"
#include "Entity.h"
#include "Items.h"

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
    SDL_Texture *Texture = nullptr;
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
    int direction = 2; //1,2,3,4 (N,S,W,E)
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

struct Spawner {
    std::function<void()> spawnCallback{};
    bool isFinished = (bool) spawnCount;
    int spawnCount = 0;
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
    Entity *parent = nullptr;
};

struct Children {
    std::vector<Entity *> children{};
};

enum class LabelType {
    PlayerPosition,
    Damage,
    Health
};

struct Label {
    std::string text;
    TTF_Font *font = nullptr;
    SDL_Color color{};
    LabelType type = LabelType::PlayerPosition;
    std::string textureCacheKey{};
    SDL_Texture *texture = nullptr;
    SDL_FRect dst{};
    bool visible = true;
    bool dirty = false;
};

struct PlayerTag {
};

enum class DayPhase {
    Morning,
    ShopOpen,
    Evening,
    GameOver
};

struct DayCycle {
    DayPhase currentPhase = DayPhase::Morning;
    int date = {};
    int weekDay = 0; //0 Sunday, 6 =Saturday
    SDL_Texture *mapTilesetTexture = nullptr;

    bool phaseSwapReady = false;
};

struct MarketTrend {
    std::unordered_map<std::string, float> Trend;
};

struct Wallet {
    int balance;
};

struct Debt {
    int amount;
};

struct DisplayStand {
    ItemDef item = {};
    int quantity = 0;
    int reserved_quantity = 0; //incremented to prevent more than 1 customer buying the same item.
};

struct Customer {
    DisplayStand *DisplayStand{};
    int budget{}; // Maybe simplify and remove budget rng, and just deal with basevalue manip
    float mood{}; // Maybe simplify and remove budget rng, and just deal with basevalue manip
    int patience = 3;
};

struct CustomerAIState {
    enum state {
        Browsing,
        HeadingToRegister,
        LeavingStore
    };
};

// TODO: Break out Customer AI with Pathfinding Component
// struct PathFinding {
//     std::vector<SDL_Point> path; // List of grid coordinates from A*
//     int pathIndex = 0;           // Which node we are currently walking toward
// };

struct CustomerAI {
    CustomerAIState::state currentState = CustomerAIState::Browsing;
    // Pathfinding data
    std::vector<SDL_Point> path; // List of grid coordinates from A*
    int pathIndex = 0; // Which node we are currently walking toward

    // Logic Timers
    float stateTimer = 0.0f; // Stay in "Browsing" for 5 seconds
    bool isWaiting = false;
    SDL_Point targetGridpos; // Current heading

    int itemsToBrowse = 3; // How many shelves to visit before paying
    int itemsBrowsed = 0; // How many they have visited so far
};

struct Inventory {
    //Iron Sword : 100g
    std::unordered_map<std::string, int> inventory;
};

struct ShopReputation {
    float Reputation = 1;
};

struct Interaction {
    std::function<void()> onInteract;
    Interaction() = default;
    Interaction(std::function<void()> callback) : onInteract(callback) {
    }
};

#endif //PROJECT_COMPONENTS_H
