//
// Created by Curry on 2026-03-17.
//

#ifndef PROJECT_PATHFINDING_H
#define PROJECT_PATHFINDING_H
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_rect.h>

struct PathNode {
    SDL_Point pos;
    int gCost = 0; // Cost from start
    int hCost = 0; // Heuristic (estimated cost to target)
    SDL_Point parent = {-1, -1}; // Grid coordinates of the parent tile

    int fCost() const { return gCost + hCost; }

    // This operator allows std::priority_queue to sort nodes by lowest fCost
    bool operator>(const PathNode &other) const {
        return fCost() > other.fCost() || (fCost() == other.fCost() && hCost > other.hCost);
    }
};

class PathfindingSystem {
public:
    static void InitMap(int width, int height, int tSize, const std::vector<int>& collisionLayer);
    static std::vector<SDL_Point> FindPath(SDL_Point start, SDL_Point target);

    static void GenerateBrowsePoints(int numberOfPoints);
    static SDL_Point GetRandomBrowsePoint();
    static int GetTile(int x, int y);
    static bool IsValid(int x, int y);

private:
    static int mapWidth;
    static int mapHeight;
    static int tileSize;
    static std::vector<int> grid;
    static std::vector<SDL_Point> walkableNodes;
    static std::vector<SDL_Point> browseNodes;

    static int GetHeuristic(SDL_Point a, SDL_Point b);
    static std::vector<SDL_Point> RetracePath(const std::unordered_map<int, PathNode>& allNodes, SDL_Point start, SDL_Point target);
};
#endif //PROJECT_PATHFINDING_H
