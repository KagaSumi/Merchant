# Relics and Receipts

A 2D shop management game built from the ground up in C++ using SDL. Step into the shoes of a shopkeeper to manage inventory, interact with customers, and build a thriving storefront.

## 🛠️ Tech Stack & Architecture

* **Language:** C++
* **Rendering & Input:** SDL3
* **Architecture:** Custom Entity Component System (ECS)
* **Design Patterns:** Scene Management, State Machine (for Customer AI)

## ✨ Core Features

* **Shop Management:** Run the day-to-day operations of a fantasy item shop.
* **Customer AI:** Dynamic customer behaviors with pathfinding to navigate the store, browse items, and approach the counter.
* **Inventory System:** Robust underlying item management and stock tracking.
* **Custom ECS:** Highly modular and performant engine architecture separating data (components) from logic (systems).
* **Scene Management:** Seamless transitions between main menus, the shop floor, and end-of-day result screens.

## 🚀 Getting Started
### Download
Download the latest release from the [releases page](https://github.com/KagaSumi/Merchant/releases).

Run the executable file to play the game.

## Building from Source

### Prerequisites

To build and run this project, you will need the following installed on your system:

* A C++17 (or newer) compatible compiler (e.g., GCC, Clang, or MSVC)
* [CMake](https://cmake.org/) (Version 3.10+)
* [SDL3.4](https://libsdl.org/) development libraries
* [SDL3.26_image](https://github.com/libsdl-org/SDL_image) (if handling PNG/JPG assets)
* [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf) (for text rendering)
* [SDL3.2_mixer](https://github.com/libsdl-org/SDL_mixer/releases) (for audio playback)

### Building

1. **Clone the repository:**
   ```bash
   git clone https://github.com/KagaSumi/Merchant.git cd [YourRepoName]
   ```
2. Generate build files using CMake:
3. Compile the game:

## 🎮 Controls

* **WASD:** Move the player.
* **E:** Interact.
* **I:** Open Inventory.
* **Mouse:** Interact with Menus.

## 📂 Project Structure

- `/src`: Contains all C++ source files (.cpp).

