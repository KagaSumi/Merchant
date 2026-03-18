//
// Created by Curry on 2026-03-17.
//
#include <algorithm>
#include <queue>

#include "PathfindingSystem.h"

// Static variable definitions
int PathfindingSystem::mapWidth = 0;
int PathfindingSystem::mapHeight = 0;
std::vector<int> PathfindingSystem::grid;

void PathfindingSystem::InitMap(int width, int height, const std::vector<int>& collisionLayer) {
    mapWidth = width;
    mapHeight = height;
    grid = collisionLayer;
}

bool PathfindingSystem::IsValid(int x, int y) {
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) return false; // Out of bounds
    int index = y * mapWidth + x;
    return grid[index] == 0; // Assuming 0 is walkable in your Tiled map
}

// Manhattan Distance (perfect for 4-way grid movement)
int PathfindingSystem::GetHeuristic(SDL_Point a, SDL_Point b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

// Helper to generate a unique 1D key for a 2D coordinate
int GetKey(int x, int y, int width) {
    return y * width + x;
}

std::vector<SDL_Point> PathfindingSystem::FindPath(SDL_Point start, SDL_Point target) {
    std::vector<SDL_Point> path;

    // If start or target is solid wall, bail immediately
    if (!IsValid(start.x, start.y) || !IsValid(target.x, target.y)) return path;
    if (start.x == target.x && start.y == target.y) return path;

    // Open list (tiles to evaluate)
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openSet;

    // Tracks all node data and whether they've been closed
    std::unordered_map<int, PathNode> allNodes;
    std::vector<bool> closedSet(mapWidth * mapHeight, false);

    PathNode startNode{start, 0, GetHeuristic(start, target), {-1, -1}};
    openSet.push(startNode);
    allNodes[GetKey(start.x, start.y, mapWidth)] = startNode;

    // 4-way movement directions (Right, Left, Down, Up)
    SDL_Point directions[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    while (!openSet.empty()) {
        PathNode current = openSet.top();
        openSet.pop();
        int currentKey = GetKey(current.pos.x, current.pos.y, mapWidth);

        if (closedSet[currentKey]) continue; // Skip if already evaluated
        closedSet[currentKey] = true;

        // Path found!
        if (current.pos.x == target.x && current.pos.y == target.y) {
            return RetracePath(allNodes, start, target);
        }

        // Check neighbors
        for (const auto& dir : directions) {
            SDL_Point neighborPos = {current.pos.x + dir.x, current.pos.y + dir.y};

            if (!IsValid(neighborPos.x, neighborPos.y)) continue;

            int neighborKey = GetKey(neighborPos.x, neighborPos.y, mapWidth);
            if (closedSet[neighborKey]) continue;

            int newGCost = current.gCost + 10; // Cost of moving 1 tile

            // If neighbor isn't in allNodes, or we found a faster way to it
            if (allNodes.find(neighborKey) == allNodes.end() || newGCost < allNodes[neighborKey].gCost) {
                PathNode neighborNode;
                neighborNode.pos = neighborPos;
                neighborNode.gCost = newGCost;
                neighborNode.hCost = GetHeuristic(neighborPos, target) * 10; // Scale heuristic to match G cost
                neighborNode.parent = current.pos;

                allNodes[neighborKey] = neighborNode;
                openSet.push(neighborNode);
            }
        }
    }

    return path; // Return empty path if no route exists
}

std::vector<SDL_Point> PathfindingSystem::RetracePath(const std::unordered_map<int, PathNode>& allNodes, SDL_Point start, SDL_Point target) {
    std::vector<SDL_Point> path;
    SDL_Point current = target;

    while (current.x != start.x || current.y != start.y) {
        path.push_back(current);
        int key = current.y * mapWidth + current.x;
        current = allNodes.at(key).parent;
    }

    // Reverse the path so it goes from Start -> Target
    std::reverse(path.begin(), path.end());
    return path;
}