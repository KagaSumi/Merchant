//
// Created by Curry on 2026/03/12.
//

#include "MainMenuSystem.h"

#include "Game.h"

void MainMenuSystem::update(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            case SDLK_SPACE:
                Game::onSceneChangeRequest("shop");
                break;
            default:
                break;
        }
    }
}
