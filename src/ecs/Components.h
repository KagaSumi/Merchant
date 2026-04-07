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
    // How far to push the box away from the Transform's top-left origin
    float offsetX = 0.0f;
    float offsetY = 0.0f;
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
    int spawnCount = 0;
    int maxSpawns = 0;
    float spawnInterval = 2.0f;
    bool isFinished = false;
};

//Game State, scene because might have multiple scenes
struct SceneState {
    int coinsCollected = 0;
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
    Static
};

struct Label {
    std::string text;
    TTF_Font *font = nullptr;
    SDL_Color color{};
    LabelType type = LabelType::Static;
    std::string textureCacheKey{};
    float wrapLength = 0;

    SDL_Texture *texture = nullptr;
    SDL_FRect dst{};
    bool visible = true;
    bool dirty = false;
};

struct PlayerTag {
    bool movementLocked = false;
};

enum class DayPhase {
    Init,
    Morning,
    ShopOpen,
    Evening,
    FadeToBlack,
};

struct DayCycle {
    DayPhase currentPhase = DayPhase::Init;
    int date = {};
    int weekDay = 0; //0 Sunday, 6 =Saturday
    SDL_Texture *mapTilesetTexture = nullptr;

    bool phaseSwapReady = false;
};

struct Wallet {
    int balance;
    int dailyIncome;
    int dailyExpenses;
};

struct Debt {
    int amount;
};

struct DisplayStand {
    ItemDef item = {};
    int quantity = 0;
    int reserved_quantity = 0; //incremented to prevent more than 1 customer buying the same item.
    SDL_FRect emptySrc = {};
    Entity* owner = nullptr;
};

struct Customer {
    Entity* displayStandEntity = nullptr;
    DisplayStand *displayStand = nullptr;
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

struct PathFinding {
    std::vector<SDL_Point> path;
    int pathIndex = 0;
    SDL_Point targetGridPos;
};

struct CustomerAI {
    CustomerAIState::state currentState = CustomerAIState::Browsing;
    float stateTimer = 0.0f;
    bool isWaiting = false;
    int itemsToBrowse = 3;
    int itemsBrowsed = 0;
};

struct InventoryEntry {
    ItemDef item;
    int quantity;
};

struct Inventory {
    std::vector<InventoryEntry> items;
    std::function<void(const std::vector<InventoryEntry>&)> onOpenUI;

    void openUI() {
        if (onOpenUI) onOpenUI(items);
    }
    void addItem(const ItemDef& def, int count) {
        for (auto& entry : items) {
            if (entry.item.name == def.name) {
                entry.quantity += count;
                return;
            }
        }
        items.push_back({def, count});
    }
};

struct ShopReputation {
    int reputation = 1;
    ShopReputation(int rep) : reputation(rep) {}
};

struct Interaction {
    std::function<void()> onInteract;
    Interaction() = default;
    Interaction(std::function<void()> callback) : onInteract(callback) {
    }
};

#endif //PROJECT_COMPONENTS_H
