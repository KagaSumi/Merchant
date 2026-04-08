# Relics & Receipts
<p align="center">
  <img src="https://github.com/user-attachments/assets/9950609a-2c71-41dc-ab87-195d1d2195d1" width="800" />
</p>
A 2D shop management game built from scratch in C++ using SDL3. Play as a shopkeeper — manage your stockroom, haggle with customers, track your finances, and pay off your debt before time runs out.

---

## Tech Stack

| | |
|---|---|
| **Language** | C++17 |
| **Rendering / Input / Audio** | SDL3, SDL3_image, SDL3_ttf, SDL3_mixer |
| **Architecture** | Custom Entity Component System (ECS) |
| **Build System** | CMake |
| **XML Parsing** | tinyxml2 |

---

## Features

- **Haggle System** — Customers arrive with moods and patience. Propose a price; they accept or walk away based on mood, patience, and market trends.
- **Customer AI** — State machine–driven customers pathfind through the store, browse display stands, and queue at the register.
- **Inventory & Display Stands** — Stock items onto display cases from your backroom inventory. Track quantities and reservations per stand.
- **Day Cycle** — Morning prep → shop open → evening close. Each phase locks/unlocks player actions and triggers events.
- **Economy** — Earn gold from sales, spend it on restocking orders and new display shelves. Make weekly debt payments or face game over.
- **Reputation System** — Profitable sales grow your shop reputation, unlocking higher-tier items in the order catalogue.
- **Market Trends** — Daily trend rolls modify item price modifiers, rewarding players who read the market.
- **Order System** — End-of-day ordering screen lets you restock from available items gated by your current reputation level.
- **Custom ECS Engine** — Components hold data, systems hold logic. No inheritance chains — entities are just IDs with attached components.

---

## Project Structure

```
src/
├── ecs/
│   ├── event/          # Audio event queue, base events
│   ├── system/         # All game systems (AI, haggle, day cycle, rendering, etc.)
│   ├── Components.h    # All component definitions
│   ├── Entity.h
│   └── World.h         # System registry and entity management
├── manager/            # Asset, audio, scene, and UI visibility managers
├── scene/
│   ├── UI/             # All UI builders (HaggleUI, InventoryUI, HudUI, etc.)
│   ├── Scene.cpp/.h    # Scene init and system wiring
│   └── SceneDisplayCase.cpp
├── utils/              # Collision, Vector2D, render utilities
├── vendor/             # tinyxml2
├── Game.cpp/.h         # Entry point, game loop, global state
└── main.cpp
```

---

## Getting Started

### Download

Grab the latest build from the [Releases page](https://github.com/KagaSumi/Relics_and_Receipts/releases) and run the executable.

### Building from Source

**Prerequisites**

- C++17 compatible compiler (GCC, Clang, or MSVC)
- [CMake](https://cmake.org/) 3.10+
- SDL3 development libraries
- SDL3_image
- SDL3_ttf
- SDL3_mixer

**Steps**

```bash
git clone https://github.com/KagaSumi/Relics_and_Receipts.git
cd Merchant
cmake -S . -B build
cmake --build build
```

> Make sure SDL3 and its extension libraries are findable by CMake. On Linux, install via your package manager. On Windows, point `CMAKE_PREFIX_PATH` at your SDL3 install directory.

---

## Controls

| Key | Action |
|---|---|
| `WASD` | Move |
| `E` | Interact |
| `I` | Open inventory |
| `LShift` | Sprint |
| `Mouse` | Navigate menus |

