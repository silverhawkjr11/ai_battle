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

    // Base damage handling: non-warriors are removed from play when HP drops to 0.
    // Warriors override this to remain on the field (incapacitated but still "alive" for rendering/medic).
    virtual void takeDamage(int dmg) {
        if (!alive) return;
        hp -= dmg;
        if (hp <= 0) {
            hp = 0;
            incapacitated = true;
            alive = false; // default behavior for non-revivable agents
        }
    }

    virtual void revive(int healAmount) {
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
    int reviveCount{ 0 };          // NEW: track number of revives
    int resupplyCount{ 0 };        // NEW: track number of resupplies

    Warrior(Team t, IVec2 p)
        : Agent(t, p, 'W', "Warrior") {
        ammo = 20;
        grenades = 2;
    }

    // Override: keep warrior on field when incapacitated so medic can revive.
    void takeDamage(int dmg) override {
        if (!alive && !incapacitated) return;
        hp -= dmg;
        if (hp <= 0) {
            hp = 0;
            incapacitated = true;
            // stay "alive" visually
        }
    }

    void revive(int healAmount) override {
        if (!incapacitated) return;
        if (reviveCount >= kMaxWarriorRevives) {
            // Exceeded revive limit: convert to permanently dead
            alive = false;
            incapacitated = false;
            hp = 0;
            return;
        }
        reviveCount++;
        hp = healAmount;
        if (hp > 0) {
            incapacitated = false;
            alive = true;
        }
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
