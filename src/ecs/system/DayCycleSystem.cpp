//
// Created by Curry on 2026-03-17.
//

#include "DayCycleSystem.h"
#include "scene/Scene.h"
void DayCycleSystem::update(const std::vector<std::unique_ptr<Entity>> &entities, float deltaTime) {

    // 1. Safely find the Spawner component
    Spawner* spawnerRef = nullptr;
    for (auto &e: entities) {
        if (e->hasComponent<Spawner>()) {
            spawnerRef = &e->getComponent<Spawner>();
        }
        if (e->hasComponent<DayCycle>()) {
            cycle = &e->getComponent<DayCycle>();

        }
    }

    switch (cycle->currentPhase) {
        case DayPhase::Morning:
            if (cycle->phaseSwapReady == true) {
                cycle->phaseSwapReady = false;
                openStore();
            }
            break;

        case DayPhase::ShopOpen: {
            if (!spawnerRef) break; // Safety bailout if spawner doesn't exist yet

            // 2. Lock in the daily total on the very first frame of the phase
            if (totalCustomersForDay == 0 && spawnerRef->spawnCount > 0) {
                totalCustomersForDay = spawnerRef->spawnCount;
                customersServed = 0; // Reset just to be safe
                activeCustomers = 0;
            }

            // 3. Calculate Lerp progress (0.0f to 1.0f)
            float progress = 0.0f;
            if (totalCustomersForDay > 0) {
                progress = static_cast<float>(customersServed) / totalCustomersForDay;
            }

            // 4. Apply the gradual tint
            rgba current_tint = lerpRGBA(morning_target, evening_target, progress);
            applyTint(entities, current_tint);

            // 5. Check if the day is completely over!
            // If everyone who entered has now left...
            if (spawnerRef->isFinished && activeCustomers == 0) {

                // Reset our tracking variables for tomorrow
                totalCustomersForDay = 0;
                customersServed = 0;
                activeCustomers = 0;

                finishShop(); // Transition to evening
            }
            break;
        }

        case DayPhase::Evening:
            applyTint(entities, evening_target);
            if (cycle->phaseSwapReady == true) {
                cycle->phaseSwapReady = false;
                cycle->currentPhase = DayPhase::FadeToBlack;
                fadeTimer = 0.0f;
                holdTimer = 0.0f;
                fadeInTimer = 0.0f; // Good practice to ensure these are reset
                fadedToBlack = false;
                fadingIn = false;

                if (onHudVisibilityChange) onHudVisibilityChange(false);
            }
            break;

        case DayPhase::FadeToBlack: {
            if (!fadedToBlack) {
                // Phase 1: evening tint → black
                fadeTimer += deltaTime;
                float t = fadeTimer / fadeDuration;
                if (t >= 1.0f) {
                    t = 1.0f;
                    fadedToBlack = true;
                    holdTimer = 0.0f;
                }
                rgba current = lerpRGBA(evening_target, black_target, t);
                applyTint(entities, current);

            } else if (!fadingIn) {
                // Phase 2: hold at black
                holdTimer += deltaTime;
                applyTint(entities, black_target);

                if (holdTimer >= holdDuration) {
                    fadingIn = true;
                    fadeInTimer = 0.0f;
                    finishEvening(); // date++, phase = Morning, onMorningStart fires (Updates HUD text)
                }

            } else {
                // Phase 3: black → morning tint (fade in)
                fadeInTimer += deltaTime;
                float t = fadeInTimer / fadeInDuration;
                if (t >= 1.0f) {
                    t = 1.0f;
                    // Fully back to morning, reset all fade state
                    cycle->currentPhase = DayPhase::Morning;
                    fadedToBlack = false;
                    fadingIn = false;
                    fadeTimer = 0.0f;
                    fadeInTimer = 0.0f;
                    holdTimer = 0.0f;

                    //Show the HUD exactly once when the fade-in is complete
                    if (onHudVisibilityChange) onHudVisibilityChange(true);
                }
                rgba current = lerpRGBA(black_target, morning_target, t);
                applyTint(entities, current);
            }
            break;
        }

        case DayPhase::GameOver:
            break;
    }
}