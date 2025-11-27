AI Battle — Simple C++ AI skirmish simulator
=============================================

Short description
-----------------
A small 2D tactical skirmish simulator written in modern C++ (C++17) that demonstrates simple AI behaviours: commanders, medics, porters (resupply), warriors, line-of-sight, A* pathfinding and a lightweight risk map.

Key features
------------
- Turn-based tick simulation with simple combat (bullets, grenades).
- Commander AI that coordinates medics and porters, issues movement orders and builds visibility maps.
- Warriors that pursue enemies using A* pathfinding and risk-aware routing.
- Medic logic that can revive incapacitated warriors.
- Logging and debug output to trace agent positions and events.

Repository layout
-----------------
- `include/` — Public headers (`Agents.h`, `Types.h`, `Game.h`, `AStar.h`, `BFS.h`, `CommanderAI.h`, ...).
- `src/` — Implementation files (`Game.cpp`, `CommanderAI.cpp`, `Agents.cpp`, `AStar.cpp`, `BFS.cpp`, `Visibility.cpp`, ...).
- `assets/` — Map files (e.g. `sample_map_80x50.txt`).
- Visual Studio project files (`*.vcxproj`, `*.sln`) for building on Windows with MSVC.

Build & run
-----------
Requirements:
- Windows 10/11
- Visual Studio 2019/2022 (MSVC toolset) or any compiler that supports C++17.

To build:
1. Open the solution or project in Visual Studio (`ai_battle.sln` or the `.vcxproj`).
2. Build the project (Debug/Release as desired).

To run from command line:
- `.in\Debug\ai_battle.exe 1` — run using configuration `1` (balanced).
- Valid command-line options: `1` = Balanced, `2` = Blue advantage, `3` = Orange advantage.

There is also a console fallback. Define `USE_CONSOLE` to enable the console run path when building.

Runtime logs
------------
The simulation writes debug information for diagnosis:
- `game_debug.log` — high-frequency per-tick positions and state (use to inspect zig-zagging and movement traces).
- `game_log.txt` — periodic detailed state snapshots (HP, ammo, revive/resupply counts).

Troubleshooting and notes
-------------------------
- The code targets C++17; make sure your project language standard is set accordingly.
- If you observe warriors disappearing at 0 HP, the code was updated so warriors become `incapacitated` rather than removed; medics can revive them (subject to configured revive limits).
- Stalemates can occur depending on configuration and map; configurable constants in `include/Types.h` (for example `kForceCommanderFocusTick`, revive/resupply limits) let you tune AI aggression and sustain.
- To debug movement: inspect `game_debug.log` to follow per-tick positions and path lengths.

Configuration & tuning
----------------------
Major tuning parameters live in `include/Types.h` and AI logic in `src/CommanderAI.cpp` and `src/Game.cpp`:
- `kGunRange`, `kGrenadeRange`, `kMedCallHP`, `kPorterCooldown` — combat and logistics thresholds.
- `kForceCommanderFocusTick` — tick after which commanders force warriors to prioritize enemy commander.
- `kMaxWarriorRevives`, `kMaxResuppliesPerWarrior` — limits to avoid infinite sustain.

Contributing
------------
Contributions welcome. Suggested workflow:
1. Fork the repository.
2. Create a feature branch.
3. Submit a pull request with a clear description and tests/logs if applicable.