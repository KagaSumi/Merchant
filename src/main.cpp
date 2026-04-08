
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
    game->init("Relics & Receipts", 800,600, false);

    float titleTimer = 0.0f;
    int frameCount = 0;

    while (game->running()) {

        Uint64 currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - ticks) / 1000.0f;
        ticks = currentTicks;

        game->handleEvents();
        game->update(deltaTime);
        game->render();

        actualFrameTime = SDL_GetTicks() - ticks;// Elapsed time in ms it took the frame

        //Metric Tracking
        titleTimer += deltaTime;
        frameCount++;

        if (titleTimer >= 1.0f) {
            float fps = frameCount / titleTimer;
            float avgFrameTime = (titleTimer / frameCount) * 1000.0f; // Convert back to ms

            // Format the title (using substr to trim long decimals)
            std::string newTitle = "Relics & Receipts | FPS: " + std::to_string((int)fps) +
                                   " | FT: " + std::to_string(avgFrameTime).substr(0, 4) + "ms";

            // Call SDL to update the title
            SDL_SetWindowTitle(game->getWindow(), newTitle.c_str());

            // Reset trackers for the next second
            titleTimer = 0.0f;
            frameCount = 0;
        }

        //Frame Limiter
        if (actualFrameTime < desiredFrametime) {
            SDL_Delay((desiredFrametime - actualFrameTime));
            //If the frame took less time than desired frame (delay the difference)
        }

    }
    delete game;

    return 0;
}
