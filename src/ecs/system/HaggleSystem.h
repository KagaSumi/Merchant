//
// Created by Curry on 2026/04/04.
//
#ifndef PROJECT_HAGGLESYSTEM_H
#define PROJECT_HAGGLESYSTEM_H
#include <queue>
#include <string>
#include <functional>

class Entity;
class Scene;
struct Customer;
struct ItemDef;

class HaggleSystem {
public:
    HaggleSystem() = default;

    void enqueue(Entity* customerEntity);
    void update();
    void submitOffer(int offeredPrice);
    void dismissFeedback();
    void pauseQueue() { showingFeedback = true; }
    void resumeQueue() {
        showingFeedback = false;
        update();
    }

    // --- WIRED FROM initGameplay ---
    std::function<void(int salePrice, int profitMargin)> onSaleComplete;
    std::function<void(const ItemDef&)> onBeginHaggle;   // opens haggle UI
    std::function<void(const std::string&)> onShowFeedback; // opens dialogue UI
    std::function<void(const ItemDef&)> onRetryHaggle;   // skips dialogue, re-opens haggle UI
    std::function<float(const ItemDef&)> getPriceModifier;  // market trend hook

    bool showingFeedback = false;
    std::string feedbackMessage;
    Entity* activeCustomer = nullptr;

private:
    std::queue<Entity*> waitingCustomers;


    void beginHaggle();
    void showFeedback(const std::string& msg);
    bool willAccept(const Customer& customer, const ItemDef& item, int offeredPrice);
    // Returns the effective max price the customer will pay,
    // factoring in mood AND any active market trend modifier
    int getMaxAcceptablePrice(const Customer& customer, const ItemDef& item);
};

#endif