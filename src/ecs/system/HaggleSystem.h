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
    void onDialogueConfirmed();
    void resumeQueue() {update();}
    void submitOffer(int offeredPrice);
    void dismissFeedback();

    // --- WIRED FROM Scene ---
    std::function<void(const std::string&)> onShowDialogue;
    std::function<void(const ItemDef&)> onShowHaggleUI;
    std::function<void(int, int)> onSaleComplete;
    std::function<float(const ItemDef&)> getPriceModifier;

    // CustomerDialogueSystem wires these
    std::function<std::string(float mood)> onGetOpeningLine;
    std::function<std::string(int patience)> onGetRejectionLine;
    std::function<std::string()> onGetWalkawayLine;
    std::function<std::string(int salePrice, int basePrice)> onGetSuccessLine;

    std::string feedbackMessage;
    Entity* activeCustomer = nullptr;
    std::function<void()> pendingConfirm;
    void pushDialogue(const std::string& msg, std::function<void()> onConfirm);

private:
    std::queue<Entity*> waitingCustomers;
    std::queue<std::pair<std::string, std::function<void()>>> dialogueQueue;
    bool dialogueBusy = false;

    void processDialogueQueue();


    void beginHaggle();
    void showFeedback(const std::string& msg);
    bool willAccept(const Customer& customer, const ItemDef& item, int offeredPrice);
    // Returns the effective max price the customer will pay,
    // factoring in mood AND any active market trend modifier
    int getMaxAcceptablePrice(const Customer& customer, const ItemDef& item);
};

#endif