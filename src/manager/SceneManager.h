//
// Created by Curry on 2026-02-25.
//

#ifndef PROJECT_SCENEMANAGER_H
#define PROJECT_SCENEMANAGER_H
#include <iostream>
#include <memory>

#include "../scene/Scene.h"

struct SceneParams {
    SceneType sceneType;
    const char* name;
    const char* mapPath;
    int windowWidth;
    int windowHeight;
};

class SceneManager {
    std::unordered_map<std::string,SceneParams> sceneParam;
    std::string pendingScene;

    void changeScene(const std::string& name) {
        auto it = sceneParam.find(name);
        if (it != sceneParam.end()) {
            const auto& params = it->second;

            //create Scene Object
            currentScene = std::make_unique<Scene>(
                params.sceneType,
                params.name,
                params.mapPath,
                params.windowWidth,
                params.windowHeight
                );
        }else {
            std::cerr << "Scene " << name << " not found!" << std::endl;
        }
    };

public:
    std::unique_ptr<Scene> currentScene;
    void loadScene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight) {
        sceneParam[sceneName] = {sceneType, sceneName, mapPath, windowWidth, windowHeight};
    }

    void changeSceneDeferred(const std::string& name) {
        pendingScene = name;
    }

    void update(const float dt, const SDL_Event &e ) {
        if (currentScene) currentScene->update(dt,e);
        if (!pendingScene.empty()) {
            changeScene(pendingScene);
            pendingScene.clear();
        }
    }

    Scene* getCurrentScene() {return currentScene.get();}
    void render() const {
        if (currentScene) currentScene->render();
    }
};


#endif //PROJECT_SCENEMANAGER_H