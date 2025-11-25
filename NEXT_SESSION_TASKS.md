# NEXT SESSION TASKS - AI Battle Game
**Date:** 2025-11-25
**Current Status:** Game is working but needs detailed logging to debug visual issues

## IMMEDIATE ISSUE TO FIX
**Problem:** Compilation error - `logDetailedState()` is not declared in Game.h header

**Solution:**
1. Open `include\Game.h`
2. Add this line in the `private:` section of the Game class:
   ```cpp
   void logDetailedState();
   ```

## WHAT'S WORKING NOW
✅ Game runs and reaches conclusions (win/loss/draw)
✅ Configuration system allows choosing team advantages
✅ Warriors fight, medics heal, porters resupply
✅ Grenade and shooting combat works
✅ Revival system works (medics revive dead warriors)
✅ Stalemate detection (500 ticks without change)
✅ Win conditions: Commander death OR stalemate

## CURRENT PROBLEM
User reports seeing strange behavior in the animation:
- Zigzagging movement
- Agents doing things that seem wrong
- General "off" feeling

## TASKS FOR NEXT SESSION

### 1. FIX COMPILATION ERROR (PRIORITY 1)
- Add `void logDetailedState();` declaration to Game.h header file
- Build and verify it compiles

### 2. RUN GAME AND ANALYZE LOGS
The game already writes to `game_log.txt` every 100 ticks with:
- All agent positions
- HP, ammo, grenades
- Path lengths
- States

**To analyze:**
- Run the game with configuration option 2 (Blue Advantage)
- Watch the animation carefully
- Check `game_log.txt` after the game
- Look for suspicious patterns:
  - Agents at same position
  - Rapid position changes (zigzagging)
  - Incorrect state transitions
  - Path recalculations

### 3. SPECIFIC THINGS TO CHECK
Based on previous debugging:

**Porter Issues:**
- Does porter actually move between depot and warriors?
- Is there an infinite loop between states?
- Check porter state transitions in log

**Warrior Issues:**
- Are warriors recalculating paths too often?
- Do they get stuck at territory boundaries?
- Are they shooting when they should?
- Do they advance properly?

**Medic Issues:**
- Does medic actually reach wounded warriors?
- Does revival work correctly?
- Is medic zigzagging?

**Combat Issues:**
- Are bullets/grenades hitting the right targets?
- Distance calculations correct?
- "ERROR: no agent found" messages - why?

### 4. KNOWN ISSUES TO INVESTIGATE
1. **Zigzagging:** Likely caused by recalculating paths every tick
   - Solution: Only recalculate when path is empty or blocked
   
2. **Distance 0 warriors:** Two warriors at same position can't fight
   - Should never happen - check collision detection
   
3. **Out of ammo deadlocks:** Warriors at distance 7-9 with no ammo/grenades
   - Porter should resupply but might not be reaching them
   
4. **"No agent found" errors:** Shooting at positions where agent moved
   - Combat happens before movement? Check order of operations

### 5. CONFIGURATION CHANGES MADE
Recent commits have:
- Increased advantages to make decisive wins:
  - Blue Advantage: +40 HP, +10 Ammo, +2 Grenades
  - Orange Advantage: +40 HP, +10 Ammo, +2 Grenades
- Added stalemate detection (500 ticks no change)
- Added configuration selection menu at game start

### 6. FILES THAT NEED ATTENTION
Priority order:
1. `include\Game.h` - Add logDetailedState declaration
2. `src\WarriorAI.cpp` - Check path recalculation logic
3. `src\PorterAI.cpp` - Check state machine and movement
4. `src\MedicAI.cpp` - Check state machine and movement
5. `src\CommanderAI.cpp` - Check order-giving logic

### 7. LOG FILE LOCATIONS
- `game_debug.log` - Configuration and setup info
- `game_log.txt` - Detailed agent states every 100 ticks (APPEND mode)
- Console output - Real-time combat events

## TESTING PROCEDURE
1. Fix compilation error
2. Delete old log files
3. Run game with Blue Advantage config (option 2)
4. Watch animation carefully - note specific issues
5. After game ends, analyze `game_log.txt`
6. Compare logged positions with what you saw visually
7. Identify discrepancies

## LONG-TERM GOALS
- Fix any movement/combat issues found in logs
- Ensure game is visually correct and matches logs
- Polish for final submission
- Maybe add more configurations or features if time

## COMMIT STATUS
Last successful commit: "Increased team advantages for more decisive outcomes"
- Working game with configs
- Need to add header declaration and test

## REMEMBER
- User has limited time for submission
- Game needs to work correctly for grading
- Visual bugs are unacceptable even if logic seems right
- Logs will reveal the truth!
