# AI Battle Game - Configuration Guide

## How to Select Game Configuration

When you start the game, you'll see a menu:

```
========== AI BATTLE GAME ==========
Select configuration:
1. Balanced (Equal strength)
2. Blue Advantage (+20% HP/Ammo)
3. Orange Advantage (+20% HP/Ammo)
Enter choice (1-3):
```

## Configuration Options

### 1. Balanced (Default)
- **Blue Team**: 100 HP, 20 Ammo, 2 Grenades
- **Orange Team**: 100 HP, 20 Ammo, 2 Grenades
- **Result**: Most games end in a draw after timeout

### 2. Blue Advantage
- **Blue Team**: 120 HP, 24 Ammo, 2 Grenades (+20%)
- **Orange Team**: 100 HP, 20 Ammo, 2 Grenades
- **Result**: Blue team has a significant advantage and usually wins

### 3. Orange Advantage
- **Orange Team**: 120 HP, 24 Ammo, 2 Grenades (+20%)
- **Blue Team**: 100 HP, 20 Ammo, 2 Grenades
- **Result**: Orange team has a significant advantage and usually wins

## Command Line Usage

You can also select configuration via command line:

```bash
# Balanced
ai_battle.exe 1

# Blue Advantage
ai_battle.exe 2

# Orange Advantage
ai_battle.exe 3
```

## Game End Conditions

The game ends when:

1. **Commander killed**: If either team's commander dies, that team loses
2. **Stalemate detected**: If no combat happens for 200 ticks (no HP changes, no deaths)
3. **Timeout**: Maximum 3000 ticks, after which the team with more alive warriors wins

## Configuration Details

The configurations give strategic advantages by:
- **More HP**: Warriors can survive more hits before needing medical attention
- **More Ammo**: Warriors can shoot more before needing resupply
- **More Grenades**: Warriors have additional grenades for area attacks

The +20% advantage is enough to break stalemates and create decisive victories.
