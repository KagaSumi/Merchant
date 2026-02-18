//
// Created by Curry on 2026-01-07.
//

#include "Game.h"
#include "Map.h"
#include <iostream>
#include <ostream>

#include "AssetManager.h"
#include "Components.h"
#include "TextureManager.h"

//#include "GameObject.h"

//GameObject *player = nullptr;

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
        isRunning = true;
    }else {
        isRunning = false;
    }

    AssetManager::loadAnimation("player","../asset/animations/fox_animations.xml");

    //load map
    world.getMap().load("../asset/map.tmx",TextureManager::load("../asset/tileset.png"));
    for (auto& collider : world.getMap().colliders) {
        auto& e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x,collider.rect.y),0.0f,1.0f);
        auto& c = e.addComponent<Collider>("wall");

        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;

        SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
        SDL_FRect colSrc {0,32,32,32};
        SDL_FRect colDst {c.rect.x,c.rect.y,c.rect.w,c.rect.h};
        e.addComponent<Sprite>(tex,colSrc,colDst);
    }


    //add coins
    for (auto& coin : world.getMap().coins) {
        auto& item = world.createEntity();
        item.addComponent<Transform>(Vector2D(coin.rect.x,coin.rect.y),0.0f,1.0f);
        auto& c = item.addComponent<Collider>("item");

        c.rect.x = coin.rect.x;
        c.rect.y = coin.rect.y;

        SDL_Texture* tex = TextureManager::load("../asset/coin.png");
        SDL_FRect colSrc {0,0,32,32};
        SDL_FRect colDst {c.rect.x,c.rect.y,32,32};
        item.addComponent<Sprite>(tex,colSrc,colDst);
    }
    // auto& item(world.createEntity());
    // auto& itemTransform = item.addComponent<Transform>(Vector2D(100,200), 0.0f,1.0f);
    //
    // SDL_Texture*  itemTex = TextureManager::load("../asset/coin.png");
    // SDL_FRect itemSrc{0,0,32,32};
    //
    // SDL_FRect itemDest {itemTransform.position.x, itemTransform.position.y,32,32};
    // item.addComponent<Sprite>(itemTex,itemSrc,itemDest);
    //
    // auto& itemCollider = item.addComponent<Collider>("item");
    // itemCollider.rect.w = itemDest.w;
    // itemCollider.rect.h = itemDest.h;

    auto& cam = world.createEntity();
    SDL_FRect camView{};
    camView.w = width;
    camView.h = height;
    cam.addComponent<Camera>(camView, static_cast<float>(world.getMap().width * 32), static_cast<float>(world.getMap().height * 32));


    auto& player(world.createEntity());
    auto& playerTransform = player.addComponent<Transform>(Vector2D(0,0),1.0f);
    player.addComponent<Velocity>(Vector2D(0,0), 120.0f);

    Animation anim = AssetManager::getAnimation("player");
    player.addComponent<Animation>(anim);

    SDL_Texture* tex = TextureManager::load("../asset/animations/fox_anim.png");
    // SDL_FRect playerSrc {0,0,32,44};
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndicies[0];
    SDL_FRect playerDst {playerTransform.position.x,playerTransform.position.y,64,64};
    player.addComponent<Sprite>(tex,playerSrc,playerDst);

    auto& playerCollider = player.addComponent<Collider>("player");
    playerCollider.rect.w = playerDst.w;
    playerCollider.rect.h = playerDst.h;

    player.addComponent<PlayerTag>();

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
    world.update(deltaTime, event);

}

void Game::render() {
    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    SDL_RenderClear(renderer);

    //All drawing would go here
    world.render();
    //displays everything that was just dawn
    //draws it in memory first to a back buffer
    //swaps the back buffer to the screen

    SDL_RenderPresent(renderer);
}

void Game::destroy() {
    TextureManager::clean();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Game Destroyed" <<std::endl;
}
