//
// Created by Curry on 2026-04-07.
//

#ifndef PROJECT_HUDUI_H
#define PROJECT_HUDUI_H

#pragma once
#include "../World.h"
#include "../scene/Scene.h"

struct HUDSession {
    Entity* dayLabelRef = nullptr;
    Entity* phaseLabelRef = nullptr;
    Entity* walletLabelRef = nullptr;
    Entity* walletIconRef = nullptr;
};

namespace HudUI {
    Entity& create(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIHud);
    void update(Scene& scene, const Wallet& wallet, const DayCycle& dayCycle, Entity* UIHud);
}
#endif //PROJECT_HUDUI_H
