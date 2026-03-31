//
// Created by Curry on 2026-01-07.
//

#include "Game.h"
#include "Map.h"
#include <iostream>
#include <ostream>

#include "manager/AssetManager.h"
#include "Components.h"
#include "TextureManager.h"

//#include "GameObject.h"

//GameObject *player = nullptr;

GameState Game::gameState{};
std::function<void(std::string)> Game::onSceneChangeRequest;

Game::Game() {}

Game::~Game() {
    destroy();
}

void Game::init(const char *title, int width, int height, bool fullscreen) {

    int flags = 0;
    if (fullscreen){
        flags = SDL_WINDOW_FULLSCREEN;
    }

    //Initialize SDL library
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 1) {
        std::cout <<"Subsystem initialized..." << std::endl;
        window = SDL_CreateWindow(title, width, height,flags);

        if (window) {
            std::cout <<"Window created..." << std::endl;
        }

        //null lets SDL pick best
        renderer = SDL_CreateRenderer(window, nullptr);
        if (renderer) {
            std::cout <<"Renderer created..." << std::endl;
        }else {
            std::cout <<"Renderer could not be created." << std::endl;
        }

        if (TTF_Init() != 1) {
            std::cout <<"TTF_Init could not be initialized..." << std::endl;
        }


        isRunning = true;
    }else {
        isRunning = false;
    }
    //load fonts
    AssetManager::loadFont("arial","../asset/fonts/arial.ttf",16);

    //load asset
    AssetManager::loadAnimation("player","../asset/animations/fox_animations.xml");
    AssetManager::loadAnimation("customer","../asset/animations/customer_animations.xml");

    //load scenes
    sceneManager.loadScene(SceneType::MainMenu,"mainmenu",nullptr,width,height);
    sceneManager.loadScene(SceneType::Gameplay,"level1","../asset/Shop.tmx",width,height);
    //sceneManager.loadScene(SceneType::Gameplay,"level1","../asset/map.tmx",width,height);

    //init game data/state
    gameState.playerHealth = 5;
    gameState.shopReputation = 1; //1 = Normal Repuation
    gameState.DayCycle = 1; //Day 1
    gameState.Wallet = 500; //Starting Cash
    gameState.Debt = 5000; //Total until Game Clear

    //start level 1
    sceneManager.changeSceneDeferred("mainmenu");

    //resolve scene callback
    onSceneChangeRequest = [this](std::string sceneName) {

        //some game state happening here
        if (sceneManager.currentScene->getName() == "level2" && sceneName == "level2") {
            std::cout <<"You win!" << std::endl;
            isRunning = false;
            return;
        }
        if (sceneName == "gameover") {
            std::cout <<"You lose!" << std::endl;
            isRunning = false;
            return;
        }

        sceneManager.changeSceneDeferred(sceneName);
    };
}

void Game::handleEvents() {
    //SDL listen to OS events and adds to Queue


    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_EVENT_QUIT: //Usually triggered when User closes window
            isRunning = false;
            break;
        default:
            break;
    }
}

void Game::update(float deltaTime) {
    sceneManager.update(deltaTime,event);

}

void Game::render() {
    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    SDL_RenderClear(renderer);

    //All drawing would go here
    //displays everything that was just dawn
    //draws it in memory first to a back buffer
    //swaps the back buffer to the screen
    sceneManager.render();

    SDL_RenderPresent(renderer);
}

void Game::destroy() {
    TextureManager::clean();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Game Destroyed" <<std::endl;
}
