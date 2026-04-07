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
bool Game::isRunning = true;
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
    //Load Audio
    //Music
    audioManager.loadAudio("theme", "../asset/audio/果ての村.mp3");
    audioManager.loadAudio("morning", "../asset/audio/アルフヘイムの市場.mp3");
    audioManager.loadAudio("gameover", "../asset/audio/回顧録.mp3");
    audioManager.loadAudio("shopOpen", "../asset/audio/市場でお開門.mp3");
    audioManager.loadAudio("evening", "../asset/audio/旅支度_2.mp3");
    //UI SFX
    audioManager.loadAudio("summary", "../asset/audio/紙のページをめくる音.mp3");
    audioManager.loadAudio("clickHard", "../asset/audio/click-hard.ogg");
    audioManager.loadAudio("clickSoft", "../asset/audio/click-soft.ogg");
    audioManager.loadAudio("sell", "../asset/audio/sell.mp3");

    //Transition Noises
    audioManager.loadAudio("doorClose", "../asset/audio/DoorClose.mp3");
    audioManager.loadAudio("doorBell", "../asset/audio/ウエスタンドアを開くと鳴る入店音.mp3");

    //Environmental SFX
    audioManager.loadAudio("place", "../asset/audio/Placing Object.mp3");
    audioManager.loadAudio("customerSpawn", "../asset/audio/shopBell.ogg");


    //Start Music
    audioManager.playMusic("theme", 0);


    //load fonts
    AssetManager::loadFont("arial","../asset/fonts/arial.ttf",16);
    AssetManager::loadFont("arial-small", "../asset/fonts/arial.ttf", 13);

    //load asset
    AssetManager::loadAnimation("customer","../asset/animations/customer_animations.xml");

    //load scenes
    sceneManager.loadScene(SceneType::MainMenu,"mainmenu",nullptr,width,height);
    sceneManager.loadScene(SceneType::Gameplay,"shop","../asset/Shop.tmx",width,height);
    sceneManager.loadScene(SceneType::Victory,"victory",nullptr,width,height);
    sceneManager.loadScene(SceneType::GameOver,"gameover",nullptr,width,height);


    //start level 1
    sceneManager.changeSceneDeferred("mainmenu");

    //resolve scene callback
    onSceneChangeRequest = [this](std::string sceneName) {

        //some game state happening here
        if (sceneName == "gameover") {
            //Audio?
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
