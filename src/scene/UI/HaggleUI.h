//
// Created by Curry on 2026-04-07.
//

#ifndef PROJECT_HAGGLEUI_H
#define PROJECT_HAGGLEUI_H

#pragma once
#include <vector>
#include "../World.h"      // Need this for Entity, World, ItemDef
#include "../scene/Scene.h" // Forward declare or include Scene if you need Scene-specific methods


struct HaggleSession {
    ItemDef currentItem;
    std::vector<int> digits = {0, 0, 0, 0, 0};

    Entity* percentLabelRef = nullptr;
    Entity* itemNameRef = nullptr;
    Entity* itemIconRef = nullptr;
    Entity* itemBaseValRef = nullptr;
    Entity* digitRefs[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    float columnCenters[5] = {0, 0, 0, 0, 0};

    float subWidth = 180.0f;
    float absoluteX = 0.0f;

    int getProposedPrice() const;
    int getPercentage() const;
};

// 2. Create a namespace for the UI Builder functions
namespace HaggleUI {
    // We pass Scene reference so the UI can access scene.world and scene methods
    // We pass outUIMenu pointer reference so it can update Scene's tracked UI pointer
    Entity& create(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIMenu);

    Entity& update(Scene& scene, ItemDef& item, Entity* UIMenu);
}



#endif //PROJECT_HAGGLEUI_H
