//
// Created by Curry on 2026-03-17.
//
#include "CustomerAISystem.h"
#include <cmath>
#include <iostream>
#include <random>

#include "DayCycleSystem.h"

// Helper to convert pixel position to grid position (assuming 32x32 tiles)
SDL_Point GetGridPos(const Transform& t) {
    return { static_cast<int>(t.position.x / 32), static_cast<int>(t.position.y / 32) };
}

void CustomerAISystem::HandleHeadingToRegister(CustomerAI& ai, Transform& t, Velocity& v) {
    // 1. ARE WE CURRENTLY IN THE MINI-GAME?
    if (ai.isWaiting) {

        // ==========================================
        // WAIT FOR MINI-GAME TO FINISH
        // ==========================================
        // Instead of a timer, check a boolean from your HaggleSystem.
        // e.g., if (HaggleSystem::IsComplete(ai.customerID)) {
        //     ai.isWaiting = false;
        //     ai.currentState = CustomerAIState::LeavingStore;
        // }
        // ==========================================

        ai.isWaiting = false;
        ai.currentState = CustomerAIState::LeavingStore;

        return; // Lock the AI in place while the player plays the mini-game
    }

    // 2. GET PATH TO REGISTER
    if (ai.path.empty()) {
        SDL_Point startPos = GetGridPos(t);
        ai.path = PathfindingSystem::FindPath(startPos, Register);
        ai.pathIndex = 0;
    }

    // 3. WALK
    CustomerAISystem::MoveAlongPath(ai, t, v);

    // 4. DID WE JUST ARRIVE AT THE REGISTER?
    if (!ai.path.empty() && ai.pathIndex >= ai.path.size()) {
        ai.path.clear();
        v.direction.x = 0.0f;
        v.direction.y = 0.0f;

        ai.isWaiting = true; // Freeze the customer

        // ==========================================
        // TRIGGER HAGGLE MINI-GAME HERE
        // ==========================================
        std::cout << "Customer reached register! Press 'E' to Haggle!" << std::endl;
        // e.g., HaggleSystem::QueueCustomer(entity);
        // ==========================================
    }
}

void CustomerAISystem::HandleBrowsing(CustomerAI &ai, Transform &t, Velocity &v,float deltaTime) {
    // 1. ARE WE CURRENTLY WAITING?
    if (ai.isWaiting) {
        ai.stateTimer -= deltaTime;
        if (ai.stateTimer <= 0.0f) {
            ai.isWaiting = false;
            ai.itemsBrowsed++; // <--- THEY FINISHED LOOKING AT AN ITEM!
            // std::cout << "Finished looking at item " << ai.itemsBrowsed
            //           << " out of " << ai.itemsToBrowse << std::endl;
        } else {
            return;
        }
    }

    // 2. DID WE JUST ARRIVE?
    if (!ai.path.empty() && ai.pathIndex >= ai.path.size()) {
        ai.path.clear();
        v.direction.x = 0.0f;
        v.direction.y = 0.0f;
        ai.isWaiting = true;
        ai.stateTimer = 5.0f;
        return;
    }

    // 3. FIND A NEW PATH
    if (ai.path.empty() && !ai.isWaiting) {

        // --- CHECK IF DONE SHOPPING ---
        if (ai.itemsBrowsed >= ai.itemsToBrowse) {
            ai.currentState = CustomerAIState::HeadingToRegister;
            return; // Exit this function. The system will run HandleHeadingToRegister on the next frame!
        }

        SDL_Point startPos = GetGridPos(t);
        SDL_Point randomTarget;

        // --- SAFE PATH SELECTION ---
        // Pick a random shelf. If it's the exact one we are standing on, pick again!
        // We limit it to 10 attempts just in case the map only has 1 shelf, preventing an infinite loop freeze.
        int attempts = 0;
        do {
            randomTarget = PathfindingSystem::GetRandomBrowsePoint();
            attempts++;
        } while (startPos.x == randomTarget.x && startPos.y == randomTarget.y && attempts < 10);

        // Generate the path to the new shelf
        ai.path = PathfindingSystem::FindPath(startPos, randomTarget);
        ai.pathIndex = 0;
    }

    // 4. WALK
    CustomerAISystem::MoveAlongPath(ai, t, v);
}

void CustomerAISystem::HandleLeavingStore(Entity& entity,CustomerAI &ai,DayCycleSystem& dayCycleSystem, Transform &t, Velocity &v) {
    if (ai.path.empty()) {
        SDL_Point startPos = GetGridPos(t);
        ai.path = PathfindingSystem::FindPath(startPos, Door);
        ai.pathIndex = 0;
    }

    CustomerAISystem::MoveAlongPath(ai, t, v);

    if (!ai.path.empty() && ai.pathIndex >= ai.path.size()) {
        std::cout << "Customer left the store!" << std::endl;
        // DO NOT FORGET TO DESTROY THE ENTITY HERE!
        dayCycleSystem.customerDeparted();
        entity.destroy();
    }
}

void CustomerAISystem::MoveAlongPath(CustomerAI& ai, Transform& t, Velocity& v) {
    if (ai.pathIndex >= ai.path.size()) {
        v.direction.x = 0.0f;
        v.direction.y = 0.0f;
        return;
    }

    // 1. Get the target grid tile and convert to dead-center PIXELS
    SDL_Point targetGrid = ai.path[ai.pathIndex];
    float targetX = (targetGrid.x * 32.0f) - 16.0f;
    float targetY = (targetGrid.y * 32.0f) - 16.0f;

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