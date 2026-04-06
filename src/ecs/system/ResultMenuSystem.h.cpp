//
// Created by Curry on 2026/04/05.
//

#include "ResultMenuSystem.h"
#include "Game.h"

void ResultMenuSystem::update(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_SPACE:
                Game::setRunning(false);
                break;
            default:
                break;
        }
    }
}
