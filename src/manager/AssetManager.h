//
// Created by Curry on 2026/02/17.
//

#ifndef PROJECT_ASSETMANAGER_H
#define PROJECT_ASSETMANAGER_H
#include <unordered_map>

#include "Components.h"

class AssetManager {
    static std::unordered_map<std::string, Animation> animations;
    static std::unordered_map<std::string, TTF_Font*> fonts;
    static Animation loadAnimationFromXML(const char * path);
public:

    static void loadFont(const std::string& name, const char* path, float fontSize);
    static TTF_Font* getFont(const std::string& name);

    static void loadAnimation(const std::string& clipName,const char * path);
    static const Animation& getAnimation(const std::string& clipName);
};

#endif //PROJECT_ASSETMANAGER_H