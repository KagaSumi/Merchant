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
    void drawLayer(const std::vector<std::vector<int>>& layer,const Camera& cam, int firstGid);
    void debugDrawNavMesh(const Camera& cam, SDL_Renderer* renderer);

    SDL_Texture *tileset = nullptr;
    int width{}, height{};
    std::vector<std::vector<int>> floorData;
    std::vector<std::vector<int>> wallData;
    std::vector<std::vector<int>> furnitureData;
    std::vector<int> AIWalkable;
    std::vector<Collider> colliders;
    std::vector<Collider> coins;
    SDL_Point Door;
    SDL_Point Register;

};
#endif //PROJECT_MAP_H