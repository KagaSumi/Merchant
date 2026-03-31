//
// Created by Curry on 2026-01-14.
//

#ifndef PROJECT_MAP_H
#define PROJECT_MAP_H

#include <vector>
#include <SDL3/SDL.h>
#include <Components.h>

class Map {
    public:
    Map() = default;
    ~Map() = default;

    void load(const char *path, SDL_Texture *ts);
    void draw(const Camera& cam);

    SDL_Texture *tileset = nullptr;
    int width{}, height{};
    std::vector<std::vector<int>> tileData;
    std::vector<int> AIWalkable;
    std::vector<Collider> colliders;
    std::vector<Collider> coins;
    SDL_Point Door;
    SDL_Point Register;

};
#endif //PROJECT_MAP_H