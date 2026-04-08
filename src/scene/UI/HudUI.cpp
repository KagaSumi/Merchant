//
// Created by Curry on 2026-04-07.
//
#include "HudUI.h"
#include "BaseUI.h"
#include "../manager/AssetManager.h"

namespace HudUI {
    Entity& create(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIHud) {
        auto& overlay = scene.world.createEntity();

        // Top-right bar dimensions
        float barW = 280.0f;
        float barH = 40.0f;
        float barX = windowWidth - barW - 10.0f;
        float barY = 10.0f;

        SDL_Texture* bgTex = TextureManager::load("../asset/ui/UI-Base.png");
        overlay.addComponent<Transform>(Vector2D(barX, barY), 0.0f, 1.0f);
        overlay.addComponent<Sprite>(bgTex, SDL_FRect{0, 0, barW, barH}, SDL_FRect{barX, barY, barW, barH}, RenderLayer::UI, true);
        overlay.addComponent<Children>();

        auto& session = overlay.addComponent<HUDSession>();

        // --- 1. DAY LABEL (Left aligned in bar) ---
        // We render on boot and set visible true immediately for the HUD
        session.dayLabelRef = &BaseUI::createLabel(scene, overlay, "Day 1", Vector2D(barX + 15.0f, 0), {0, 0, 0, 255}, "hudDay", true);
        session.dayLabelRef->getComponent<Label>().visible = true;

        // Center vertically in bar
        float dayH = session.dayLabelRef->getComponent<Label>().dst.h;
        session.dayLabelRef->getComponent<Transform>().position.y = barY + (barH / 2.0f) - (dayH / 2.0f);

        // --- 2. PHASE LABEL (Right aligned in bar) ---
        session.phaseLabelRef = &BaseUI::createLabel(scene, overlay, "Morning", Vector2D(0, 0), {0, 0, 0, 255}, "hudPhase", true);
        session.phaseLabelRef->getComponent<Label>().visible = true;

        auto& phComp = session.phaseLabelRef->getComponent<Label>();
        session.phaseLabelRef->getComponent<Transform>().position = Vector2D(
            barX + barW - phComp.dst.w - 15.0f,
            barY + (barH / 2.0f) - (phComp.dst.h / 2.0f)
        );

        // --- 3. WALLET ICON ---
        float iconSize = 40.0f;
        float iconX = windowWidth - iconSize - 10.0f;
        float iconY = windowHeight - iconSize - 10.0f;

        auto& iconEnt = scene.world.createEntity();
        SDL_Texture* coinTex = TextureManager::load("../asset/ui/Gold.png");
        iconEnt.addComponent<Transform>(Vector2D(iconX, iconY), 0.0f, 1.0f);
        iconEnt.addComponent<Sprite>(coinTex, SDL_FRect{0, 0, 256, 256}, SDL_FRect{0, 0, iconSize, iconSize}, RenderLayer::UI, true);
        iconEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&iconEnt);
        session.walletIconRef = &iconEnt;

        // --- 4. WALLET LABEL (Left of icon) ---
        session.walletLabelRef = &BaseUI::createLabel(scene, overlay, "1000G", Vector2D(0, 0), {255, 215, 0, 255}, "hudWallet", true);
        session.walletLabelRef->getComponent<Label>().visible = true;

        auto& wComp = session.walletLabelRef->getComponent<Label>();
        session.walletLabelRef->getComponent<Transform>().position = Vector2D(
            iconX - wComp.dst.w - 8.0f,
            iconY + (iconSize / 2.0f) - (wComp.dst.h / 2.0f)
        );

        outUIHud = &overlay;
        scene.world.getUIVisibilityManager().registerPanel("hud", outUIHud);
        return overlay;
    }

    void update(Scene& scene, const Wallet& wallet, const DayCycle& dayCycle, Entity* UIHud) {
        if (!UIHud) return;
        auto& session = UIHud->getComponent<HUDSession>();

        // Update day
        if (session.dayLabelRef) {
            auto& lbl = session.dayLabelRef->getComponent<Label>();
            lbl.text = "Day " + std::to_string(dayCycle.date);
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);
        }

        // Update phase and re-right-align
        if (session.phaseLabelRef) {
            auto& lbl = session.phaseLabelRef->getComponent<Label>();
            switch (dayCycle.currentPhase) {
                case DayPhase::Morning:      lbl.text = "Morning"; break;
                case DayPhase::ShopOpen:     lbl.text = "Open";    break;
                case DayPhase::Evening:      lbl.text = "Evening"; break;
                case DayPhase::FadeToBlack:  lbl.text = "...";     break;
            }
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);

            auto& barT = UIHud->getComponent<Transform>();
            auto& barS = UIHud->getComponent<Sprite>();
            session.phaseLabelRef->getComponent<Transform>().position.x = barT.position.x + barS.dst.w - lbl.dst.w - 15.0f;
        }

        // Update wallet and re-right-align
        if (session.walletLabelRef && session.walletIconRef) {
            auto& lbl = session.walletLabelRef->getComponent<Label>();
            lbl.text = std::to_string(wallet.balance) + "G";
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);

            auto& iconT = session.walletIconRef->getComponent<Transform>();
            auto& lblT = session.walletLabelRef->getComponent<Transform>();
            lblT.position.x = iconT.position.x - lbl.dst.w - 8.0f;
            lblT.position.y = iconT.position.y + (40.0f / 2.0f) - (lbl.dst.h / 2.0f);
        }
    }
}