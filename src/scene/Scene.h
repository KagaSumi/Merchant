//
// Created by Curry on 2026-02-25.
//

#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H
#include <SDL3/SDL_events.h>

#include "Vector2D.h"
#include "World.h"
#include  "SceneType.h"
#include "UI/InventoryUI.h"

class Scene {
public:
    //UI Entity References
    Entity* UIMenu = nullptr; //Haggle
    Entity* UIDaySummary = nullptr;
    Entity* UIQuantityScreen = nullptr;
    Entity* UIOrderScreen = nullptr;
    Entity* UIDialogue = nullptr;
    Entity* UIHud = nullptr;
    Entity* UIInventory = nullptr;
    std::function<void()> simpleDialogueConfirm = nullptr;

    //Entity References
    Entity* storeEntity = nullptr;
    Entity* playerEntity = nullptr;
    Entity* spawnerEntity = nullptr;
    SDL_Texture* tilemapTex = nullptr;

    Scene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight);


    void update(const float dt, const SDL_Event &e) {
        world.update(dt,e,type);
    }

    void render() {
        world.render();
    }

    World world;

    const std::string& getName() const {return name;}

private:
    std::string name;
    SceneType type;

    void initMainMenu(int windowWidth, int windowHeight);
    void initLose(int windowWidth, int windowHeight);
    void initWin(int windowWidth, int windowHeight);
    void initGameplay(const char* mapPath, int windowWidth, int windowHeight);

    //Gameplay Helpers
    void initAssets(const char* mapPath);
    void initUI(int windowWidth, int windowHeight);
    void initWorld(int windowWidth, int windowHeight);
    void initPlayer();
    void initSystems();
    void initHaggleSystem();
    void initDayCycleCallbacks();
    void initEntities();

    //Helper Function
    Entity& createDisplaycase(Vector2D location, SDL_Texture* texture,SDL_FRect src, SDL_FRect dst, DayCycle& dayCycle,Entity* playerRef);
};

#endif //PROJECT_SCENE_H