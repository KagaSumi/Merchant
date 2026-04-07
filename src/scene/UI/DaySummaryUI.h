//
// Created by Curry on 2026-04-07.
//

#ifndef PROJECT_DAYSUMMARYUI_H
#define PROJECT_DAYSUMMARYUI_H
#pragma once
#include "../World.h"
#include "../scene/Scene.h"

struct DaySummaryData {
    int grossSales = 0;
    int orderExpenses = 0;
    int currentBalance = 0;
    int weeklyPayment = 0;
    int weeklyPaymentAmount = 0;
    int daysUntilPayment = 0;
    int totalDebt = 0;
    bool isBankrupt = false;
    bool isGameWon = false;

    int getGrossProfit() const {
        return grossSales - orderExpenses - weeklyPayment;
    }
};

struct DaySummarySession {
    DaySummaryData currentData;

    Entity* grossSalesValRef = nullptr;
    Entity* customerPurchasesValRef = nullptr;
    Entity* grossProfitValRef = nullptr;
    Entity* weeklyPaymentLabelRef = nullptr;
    Entity* weeklyPaymentValRef = nullptr;
    Entity* debtSubTextRef = nullptr;
    Entity* balanceTextRef = nullptr;
    Entity* totalDebtTextRef = nullptr;
};

namespace DaySummaryUI {
    Entity& create(Scene& scene, int windowWidth, int windowHeight, DayCycle& dayCycle, Entity*& outUIDaySummary);
    Entity& update(Scene& scene, const DaySummaryData& data, Entity* UIDaySummary);
}





#endif //PROJECT_DAYSUMMARYUI_H
