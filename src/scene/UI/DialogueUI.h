//
// Created by Curry on 2026-04-07.
//

#ifndef PROJECT_DIALOGUEUI_H
#define PROJECT_DIALOGUEUI_H

#pragma once
#include <string>
#include <functional>
#include "../World.h"
#include "../scene/Scene.h"

struct DialogueSession {
    std::string currentMessage;
    Entity* messageLabelRef = nullptr;
    Entity* confirmBtnRef = nullptr;
};

namespace DialogueUI {
    Entity& create(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIDialogue, std::function<void()>& outSimpleDialogueConfirm);
    Entity& update(Scene& scene, const std::string& message, Entity* UIDialogue);
    void showSimple(Scene& scene, const std::string& message, Entity* UIDialogue, std::function<void()>& outSimpleDialogueConfirm, Entity* playerEntity);
}

#endif //PROJECT_DIALOGUEUI_H
