//
// Created by Curry on 2026/04/04.
//
#include "MarketTrendSystem.h"
#include <random>
#include <ctime>
#include <iostream>
#include "../vendor/tinyxml2.h"

using namespace tinyxml2;

void MarketTrendSystem::load(const char* path) {
    XMLDocument doc;
    if (doc.LoadFile(path) != XML_SUCCESS) {
        std::cerr << "Failed to load market_trends.xml\n";
        return;
    }

    XMLElement* root = doc.FirstChildElement("marketTrends");
    if (!root) return;

    for (XMLElement* tEl = root->FirstChildElement("trend");
         tEl; tEl = tEl->NextSiblingElement("trend")) {

        MarketTrend trend;
        trend.id = tEl->IntAttribute("id");

        if (auto* n = tEl->FirstChildElement("name"))
            trend.name = n->GetText() ? n->GetText() : "";
        if (auto* b = tEl->FirstChildElement("blurb"))
            trend.blurb = b->GetText() ? b->GetText() : "";

        for (XMLElement* mEl = tEl->FirstChildElement("modifier");
             mEl; mEl = mEl->NextSiblingElement("modifier")) {

            TrendModifier mod;
            if (mEl->Attribute("type")) mod.itemType = mEl->Attribute("type");
            if (mEl->Attribute("tag"))  mod.tag = mEl->Attribute("tag");
            mod.multiplier = mEl->FloatAttribute("multiplier", 1.0f);
            trend.modifiers.push_back(mod);
        }

        trends.push_back(trend);
    }

    std::cout << "Loaded " << trends.size() << " market trends\n";

    // Start with a default quiet day
    if (!trends.empty()) activeTrend = trends[0];
}

void MarketTrendSystem::rollDailyTrend() {
    if (trends.empty()) return;

    static std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(0, trends.size() - 1);
    activeTrend = trends[dist(rng)];

    std::cout << "Today's trend: " << activeTrend.name << "\n";

    if (onTrendChanged) onTrendChanged(activeTrend);
}

float MarketTrendSystem::getModifier(const ItemDef& item) const {
    float result = 1.0f;

    for (const auto& mod : activeTrend.modifiers) {
        // Check type match (empty type = applies to all)
        if (!mod.itemType.empty() && mod.itemType != itemTypeToString(item.type))
            continue;

        // Check tag match (empty tag = applies to all of that type)
        if (!mod.tag.empty() && !tagMatches(item, mod))
            continue;

        // Apply — if multiple modifiers match, take the highest one
        if (mod.multiplier > result) result = mod.multiplier;
    }

    return result;
}

bool MarketTrendSystem::tagMatches(const ItemDef& item, const TrendModifier& mod) const {
    for (const auto& tag : item.tags) {
        if (tag == mod.tag) return true;
    }
    return false;
}

std::string MarketTrendSystem::itemTypeToString(ItemType type) const {
    switch (type) {
        case ItemType::Consumable: return "Consumable";
        case ItemType::Equipment:  return "Equipment";
        case ItemType::Material:   return "Material";
        case ItemType::Luxury:     return "Luxury";
        default:                   return "";
    }
}
