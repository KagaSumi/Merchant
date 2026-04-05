//
// Created by Curry on 2026/04/04.
//

#ifndef PROJECT_MARKETTRENDSYSTEM_H
#define PROJECT_MARKETTRENDSYSTEM_H

#include <string>
#include <vector>
#include <functional>
#include "Components.h" // for ItemDef, ItemType

struct TrendModifier {
    std::string itemType;   // "Consumable", "Equipment" etc — empty = all types
    std::string tag;        // specific tag filter — empty = all tags of that type
    float multiplier = 1.0f;
};

struct MarketTrend {
    int id = 0;
    std::string name;
    std::string blurb;
    std::vector<TrendModifier> modifiers;
};

class MarketTrendSystem {
public:
    MarketTrendSystem() = default;

    void load(const char* path);
    void rollDailyTrend();

    // Returns the price modifier for a given item based on active trend
    float getModifier(const ItemDef& item) const;

    const MarketTrend& getActiveTrend() const { return activeTrend; }

    // Fired when a new trend is rolled — wire to bulletin board update
    std::function<void(const MarketTrend&)> onTrendChanged;

private:
    std::vector<MarketTrend> trends;
    MarketTrend activeTrend;

    bool tagMatches(const ItemDef& item, const TrendModifier& mod) const;
    std::string itemTypeToString(ItemType type) const;
};

#endif