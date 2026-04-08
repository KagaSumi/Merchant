//
// Created by Curry on 2026-01-14.
//
#include "Map.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "TextureManager.h"
#include <sstream>
#include <tinyxml2.h>

#include "CustomerAISystem.h"

void Map::load(const char *path, SDL_Texture *ts) {
    tileset = ts;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path);

    //Parse width and height of map
    auto *mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");

    //parse terrain data
    for (auto *layer = mapNode->FirstChildElement("layer");
         layer != nullptr;
         layer = layer->NextSiblingElement("layer")
    ) {
        auto *data = layer->FirstChildElement("data");
        std::string layerName = layer->Attribute("name");
        std::string csv = data->GetText();
        std::stringstream ss(csv);
        if (layerName == "Texture") {
            floorData = std::vector(height, std::vector<int>(width));
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    std::string val;
                    if (!std::getline(ss, val, ',')) { break; }
                    floorData[i][j] = std::stoi(val);
                }
            }
        }
        if (layerName == "Walls") {
            wallData = std::vector(height, std::vector<int>(width));
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    std::string val;
                    if (!std::getline(ss, val, ',')) { break; }
                    wallData[i][j] = std::stoi(val);
                }
            }
        }

        if (layerName == "Furniture") {
            furnitureData = std::vector(height, std::vector<int>(width));
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    std::string val;
                    if (!std::getline(ss, val, ',')) { break; }
                    // Removed the broken if(tileData[]) line
                    furnitureData[i][j] = std::stoi(val);
                }
            }
        }
        if (layerName == "AI-NavMesh") {
            AIWalkable.assign(width * height, 0);
            std::string csv = data->GetText();

            // 1. Replace all commas with spaces.
            // This turns "0,3,\n0" into "0 3 \n0"
            std::replace(csv.begin(), csv.end(), ',', ' ');

            std::stringstream ss(csv);

            for (int i = 0; i < (width * height); i++) {
                int val;
                // 2. The >> operator natively skips spaces and newlines!
                // It safely reads the next integer, no string conversion needed.
                if (ss >> val) {
                    AIWalkable[i] = val;
                } else {
                    // Safety net: warns you if the map data is too short
                    std::cerr << "Map Parsing Error: Ran out of data at index " << i << "!" << std::endl;
                    break;
                }
            }
        }
    }


    //for each object group after ^ then use name to determine
    for (auto *objectGroup = mapNode->FirstChildElement("objectgroup");
         objectGroup != nullptr;
         objectGroup = objectGroup->NextSiblingElement("objectgroup")
    ) {
        std::string groupName = objectGroup->Attribute("name");
        if (groupName == "Collider") {
            //parse collider data
            //create a for loop with initialization, condition and an increment
            for (auto *obj = objectGroup->FirstChildElement("object");
                 obj != nullptr;
                 obj = obj->NextSiblingElement("object")) {
                Collider c;
                c.rect.x = obj->FloatAttribute("x");
                c.rect.y = obj->FloatAttribute("y");
                c.rect.h = obj->FloatAttribute("height");
                c.rect.w = obj->FloatAttribute("width");
                colliders.push_back(c);
            }
        }
        if (groupName =="Display Cases") {
            for (auto *obj = objectGroup->FirstChildElement("object");
             obj != nullptr;
             obj = obj->NextSiblingElement("object")) {

                // 1. Get the "name" attribute as a string, then convert to int for our order (1-15)
                const char* nameAttr = obj->Attribute("name");
                if (nameAttr != nullptr) {
                    int order = std::stoi(nameAttr);

                    // 2. Extract X and Y.
                    // Since the XML provides raw pixel values (64, 160), we don't need to multiply or divide by 32!
                    float x = obj->FloatAttribute("x");
                    float y = obj->FloatAttribute("y");

                    // 3. Save it to the map
                    displayCaseSpawns[order] = Vector2D(x, y);
                }
             }
        }
        if (groupName =="Door") {
            auto *obj = objectGroup->FirstChildElement("object");
            Door = {
                static_cast<int>(obj->IntAttribute("x")/32),
                static_cast<int>(obj->IntAttribute("y")/32)
            };
        }
        if (groupName =="Register") {
            auto *obj = objectGroup->FirstChildElement("object");
            Register = {
                static_cast<int>(obj->IntAttribute("x")/32),
                static_cast<int>(obj->IntAttribute("y")/32)
            };
        }
    }
}

void Map::drawLayer(const std::vector<std::vector<int> > &layer, const Camera &cam, int firstGid) {
    SDL_FRect src{}, dest{};
    const int tileSize = 32;
    const int tilesetColumns = 32;

    src.w = src.h = tileSize;
    dest.w = dest.h = tileSize;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = layer[row][col];

            // 0 always means "Empty Space" in Tiled
            if (type == 0) continue;

            // SAFETY CHECK: If the tile ID is less than this tileset's starting ID,
            // it belongs to a different tileset! Skip it so we don't get negative numbers.
            if (type < firstGid) continue;

            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(row) * dest.h;

            dest.x = std::round(worldX - cam.view.x);
            dest.y = std::round(worldY - cam.view.y);

            // Subtract the actual GID offset provided by Tiled
            int tileIndex = type - firstGid;

            int sheetCol = tileIndex % tilesetColumns;
            int sheetRow = tileIndex / tilesetColumns;

            src.x = sheetCol * tileSize;
            src.y = sheetRow * tileSize;

            TextureManager::draw(tileset, &src, &dest);
        }
    }
}