//
// Created by Curry on 2026-04-07.
//

#ifndef PROJECT_BASEUI_H
#define PROJECT_BASEUI_H

#pragma once
#include "../scene/Scene.h"

namespace BaseUI {
    Entity &createSettingsOverlay(Scene &scene, int windowWidth, int windowHeight);

    Entity &createCogButton(Scene &scene, int windowWidth, int windowHeight, Entity &overlay);

    Entity &createBaseMenuOverlay(Scene &scene, int windowWidth, int windowHeight);

    Entity &createStandardButton(Scene &scene, Entity &parent, Vector2D pos, float width, float height,
                                 SDL_FRect spriteSrc, std::function<void()> onReleased);

    Entity &createLabel(Scene &scene, Entity &parent, const std::string &text, Vector2D pos,
                        SDL_Color color, const std::string &id, bool renderOnBoot = false);
};


#endif //PROJECT_BASEUI_H
