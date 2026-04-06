//
// Created by Curry on 2026/04/06.
//

#include "UIVisibilityManager.h"
#include "Components.h"
#include <iostream>

void UIVisibilityManager::registerPanel(const std::string& name, Entity* panel) {
    if (!panel) {
        std::cerr << "UIVisibilityManager: tried to register null panel '" << name << "'\n";
        return;
    }
    panels[name] = panel;
}

void UIVisibilityManager::show(const std::string& name) {
    auto it = panels.find(name);
    if (it == panels.end()) {
        std::cerr << "UIVisibilityManager: panel '" << name << "' not found\n";
        return;
    }
    setVisible(*it->second, true);
}

void UIVisibilityManager::hide(const std::string& name) {
    auto it = panels.find(name);
    if (it == panels.end()) {
        std::cerr << "UIVisibilityManager: panel '" << name << "' not found\n";
        return;
    }
    setVisible(*it->second, false);
}

void UIVisibilityManager::toggle(const std::string& name) {
    auto it = panels.find(name);
    if (it == panels.end()) {
        std::cerr << "UIVisibilityManager: panel '" << name << "' not found\n";
        return;
    }
    Entity& panel = *it->second;
    bool current = panel.hasComponent<Sprite>() &&
                   panel.getComponent<Sprite>().visible;
    setVisible(panel, !current);
}

bool UIVisibilityManager::isVisible(const std::string& name) const {
    auto it = panels.find(name);
    if (it == panels.end()) return false;
    if (!it->second->hasComponent<Sprite>()) return false;
    return it->second->getComponent<Sprite>().visible;
}

Entity* UIVisibilityManager::getPanel(const std::string& name) const {
    auto it = panels.find(name);
    if (it == panels.end()) return nullptr;
    return it->second;
}

void UIVisibilityManager::setVisible(Entity& overlay, bool visible) {
    setVisibleRecursive(overlay, visible);
}

void UIVisibilityManager::setVisibleRecursive(Entity& overlay, bool visible) {
    if (overlay.hasComponent<Sprite>())
        overlay.getComponent<Sprite>().visible = visible;
    if (overlay.hasComponent<Label>())
        overlay.getComponent<Label>().visible = visible;
    if (overlay.hasComponent<Collider>())
        overlay.getComponent<Collider>().enabled = visible;

    if (overlay.hasComponent<Children>()) {
        for (auto* child : overlay.getComponent<Children>().children) {
            if (child) setVisibleRecursive(*child, visible);
        }
    }
}