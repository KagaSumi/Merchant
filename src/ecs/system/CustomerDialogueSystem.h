//
// Created by Curry on 2026/04/05.
//

#ifndef PROJECT_CUSTOMERDIALOGUESYSTEM_H
#define PROJECT_CUSTOMERDIALOGUESYSTEM_H

#include <string>
#include <vector>
#include <random>
#include <ctime>

class CustomerDialogueSystem {
public:
    CustomerDialogueSystem() : rng(static_cast<unsigned>(std::time(nullptr))) {}

    void load(const char* path);

    // Call these from HaggleSystem at the right moments
    std::string getOpeningLine(float mood) const;
    std::string getRejectionLine(int patienceRemaining) const;
    std::string getWalkawayLine() const;
    std::string getSuccessLine(int offeredPrice, int basePrice) const;

private:
    std::vector<std::string> openingHigh;
    std::vector<std::string> openingMedium;
    std::vector<std::string> openingLow;

    std::vector<std::string> rejectionHigh;
    std::vector<std::string> rejectionMedium;
    std::vector<std::string> rejectionLow;

    std::vector<std::string> walkaway;

    std::vector<std::string> successGreat;
    std::vector<std::string> successFair;
    std::vector<std::string> successSteep;

    mutable std::mt19937 rng;

    std::string pickRandom(const std::vector<std::string>& lines) const {
        if (lines.empty()) return "...";
        std::uniform_int_distribution<int> dist(0, lines.size() - 1);
        return lines[dist(rng)];
    }
};

#endif