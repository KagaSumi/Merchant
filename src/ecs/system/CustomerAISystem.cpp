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
    // We move the check point slightly INWARD from the edges (2 pixels)
    // This prevents "A* FAIL" when brushing against a wall.
    float centerX = t.position.x + 16.0f;

    // Instead of 31 (the very edge), use 28 or 30.
    // This ensures we are checking the floor the feet are ON,
    // not the tile the feet are touching at the bottom edge.
    float feetY = t.position.y + 28.0f;

    return {
        static_cast<int>(std::floor(centerX / 32.0f)),
        static_cast<int>(std::floor(feetY / 32.0f))
    };
}

void CustomerAISystem::HandleHeadingToRegister(Entity& entity, CustomerAI& ai,PathFinding& pf, Transform& t,Animation& anim, Velocity& v,HaggleSystem* haggleSystem) {
    // 1. ARE WE CURRENTLY IN THE MINI-GAME?
    if (ai.isWaiting) {
        // The HaggleSystem will set ai.isWaiting = false and change state when done
        return; // Lock the AI in place while the player plays the mini-game
    }

    // 2. GET PATH TO REGISTER
    if (pf.path.empty()) {
        SDL_Point startPos = GetGridPos(t);
        pf.path = PathfindingSystem::FindPath(startPos, Register);
        pf.pathIndex = 0;
    }

    // 3. WALK
    CustomerAISystem::MoveAlongPath(pf, t, v);

    // 4. DID WE JUST ARRIVE AT THE REGISTER?
    if (!pf.path.empty() && pf.pathIndex >= pf.path.size()) {
        pf.path.clear();
        v.direction = {0, 0};
        anim.direction = 2;
        ai.isWaiting = true;

        if (haggleSystem) {
            auto& customer = entity.getComponent<Customer>();

            // Build list of stands that have stock AND aren't fully reserved
            auto& browsePoints = PathfindingSystem::GetBrowsePoints();
            std::vector<const BrowsePoint*> available;

            for (auto& bp : browsePoints) {
                if (!bp.standEntity) continue;
                if (!bp.standEntity->hasComponent<DisplayStand>()) continue;

                auto& stand = bp.standEntity->getComponent<DisplayStand>();

                if (stand.quantity <= 0) continue;
                if (stand.item.name.empty()) continue;
                if (stand.reserved_quantity >= stand.quantity) continue;

                available.push_back(&bp);  // store the BrowsePoint, not just the stand
            }

            if (available.empty()) {
                ai.isWaiting = false;
                ai.currentState = CustomerAIState::LeavingStore;
                return;
            }

            // Pick random
            std::mt19937 rng(std::random_device{}());
            int idx = std::uniform_int_distribution<int>(0, (int)available.size() - 1)(rng);

            const BrowsePoint* chosen = available[idx];
            customer.displayStandEntity = chosen->standEntity;
            customer.displayStand = &chosen->standEntity->getComponent<DisplayStand>();
            customer.displayStand->reserved_quantity++;

            haggleSystem->enqueue(&entity);
        }
    }
}

void CustomerAISystem::HandleBrowsing(CustomerAI &ai, PathFinding& pf,Transform &t, Velocity &v,Animation& anim, float deltaTime){
    // 1. ARE WE CURRENTLY WAITING?
    if (ai.isWaiting) {
        ai.stateTimer -= deltaTime;
        if (ai.stateTimer <= 0.0f) {
            ai.isWaiting = false;
            ai.itemsBrowsed++; // <--- THEY FINISHED LOOKING AT AN ITEM!
        } else {
            return;
        }
    }

    // 2. DID WE JUST ARRIVE?
    if (!pf.path.empty() && pf.pathIndex >= pf.path.size()) {
        pf.path.clear();
        v.direction.x = 0.0f;
        v.direction.y = 0.0f;
        ai.isWaiting = true;
        ai.stateTimer = 3.0f;
        anim.direction = 1; //Face up while at shelf
        return;
    }

    // 3. FIND A NEW PATH
    if (pf.path.empty() && !ai.isWaiting) {

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
        pf.path = PathfindingSystem::FindPath(startPos, randomTarget);
        pf.pathIndex = 0;
    }

    // 4. WALK
    CustomerAISystem::MoveAlongPath(pf, t, v);
}

void CustomerAISystem::HandleLeavingStore(Entity& entity,CustomerAI &ai,PathFinding& pf, DayCycleSystem& dayCycleSystem, Transform &t, Velocity &v) {
    if (pf.path.empty()) {
        SDL_Point startPos = GetGridPos(t);
        pf.path = PathfindingSystem::FindPath(startPos, Door);
        pf.pathIndex = 0;
    }

    CustomerAISystem::MoveAlongPath(pf, t, v);

    if (!pf.path.empty() && pf.pathIndex >= pf.path.size()) {
        std::cout << "Customer left the store!" << std::endl;
        // DO NOT FORGET TO DESTROY THE ENTITY HERE!
        dayCycleSystem.customerDeparted();
        entity.destroy();
    }
}

void CustomerAISystem::MoveAlongPath(PathFinding& pf, Transform& t, Velocity& v) {
    if (pf.pathIndex >= pf.path.size()) {
        v.direction.x = 0.0f;
        v.direction.y = 0.0f;
        return;
    }

    // 1. Get the target grid tile and convert to dead-center PIXELS
    SDL_Point targetGrid = pf.path[pf.pathIndex];
    float targetX = (targetGrid.x * 32.0f) ; //Off set 16 pixels because scaling 32 sprite -> 64 pixel
    float targetY = (targetGrid.y * 32.0f) ; //Off set 16 pixels because scaling 32 sprite -> 64 pixel

    // 2. Math to find direction and distance
    float dirX = targetX - t.position.x;
    float dirY = targetY - t.position.y;
    float distance = std::sqrt(dirX * dirX + dirY * dirY);

    // 3. Move or Snap!
    // IMPORTANT: If your AI moves 2 pixels per frame, this number MUST be > 2.0f.
    // 3.0f is a safe sweet spot to prevent overshooting.
    if (distance > 4.0f) {
        v.direction.x = dirX / distance;
        v.direction.y = dirY / distance;
    } else {
        t.position.x = targetX;
        t.position.y = targetY;

        // Target the next tile in the path
        pf.pathIndex++;
    }
}