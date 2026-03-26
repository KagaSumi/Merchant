//
// Created by Curry on 2026-03-17.
//

#ifndef PROJECT_DAYCYCLESYSTEM_H
#define PROJECT_DAYCYCLESYSTEM_H
#include <memory>
#include <vector>

#include "Components.h"
#include "Entity.h"

struct rgba {
    int r, g, b, a;
};

class DayCycleSystem {
public:
    DayCycle cycle;
    SDL_Texture* mapTilesetTexture = nullptr;
    std::function<void()> onMorningStart;
    std::function<void()> onShopOpenStart;
    std::function<void()> onEveningStart;
    std::function<void()> onWeekPassed;

    void update(const std::vector<std::unique_ptr<Entity>> &entities);

    void openStore() {
        if (cycle.currentPhase != DayPhase::Morning) return;

        cycle.currentPhase = DayPhase::ShopOpen;
        if (onShopOpenStart) onShopOpenStart();
    }

    // Called automatically when all customers have finished
    void finishShop() {
        if (cycle.currentPhase != DayPhase::ShopOpen) return;

        cycle.currentPhase = DayPhase::Evening;
        if (onEveningStart) onEveningStart();
    }
    void finishEvening() {
        if (cycle.currentPhase != DayPhase::Evening) return;

        cycle.currentPhase = DayPhase::Morning;
        cycle.date++;
        cycle.weekDay = (cycle.weekDay + 1) % 7;

        if (onMorningStart) onMorningStart();

        // Trigger weekly events
        if (cycle.weekDay == 0 && onWeekPassed) onWeekPassed();
    }
    void customerDeparted() {
        customersServed++;
    }

    void applyTint(const std::vector<std::unique_ptr<Entity> > &entities, const rgba rgba) {
        for (auto &entity: entities) {
            if (entity->hasComponent<Sprite>()) {
                if (entity->getComponent<Sprite>().renderLayer == RenderLayer::World){
                    auto &Texture = entity->getComponent<Sprite>().Texture;
                    SDL_SetTextureColorMod(Texture,
                                           rgba.r,
                                           rgba.g,
                                           rgba.b);
                    if (entity->hasComponent<PlayerTag>() || entity->hasComponent<Customer>() || entity->hasComponent<DisplayStand>()) {
                        SDL_SetTextureAlphaMod(Texture, 255);
                    }else {
                        SDL_SetTextureAlphaMod(Texture, rgba.a);
                    }
                }
            }
        }
        if (mapTilesetTexture != nullptr) {
            SDL_SetTextureColorMod(mapTilesetTexture, rgba.r, rgba.g, rgba.b);
            SDL_SetTextureAlphaMod(mapTilesetTexture, 255);
        }
    }
    rgba lerpRGBA(const rgba& start, const rgba& end, float t) {
        // Clamp 't' between 0.0f and 1.0f just in case math gets weird
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        rgba result;
        // result = start + (end - start) * progress
        result.r = start.r + static_cast<int>((end.r - start.r) * t);
        result.g = start.g + static_cast<int>((end.g - start.g) * t);
        result.b = start.b + static_cast<int>((end.b - start.b) * t);
        result.a = start.a + static_cast<int>((end.a - start.a) * t);

        return result;
    }


private:
    rgba evening_target = {255,180,120,220};
    rgba morning_target = {255,255,255,255};
    int totalCustomersForDay = 0;
    int customersServed = 0;
};
#endif //PROJECT_DAYCYCLESYSTEM_H
