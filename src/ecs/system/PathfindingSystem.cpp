//
// Created by Curry on 2026-03-17.
//
#include "PathfindingSystem.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cmath>

// Static variable definitions
int PathfindingSystem::mapWidth = 0;
int PathfindingSystem::mapHeight = 0;
std::vector<int> PathfindingSystem::grid;

void PathfindingSystem::InitMap(int width, int height, const std::vector<int>& collisionLayer) {
    mapWidth = width;
    mapHeight = height;
    grid = collisionLayer;
    //PrintMap();
}

void PathfindingSystem::PrintMap() {
    if (grid.empty()) {
        std::cout << "Grid is empty! Nothing to print." << std::endl;
        return;
    }

    std::cout << "\n--- AI Navigation Mesh (Size: " << mapWidth << "x" << mapHeight << ") ---" << std::endl;

    for (int y = 0; y < mapHeight; y++) {
        // Print row number for easier debugging
        if (y < 10) std::cout << "0";
        std::cout << y << ": ";

        for (int x = 0; x < mapWidth; x++) {
            int value = grid[y * mapWidth + x];

            if (value == 3) {
                std::cout << ". "; // Walkable
            } else if (value == 0) {
                std::cout << "# "; // Wall/Empty
            } else {
                std::cout << "? "; // Unexpected value (like a newline char)
            }
        }
        std::cout << "\n";
    }
    std::cout << "--------------------------------------------------\n" << std::endl;
}

bool PathfindingSystem::IsValid(int x, int y) {
    // 1. Safety check: Is the grid even loaded?
    if (grid.empty()) return false;

    // 2. Boundary Check
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) return false;

    // 3. Collision Check (Assuming 0 is walkable, anything else is a wall)
    int index = y * mapWidth + x;
    return grid[index] != 0;
}

int PathfindingSystem::GetHeuristic(SDL_Point a, SDL_Point b) {
    // Manhattan distance scaled by 10 to match G-cost
    return (std::abs(a.x - b.x) + std::abs(a.y - b.y)) * 10;
}

int GetKey(int x, int y, int width) {
    return y * width + x;
}

std::vector<SDL_Point> PathfindingSystem::FindPath(SDL_Point start, SDL_Point target) {
    std::vector<SDL_Point> path;

    // CRASH PREVENTION: Check if InitMap was called
    if (grid.empty() || mapWidth <= 0 || mapHeight <= 0) {
        std::cerr << "Pathfinding Error: Grid not initialized!" << std::endl;
        return path;
    }

    // CRASH PREVENTION: Check if start/end are valid
    if (!IsValid(start.x, start.y)) {
        std::cerr << "Pathfinding Error: Start (" << start.x << "," << start.y << ") is a wall or OOB." << std::endl;
        return path;
    }
    if (!IsValid(target.x, target.y)) {
        std::cerr << "Pathfinding Error: Target (" << target.x << "," << target.y << ") is a wall or OOB." << std::endl;
        return path;
    }

    if (start.x == target.x && start.y == target.y) return path;

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openSet;
    std::unordered_map<int, PathNode> allNodes;
    std::vector<bool> closedSet(mapWidth * mapHeight, false);

    PathNode startNode{start, 0, GetHeuristic(start, target), {-1, -1}};
    openSet.push(startNode);
    allNodes[GetKey(start.x, start.y, mapWidth)] = startNode;

    SDL_Point directions[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    while (!openSet.empty()) {
        PathNode current = openSet.top();
        openSet.pop();

        int currentKey = GetKey(current.pos.x, current.pos.y, mapWidth);
        if (closedSet[currentKey]) continue;
        closedSet[currentKey] = true;

        // Found the goal
        if (current.pos.x == target.x && current.pos.y == target.y) {
            return RetracePath(allNodes, start, target);
        }

        for (const auto& dir : directions) {
            SDL_Point neighborPos = {current.pos.x + dir.x, current.pos.y + dir.y};

            if (!IsValid(neighborPos.x, neighborPos.y)) continue;

            int neighborKey = GetKey(neighborPos.x, neighborPos.y, mapWidth);
            if (closedSet[neighborKey]) continue;

            int newGCost = current.gCost + 10;

            // If node is new or we found a shorter path to it
            if (allNodes.find(neighborKey) == allNodes.end() || newGCost < allNodes[neighborKey].gCost) {
                PathNode neighborNode;
                neighborNode.pos = neighborPos;
                neighborNode.gCost = newGCost;
                neighborNode.hCost = GetHeuristic(neighborPos, target);
                neighborNode.parent = current.pos;

                allNodes[neighborKey] = neighborNode;
                openSet.push(neighborNode);
            }
        }
    }

    return path;
}

std::vector<SDL_Point> PathfindingSystem::RetracePath(const std::unordered_map<int, PathNode>& allNodes, SDL_Point start, SDL_Point target) {
    std::vector<SDL_Point> path;
    SDL_Point current = target;

    // Safety limit to prevent infinite loops if a parent link is broken
    int safetyCounter = 0;
    int maxCells = mapWidth * mapHeight;

    while ((current.x != start.x || current.y != start.y) && safetyCounter < maxCells) {
        path.push_back(current);
        int key = GetKey(current.x, current.y, mapWidth);

        auto it = allNodes.find(key);
        if (it == allNodes.end()) break; // Should not happen in valid A*

        current = it->second.parent;
        safetyCounter++;
    }

    std::reverse(path.begin(), path.end());
    return path;
}