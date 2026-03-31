
#include "Game.h"
#include "utils/Vector2D.h"
#include "iostream"

//global Variable
Game *game = nullptr;

int main(){
    //testVector2D();
    const int FPS = 60; //60 is the norm
    const int desiredFrametime = 1000/FPS;

    Uint64 ticks = SDL_GetTicks(); //time in ms since we initialized SDL
    float deltaTime = 0.0f;
    int actualFrameTime ;

   game = new Game();
    game->init("Assignment 1", 800,600, false);

    while (game->running()) {

        Uint64 currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - ticks) / 1000.0f;
        ticks = currentTicks;

        game->handleEvents();
        game->update(deltaTime);
        game->render();

        actualFrameTime = SDL_GetTicks() - ticks;// Elapsed time in ms it took the frame

        //Frame Limiter
        if (actualFrameTime < desiredFrametime) {
            SDL_Delay((desiredFrametime - actualFrameTime));
            //If the frame took less time than desired frame (delay the difference)
        }

    }
    delete game;

    return 0;
}
