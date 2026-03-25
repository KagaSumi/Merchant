//
// Created by Curry on 2026/02/17.
//

#include "AssetManager.h"

#include <bits/fs_fwd.h>
#include <bits/locale_facets_nonio.h>

#include "tinyxml2.h"

std::unordered_map<std::string, Animation> AssetManager::animations;
std::unordered_map<std::string, TTF_Font*> AssetManager::fonts;

void AssetManager::loadFont(const std::string &name, const char *path, float fontSize) {
    fonts.emplace(name, TTF_OpenFont(path,fontSize));
}

TTF_Font* AssetManager::getFont(const std::string &name) {
    return fonts[name];
}

void AssetManager::loadAnimation(const std::string &clipName, const char * path) {
    Animation animation = loadAnimationFromXML(path);
    animations[clipName] = animation;
}

const Animation& AssetManager::getAnimation(const std::string &clipName) {
    return animations[clipName];
}

Animation AssetManager::loadAnimationFromXML(const char * path) {
    Animation anim;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path);

    auto* root = doc.FirstChildElement("animations");
    for (auto * clipElem = root->FirstChildElement();
        clipElem;
        clipElem = clipElem->NextSiblingElement()) {
        std::string clipName = clipElem->Name();
        AnimationClip clip;

        for (auto* frameElem = clipElem->FirstChildElement("frame");
            frameElem;
            frameElem = frameElem->NextSiblingElement("frame")) {

            SDL_FRect rect;
            frameElem->QueryFloatAttribute("x", &rect.x);
            frameElem->QueryFloatAttribute("y", &rect.y);
            frameElem->QueryFloatAttribute("w", &rect.w);
            frameElem->QueryFloatAttribute("h", &rect.h);
            clip.frameIndicies.push_back(rect);
        }

        anim.clips[clipName] = clip;
    }

    if (!anim.clips.empty()) {
        anim.currentClip = anim.clips.begin()->first;
    }

    return anim;
}
