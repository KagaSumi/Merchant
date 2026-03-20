//
// Created by Curry on 2026-01-14.
//
#include "Map.h"

#include <cmath>
#include <iostream>

#include "TextureManager.h"
#include <sstream>
#include <tinyxml2.h>

void Map::load(const char *path, SDL_Texture *ts) {
    tileset = ts;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path);

    //Parse width and height of map
    auto *mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");

    //parse terraindata
    for (auto *layer = mapNode->FirstChildElement("layer");
         layer != nullptr;
         layer = layer->NextSiblingElement("layer")
    ) {
        auto *data = layer->FirstChildElement("data");
        std::string layerName = layer->Attribute("name");
        std::string csv = data->GetText();
        std::stringstream ss(csv);
        if (layerName == "Texture") {
            tileData = std::vector(height, std::vector<int>(width));
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    std::string val;
                    //read characters from a ss into val until hits comma or end of string
                    if (!std::getline(ss, val, ',')) { break; }
                    tileData[i][j] = std::stoi(val); //stoi is a string to integer converter
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

    // auto *layer = mapNode->FirstChildElement("layer");
    // auto *data = layer->FirstChildElement("data");
    // std::string csv = data->GetText();
    // std::stringstream ss(csv);
    // tileData = std::vector(height, std::vector<int>(width));
    // for (int i = 0; i < height; i++) {
    //     for (int j = 0; j < width; j++) {
    //         std::string val;
    //         //read characters from a ss into val until hits comma or end of string
    //         if (!std::getline(ss, val, ',')) { break; }
    //         tileData[i][j] = std::stoi(val); //stoi is a string to integer converter
    //     }
    // }

    //for each object group after ^ then use name to determine
    for (auto *objectGroup = mapNode->FirstChildElement("objectgroup");
         objectGroup != nullptr;
         objectGroup = objectGroup->NextSiblingElement("objectgroup")
    ) {
        std::string groupName = objectGroup->Attribute("name");
        if (groupName == "Collision") {
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

        if (groupName == "Item Layer")
            //parse coin data
            for (auto *obj = objectGroup->FirstChildElement("object");
                 obj != nullptr;
                 obj = obj->NextSiblingElement("object")) {
                Collider c;
                c.rect.x = obj->FloatAttribute("x");
                c.rect.y = obj->FloatAttribute("y");
                coins.push_back(c);
            }
    }
}

void Map::draw(const Camera &cam) {
    SDL_FRect src{}, dest{};

    dest.w = dest.h = 32;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = tileData[row][col];


            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(row) * dest.h;

            //Move map relative to the Camera
            //Convert from world space to screen space
            dest.x = std::round(worldX - cam.view.x);
            dest.y = std::round(worldY - cam.view.y);

            switch (type) {
                case 1:
                    //Floor
                    src.x = 0;
                    src.y = 0;
                    src.w = 32;
                    src.h = 32;
                    break;
                case 2:
                    //Wall
                    src.x = 32;
                    src.y = 0;
                    src.w = 32;
                    src.h = 32;
                    break;
                case 3:
                    //AI walk path
                    src.x = 64;
                    src.y = 0;
                    src.w = 32;
                    src.h = 32;
                    break;
                case 4:
                    //Collider
                    src.x = 0;
                    src.y = 32;
                    src.w = 32;
                    src.h = 32;
                    break;
                case 5:
                    //Register
                    src.x = 32;
                    src.y = 32;
                    src.w = 32;
                    src.h = 32;
                    break;
                case 6:
                    //Door
                    src.x = 64;
                    src.y = 32;
                    src.w = 32;
                    src.h = 32;
                    break;
                default:
                    break;
            }
            TextureManager::draw(tileset, src, dest);
        }
    }

    // void Map::draw(const Camera &cam) {
    //     SDL_FRect src{}, dest{};
    //
    //     dest.w = dest.h = 32;
    //
    //     for (int row = 0; row < height; row++) {
    //         for (int col = 0; col < width; col++) {
    //             int type = tileData[row][col];
    //
    //
    //             float worldX = static_cast<float>(col) * dest.w;
    //             float worldY = static_cast<float>(row) * dest.h;
    //
    //             //Move map relative to the Camera
    //             //Convert from world space to screen space
    //             dest.x = std::round(worldX - cam.view.x);
    //             dest.y = std::round(worldY - cam.view.y);
    //
    //             switch (type) {
    //                 case 1:
    //                     //Dirt
    //                     src.x = 0;
    //                     src.y = 0;
    //                     src.w = 32;
    //                     src.h = 32;
    //                     break;
    //                 case 2:
    //                     //Grass
    //                     src.x = 32;
    //                     src.y = 0;
    //                     src.w = 32;
    //                     src.h = 32;
    //                     break;
    //                 case 4:
    //                     //Water
    //                     src.x = 32;
    //                     src.y = 32;
    //                     src.w = 32;
    //                     src.h = 32;
    //                     break;
    //                 default:
    //                     break;
    //             }
    //             TextureManager::draw(tileset, src, dest);
    //         }
    //     }
}
