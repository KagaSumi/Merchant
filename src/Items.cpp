//
// Created by Curry on 2026/03/27.
//

#include "Items.h"

#include <map>
#include <tinyxml2.h>

#include "TextureManager.h"

void Items::load(const char *path) {
    std::map<std::string, ItemType> typeMap = {
        {"Equipment", ItemType::Equipment},
        {"Consumable", ItemType::Consumable},
        {"Material", ItemType::Material},
        {"Luxury", ItemType::Luxury}
    };
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path) != tinyxml2::XML_SUCCESS) return;

    auto *root = doc.FirstChildElement("items");
    if (!root) return;

    for (auto *itemElem = root->FirstChildElement("item");
         itemElem != nullptr;
         itemElem = itemElem->NextSiblingElement("item")) {
        ItemDef itemObj;
        itemObj.id = global_id;

        // 1. Parse Name (Element Text)
        if (auto *nameElem = itemElem->FirstChildElement("name")) {
            itemObj.name = nameElem->GetText();
        }

        // 2. Parse Price (Element Text)
        if (auto *priceElem = itemElem->FirstChildElement("basePrice")) {
            priceElem->QueryIntText(&itemObj.basePrice);
        }

        // 3. Parse Source Attributes (x, y, w, h)
        if (auto *srcElem = itemElem->FirstChildElement("source")) {
            srcElem->QueryFloatAttribute("x", &itemObj.src.x);
            srcElem->QueryFloatAttribute("y", &itemObj.src.y);
            srcElem->QueryFloatAttribute("w", &itemObj.src.w);
            srcElem->QueryFloatAttribute("h", &itemObj.src.h);
        }

        // 4. Parse Tags List
        if (auto *tagsRoot = itemElem->FirstChildElement("tags")) {
            for (auto *t = tagsRoot->FirstChildElement("tag"); t; t = t->NextSiblingElement("tag")) {
                if (t->GetText()) {
                    itemObj.tags.push_back(t->GetText());
                }
            }
        }

        // 5. Parse Required Reputation
        if (auto *requiredRepElem = itemElem->FirstChildElement("requiredRep")) {
            requiredRepElem->QueryFloatAttribute("rep", &itemObj.requiredReputation);
        }

        const char* typeStr = itemElem->Attribute("type");
        itemObj.type = typeMap[typeStr];

        items[global_id] = itemObj;
        global_id++;
    }
}
