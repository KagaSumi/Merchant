#include "DaySummaryUI.h"
#include "BaseUI.h"
#include "HudUI.h"
#include "../manager/AssetManager.h"

namespace {
    // Helper: Combines two BaseUI labels into a ledger row
    std::pair<Entity*, Entity*> makeLedgerRow(Scene& scene, Entity& parent, const std::string& title, int value, float xLeft, float xRight, float y, SDL_Color valColor) {
        auto& titleEnt = BaseUI::createLabel(scene, parent, title, Vector2D(xLeft, y), {0,0,0,255}, title, true);
        auto& valEnt = BaseUI::createLabel(scene, parent, std::to_string(value) + "g", Vector2D(xRight, y), valColor, title + "_val", false);
        return {&titleEnt, &valEnt};
    }

    void buildTitle(Scene& scene, Entity& overlay, float centerLineX, float baseY) {
        auto& titleEnt = BaseUI::createLabel(scene, overlay, "Today's Report", Vector2D(0, baseY + 30.0f), {0,0,0,255}, "reportTitle", true);
        float titleW = titleEnt.getComponent<Label>().dst.w;
        titleEnt.getComponent<Transform>().position.x = centerLineX - (titleW / 2.0f);
    }

    void buildLedgerRows(Scene& scene, Entity& overlay, DaySummarySession& session, const DaySummaryData& data, float baseX, float rightColX, float baseY) {
        float leftColX = baseX + 60.0f;
        float startY = baseY + 120.0f;
        float rowSpacing = 45.0f;

        SDL_Color cPos = {76, 175, 80, 255}, cNeg = {211, 47, 47, 255}, cNeu = {0, 0, 0, 255};

        session.grossSalesValRef = makeLedgerRow(scene, overlay, "Gross Sales", data.grossSales, leftColX, rightColX, startY, data.grossSales > 0 ? cPos : cNeu).second;
        session.customerPurchasesValRef = makeLedgerRow(scene, overlay, "Stock Orders", data.orderExpenses, leftColX, rightColX, startY + rowSpacing, data.orderExpenses < 0 ? cNeg : cNeu).second;

        auto wpRow = makeLedgerRow(scene, overlay, "Weekly Payment", data.weeklyPaymentAmount, leftColX, rightColX, startY + rowSpacing * 2, cNeg);
        session.weeklyPaymentLabelRef = wpRow.first;
        session.weeklyPaymentValRef = wpRow.second;

        float profitY = static_cast<float>(static_cast<int>(startY + (rowSpacing * 2.5f)));
        int profit = data.getGrossProfit();
        session.grossProfitValRef = makeLedgerRow(scene, overlay, "Gross Profit", profit, leftColX, rightColX, profitY, profit > 0 ? cPos : (profit < 0 ? cNeg : cNeu)).second;
    }

    void buildFooter(Scene& scene, Entity& overlay, DaySummarySession& session, const DaySummaryData& data, float baseX, float menuWidth, float footerY, float footerHeight, DayCycle& dayCycle) {
        // Divider
        auto &lineEnt = scene.world.createEntity();
        SDL_Texture *texUI = TextureManager::load("asset/ui/UI-Sub.png");
        lineEnt.addComponent<Transform>(Vector2D(baseX + 10.0f, footerY), 0.0f, 1.0f);
        lineEnt.addComponent<Sprite>(texUI, SDL_FRect{0, 0, 8, 8}, SDL_FRect{0, 0, menuWidth - 20.0f, 2.0f}, RenderLayer::UI, false);
        lineEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&lineEnt);

        // Labels
        BaseUI::createLabel(scene, overlay, "Weekly Payment", Vector2D(baseX + 30.0f, footerY + 15.0f), {0,0,0,255}, "debtTitleText", true);
        session.debtSubTextRef = &BaseUI::createLabel(scene, overlay, "", Vector2D(baseX + 30.0f, footerY + 50.0f), {0,0,0,255}, "debtSubText", false);
        session.balanceTextRef = &BaseUI::createLabel(scene, overlay, "", Vector2D(baseX + (menuWidth * 0.45f), footerY + 20.0f), {0,0,0,255}, "balText", false);
        session.totalDebtTextRef = &BaseUI::createLabel(scene, overlay, "", Vector2D(baseX + (menuWidth * 0.45f), footerY + 50.0f), {211,47,47,255}, "totalDebtText", false);

        // Button
        float btnW = 160.0f, btnH = 40.0f;
        Vector2D btnPos(baseX + menuWidth - btnW - 30.0f, footerY + (footerHeight / 2.0f) - (btnH / 2.0f) - 20.0f);
        BaseUI::createStandardButton(scene, overlay, btnPos, btnW, btnH, {0, 33, 64, 16}, [&scene, &dayCycle]() {
            scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("clickHard"));
            scene.world.getUIVisibilityManager().hide("summary");
            scene.world.getUIVisibilityManager().show("hud");
            HudUI::update(scene, scene.storeEntity->getComponent<Wallet>(), dayCycle, scene.UIHud);
            dayCycle.phaseSwapReady = true;
        });
    }

    void updateLedgerValue(Entity *ent, int value, SDL_Color color, float rightColX) {
        if (!ent) return;
        auto &label = ent->getComponent<Label>();
        label.text = std::to_string(value) + "g";
        label.color = color;
        label.dirty = true;
        TextureManager::updateLabel(label);
        ent->getComponent<Transform>().position.x = rightColX - label.dst.w;
    }

    void updateText(Entity* ent, const std::string& newText) {
        if (!ent) return;
        auto& lbl = ent->getComponent<Label>();
        lbl.text = newText;
        lbl.dirty = true;
        TextureManager::updateLabel(lbl);
    }
}

namespace DaySummaryUI {
    Entity &create(Scene &scene, int windowWidth, int windowHeight, DayCycle &dayCycle, Entity *&outUIDaySummary) {
        auto &mainOverlay = BaseUI::createBaseMenuOverlay(scene, windowWidth, windowHeight);
        mainOverlay.getComponent<Sprite>().visible = false;
        if (!mainOverlay.hasComponent<Children>()) mainOverlay.addComponent<Children>();

        auto &session = mainOverlay.addComponent<DaySummarySession>();
        auto &sprite = mainOverlay.getComponent<Sprite>();
        float baseX = mainOverlay.getComponent<Transform>().position.x;
        float baseY = mainOverlay.getComponent<Transform>().position.y;
        float footerHeight = 140.0f;
        float footerY = baseY + sprite.dst.h - footerHeight;

        buildTitle(scene, mainOverlay, baseX + (sprite.dst.w / 2.0f), baseY);
        buildLedgerRows(scene, mainOverlay, session, DaySummaryData(), baseX, baseX + sprite.dst.w - 120.0f, baseY);
        buildFooter(scene, mainOverlay, session, DaySummaryData(), baseX, sprite.dst.w, footerY, footerHeight, dayCycle);

        outUIDaySummary = &mainOverlay;
        scene.world.getUIVisibilityManager().registerPanel("summary", outUIDaySummary);
        return mainOverlay;
    }

    Entity &update(Scene &scene, const DaySummaryData &data, Entity *UIDaySummary) {
        if (!UIDaySummary) return *UIDaySummary;
        auto &session = UIDaySummary->getComponent<DaySummarySession>();
        session.currentData = data;

        // Visibility order fix (Show first, then hide specifics)
        scene.world.getUIVisibilityManager().show("summary");
        scene.world.getAudioEventQueue().push(std::make_unique<AudioEvent>("summary"));
        scene.world.getUIVisibilityManager().hide("hud");

        float rightColX = UIDaySummary->getComponent<Transform>().position.x + UIDaySummary->getComponent<Sprite>().dst.w - 120.0f;

        updateLedgerValue(session.grossSalesValRef, data.grossSales, data.grossSales > 0 ? SDL_Color{76,175,80,255} : SDL_Color{0,0,0,255}, rightColX);
        updateLedgerValue(session.customerPurchasesValRef, -data.orderExpenses, SDL_Color{211,47,47,255}, rightColX);

        int profit = data.getGrossProfit();
        updateLedgerValue(session.grossProfitValRef, profit, profit > 0 ? SDL_Color{76,175,80,255} : SDL_Color{211,47,47,255}, rightColX);

        bool isPaymentDay = (data.daysUntilPayment == 0);
        if (session.weeklyPaymentLabelRef) session.weeklyPaymentLabelRef->getComponent<Label>().visible = isPaymentDay;
        if (session.weeklyPaymentValRef) {
            session.weeklyPaymentValRef->getComponent<Label>().visible = isPaymentDay;
            if (isPaymentDay) updateLedgerValue(session.weeklyPaymentValRef, -data.weeklyPayment, SDL_Color{211,47,47,255}, rightColX);
        }

        updateText(session.debtSubTextRef, "In " + std::to_string(data.daysUntilPayment) + " days: " + std::to_string(data.weeklyPaymentAmount) + "g");
        updateText(session.balanceTextRef, "Current Balance: " + std::to_string(data.currentBalance) + "g");
        updateText(session.totalDebtTextRef, "Total Debt: " + std::to_string(data.totalDebt) + "g");

        return *UIDaySummary;
    }
}