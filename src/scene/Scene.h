//
// Created by Curry on 2026-02-25.
//

#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H
#include <SDL3/SDL_events.h>

#include "Vector2D.h"
#include "World.h"
#include  "SceneType.h"

class Scene {
    struct HaggleSession {
        ItemDef currentItem;
        std::vector<int> digits = {0, 0, 0, 0, 0};

        // Cached entity references for fast UI updates when switching items
        Entity* percentLabelRef = nullptr;
        Entity* itemNameRef = nullptr;
        Entity* itemIconRef = nullptr;
        Entity* itemBaseValRef = nullptr;
        Entity* digitRefs[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};

        int getProposedPrice() const {
            int finalPrice = 0;
            int multiplier = 10000;
            for (int i = 0; i < 5; ++i) {
                finalPrice += digits[i] * multiplier;
                multiplier /= 10;
            }
            return finalPrice;
        }

        int getPercentage() const {
            if (currentItem.basePrice <= 0) return 0;
            return static_cast<int>(((float)getProposedPrice() / currentItem.basePrice) * 100.0f);
        }
    };

    //Data Packages
    struct DaySummaryData {
        int grossSales = 0;
        int customerPurchases = 0;
        int currentBalance = 0;
        int weeklyPaymentAmount = 0;
        int daysUntilPayment = 0;

        int getGrossProfit() const {
            return grossSales - customerPurchases;
        }
    };

    struct DaySummarySession {
        DaySummaryData currentData;

        // Cached references to the dynamic text entities
        Entity* grossSalesValRef = nullptr;
        Entity* customerPurchasesValRef = nullptr;
        Entity* grossProfitValRef = nullptr;

        Entity* weeklyPaymentLabelRef = nullptr;
        Entity* weeklyPaymentValRef = nullptr;

        Entity* debtSubTextRef = nullptr;
        Entity* balanceTextRef = nullptr;
    };

    struct InventorySlotRefs {
        Entity* container = nullptr;
        Entity* icon = nullptr;
        Entity* label = nullptr;
    };

    struct QuantityScreenSession {
        InventoryEntry selectedItem;

        // Callbacks to tell the Scene what to do when the screen closes
        std::function<void(InventoryEntry, int)> onConfirm;
        std::function<void()> onCancel;

        Entity* itemIconRef = nullptr;
        Entity* itemNameRef = nullptr;
        std::vector<Entity*> quantityButtonRefs;
    };

    struct QuantitySession {
        InventoryEntry selectedItem;
        int maxQuantity = 0;

        Entity* itemIconRef = nullptr;
        Entity* itemNameRef = nullptr;
        Entity* quantityButtonContainer = nullptr; // parent of the number buttons
        std::vector<Entity*> quantityButtonRefs;   // the clickable number buttons

        std::function<void(InventoryEntry, int)> onConfirm; // fires when player picks qty
    };

    enum class InventoryUIState {
        Grid,
        QuantitySelect
    };

    enum class InventoryMode {
        Browse,      // opened via I key - view only
        PlaceItem    // opened via display case - clickable
    };
    struct InventorySession {
        std::vector<InventoryEntry> cachedInventory;
        std::vector<InventorySlotRefs> slots;
        InventoryMode mode = InventoryMode::Browse;

        InventoryUIState state = InventoryUIState::Grid;
        QuantitySession quantitySession;

        Entity* targetStockIconRef = nullptr;
        Entity* targetStockLabelRef = nullptr;
        DisplayStand* currentStand = nullptr;

        // grid root container ref so you can hide/show the whole grid at once
        Entity* gridContainerRef = nullptr;
        // quantity panel root ref
        Entity* quantityPanelRef = nullptr;
    };

    // Order UI
    struct OrderSlotRefs {
        Entity* icon = nullptr;
        Entity* nameLabel = nullptr;
        Entity* priceLabel = nullptr;
        Entity* buyBtn = nullptr;
        Entity* buyBtnCollider = nullptr; // for enable/disable
    };

    struct OrderSession {
        std::vector<OrderSlotRefs> slots;
        std::vector<ItemDef> currentItems;
        Entity* walletLabelRef = nullptr;
        int* walletRef = nullptr; // live pointer to game wallet
        Inventory* inventoryRef = nullptr;
        std::function<void()> onContinue;
    };


public:
    Scene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight);


    void update(const float dt, const SDL_Event &e) {
        world.update(dt,e,type);
    }

    void render() {
        world.render();
    }

    World world;

    const std::string& getName() const {return name;}

    Entity* UIInventory = nullptr;
    void openInventory(const std::vector<InventoryEntry>& items, InventoryMode mode) {
        updateInventoryUI(items,mode);
    }

private:
    std::string name;
    SceneType type;
    //void createProjectile(Vector2D pos, Vector2D dir, int speed);

    void initMainMenu(int windowWidth, int windowHeight);
    void initGameplay(const char* mapPath, int windowWidth, int windowHeight);

    //Helper Function
    Entity& createDisplaycase(Vector2D location, SDL_Texture* texture,SDL_FRect src, SDL_FRect dst, DayCycle& dayCycle,Entity* playerRef);

    //HUD
    Entity& createhudOverlay(int windowWidth, int windowHeight);

    //UI
    // ==================
    Entity& createSettingsOverlay(int windowWidth, int windowHeight);
    Entity& createCogButton(int windowWidth, int windowHeight, Entity& overlay);

    //Base Layer (Reuseable)
    Entity& createBaseMenuOverlay(int windowWidth, int windowHeight);
    Entity& createConfirmButton(int windowWidth, int windowHeight, Entity& overlay);

    //Haggling UI
    Entity* UIMenu = nullptr;
    Entity& updateHaggleUI(ItemDef &item);
    Entity& createHaggleUI(int windowWidth, int windowHeight);
    Entity& createItemHaggleDisplay(Entity& parent); // Item Overlay
    Entity& createPriceSelection(int windowWidth, int windowHeight, Entity& overlay); //Price Selection
    Entity& createHaggleButton(int windowWidth, int windowHeight, Entity& overlay); // Confirm Button

    //Inventory UI
    Entity& createInventoryUI(int windowWidth, int windowHeight);
    Entity& updateInventoryUI(const std::vector<InventoryEntry>& inventoryData, InventoryMode mode, DisplayStand* targetStand = nullptr);

    //Summary UI
    Entity* UIDaySummary = nullptr;
    Entity& createDaySummaryUI(int windowWidth, int windowHeight);
    Entity& updateDaySummaryUI(const DaySummaryData& data);
    void createDaySummaryContent(Entity& parent, const DaySummaryData& data);
    void createDaySummaryFooter(Entity& parent, const DaySummaryData& data);

    //Select UI (Display Case)
    Entity* UIQuantityScreen = nullptr;
    Entity& createQuantityScreen(int windowWidth, int windowHeight);
    void openQuantityScreen(const InventoryEntry& item, int maxQty,
                            std::function<void(InventoryEntry, int)> onConfirm,
                            std::function<void()> onCancel);

    //Order Screen
    Entity* UIOrderScreen = nullptr;
    Entity& createOrderUI(int windowWidth, int windowHeight);
    Entity& updateOrderUI(std::vector<ItemDef> availableItems, Wallet& wallet, Inventory& inv,
                          std::function<void()> onContinue);
    void refreshOrderSlot(int slotIdx, const ItemDef& item, int walletAmount);

    //Dialogue Screen
    Entity* UIDialogue = nullptr;
    Entity& createDialogueOverlay(int windowWidth, int windowHeight);

    void createSettingsUIComponents(Entity& overlay);

    void toggleSettingsOverlayVisibility(Entity& overlay, bool* forceState);

    Entity& createPlayerPosLabel();
};

#endif //PROJECT_SCENE_H