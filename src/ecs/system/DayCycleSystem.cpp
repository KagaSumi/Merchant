//
// Created by Curry on 2026-03-17.
//

#include "DayCycleSystem.h"
void DayCycleSystem::update(const std::vector<std::unique_ptr<Entity>> &entities) {

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
            applyTint(entities, morning_target);
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
            // If the spawner is done, AND everyone who entered has now left...
            if (spawnerRef->isFinished && customersServed >= totalCustomersForDay) {

                // Reset our tracking variables for tomorrow
                totalCustomersForDay = 0;
                customersServed = 0;

                finishShop(); // Transition to evening
            }
            break;
        }

        case DayPhase::Evening:
            applyTint(entities, evening_target);
            if (cycle->phaseSwapReady == true) {
                cycle->phaseSwapReady = false;
                finishEvening();
            }

            break;

        case DayPhase::GameOver:
            break;
    }
}
