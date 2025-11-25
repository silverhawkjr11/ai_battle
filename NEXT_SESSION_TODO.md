# Next Session TODO - AI Battle Game

**Date:** 2025-11-25
**Status:** Game is mostly working but needs debugging

## Current Status
- ✅ Game runs and completes with win/loss/draw conditions
- ✅ Configuration system working (Balanced, Blue Advantage, Orange Advantage)
- ✅ Revival system working (medics can revive dead warriors)
- ✅ Combat system working (shooting, grenades, explosions)
- ✅ Porter resupply working (with cooldown)
- ✅ Commander AI coordinating teams
- ❌ User reports zigzagging and strange behavior in GUI animation
- ❌ File logging incomplete (compilation errors)

## Immediate Issues to Fix

### 1. Add `logDetailedState()` to Game.h
**File:** `include/Game.h`
**Action:** Add this line in the `private:` section:
```cpp
void logDetailedState();
```
This will fix the compilation errors about undefined identifier.

### 2. Verify and Debug Agent Behavior
The user reports seeing:
- Zigzagging movement (agents moving back and forth)
- Behavior that doesn't match what should happen
- Need to watch the GUI animation and compare with logs

**Plan:**
1. Fix the compilation error above
2. Run the game with configuration option 2 (Blue Advantage)
3. Open `game_log.txt` after running
4. Check for patterns:
   - Are agents recalculating paths too often?
   - Are porters/medics moving efficiently?
   - Are warriors getting stuck in loops?

### 3. Known Zigzag Issues
From earlier debugging, possible causes:
- **Porter zigzagging:** Porter may be recalculating path every tick instead of following existing path
- **Medic zigzagging:** Same issue as porter
- **Warriors circling:** May be caused by A* recalculation when target moves

**Fix Strategy:**
- Only recalculate path when:
  1. Path is empty
  2. Target has moved significantly (> 5 tiles)
  3. Path is blocked
- Add "path following" state where agent doesn't recalculate

### 4. Increase Team Advantages for Decisive Wins
Current advantages are not strong enough - games still end in draws.

**File:** `include/GameConfig.h`

Current advantages:
- Blue: +40 HP, +10 Ammo, +2 Grenades
- Orange: +40 HP, +10 Ammo, +2 Grenades

**Suggested stronger advantages:**
```cpp
static GameConfig blueAdvantage() {
    return { 50, 15, 3, 0, 0, 0 };  // Blue: +50 HP, +15 Ammo, +3 Grenades
}

static GameConfig orangeAdvantage() {
    return { 0, 0, 0, 50, 15, 3 };  // Orange: +50 HP, +15 Ammo, +3 Grenades
}
```

## Files Currently Modified
- `src/Game.cpp` - Already has `logDetailedState()` implementation
- `include/Game.h` - NEEDS `void logDetailedState();` declaration
- `include/GameConfig.h` - Has configuration system
- `src/main.cpp` - Has configuration menu

## Testing Checklist
After fixing compilation:
1. ✅ Compile successfully
2. ❌ Run with Blue Advantage (option 2)
3. ❌ Check `game_log.txt` for detailed state every 100 ticks
4. ❌ Watch GUI for zigzagging
5. ❌ Verify Blue wins decisively (not draw)
6. ❌ Test Orange Advantage (option 3)
7. ❌ Test Balanced (option 1)

## Project Requirements Verification
According to the Hebrew requirements:

### ✅ Implemented:
- Commander plans and gives orders (CommanderAI)
- 2 Warriors per team with guns and grenades
- Medic heals wounded soldiers (including revival from 0 HP)
- Porter brings ammunition
- A* pathfinding for all agents
- Visibility system (agents report what they see)
- Terrain: Rocks, Trees, Water (impassable/blocks line of sight)
- Depots for each team (yellow squares)
- Game ends when Commander dies OR stalemate

### ❌ Not Fully Verified:
- Safety map usage (agents should use safe routes) - needs verification
- Warriors should retreat when low HP - implemented but needs testing
- BFS for finding safe points - might not be implemented
- Commander visibility map (combined from all soldiers) - unclear if working

## Next Steps
1. Add `void logDetailedState();` to `Game.h`
2. Compile and test
3. Run game and review `game_log.txt`
4. Identify zigzag patterns
5. Fix path recalculation logic
6. Increase team advantages
7. Final testing and commit

## Notes
- User is happy with current functionality but wants polish
- User needs to submit this project for a grade
- Focus on: making it look good in GUI animation and ensuring decisive wins
