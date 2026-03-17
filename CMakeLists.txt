cmake_minimum_required(VERSION 4.0)
project(Project)

set(CMAKE_CXX_STANDARD 20)

#Set SDL3 include and lib paths

if (WIN32)
    set(SDL3_PATH "D:/NextCloud Files/Game Dev/Game Architecture/Dev/SDL3.4")
    set(SDL_IMAGE_PATH "D:/NextCloud Files/Game Dev/Game Architecture/Dev/SDL_Image 3.26")
    include_directories(
            ${SDL3_PATH}/include
            ${SDL_IMAGE_PATH}/include
            ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_SOURCE_DIR}/src/ecs
            ${PROJECT_SOURCE_DIR}/src/ecs/system
            ${PROJECT_SOURCE_DIR}/src/utils
            ${PROJECT_SOURCE_DIR}/src/vendor
    )
    link_directories(
            ${SDL3_PATH}/lib
            ${SDL_IMAGE_PATH}/lib
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
)

target_link_libraries(Project
    SDL3
    SDL3_image
)