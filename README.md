# HookLeap

HookLeap is a 2D platformer built with **C++17** and **SFML 3**.  
Move through maps, collect pickups, reach checkpoints, and use a grappling hook to swing between platforms and finish the level.

## Features

- Main menu with map selection
- Two playable maps (`map1.txt`, `map2.txt`)
- Grappling hook movement and rope constraints
- Platforming with jump, gravity, friction, and collision handling
- Pickups:
  - `Coin` (score)
  - `Checkpoint` (respawn point)
  - `Win` pickup (completes level)
- Win screen with score, time, and death counter

## Controls

- **A / Left Arrow**: Move left
- **D / Right Arrow**: Move right
- **W / Space / Up Arrow**: Jump
- **Left Mouse Button**: Shoot hook
- **Right Mouse Button** (or jump keys while hooked): Release hook
- **Esc**: Pause while playing
- **Any key**: Resume from pause

## Requirements

- CMake `>= 3.16`
- C++17 compiler
- SFML 3 (`System`, `Window`, `Graphics`, `Audio`, `Network`)

## Build

From repository root:

```bash
cmake -S . -B build
cmake --build build
```

> Note: `CMakeLists.txt` currently contains a Windows-specific `SFML_DIR` example path:
> `C:/Libraries/SFML-3.0.2/lib/cmake/SFML`  
> Update or remove it to match your local SFML installation.

## Run

After build, run the produced `HookLeap` executable from the build output directory.

The build copies `assets/` next to the executable automatically.

## Project Structure

- `/src` - game logic and application entry point
- `/include` - headers for core systems and gameplay objects
- `/assets` - textures, font, and map definitions
- `/assets/maps` - map files loaded by the menu
