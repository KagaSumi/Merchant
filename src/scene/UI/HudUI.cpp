//
// Created by Curry on 2026-04-07.
//
#include "HudUI.h"
#include "../manager/AssetManager.h"

namespace HudUI {
    Entity& create(Scene& scene, int windowWidth, int windowHeight, Entity*& outUIHud) {
        auto& overlay = scene.world.createEntity();

        // Top-right bar — thin strip like your mockup
        float barW = 280.0f;
        float barH = 40.0f;
        float barX = windowWidth - barW - 10.0f;
        float barY = 10.0f;

        SDL_Texture* bgTex = TextureManager::load("../asset/ui/UI-Base.png");
        SDL_FRect src{0, 0, barW, barH};
        SDL_FRect dst{barX, barY, barW, barH};

        overlay.addComponent<Transform>(Vector2D(barX, barY), 0.0f, 1.0f);
        overlay.addComponent<Sprite>(bgTex, src, dst, RenderLayer::UI, true);
        overlay.addComponent<Children>();

        auto& session = overlay.addComponent<HUDSession>();

        // --- DAY LABEL (left side of bar) ---
        auto& dayEnt = scene.world.createEntity();
        Label dayData = {
            "Day 1", AssetManager::getFont("arial"),
            {0, 0, 0, 255}, LabelType::Static, "hudDay"
        };
        auto& dayComp = dayEnt.addComponent<Label>(dayData);
        dayComp.dirty = true;
        dayComp.visible = true;
        TextureManager::updateLabel(dayComp);

        dayEnt.addComponent<Transform>(
            Vector2D(barX + 15.0f, barY + (barH / 2) - (dayComp.dst.h / 2)),
            0.0f, 1.0f);
        dayEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&dayEnt);
        session.dayLabelRef = &dayEnt;

        // --- PHASE LABEL (right side of bar) ---
        auto& phaseEnt = scene.world.createEntity();
        Label phaseData = {
            "Morning", AssetManager::getFont("arial"),
            {0, 0, 0, 255}, LabelType::Static, "hudPhase"
        };
        auto& phaseComp = phaseEnt.addComponent<Label>(phaseData);
        phaseComp.dirty = true;
        phaseComp.visible = true;
        TextureManager::updateLabel(phaseComp);

        phaseEnt.addComponent<Transform>(
            Vector2D(barX + barW - phaseComp.dst.w - 15.0f,
                     barY + (barH / 2) - (phaseComp.dst.h / 2)),
            0.0f, 1.0f);
        phaseEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&phaseEnt);
        session.phaseLabelRef = &phaseEnt;

        // --- WALLET ICON (bottom right, coin sprite) ---
        float iconSize = 40.0f;
        float iconX = windowWidth - iconSize - 10.0f;
        float iconY = windowHeight - iconSize - 10.0f;

        auto& iconEnt = scene.world.createEntity();
        SDL_Texture* coinTex = TextureManager::load("../asset/ui/Gold.png");

        iconEnt.addComponent<Transform>(Vector2D(iconX, iconY), 0.0f, 1.0f);
        iconEnt.addComponent<Sprite>(
            coinTex,
            SDL_FRect{0, 0, 256, 256},
            SDL_FRect{0, 0, iconSize, iconSize},
            RenderLayer::UI, true);
        iconEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&iconEnt);
        session.walletIconRef = &iconEnt;

        // --- WALLET LABEL (left of coin icon) ---
        auto& walletEnt = scene.world.createEntity();
        Label walletData = {
            "1000G", AssetManager::getFont("arial"),
            {255, 215, 0, 255}, LabelType::Static, "hudWallet"
        };
        auto& walletComp = walletEnt.addComponent<Label>(walletData);
        walletComp.dirty = true;
        walletComp.visible = true;
        TextureManager::updateLabel(walletComp);

        walletEnt.addComponent<Transform>(
            Vector2D(iconX - walletComp.dst.w - 8.0f,
                     iconY + (iconSize / 2) - (walletComp.dst.h / 2)),
            0.0f, 1.0f);
        walletEnt.addComponent<Parent>(&overlay);
        overlay.getComponent<Children>().children.push_back(&walletEnt);
        session.walletLabelRef = &walletEnt;

        outUIHud = &overlay;
        scene.world.getUIVisibilityManager().registerPanel("hud", outUIHud);
        return overlay;
    }

    void update(Scene& scene, const Wallet& wallet, const DayCycle& dayCycle, Entity* UIHud) {
        if (!UIHud) return;
        auto& session = UIHud->getComponent<HUDSession>();

        // Update day label
        if (session.dayLabelRef) {
            auto& lbl = session.dayLabelRef->getComponent<Label>();
            lbl.text = "Day " + std::to_string(dayCycle.date);
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);
        }

        // Update phase label
        if (session.phaseLabelRef) {
            auto& lbl = session.phaseLabelRef->getComponent<Label>();
            switch (dayCycle.currentPhase) {
                case DayPhase::Morning:      lbl.text = "Morning"; break;
                case DayPhase::ShopOpen:     lbl.text = "Open";    break;
                case DayPhase::Evening:      lbl.text = "Evening"; break;
                case DayPhase::FadeToBlack:  lbl.text = "...";     break;
            }

            // Right-align phase label inside the bar
            auto& barSprite = UIHud->getComponent<Sprite>();
            auto& barTransform = UIHud->getComponent<Transform>();
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);

            auto& t = session.phaseLabelRef->getComponent<Transform>();
            t.position.x = barTransform.position.x + barSprite.dst.w - lbl.dst.w - 15.0f;
        }

        // Update wallet label and re-right-align it
        if (session.walletLabelRef && session.walletIconRef) {
            auto& lbl = session.walletLabelRef->getComponent<Label>();
            lbl.text = std::to_string(wallet.balance) + "G";
            lbl.dirty = true;
            TextureManager::updateLabel(lbl);

            // Use transform position, not sprite dst
            auto& iconT = session.walletIconRef->getComponent<Transform>();
            auto& lblT = session.walletLabelRef->getComponent<Transform>();

            lblT.position.x = iconT.position.x - lbl.dst.w - 8.0f;
            lblT.position.y = iconT.position.y + (40.0f / 2) - (lbl.dst.h / 2);
        }
    }
}