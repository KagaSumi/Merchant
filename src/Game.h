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
#include "manager/AudioManager.h"
#include "manager/SceneManager.h"

struct GameState {
    int shopReputation = 1;
    int currentRepXP = 0;
    int xpToNextLevel = 100;
    int displayCasesUnlocked = 3;
    int walletBalance = 500;
    int debtTotal = 5000;
    int dayCount = 1;
};


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
    static void setRunning(bool state) {
        isRunning = state;
    }

    SDL_Window* getWindow() const { return window; }

    SDL_Renderer* renderer = nullptr;
    static AudioManager audioManager;
    SceneManager sceneManager;

    static std::function<void(std::string)> onSceneChangeRequest;
    static GameState gameState;

private:
    int frameCount = 0;
    static bool isRunning;

    SDL_Window* window = nullptr;
    SDL_Event event;

    //sdl tones colour channels as 8-bit unsigned
    //Range: 0-255
    Uint8 r,g,b,a;
};
