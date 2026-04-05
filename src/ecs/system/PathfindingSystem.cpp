#include "PathfindingSystem.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>

// --- Static Variable Definitions ---
int PathfindingSystem::mapWidth = 0;
int PathfindingSystem::mapHeight = 0;
int PathfindingSystem::tileSize = 32;
std::vector<int> PathfindingSystem::grid;
std::vector<SDL_Point> PathfindingSystem::walkableNodes;
std::vector<BrowsePoint> PathfindingSystem::browseNodes;
std::mt19937 PathfindingSystem::rng(static_cast<unsigned int>(std::time(nullptr)));

// --- Initialization ---
void PathfindingSystem::InitMap(int width, int height, int tSize, const std::vector<int>& collisionLayer) {
    mapWidth = width;
    mapHeight = height;
    tileSize = tSize;
    grid = collisionLayer;
    int expectedSize = width * height;
    if (grid.size() != expectedSize) {
        std::cerr << "\nCRITICAL WARNING: Map Size Mismatch!\n"
                  << "Expected " << expectedSize << " tiles, but the array only has "
                  << grid.size() << " tiles.\n"
                  << "Your CSV Map Parser is missing data!\n\n";
    }

    walkableNodes.clear();

    // Scan the map for walkable floor tiles (Value: 3)
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (IsValid(x, y)) {
                walkableNodes.push_back({x, y});
            }
        }
    }

}

// --- Helper Functions ---
int PathfindingSystem::GetTile(int x, int y) {
    // 1. Check if the X/Y coordinates are outside the map bounds
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) return 0;

    int index = y * mapWidth + x;

    // --- THE CRASH PREVENTER ---
    // 2. Check if the index is larger than the actual array size!
    if (index >= grid.size()) {
        return 0; // Treat missing data as a wall, rather than crashing
    }
    return grid[index];
}

bool PathfindingSystem::IsValid(int x, int y) {
    if (grid.empty()) return false;
    return GetTile(x, y) == 3  ; // Anything not 0 is considered walkable
}

int PathfindingSystem::GetHeuristic(SDL_Point a, SDL_Point b) {
    return (std::abs(a.x - b.x) + std::abs(a.y - b.y)) * 10;
}

int GetKey(int x, int y, int width) {
    return y * width + x;
}

// --- Random Point Generation ---
SDL_Point PathfindingSystem::GetRandomBrowsePoint() {
    if (browseNodes.empty()) return {5, 5};
    std::uniform_int_distribution<int> dist(0, browseNodes.size() - 1);
    return browseNodes[dist(rng)].gridPos;
}

// --- Core A* Pathfinder ---
std::vector<SDL_Point> PathfindingSystem::FindPath(SDL_Point start, SDL_Point target) {
    std::vector<SDL_Point> path;

    if (grid.empty() || mapWidth <= 0 || mapHeight <= 0) return path;
    // DEBUG: If pathfinding is failing, this will tell you exactly why.
    if (!IsValid(start.x, start.y)) {
        std::cout << "A* FAIL: Start (" << start.x << "," << start.y << ") is a wall! (Value: " << GetTile(start.x, start.y) << ")\n";
        return path;
    }
    if (!IsValid(target.x, target.y)) {
        std::cout << "A* FAIL: Target (" << target.x << "," << target.y << ") is a wall! (Value: " << GetTile(target.x, target.y) << ")\n";
        return path;
    }
    if (start.x == target.x && start.y == target.y) return path;

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openSet;
    std::unordered_map<int, PathNode> allNodes;
    std::vector<bool> closedSet(mapWidth * mapHeight, false);

    PathNode startNode{start, 0, GetHeuristic(start, target), {-1, -1}};
    openSet.push(startNode);
    allNodes[GetKey(start.x, start.y, mapWidth)] = startNode;

    // Strictly Up, Down, Left, Right (No diagonals possible)
    SDL_Point directions[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    while (!openSet.empty()) {
        PathNode current = openSet.top();
        openSet.pop();

        int currentKey = GetKey(current.pos.x, current.pos.y, mapWidth);
        if (closedSet[currentKey]) continue;
        closedSet[currentKey] = true;

        if (current.pos.x == target.x && current.pos.y == target.y) {
            return RetracePath(allNodes, start, target);
        }

        for (const auto& dir : directions) {
            SDL_Point neighborPos = {current.pos.x + dir.x, current.pos.y + dir.y};

            if (!IsValid(neighborPos.x, neighborPos.y)) continue;

            int neighborKey = GetKey(neighborPos.x, neighborPos.y, mapWidth);
            if (closedSet[neighborKey]) continue;

            int newGCost = current.gCost + 10;

            if (allNodes.find(neighborKey) == allNodes.end() || newGCost < allNodes[neighborKey].gCost) {
                PathNode neighborNode{neighborPos, newGCost, GetHeuristic(neighborPos, target), current.pos};
                allNodes[neighborKey] = neighborNode;
                openSet.push(neighborNode);
            }
        }
    }
    return path;
}

void PathfindingSystem::AddBrowsePoint(SDL_Point tilePos, Entity* standEntity) {
    SDL_Point point = {tilePos.x, tilePos.y + 1};
    browseNodes.push_back({point, standEntity});
}

const std::vector<BrowsePoint>& PathfindingSystem::GetBrowsePoints() {
    return browseNodes;
}

void PathfindingSystem::ClearBrowsePoints() {
    browseNodes.clear();
}

std::vector<SDL_Point> PathfindingSystem::RetracePath(const std::unordered_map<int, PathNode>& allNodes, SDL_Point start, SDL_Point target) {
    std::vector<SDL_Point> path;
    SDL_Point current = target;
    int safetyCounter = 0;
    int maxCells = mapWidth * mapHeight;

    while ((current.x != start.x || current.y != start.y) && safetyCounter < maxCells) {
        path.push_back(current);
        int key = GetKey(current.x, current.y, mapWidth);
        auto it = allNodes.find(key);
        if (it == allNodes.end()) break;
        current = it->second.parent;
        safetyCounter++;
    }
    std::reverse(path.begin(), path.end());
    return path;
}