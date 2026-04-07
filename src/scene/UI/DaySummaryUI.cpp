//
// Created by Curry on 2026-04-07.
//

#include "DaySummaryUI.h"
#include "BaseUI.h"
#include "HudUI.h"
#include "../manager/AssetManager.h"

namespace {
    void createDaySummaryContent(Scene &scene, Entity &overlay, const DaySummaryData &data) {
        auto &overlayTransform = overlay.getComponent<Transform>();
        auto &overlaySprite = overlay.getComponent<Sprite>();

        float baseX = overlayTransform.position.x;
        float baseY = overlayTransform.position.y;
        float centerLineX = baseX + (overlaySprite.dst.w / 2.0f);

        // --- 1. TITLE ---
        auto &titleLabel = scene.world.createEntity();
        Label tData = {
            "Today's Report", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "reportTitle"
        };
        tData.dirty = true;
        tData.visible = false;
        auto &tComp = titleLabel.addComponent<Label>(tData);
        TextureManager::updateLabel(tComp);

        titleLabel.addComponent<Transform>(Vector2D(centerLineX - (tComp.dst.w / 2.0f), baseY + 30.0f), 0.0f, 1.0f);
        titleLabel.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&titleLabel);

        // --- 2. LEDGER CONFIG ---
        float leftColX = baseX + 60.0f;
        float rightColX = baseX + overlaySprite.dst.w - 120.0f; // Anchor for right-aligned text
        float startY = baseY + 120.0f;
        float rowSpacing = 45.0f;

        SDL_Color colorPositive = {76, 175, 80, 255}; // Green
        SDL_Color colorNegative = {211, 47, 47, 255}; // Red
        SDL_Color colorNeutral = {0, 0, 0, 255}; // Black

        // --- HELPER LAMBDA FOR ROWS ---
        auto createRow = [&](const std::string &labelText, int value, float yPos,
                             SDL_Color valColor) -> std::pair<Entity *, Entity *> {
            // Left Label
            auto &textEnt = scene.world.createEntity();
            Label lData = {labelText, AssetManager::getFont("arial"), colorNeutral, LabelType::Static, labelText};
            lData.dirty = true;
            lData.visible = false;
            TextureManager::updateLabel(textEnt.addComponent<Label>(lData));
            textEnt.addComponent<Transform>(Vector2D(leftColX, yPos), 0.0f, 1.0f);
            textEnt.addComponent<Parent>(&overlay);
            overlay.getComponent<Children>().children.push_back(&textEnt);

            // Right Value
            auto &valEnt = scene.world.createEntity();
            std::string valString = std::to_string(value) + "g";
            // If it's negative, std::to_string handles the minus sign. For positive > 0, we can add a plus if desired.

            Label vData = {valString, AssetManager::getFont("arial"), valColor, LabelType::Static, labelText + "_val"};
            auto &vComp = valEnt.addComponent<Label>(vData);
            vComp.dirty = true;
            vComp.visible = false;
            TextureManager::updateLabel(vComp);

            // Right-align math: Anchor X minus the width of the generated text
            valEnt.addComponent<Transform>(Vector2D(rightColX - vComp.dst.w, yPos), 0.0f, 1.0f);
            valEnt.addComponent<Parent>(&overlay);
            overlay.getComponent<Children>().children.push_back(&valEnt);

            return {&textEnt, &valEnt};
        };
        auto &session = overlay.getComponent<DaySummarySession>();

        // --- 3. GENERATE ROWS ---
        session.grossSalesValRef = createRow("Gross Sales", data.grossSales, startY,
                                             data.grossSales > 0 ? colorPositive : colorNeutral).second;
        session.customerPurchasesValRef = createRow("Stock Orders", data.orderExpenses, startY + rowSpacing,
                                                    data.orderExpenses < 0 ? colorNegative : colorNeutral).second;
        auto wpRow = createRow("Weekly Payment", data.weeklyPaymentAmount, startY + rowSpacing * 2, colorNegative);
        session.weeklyPaymentLabelRef = wpRow.first;
        session.weeklyPaymentValRef = wpRow.second;

        int profit = data.getGrossProfit();
        SDL_Color profitColor = profit > 0 ? colorPositive : (profit < 0 ? colorNegative : colorNeutral);
        session.grossProfitValRef = createRow("Gross Profit", profit, startY + (rowSpacing * 2.5f), profitColor).second;
        // Extra gap for the total
    }

    void createDaySummaryFooter(Scene &scene, Entity &overlay, const DaySummaryData &data, DayCycle &dayCycle) {
    auto &session = overlay.getComponent<DaySummarySession>();
    auto &overlayTransform = overlay.getComponent<Transform>();
    auto &overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;

    float footerHeight = 140.0f;
    float footerY = baseY + overlaySprite.dst.h - footerHeight;

    // --- 1. DIVIDER LINE ---
    // Hack: Stretch a tiny UI texture to act as a 2-pixel tall separator line
    auto &lineEnt = scene.world.createEntity();
    SDL_Texture *texUI = TextureManager::load("../asset/ui/UI-Sub.png");
    SDL_FRect lineSrc{0, 0, 8, 8};
    SDL_FRect lineDst{baseX + 10.0f, footerY, overlaySprite.dst.w - 20.0f, 2.0f};

    lineEnt.addComponent<Transform>(Vector2D(lineDst.x, lineDst.y), 0.0f, 1.0f);
    lineEnt.addComponent<Sprite>(texUI, lineSrc, SDL_FRect{0, 0, lineDst.w, lineDst.h}, RenderLayer::UI, false);
    lineEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&lineEnt);

    // --- 2. WEEKLY PAYMENT TEXT ---
    // Title
    auto &debtTitleEnt = scene.world.createEntity();
    Label dtData = {
        "Weekly Payment", AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "debtTitleText"
    };
    dtData.dirty = true;
    dtData.visible = false;
    TextureManager::updateLabel(debtTitleEnt.addComponent<Label>(dtData));

    // Position the title near the top of the footer
    debtTitleEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, footerY + 15.0f), 0.0f, 1.0f);
    debtTitleEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&debtTitleEnt);

    // Subtitle (The actual days and amount)
    auto &debtSubEnt = scene.world.createEntity();
    session.debtSubTextRef = &debtSubEnt;
    std::string debtSubText = "In " + std::to_string(data.daysUntilPayment) + " days: " + std::to_string(
                                  data.weeklyPaymentAmount) + "g";
    Label dsData = {debtSubText, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "debtSubText"};
    dsData.dirty = true;
    dsData.visible = false;
    TextureManager::updateLabel(debtSubEnt.addComponent<Label>(dsData));

    // Position the subtitle directly below the title (Y offset + 35.0f)
    debtSubEnt.addComponent<Transform>(Vector2D(baseX + 30.0f, footerY + 50.0f), 0.0f, 1.0f);
    debtSubEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&debtSubEnt);

    // --- 3. CURRENT BALANCE TEXT ---
    auto &balEnt = scene.world.createEntity();
    session.balanceTextRef = &balEnt;
    std::string balText = "Current Balance: " + std::to_string(data.currentBalance) + "g";
    Label bData = {balText, AssetManager::getFont("arial"), {0, 0, 0, 255}, LabelType::Static, "balText"};
    bData.dirty = true;
    bData.visible = false;
    TextureManager::updateLabel(balEnt.addComponent<Label>(bData));

    balEnt.addComponent<Transform>(Vector2D(baseX + (overlaySprite.dst.w * 0.45f), footerY + 20.0f), 0.0f, 1.0f);
    balEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&balEnt);

    // --- 3.5. TOTAL DEBT TEXT (NEW) ---
    auto &totalDebtEnt = scene.world.createEntity();
    session.totalDebtTextRef = &totalDebtEnt;
    std::string tdText = "Total Debt: " + std::to_string(data.totalDebt) + "g";
    // Using your UI's red color {211, 47, 47, 255} so it looks like a debt
    Label tdData = {tdText, AssetManager::getFont("arial"), {211, 47, 47, 255}, LabelType::Static, "totalDebtText"};
    tdData.dirty = true;
    tdData.visible = false;
    TextureManager::updateLabel(totalDebtEnt.addComponent<Label>(tdData));

    // Place it exactly below the Current Balance text
    totalDebtEnt.addComponent<Transform>(Vector2D(baseX + (overlaySprite.dst.w * 0.45f), footerY + 50.0f), 0.0f, 1.0f);
    totalDebtEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&totalDebtEnt);

    // --- 4. CONFIRM BUTTON ---
    auto &btnEnt = scene.world.createEntity();
    float btnWidth = 160.0f;
    float btnHeight = 40.0f;
    float btnX = baseX + overlaySprite.dst.w - btnWidth - 30.0f;
    float btnY = footerY + (footerHeight / 2.0f) - (btnHeight / 2.0f) - 20.0f;

    auto &btnTransform = btnEnt.addComponent<Transform>(Vector2D(btnX, btnY), 0.0f, 1.0f);

    // Assuming you have a standard button sprite texture
    SDL_Texture *texBtn = TextureManager::load("../asset/ui/Buttons.png");
    SDL_FRect btnSrc{0, 33, 64, 16}; // Adjust to your actual button source rect
    SDL_FRect btnDst{btnX, btnY, btnWidth, btnHeight};

    btnEnt.addComponent<Sprite>(texBtn, btnSrc, btnDst, RenderLayer::UI, false);
    btnEnt.addComponent<Collider>("ui", btnDst).enabled = false;

    // Click Logic
    auto &clickable = btnEnt.addComponent<Clickable>();
    clickable.onPressed = [&btnTransform] { btnTransform.scale = 0.9f; };
    clickable.onCancel = [&btnTransform] { btnTransform.scale = 1.0f; };
    clickable.onReleased = [&btnTransform, &scene, &dayCycle]() {
        btnTransform.scale = 1.0f;
        scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));
        scene.world.getUIVisibilityManager().hide("summary");
        scene.world.getUIVisibilityManager().show("hud");
        HudUI::update(scene, scene.storeEntity->getComponent<Wallet>(), dayCycle, scene.UIHud);
        dayCycle.phaseSwapReady = true;
    };


    btnEnt.addComponent<Parent>(&overlay);
    overlay.getComponent<Children>().children.push_back(&btnEnt);
    }
}

namespace DaySummaryUI {
    Entity &create(Scene &scene, int windowWidth, int windowHeight, DayCycle &dayCycle, Entity *&outUIDaySummary) {
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;

        if (!mainOverlay.hasComponent<Children>()) {
            mainOverlay.addComponent<Children>();
        }

        // Create a local, zeroed-out data struct just for the initial layout calculations
        DaySummaryData defaultData;

        // Attach the session
        auto &session = mainOverlay.addComponent<DaySummarySession>();
        session.currentData = defaultData;

        // Pass the zeroed-out data to build the hidden layout
        createDaySummaryContent(scene, mainOverlay, defaultData);
        createDaySummaryFooter(scene, mainOverlay, defaultData, dayCycle);

        scene.UIDaySummary = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("summary", scene.UIDaySummary);
        return mainOverlay;
    }

    Entity &update(Scene &scene, const DaySummaryData &data, Entity *UIDaySummary) {
        if (!UIDaySummary) {
        std::cerr << "Error: UIDaySummary not initialized!" << std::endl;
        return *UIDaySummary;
    }

    auto &session = UIDaySummary->getComponent<DaySummarySession>();
    session.currentData = data;

    auto &overlayTransform = UIDaySummary->getComponent<Transform>();
    auto &overlaySprite = UIDaySummary->getComponent<Sprite>();

    // We need to recalculate the right anchor for our right-aligned text
    float rightColX = overlayTransform.position.x + overlaySprite.dst.w - 120.0f;

    SDL_Color colorPositive = {76, 175, 80, 255};
    SDL_Color colorNegative = {211, 47, 47, 255};
    SDL_Color colorNeutral = {0, 0, 0, 255};

    // --- HELPER LAMBDA FOR UPDATING RIGHT-ALIGNED LEDGER VALUES ---
    auto updateLedgerValue = [&](Entity *ent, int value, SDL_Color color) {
        if (!ent) return;
        auto &label = ent->getComponent<Label>();
        label.text = std::to_string(value) + "g";
        label.color = color;
        label.dirty = true;
        TextureManager::updateLabel(label); // This updates label.dst.w

        // Shift the X position based on the new text width to keep it right-aligned!
        auto &transform = ent->getComponent<Transform>();
        transform.position.x = rightColX - label.dst.w;
    };

    // 1. Update Ledger Rows
    updateLedgerValue(session.grossSalesValRef, data.grossSales, data.grossSales > 0 ? colorPositive : colorNeutral);
    updateLedgerValue(session.customerPurchasesValRef, -data.orderExpenses,
                      data.orderExpenses < 0 ? colorNegative : colorNeutral);

    int profit = data.getGrossProfit();
    updateLedgerValue(session.grossProfitValRef, profit,
                      profit > 0 ? colorPositive : (profit < 0 ? colorNegative : colorNeutral));

    bool isPaymentDay = (data.daysUntilPayment == 0);

    if (session.weeklyPaymentLabelRef && session.weeklyPaymentValRef) {
        // Toggle visibility for both the left and right side of the row
        session.weeklyPaymentLabelRef->getComponent<Label>().visible = isPaymentDay;
        session.weeklyPaymentValRef->getComponent<Label>().visible = isPaymentDay;

        // If it is visible, update the number and right-align it
        if (isPaymentDay) {
            updateLedgerValue(session.weeklyPaymentValRef, -data.weeklyPayment, colorNegative);
        }
    }

    // 2. Update Footer Subtext (Days Remaining & Amount)
    if (session.debtSubTextRef) {
        auto &subLabel = session.debtSubTextRef->getComponent<Label>();
        subLabel.text = "In " + std::to_string(data.daysUntilPayment) + " days: " + std::to_string(
                            data.weeklyPaymentAmount) + "g";
        subLabel.dirty = true;
        TextureManager::updateLabel(subLabel);
    }

    // 3. Update Balance Text
    if (session.balanceTextRef) {
        auto &balLabel = session.balanceTextRef->getComponent<Label>();
        balLabel.text = "Current Balance: " + std::to_string(data.currentBalance) + "g";
        balLabel.dirty = true;
        TextureManager::updateLabel(balLabel);
    }


    // 4. Open the menu (Show everything)
    scene.world.getUIVisibilityManager().show("summary");
    //Page flip noise
    scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("summary"));


    //Hide Hud
    scene.world.getUIVisibilityManager().hide("hud");

    //Hide Weekly Payment if none was made
    if (session.weeklyPaymentLabelRef && session.weeklyPaymentValRef) {
        bool isPaymentDay = (data.daysUntilPayment == 0);
        session.weeklyPaymentLabelRef->getComponent<Label>().visible = isPaymentDay;
        session.weeklyPaymentValRef->getComponent<Label>().visible = isPaymentDay;
    }


    return *UIDaySummary;
    }
};
