//
// Created by Curry on 2026/04/05.
//
#include "CustomerDialogueSystem.h"
#include "../vendor/tinyxml2.h"
#include <iostream>

using namespace tinyxml2;

void CustomerDialogueSystem::load(const char* path) {
    XMLDocument doc;
    if (doc.LoadFile(path) != XML_SUCCESS) {
        std::cerr << "Failed to load customer_dialogue.xml\n";
        return;
    }

    XMLElement* root = doc.FirstChildElement("customerDialogue");
    if (!root) return;

    // Helper to load all <line> children into a vector
    auto loadLines = [](XMLElement* parent, std::vector<std::string>& out) {
        if (!parent) return;
        for (auto* el = parent->FirstChildElement("line");
             el; el = el->NextSiblingElement("line")) {
            if (el->GetText()) out.push_back(el->GetText());
        }
    };

    // Opening
    for (auto* el = root->FirstChildElement("opening");
         el; el = el->NextSiblingElement("opening")) {
        std::string mood = el->Attribute("mood") ? el->Attribute("mood") : "";
        if      (mood == "high")   loadLines(el, openingHigh);
        else if (mood == "medium") loadLines(el, openingMedium);
        else if (mood == "low")    loadLines(el, openingLow);
    }

    // Rejection
    for (auto* el = root->FirstChildElement("rejection");
         el; el = el->NextSiblingElement("rejection")) {
        std::string patience = el->Attribute("patience") ? el->Attribute("patience") : "";
        if      (patience == "high")   loadLines(el, rejectionHigh);
        else if (patience == "medium") loadLines(el, rejectionMedium);
        else if (patience == "low")    loadLines(el, rejectionLow);
    }

    // Walkaway
    loadLines(root->FirstChildElement("walkaway"), walkaway);

    // Success
    for (auto* el = root->FirstChildElement("success");
         el; el = el->NextSiblingElement("success")) {
        std::string value = el->Attribute("value") ? el->Attribute("value") : "";
        if      (value == "great") loadLines(el, successGreat);
        else if (value == "fair")  loadLines(el, successFair);
        else if (value == "steep") loadLines(el, successSteep);
    }

    std::cout << "Loaded customer dialogue\n";
}

std::string CustomerDialogueSystem::getOpeningLine(float mood) const {
    if (mood >= 0.7f) return pickRandom(openingHigh);
    if (mood >= 0.4f) return pickRandom(openingMedium);
    return pickRandom(openingLow);
}

std::string CustomerDialogueSystem::getRejectionLine(int patienceRemaining) const {
    if (patienceRemaining >= 3) return pickRandom(rejectionHigh);
    if (patienceRemaining == 2) return pickRandom(rejectionMedium);
    return pickRandom(rejectionLow);
}

std::string CustomerDialogueSystem::getWalkawayLine() const {
    return pickRandom(walkaway);
}

std::string CustomerDialogueSystem::getSuccessLine(int offeredPrice, int basePrice) const {
    float markup = static_cast<float>(offeredPrice - basePrice) / basePrice;
    if (markup <= 0.05f) return pickRandom(successGreat); // at or near base price
    if (markup <= 0.30f) return pickRandom(successFair);  // reasonable markup
    return pickRandom(successSteep);                       // gouging
}