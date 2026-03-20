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
    world.getMap().load(mapPath,TextureManager::load("../asset/Sprite-0002.png"));
    for (auto& collider : world.getMap().colliders) {
        auto& e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x,collider.rect.y),0.0f,1.0f);
        auto& c = e.addComponent<Collider>("wall");

        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;

        SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
        SDL_FRect colSrc {0,32,32,32};
        SDL_FRect colDst {c.rect.x,c.rect.y,c.rect.w,c.rect.h};
        e.addComponent<Sprite>(tex,colSrc,colDst);
    }

    //PathFinding:
    //Find Non-Walkable layer
    PathfindingSystem::InitMap(windowWidth,windowHeight,world.getMap().AIWalkable);


    //add coins
    // for (auto& coin : world.getMap().coins) {
    //     auto& item = world.createEntity();
    //     item.addComponent<Transform>(Vector2D(coin.rect.x,coin.rect.y),0.0f,1.0f);
    //     auto& c = item.addComponent<Collider>("item");
    //
    //     c.rect.x = coin.rect.x;
    //     c.rect.y = coin.rect.y;
    //
    //     SDL_Texture* tex = TextureManager::load("../asset/coin.png");
    //     SDL_FRect colSrc {0,0,32,32};
    //     SDL_FRect colDst {c.rect.x,c.rect.y,32,32};
    //     item.addComponent<Sprite>(tex,colSrc,colDst);
    // }
    // auto& item(world.createEntity());
    // auto& itemTransform = item.addComponent<Transform>(Vector2D(100,200), 0.0f,1.0f);
    //
    // SDL_Texture*  itemTex = TextureManager::load("../asset/coin.png");
    // SDL_FRect itemSrc{0,0,32,32};
    //
    // SDL_FRect itemDest {itemTransform.position.x, itemTransform.position.y,32,32};
    // item.addComponent<Sprite>(itemTex,itemSrc,itemDest);
    //
    // auto& itemCollider = item.addComponent<Collider>("item");
    // itemCollider.rect.w = itemDest.w;
    // itemCollider.rect.h = itemDest.h;

    auto& cam = world.createEntity();
    SDL_FRect camView{};
    camView.w = windowWidth;
    camView.h = windowHeight;
    cam.addComponent<Camera>(camView, static_cast<float>(world.getMap().width * 32), static_cast<float>(world.getMap().height * 32));


    auto& player(world.createEntity());
    auto& playerTransform = player.addComponent<Transform>(Vector2D(0,0),1.0f);
    player.addComponent<Velocity>(Vector2D(0,0), 120.0f);

    Animation anim = AssetManager::getAnimation("player");
    player.addComponent<Animation>(anim);

    SDL_Texture* tex = TextureManager::load("../asset/animations/fox_anim.png");
    // SDL_FRect playerSrc {0,0,32,44};
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndicies[0];
    SDL_FRect playerDst {playerTransform.position.x,playerTransform.position.y,64,64};
    player.addComponent<Sprite>(tex,playerSrc,playerDst);

    auto& playerCollider = player.addComponent<Collider>("player");
    playerCollider.rect.w = playerDst.w;
    playerCollider.rect.h = playerDst.h;

    player.addComponent<PlayerTag>();
    player.addComponent<Health>(Game::gameState.playerHealth);

    auto & customer (world.createEntity());
    customer.addComponent<Transform>(Vector2D(651,369),1.0f);
    customer.addComponent<Velocity>(Vector2D(0,0), 120.0f);
    customer.addComponent<CustomerAI>();
    SDL_Texture* texture = TextureManager::load("../asset/animations/bird_anim.png");
    SDL_FRect src = {0,0,32,32};
    SDL_FRect dst {651,369,32,32};
    customer.addComponent<Sprite>(texture,src,dst);

    auto& spawner(world.createEntity());
    Transform t = spawner.addComponent<Transform>(Vector2D(windowWidth/2,windowHeight - 5),0.0f,1.0f);
    spawner.addComponent<TimedSpawner>(2.0f,[this,t] {

        //create projectiles
        auto& e(world.createDeferredEntity());
        e.addComponent<Transform>(Vector2D(t.position.x,t.position.y),0.0f,1.0f);
        e.addComponent<Velocity>(Vector2D(0,-1),100.0f);

        Animation anim = AssetManager::getAnimation("enemy");
        e.addComponent<Animation>(anim);

        SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
        SDL_FRect src = {0,0,32,32};
        SDL_FRect dst {t.position.x,t.position.y,32,32};
        e.addComponent<Sprite>(tex,src,dst);

        Collider c = e.addComponent<Collider>("projectile");
        c.rect.w = dst.w;
        c.rect.h = dst.h;

        e.addComponent<ProjectileTag>();

    });


    //add scene state
    auto &state(world.createEntity());
    state.addComponent<SceneState>();
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
