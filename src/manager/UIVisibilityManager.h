//
// Created by Curry on 2026/04/06.
//

#ifndef PROJECT_UIVISIBILITYMANAGER_H
#define PROJECT_UIVISIBILITYMANAGER_H

#include <string>
#include <unordered_map>
#include "Entity.h"

class UIVisibilityManager {
public:
    void registerPanel(const std::string& name, Entity* panel);

    void show(const std::string& name);
    void hide(const std::string& name);
    void toggle(const std::string& name);
    bool isVisible(const std::string& name) const;
    Entity* getPanel(const std::string& name) const;

private:
    std::unordered_map<std::string, Entity*> panels;
    void setVisible(Entity& overlay, bool visible);
    void setVisibleRecursive(Entity& overlay, bool visible);
};


#endif //PROJECT_UIVISIBILITYMANAGER_H
