//
// Created by Curry on 2026-03-17.
//
#include "CustomerAISystem.h"
#include <cmath>

// Helper to convert pixel position to grid position (assuming 32x32 tiles)
SDL_Point GetGridPos(const Transform& t) {
    return { static_cast<int>(t.position.x / 32), static_cast<int>(t.position.y / 32) };
}

void CustomerAISystem::HandleHeadingToRegister(CustomerAI& ai, Transform& t, Velocity& v) {
    // If we haven't calculated a path yet, do it once.
    if (ai.path.empty()) {
        // Use the class member Register!
        SDL_Point startPos = GetGridPos(t);
        ai.path = PathfindingSystem::FindPath(startPos, Register);
        ai.pathIndex = 0;
    }

    // Follow the path
    MoveAlongPath(ai, t, v);
}

void CustomerAISystem::MoveAlongPath(CustomerAI& ai, Transform& t, Velocity& v) {
    // 1. Check if we have reached the end of the path
    if (ai.pathIndex >= ai.path.size()) {
        v.direction.x = 0.0f;
        v.direction.y = 0.0f; // Stop moving
        return;
    }

    // 2. Get the target tile and convert to world pixels
    // (+16 offsets to center the AI on the 32x32 tile)
    SDL_Point targetTile = ai.path[ai.pathIndex];
    float targetX = (targetTile.x * 32.0f) + 16.0f;
    float targetY = (targetTile.y * 32.0f) + 16.0f;

    // 3. Calculate Direction vector
    float dirX = targetX - t.position.x;
    float dirY = targetY - t.position.y;

    // 4. Calculate Distance
    float distance = std::sqrt(dirX * dirX + dirY * dirY);

    // 5. If we are far away, move towards it. If we are close, move to next node.
    if (distance > 2.0f) {
        // Normalize the direction vector so we don't move faster diagonally
        v.direction.x = dirX / distance;
        v.direction.y = dirY / distance;
    } else {
        // We reached this specific tile, target the next one in the list
        ai.pathIndex++;
    }
}