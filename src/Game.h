//
// Created by Curry on 2026-01-07.
//

// #ifndef PROJECT_GAME_H
// #define PROJECT_GAME_H
//
// #endif //PROJECT_GAME_H

#pragma once
#include <SDL3/SDL.h>
#include "World.h"
#include "manager/SceneManager.h"

struct GameState {
    int playerHealth;
    int Wallet;
    int Debt;
    int DayCycle;
    int  shopReputation = 1;
    int currentRepXP = 0;      // This increases after every sale
    int xpToNextLevel = 100;   // Threshold to level up

    int displayCasesUnlocked = 3; // Start with 3 Shelves
};

/* To be added in haggle system
void onSuccessfulSale(int profitMargin) {
    // Better haggling = more XP
    int xpGained = 10 + (profitMargin / 10);
    Game::gameState.currentRepXP += xpGained;

    // Check for level up
    if (Game::gameState.currentRepXP >= Game::gameState.xpToNextLevel) {
        Game::gameState.shopReputation++;
        Game::gameState.currentRepXP -= Game::gameState.xpToNextLevel;
        Game::gameState.xpToNextLevel = static_cast<int>(Game::gameState.xpToNextLevel * 1.5f); // Next level is harder

        std::cout << "Shop Leveled Up to " << Game::gameState.shopLevel << "!\n";
    }
}
 */
class Game {
public:
    Game();
    ~Game();

    //Types SDL Uses
    void init(const char* title, int width, int height, bool fullscreen);

    //game loop functions (handleEvents, update, render)
    void handleEvents(); //checks for input and system event
    void update(float deltaTime); //Handles Game log and changes to game state
    void render(); //Handles Drawing of game state to screen

    //used to free resources
    void destroy();

    bool running() {
        return isRunning;
    }
    SDL_Renderer* renderer = nullptr;

    SceneManager sceneManager;

    static std::function<void(std::string)> onSceneChangeRequest;
    static GameState gameState;

private:
    int frameCount = 0;
    bool isRunning = false;

    SDL_Window* window = nullptr;
    SDL_Event event;

    //sdl tones colour channels as 8-bit unsigned
    //Range: 0-255
    Uint8 r,g,b,a;
};
