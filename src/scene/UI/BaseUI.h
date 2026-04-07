//
// Created by Curry on 2026-04-07.
//

#ifndef PROJECT_BASEUI_H
#define PROJECT_BASEUI_H

#pragma once
#include "../scene/Scene.h"

namespace BaseUI {
    Entity& createSettingsOverlay(Scene& scene, int windowWidth, int windowHeight);
    Entity& createCogButton(Scene& scene, int windowWidth, int windowHeight, Entity& overlay);
    Entity& createBaseMenuOverlay(Scene& scene, int windowWidth, int windowHeight);
};



#endif //PROJECT_BASEUI_H
