#pragma once
#include "Types.h"
#include "Grid.h"
#include "Agents.h"
#include "AStar.h"
#include "BFS.h"
#include "Risk.h"

#include <vector>

enum class OrderType { Attack, Defend, Heal, Resupply, Move };

struct Order {
    OrderType type;
    IVec2 target;
};

struct CommanderAI {
    static void step(const Grid& g,
        Commander& c,
        std::vector<Warrior>& warriors,
        Medic& med,
        Porter& port,
        const std::vector<IVec2>& enemySpots,
        int tick = 0);
};
