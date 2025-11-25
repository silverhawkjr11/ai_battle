# TODO - Next Session Instructions

## Current State (2025-11-25)
The game is mostly working! Configuration system works, combat works, revival works, but there are compilation errors with logging.

## Immediate Issues to Fix

### 1. Compilation Error - Missing Header Declaration
**File:** `include\Game.h`
**Problem:** Need to add declaration for `logDetailedState()` method

**Fix:** Add this line to the `Game` class in `include\Game.h`:
```cpp
void logDetailedState();  // Add inside the class declaration
```

### 2. Enhance Logging System
The logging infrastructure is in place but needs AI behavior logging:
- `WarriorAI.cpp` - Log movement decisions, combat actions
- `MedicAI.cpp` - Log healing decisions, pathfinding
- `PorterAI.cpp` - Log resupply decisions
- `CommanderAI.cpp` - Log strategic decisions

## User's Main Concerns

### Visual Feedback Issues
User reports seeing "zigzagging" and behaviors that don't match logs:
1. **Agents moving back and forth** - Likely recalculating paths too frequently
2. **Combat not happening when it should** - Distance calculations might be off
3. **Agents appearing/disappearing** - Dead agents visibility issue

### What to Investigate Next
1. **Run the game and read `game_log.txt`** - Detailed state every 100 ticks
2. **Check pathfinding** - Are paths being recalculated every tick?
3. **Verify combat ranges** - Gun range=6, Grenade range=10
4. **Dead agent handling** - Should stay visible at HP=0

## Configuration System (WORKING ✅)
- Balanced mode: Equal teams
- Blue Advantage: +40 HP, +10 Ammo, +2 Grenades
- Orange Advantage: +40 HP, +10 Ammo, +2 Grenades

User can select at startup with menu (1-3).

## Recent Commits
1. ✅ "Working game with revival and combat system"
2. ✅ "Add configuration system with team advantages"
3. ✅ "Increase advantages for decisive wins"

## Next Steps Priority
1. **FIX COMPILATION** - Add `logDetailedState()` to header
2. **RUN THE GAME** - Select Blue Advantage and watch for 1000 ticks
3. **READ game_log.txt** - Analyze agent behavior patterns
4. **ADD AI LOGGING** - Detailed decision logging in each AI file
5. **FIX ZIGZAGGING** - Prevent path recalculation unless needed
6. **TEST WIN CONDITIONS** - Ensure advantages lead to victory

## Key Files to Focus On
- `include\Game.h` - Add method declaration
- `src\WarriorAI.cpp` - Add movement/combat logging
- `src\MedicAI.cpp` - Add healing logic logging  
- `src\PorterAI.cpp` - Add resupply logic logging
- `game_log.txt` - Read this to understand actual behavior

## User Feedback to Address
> "a lot of things just feel off and dont seem to be true, ive seen a lot of zigzags"

This means:
- Agents are likely recalculating paths every frame
- Need to add **path stability** - only recalculate when:
  - Target moved significantly (>5 tiles)
  - Current path blocked
  - Combat state changed
  - Reached destination

## Remember
- User needs this for submission (perfect grade requirement!)
- Game mostly works but needs polish and verification
- Focus on PROVING behavior matches logs
- Commit after each successful fix
