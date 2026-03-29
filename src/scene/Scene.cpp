//
// Created by Curry on 2026-02-25.
//

#include "../scene/Scene.h"
#include "../manager/AssetManager.h"
#include "Game.h"

Scene::Scene(SceneType sceneType, const char *sceneName, const char *mapPath, int windowWidth, int windowHeight): name(sceneName), type(sceneType){

    if (sceneType == SceneType::MainMenu) {
        initMainMenu(windowWidth, windowHeight);
        return;
    }
    initGameplay(mapPath, windowWidth, windowHeight);
}


void Scene::initMainMenu(int windowWidth, int windowHeight) {
    //camera
    auto& cam = world.createEntity();
    cam.addComponent<Camera>();
    //menu
    auto& menu(world.createEntity());
    auto menuTransform = menu.addComponent<Transform>(Vector2D(0,0),0.0f, 1.0f);

    SDL_Texture *texture = TextureManager::load("../asset/menu.png");
    SDL_FRect menuSrc {0,0,(float) windowWidth,(float) windowHeight};
    SDL_FRect menuDst {menuTransform.position.x,menuTransform.position.y,menuSrc.w, menuSrc.h};
    menu.addComponent<Sprite>(texture,menuSrc,menuDst);

    auto& settingsOverlay = createSettingsOverlay(windowWidth,windowHeight);
    createCogButton(windowWidth,windowHeight,settingsOverlay);
}

void Scene::initGameplay(const char* mapPath, int windowWidth, int windowHeight) {
    //load map
    SDL_Texture* tilemapTex =TextureManager::load("../asset/Sprite-0002.png");
    world.getMap().load(mapPath,tilemapTex);
    SDL_Texture* itemsTex = TextureManager::load("../asset/items.png");
    world.getItems().load("../asset/items.xml");

     for (auto& collider : world.getMap().colliders) {
         auto& e = world.createEntity();
         e.addComponent<Transform>(Vector2D(collider.rect.x,collider.rect.y),0.0f,1.0f);
         auto& c = e.addComponent<Collider>("wall");

         c.rect.x = collider.rect.x;
         c.rect.y = collider.rect.y;
         c.rect.w = collider.rect.w;
         c.rect.h = collider.rect.h;

         // SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
         // SDL_FRect colSrc {0,32,32,32};
         // SDL_FRect colDst {c.rect.x,c.rect.y,c.rect.w,c.rect.h};
         // e.addComponent<Sprite>(tex,colSrc,colDst);
     }

    //PathFinding:
    //Find Non-Walkable layer
    PathfindingSystem::InitMap(25,19,32, world.getMap().AIWalkable);


    //Create Camera:
    auto& cam = world.createEntity();
    SDL_FRect camView{};
    camView.w = windowWidth;
    camView.h = windowHeight;
    cam.addComponent<Camera>(camView, static_cast<float>(world.getMap().width * 32), static_cast<float>(world.getMap().height * 32));

    //Store:
    auto &store(world.createEntity());
    store.addComponent<ShopReputation>(Game::gameState.shopReputation);
    store.addComponent<Wallet>(Game::gameState.Wallet);
    store.addComponent<Debt>(Game::gameState.Debt);
    auto& dayCycle = store.addComponent<DayCycle>();

    //Create Player
    auto& player(world.createEntity());
    auto& playerTransform = player.addComponent<Transform>(Vector2D(12*32,14*32),1.0f);
    player.addComponent<Velocity>(Vector2D(0,0), 120.0f);

    Animation anim = AssetManager::getAnimation("player");
    player.addComponent<Animation>(anim);

    SDL_Texture* tex = TextureManager::load("../asset/animations/fox_anim.png");
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndicies[0];
    SDL_FRect playerDst {playerTransform.position.x,playerTransform.position.y,32,32};
    player.addComponent<Sprite>(tex,playerSrc,playerDst);

    auto& playerCollider = player.addComponent<Collider>("player");
    playerCollider.rect.w = playerDst.w;
    playerCollider.rect.h = playerDst.h;

    player.addComponent<PlayerTag>();


    //Customers:
    std::vector<SDL_Texture*> customerTextures = {
        TextureManager::load("../asset/animations/CustomerA.png"),
        TextureManager::load("../asset/animations/CustomerF.png"),
        TextureManager::load("../asset/animations/CustomerM.png")
    };
    int customerIndexCount = 0; // Index on which Texture ^

    auto& spawner(world.createEntity());
    Transform t = spawner.addComponent<Transform>(Vector2D(20*32,4*32),1.0f);
    spawner.addComponent<Spawner>([this,t,customerTextures,customerIndexCount]() mutable {
        auto& e = world.createDeferredEntity();
        e.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 1.0f);
        e.addComponent<Velocity>(Vector2D(0, 0), 100.0f);
        e.addComponent<CustomerAI>(); // The AI takes over from here!
        e.addComponent<Customer>();

        Animation anim = AssetManager::getAnimation("customer");
        e.addComponent<Animation>(anim);

        // Your texture cycling logic here is excellent.
        // Because the lambda is 'mutable', customerIndexCount will correctly increment across spawns.
        SDL_Texture* tex = customerTextures[customerIndexCount++ % customerTextures.size()];
        SDL_FRect src= anim.clips[anim.currentClip].frameIndicies[0];
        SDL_FRect dst {t.position.x, t.position.y, 64, 64};
        e.addComponent<Sprite>(tex, src, dst);

    });
    //Display Case: (Test)
    SDL_FRect src = {64,32,32,32};
    SDL_FRect dst = {0,0,32,32};
    createDisplaycase(Vector2D(21*32,15*32),tilemapTex ,{64,32,32,32},{0,0,32,32});
    createDisplaycase(Vector2D(22*32,15*32),tilemapTex ,{64,32,32,32},{0,0,32,32});

    //Cash Register
    auto& cashRegister = world.createEntity();
    cashRegister.addComponent<Transform>(Vector2D(20*32,17*32));
    auto& c = cashRegister.addComponent<Collider>("wall");
    c.rect = {20*32,17 * 32,32,32};
    cashRegister.addComponent<Interaction>([&dayCycle]() {
        if (dayCycle.currentPhase == DayPhase::Morning) {
            std::cout << "Start the day ..." << std::endl;
            dayCycle.phaseSwapReady = true;
        }

    }); // -> Interact to start morning

    //add scene state
    auto &state(world.createEntity());
    state.addComponent<SceneState>();

    //Add Label
    createPlayerPosLabel();
}
Entity& Scene::createDisplaycase(Vector2D location, SDL_Texture* texture,SDL_FRect src, SDL_FRect dst) {
    auto& displayCase (world.createEntity());
    displayCase.addComponent<Transform>(location,0.0f,1.0f);
    displayCase.addComponent<DisplayStand>();
    auto& c = displayCase.addComponent<Collider>("wall");
    c.rect.w = 32;
    c.rect.h = 32;
    c.rect.x = location.x;
    c.rect.y = location.y;
    displayCase.addComponent<Sprite>(texture,src,dst);
    displayCase.addComponent<Interaction>([&displayCase]() {
        auto& dc = displayCase.getComponent<DisplayStand>();

    if (dc.quantity > 0 && dc.item.id != -1) {
        std::cout << "Display case has an "<< dc.item.name <<"! Opening modification UI...\n";
    } else {
        std::cout << "Display case is empty. Opening inventory UI to place item...\n";
    }
    });
    return displayCase;
}

Entity& Scene::createSettingsOverlay(int windowWidth, int windowHeight) {
    auto& overlay(world.createEntity());
    SDL_Texture* overlayTex = TextureManager::load("../asset/ui/settings.jpg");
    SDL_FRect overlaySrc {0,0,windowWidth*0.85f,windowHeight*0.85f};
    SDL_FRect overlayDest {(float) windowWidth/2 - overlaySrc.w /2, (float) windowHeight/2 - overlaySrc.h/2, overlaySrc.w,overlaySrc.h};
    overlay.addComponent<Transform>(Vector2D{overlayDest.x,overlayDest.y},0.0f,1.0f);
    overlay.addComponent<Sprite>(overlayTex,overlaySrc,overlayDest, RenderLayer::UI,false);

    createSettingsUIComponents(overlay);
    return overlay;
}

Entity& Scene::createBaseMenuOverlay(int windowWidth, int windowHeight) {
    auto& overlay(world.createEntity());
    SDL_Texture* overlayTex = TextureManager::load("../asset/ui/settings.jpg");
    SDL_FRect overlaySrc {0,0,windowWidth*0.85f,windowHeight*0.85f};
    SDL_FRect overlayDest {(float) windowWidth/2 - overlaySrc.w /2, (float) windowHeight/2 - overlaySrc.h/2, overlaySrc.w,overlaySrc.h};
    overlay.addComponent<Transform>(Vector2D{overlayDest.x,overlayDest.y},0.0f,1.0f);
    overlay.addComponent<Sprite>(overlayTex,overlaySrc,overlayDest, RenderLayer::UI,false);
    return overlay;
}

Entity& Scene::createHaggleOverlay(int windowWidth, int windowHeight, ItemDef& item) {
    // 1. Create the background
    auto& overlay = createBaseMenuOverlay(windowWidth, windowHeight);

    // 2. Create the Item Name Label
    auto& nameLabel = world.createEntity();
    Label labelData = { item.name, AssetManager::getFont("arial"), {255,255,255} };
    TextureManager::loadLabel(labelData);

    // Position it relative to the overlay
    auto& overlayT = overlay.getComponent<Transform>();
    nameLabel.addComponent<Transform>(Vector2D(overlayT.position.x + 50, overlayT.position.y + 50));
    nameLabel.addComponent<Label>(labelData);

    // 3. Create the Item Sprite (using the source rect from XML!)
    auto& itemIcon = world.createEntity();
    itemIcon.addComponent<Transform>(Vector2D(overlayT.position.x + 50, overlayT.position.y + 100));

    // Use the source rect we parsed from the XML!
    SDL_Texture* itemsTex = TextureManager::get("items");
    itemIcon.addComponent<Sprite>(itemsTex, item.src, SDL_FRect{0,0,64,64}, RenderLayer::UI);

    return overlay;

}

Entity& Scene::createCogButton(int windowWidth, int windowHeight, Entity& overlay) {
    auto& cog = world.createEntity();
    auto& cogTransform = cog.addComponent<Transform>(Vector2D((float) windowWidth - 50,(float) windowHeight- 50), 0.0f,1.0f);

    SDL_Texture *texture = TextureManager::load("../asset/ui/cog.png");
    SDL_FRect cogSrc {0,0,32,32};
    SDL_FRect cogDest {cogTransform.position.x,cogTransform.position.y, cogSrc.w, cogSrc.h};
    cog.addComponent<Sprite>(texture,cogSrc,cogDest, RenderLayer::UI);
    cog.addComponent<Collider>("ui", cogDest);

    auto& clickable = cog.addComponent<Clickable>();
    clickable.onPressed = [&cogTransform] {
        cogTransform.scale = 0.5f;
    };

    clickable.onReleased = [&cogTransform, this ,&overlay] {
        cogTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay);
    };

    clickable.onCancel = [&cogTransform] {
        cogTransform.scale = 1.0f;
    };

    return cog;
}

void Scene::createSettingsUIComponents(Entity& overlay) {
    if (!overlay.hasComponent<Children>()) {
        overlay.addComponent<Children>();
    }

    auto& overlayTransform = overlay.getComponent<Transform>();
    auto& overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;

    auto& closeButton = world.createEntity();
    auto& closeTransform = closeButton.addComponent<Transform>(Vector2D(baseX + overlaySprite.dst.w - 40, baseY + 10 ), 0.0f, 1.0f);

    SDL_Texture *texture = TextureManager::load("../asset/ui/close.png");
    SDL_FRect closeSrc {0,0,32,32};
    SDL_FRect closeDest {closeTransform.position.x,closeTransform.position.y,closeSrc.w, closeSrc.h};
    closeButton.addComponent<Sprite>(texture,closeSrc,closeDest,RenderLayer::UI,false);
    closeButton.addComponent<Collider>("ui",closeDest);

    auto& clickable = closeButton.addComponent<Clickable>();
    clickable.onPressed = [&closeTransform] {
        closeTransform.scale = 0.5f;
    };
    clickable.onReleased = [this,&overlay,&closeTransform] {
        closeTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay);
    };
    clickable.onCancel = [&closeTransform] {
        closeTransform.scale = 1.0f;
    };

    closeButton.addComponent<Parent>(&overlay);
    auto& parentChildren = overlay.getComponent<Children>();
    parentChildren.children.push_back(&closeButton);
}

void Scene::toggleSettingsOverlayVisibility(Entity& overlay) {
    auto& sprite = overlay.getComponent<Sprite>();
    bool newVisibility = !sprite.visible;
    sprite.visible = newVisibility;

    if (overlay.hasComponent<Children>()) {
        auto& c = overlay.getComponent<Children>();

        for (auto& child : c.children) {
            if (child && child->hasComponent<Sprite>()) {
                child->getComponent<Sprite>().visible = newVisibility;
            }

            if (child && child->hasComponent<Collider>()) {
                child->getComponent<Collider>().enabled = newVisibility;
            }
        }
    }

}

Entity &Scene::createPlayerPosLabel() {
    auto & playerPosLabel(world.createEntity());
    Label label = {
        "Text String",
        AssetManager::getFont("arial"),
        {255,255,255,255},
        LabelType::PlayerPosition,
        "playerPos"
    };
    TextureManager::loadLabel(label);
    playerPosLabel.addComponent<Label>(label);
    playerPosLabel.addComponent<Transform>(Vector2D(10,10), 0.0f,1.0f);
    return playerPosLabel;


}
