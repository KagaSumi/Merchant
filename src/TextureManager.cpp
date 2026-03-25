//
// Created by Curry on 2026-01-14.
//

#include "TextureManager.h"
#include <iostream>
#include <ostream>
#include <Game.h>

//Doesn't create any extra mem, find reference of object
extern Game *game;
std::unordered_map<std::string, SDL_Texture *> TextureManager::textures;

//load our texture with a path
SDL_Texture *TextureManager::load(const char *path) {
    //Caching
    //Check if texture alr exists in the map
    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }
    //A surface represent an image in ram
    SDL_Surface *tempSurface = IMG_Load(path);
    //Do we want to do any pre-process before offloading to GPU?

    if (!tempSurface) {
        std::cout << "Failed to load image: " << path << std::endl;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, tempSurface);

    SDL_DestroySurface(tempSurface);

    //Check if creation is successful
    if (!texture) {
        std::cout << "Failed to create texture from: " << path << std::endl;
        return nullptr;
    }
    //store new texture in cache
    textures[path] = texture;

    return texture;
}

void TextureManager::loadLabel(Label& label) {
    auto it = textures.find(label.textureCacheKey);
    if (it != textures.end()) {
        label.texture = it->second;
        return;
    }

    //update label
    updateLabel(label);
}

void TextureManager::updateLabel(Label& label) {

    if (!label.dirty) return;
    if (label.texture) {
        SDL_DestroyTexture(label.texture);
        label.texture = nullptr;
    }

    SDL_Surface* tempSurface = TTF_RenderText_Blended(
        label.font,
        label.text.c_str(),
        label.text.size(),
        label.color);

        if (!tempSurface) {
            std::cerr << "Failed to load surface: " << label.textureCacheKey << std::endl;
        }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, tempSurface);
    SDL_DestroySurface(tempSurface);

    if (!texture) {
        std::cerr << "Failed to create texture from: " << label.textureCacheKey << std::endl;
    }

    float w,h;
    SDL_GetTextureSize(texture, &w, &h);
    label.dst.w = w;
    label.dst.h = h;

    //cache now
    label.texture = texture;
    textures[label.textureCacheKey] = texture;

    label.dirty = false; //clean
}

void TextureManager::draw(SDL_Texture *texture, const SDL_FRect* src,const SDL_FRect* dst) {
    //draw texture to screen from vram
    SDL_RenderTexture(game->renderer, texture, src, dst);
}

void TextureManager::clean() {
    for (auto& tex: textures) {
        SDL_DestroyTexture(tex.second);
        tex.second = nullptr;
    }
    textures.clear();
}