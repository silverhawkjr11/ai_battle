#pragma once
#include "Types.h"
#include "Grid.h"
#include <vector>
#include <optional>
#include <string>

struct Perception {
    bool seesEnemy{ false };
    std::optional<IVec2> enemyPos;
};

struct Agent {
    Team        team;
    IVec2       pos;
    int         hp{ 100 };
    char        glyph{ '?' };
    std::string role{ "?" };
    int         ammo{ 0 };
    int         grenades{ 0 };
    bool        alive{ true };
    bool        incapacitated{ false }; // At 0 HP but can be revived
    int         lastResupplyTick{ -999 }; // Track when last resupplied

    Agent(Team t, IVec2 p, char g, const char* r)
        : team(t), pos(p), glyph(g), role(r) {}

    virtual ~Agent() = default;
    virtual void update(const Grid&) {}

    void takeDamage(int dmg) {
        if (!alive) return;
        hp -= dmg;
        if (hp <= 0) {
            hp = 0;
            incapacitated = true; // Knocked out but can be revived by medic
            alive = false;         // Remove from active play until revived
        }
    }

    void revive(int healAmount) {
        if (!incapacitated) return; // Only knocked-out agents can be revived
        hp = healAmount;
        if (hp > 0) {
            incapacitated = false;
            alive = true;
        }
    }
};

struct Warrior : Agent {
    std::vector<IVec2> path;
    int pathIndex{ 0 };

    Warrior(Team t, IVec2 p)
        : Agent(t, p, 'W', "Warrior") {
        ammo = 20;
        grenades = 2;
    }

    Perception look(const Grid& g,
        const std::vector<IVec2>& enemySpots) const;
};

struct Medic : Agent {
    enum class State { Idle, GoingToDepot, GoingToPatient, Healing };
    State state{ State::Idle };
    IVec2 targetPatient{ -1, -1 };

    Medic(Team t, IVec2 p) : Agent(t, p, 'M', "Medic") {}
};

struct Porter : Agent {
    Porter(Team t, IVec2 p) : Agent(t, p, 'P', "Porter") {}
};

struct Commander : Agent {
    std::vector<IVec2> visibilityMap;
    
    Commander(Team t, IVec2 p) : Agent(t, p, 'C', "Commander") {
        ammo = 0;      // Commander cannot attack per requirements
        grenades = 0;
    }
};
