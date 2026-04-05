cmake_minimum_required(VERSION 4.0)
project(Project)

set(CMAKE_CXX_STANDARD 20)

#Set SDL3 include and lib paths

if (WIN32)
    set(SDL3_PATH "D:/NextCloud Files/Game Dev/Game Architecture/Dev/SDL3.4")
    set(SDL_IMAGE_PATH "D:/NextCloud Files/Game Dev/Game Architecture/Dev/SDL_Image 3.26")
    set(SDL_ttf_PATH "D:/NextCloud Files/Game Dev/Game Architecture/Dev/SDL3_ttf")
    include_directories(
            ${SDL3_PATH}/include
            ${SDL_IMAGE_PATH}/include
            ${SDL_ttf_PATH}/include
            ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_SOURCE_DIR}/src/ecs
            ${PROJECT_SOURCE_DIR}/src/ecs/system
            ${PROJECT_SOURCE_DIR}/src/utils
            ${PROJECT_SOURCE_DIR}/src/vendor
    )
    link_directories(
            ${SDL3_PATH}/lib
            ${SDL_IMAGE_PATH}/lib
            ${SDL_ttf_PATH}/lib
    )
endif ()

add_executable(Project src/main.cpp
        src/Game.cpp
        src/TextureManager.cpp
        src/Map.cpp
        src/utils/Vector2D.cpp
        src/utils/Collision.cpp
        src/ecs/system/CollisionSystem.cpp
        src/vendor/tinyxml2.cpp
        src/manager/AssetManager.cpp
        src/scene/Scene.cpp
        src/ecs/system/MouseInputSystem.cpp
        src/ecs/system/EventResponseSystem.cpp
        src/ecs/system/MainMenuSystem.cpp
        src/ecs/system/DayCycleSystem.h
        src/ecs/system/DayCycleSystem.cpp
        src/ecs/system/CustomerAISystem.cpp
        src/ecs/system/PathfindingSystem.cpp
        src/ecs/system/TransactionSystem.cpp
        src/ecs/system/DebtSystem.cpp
        src/ecs/system/CustomerSpawnerSystem.cpp
        src/Items.cpp
        src/scene/SceneUI.cpp
        src/scene/SceneDisplayCase.cpp
        src/ecs/system/HaggleSystem.cpp
)

target_link_libraries(Project
    SDL3
    SDL3_image
    SDL3_ttf
)