//
// Created by Curry on 2026-03-17.
//

#include "DayCycleSystem.h"

void DayCycleSystem::update(const std::vector<std::unique_ptr<Entity> > &entities) {
    //Need to calculate how much each customer will "advance time"

    DayCycleSystem::applyTint(entities,morning_target);
}
