//
// Created by Curry on 2026-03-17.
//
#include "CustomerAISystem.h"
#include <cmath>
#include <iostream>
#include <random>

// Helper to convert pixel position to grid position (assuming 32x32 tiles)
SDL_Point GetGridPos(const Transform& t) {
    return { static_cast<int>(t.position.x / 32), static_cast<int>(t.position.y / 32) };
}

void CustomerAISystem::HandleHeadingToRegister(CustomerAI& ai, Transform& t, Velocity& v) {
    // If we haven't calculated a path yet, do it once.
    if (ai.path.empty()) {
        // Use the class member Register!
        SDL_Point startPos = GetGridPos(t);

        // Convert the target pixel position (367, 424) to grid coordinates:
        // x: 367 / 32 = 11
        // y: 424 / 32 = 13

        ai.path = PathfindingSystem::FindPath(startPos,Register);
        ai.pathIndex = 0;
    }

    // Follow the path
    CustomerAISystem::MoveAlongPath(ai, t, v);
}

void CustomerAISystem::HandleBrowsing(CustomerAI &ai, Transform &t, Velocity &v,float deltaTime) {
    // 1. ARE WE CURRENTLY WAITING?
    if (ai.isWaiting) {
        ai.stateTimer -= deltaTime; // Tick the timer down

        if (ai.stateTimer <= 0.0f) {
            ai.isWaiting = false; // 5 seconds are up! Let them move again.
        } else {
            return; // Still waiting. Exit the function so they stand completely still.
        }
    }

    // 2. DID WE JUST ARRIVE AT A SHELF?
    if (!ai.path.empty() && ai.pathIndex >= ai.path.size()) {
        ai.path.clear();       // Throw away the finished path
        v.direction.x = 0.0f;  // Stop their physical movement
        v.direction.y = 0.0f;

        // --- TRIGGER THE PAUSE ---
        ai.isWaiting = true;
        ai.stateTimer = 5.0f;  // Set the timer for 5 seconds
        return;                // Exit so they don't instantly find a new path
    }

    // 3. FIND A NEW PATH
    if (ai.path.empty() && !ai.isWaiting) {
        SDL_Point startPos = GetGridPos(t);
        SDL_Point randomTarget = PathfindingSystem::GetRandomBrowsePoint();

        if (startPos.x != randomTarget.x || startPos.y != randomTarget.y) {
            ai.path = PathfindingSystem::FindPath(startPos, randomTarget);
            ai.pathIndex = 0;
        }
    }

    // 4. WALK
    CustomerAISystem::MoveAlongPath(ai, t, v);

}

void CustomerAISystem::HandleLeavingStore(CustomerAI &ai, Transform &t, Velocity &v) {
    if (ai.path.empty()) {
        SDL_Point startPos = GetGridPos(t);
        ai.path = PathfindingSystem::FindPath(startPos,Door);
        ai.pathIndex = 0;
    }
    CustomerAISystem::MoveAlongPath(ai, t, v);
}

void CustomerAISystem::MoveAlongPath(CustomerAI& ai, Transform& t, Velocity& v) {
    if (ai.pathIndex >= ai.path.size()) {
        v.direction.x = 0.0f;
        v.direction.y = 0.0f;
        return;
    }

    // 1. Get the target grid tile and convert to dead-center PIXELS
    SDL_Point targetGrid = ai.path[ai.pathIndex];
    float targetX = (targetGrid.x * 32.0f) + 16.0f;
    float targetY = (targetGrid.y * 32.0f) + 16.0f;

    // 2. Math to find direction and distance
    float dirX = targetX - t.position.x;
    float dirY = targetY - t.position.y;
    float distance = std::sqrt(dirX * dirX + dirY * dirY);

    // 3. Move or Snap!
    // IMPORTANT: If your AI moves 2 pixels per frame, this number MUST be > 2.0f.
    // 3.0f is a safe sweet spot to prevent overshooting.
    if (distance > 3.0f) {
        v.direction.x = dirX / distance;
        v.direction.y = dirY / distance;
    } else {
        // --- THE DRIFT KILLER ---
        // Violently snap the AI to the exact center of the tile before turning.
        // This guarantees they never cut corners through your shelves.
        t.position.x = targetX;
        t.position.y = targetY;

        // Target the next tile in the path
        ai.pathIndex++;
    }
}