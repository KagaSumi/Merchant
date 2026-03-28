//
// Created by Curry on 2026/03/27.
//

#ifndef PROJECT_ITEMS_H
#define PROJECT_ITEMS_H
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

struct ItemDef {
    int id = -1; //Empty
    std::string name = "Empty";
    int basePrice = 0;;
    std::vector<std::string> tags = {};
    SDL_FRect src{};
    float requiredReputation = 1.0f;
};

class Items {
public:
    Items() = default;
    ~Items() = default;

    void load(const char *path);

    std::unordered_map<int, ItemDef> items{};

private:
    int global_id = 1;
};


#endif //PROJECT_ITEMS_H
