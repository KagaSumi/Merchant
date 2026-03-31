//
// Created by Curry on 2026-01-14.
//

#ifndef PROJECT_MAP_H
#define PROJECT_MAP_H

#include <vector>
#include <Components.h>
#include <map>

class Map {
    public:
    Map() = default;
    ~Map() = default;

    void load(const char *path, SDL_Texture *ts);
    void drawLayer(const std::vector<std::vector<int>>& layer,const Camera& cam, int firstGid);

    SDL_Texture *tileset = nullptr;
    int width{}, height{};
    std::vector<std::vector<int>> floorData;
    std::vector<std::vector<int>> wallData;
    std::vector<std::vector<int>> furnitureData;
    std::map<int,Vector2D> displayCaseSpawns;
    std::vector<int> AIWalkable;
    std::vector<Collider> colliders;
    SDL_Point Door;
    SDL_Point Register;

};
#endif //PROJECT_MAP_H