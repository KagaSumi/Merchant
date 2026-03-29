//
// Created by Curry on 2026-02-25.
//

#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H
#include <SDL3/SDL_events.h>

#include "Vector2D.h"
#include "World.h"
#include  "SceneType.h"

class Scene {
public:
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
    //void createProjectile(Vector2D pos, Vector2D dir, int speed);

    void initMainMenu(int windowWidth, int windowHeight);
    void initGameplay(const char* mapPath, int windowWidth, int windowHeight);

    //Helper Function
    Entity& createDisplaycase(Vector2D location, SDL_Texture* texture,SDL_FRect src, SDL_FRect dst);

    //HUD
    Entity& createhudOverlay(int windowWidth, int windowHeight);

    //UI
    // ==================
    Entity& createSettingsOverlay(int windowWidth, int windowHeight);
    Entity& createCogButton(int windowWidth, int windowHeight, Entity& overlay);

    //Base Layer (Reuseable)
    Entity& createBaseMenuOverlay(int windowWidth, int windowHeight);
    Entity& createConfirmButton(int windowWidth, int windowHeight, Entity& overlay);

    //Haggling UI
    Entity& createHaggleOverlay(int windowWidth, int windowHeight, ItemDef& item);

    Entity& createItemHaggleOverlay(int windowWidth, int windowHeight, Entity& overlay, ItemDef& item);
    Entity& createPlayerItemHaggleOverlay(int windowWidth, int windowHeight, Entity& overlay, ItemDef& item);
    Entity& createHaggleButton(int windowWidth, int windowHeight, Entity& overlay);

    //Inventory UI
    Entity& createItemInventoryOverlay(int windowWidth, int windowHeight, Entity& overlay);
    Entity& createInventoryOverlay(int windowWidth, int windowHeight, Entity& overlay);

    //Order Screen
    Entity& createOrderStockButton(int windowWidth, int windowHeight, Entity& overlay);

    //Dialogue Screen
    Entity& createDialogueOverlay(int windowWidth, int windowHeight);

    void createSettingsUIComponents(Entity& overlay);
    void createHaggleUIComponents(Entity& overlay, int basePrice);

    void toggleSettingsOverlayVisibility(Entity& overlay);

    Entity& createPlayerPosLabel();
};

#endif //PROJECT_SCENE_H