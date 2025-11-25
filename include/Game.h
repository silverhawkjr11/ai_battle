#pragma once
#include "Types.h"
#include "Grid.h"
#include "Agents.h"
#include "Bullets.h"
#include "CommanderAI.h"

#include <vector>
#include <optional>
#include <string>

struct TeamState {
    Commander commander;
    std::vector<Warrior> warriors;
    Medic medic;
    Porter porter;
    Team team;

    TeamState(Team t, const Grid& g,
        IVec2 cpos, IVec2 w1, IVec2 w2, IVec2 mp, IVec2 pp)
        : commander(t, cpos)
        , warriors{ Warrior(t, w1), Warrior(t, w2) }
        , medic(t, mp)
        , porter(t, pp)
        , team(t)
    {}
};

struct Game {
    Grid grid;
    TeamState blue;
    TeamState orange;

    BulletSystem bullets;
    GrenadeSystem grenades;

    bool running{ true };
    int tick{ 0 };

    Game(const Grid& g, const GameConfig& config = GameConfig::Balanced());

    void step();

    std::vector<IVec2> enemySpots(Team t) const;
    Agent* findAgentAt(Team t, IVec2 p);
};
