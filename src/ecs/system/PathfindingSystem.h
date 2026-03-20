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
    // Call this ONCE when you load your map to give the pathfinder the grid boundaries/collisions
    static void InitMap(int width, int height, const std::vector<int> &collisionLayer);

    static void PrintMap();
    // The main function your AI calls
    static std::vector<SDL_Point> FindPath(SDL_Point start, SDL_Point target);

private:
    static int mapWidth;
    static int mapHeight;
    static std::vector<int> grid; // 1D array representing 2D Tiled map (0 = walkable, 1 = wall)

    // Helper functions
    static bool IsValid(int x, int y);

    static int GetHeuristic(SDL_Point a, SDL_Point b);

    static std::vector<SDL_Point> RetracePath(const std::unordered_map<int, PathNode> &allNodes, SDL_Point start,
                                              SDL_Point target);
};
#endif //PROJECT_PATHFINDING_H
