# AI Battle Game - Configuration Guide

## How to Run Different Game Scenarios

The game currently has balanced settings. To give one team an advantage, modify these constants in the source files:

### 1. **Balanced Game (Current Default)**
No changes needed - both teams are equal.

### 2. **Blue Team Advantage**
Edit `include/Types.h` and change:

```cpp
// Give Blue team more starting ammo
constexpr int kInitialAmmo = 25;  // Default: 20
constexpr int kInitialGrenades = 3;  // Default: 2

// Or make Blue warriors tougher
constexpr int kMaxHealth = 120;  // Default: 100
```

Then recompile and run.

### 3. **Orange Team Advantage**  
Edit `src/Game.cpp` around line 50 and add this in the initialization:

```cpp
// Give Orange team advantages
for (auto& w : orange.warriors) {
    w.hp = 120;  // More HP
    w.ammo = 25;  // More ammo
    w.grenades = 3;  // More grenades
}
```

### 4. **Fast Resupply for One Team**
Edit `src/PorterAI.cpp` and change the cooldown:

```cpp
// Line ~80: Make cooldown shorter for one team
if (port.team == Team::Blue) {
    port.lastResupplyTick[warriorIdx] = tick + 25;  // Faster for Blue
} else {
    port.lastResupplyTick[warriorIdx] = tick + 50;  // Normal for Orange  
}
```

### 5. **Better Medic for One Team**
Edit `src/MedicAI.cpp` and change healing speed:

```cpp
// Heal faster for one team
if (medic.team == Team::Blue) {
    w.hp = 100;  // Instant full heal for Blue
} else {
    w.hp = std::min(100, w.hp + 30);  // Gradual heal for Orange
}
```

## Current Game Settings

- **Stalemate Detection**: 500 ticks of no changes triggers end game
- **Absolute Timeout**: 5000 ticks maximum game length
- **Gun Range**: 6 tiles
- **Grenade Range**: 10 tiles
- **Gun Damage**: 20 HP per shot
- **Grenade Damage**: 15 HP
- **Porter Resupply Cooldown**: 50 ticks
- **Health Threshold for Healing**: 25 HP (25%)
- **Ammo Threshold for Resupply**: 5 bullets

## Quick Test Scenarios

### Scenario A: Blue Dominates
1. Edit `include/Types.h`: Set `kInitialAmmo = 30`
2. Recompile
3. Run - Blue should win

### Scenario B: Orange Dominates  
1. Edit `src/Game.cpp` initialization: Give Orange 150 HP
2. Recompile
3. Run - Orange should win

### Scenario C: Draw/Stalemate
1. Keep default balanced settings
2. Run - May result in stalemate or close victory

## Recompiling After Changes

After editing source files:
```bash
# Open in Visual Studio and rebuild
# OR use MSBuild:
msbuild ai_battle.sln /p:Configuration=Debug
```
