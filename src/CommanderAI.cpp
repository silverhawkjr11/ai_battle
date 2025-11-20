#include "CommanderAI.h"
#include <algorithm>
#include <iostream>

void CommanderAI::step(const Grid& g,
    Commander& c,
    std::vector<Warrior>& warriors,
    Medic& med,
    Porter& port,
    const std::vector<IVec2>& enemySpots)
{
    if (!c.alive) return;

    auto risk = makeRisk(g, enemySpots, 0.05f);

    // ========================================
    // 1. HEALING - Check ALL warriors
    // ========================================

    // If medic is already busy, skip
    if (med.alive && med.state == Medic::State::Idle)
    {
        // Find most injured warrior
        Warrior* targetWarrior = nullptr;
        int lowestHP = 40;

        for (auto& w : warriors)
        {
            if (!w.alive) continue;
            if (w.hp < lowestHP) {
                lowestHP = w.hp;
                targetWarrior = &w;
            }
        }

        // Start healing mission
        if (targetWarrior)
        {
            med.state = Medic::State::GoingToDepot;
            med.targetPatient = targetWarrior->pos;
            std::cout << "[MEDIC] Starting healing mission!\n";
        }
    }

    // Execute current medic mission
    if (med.alive && med.state != Medic::State::Idle)
    {
        IVec2 depot = (c.team == Team::Blue ? g.blueMed : g.orangeMed);

        switch (med.state)
        {
        case Medic::State::GoingToDepot:
            if (med.pos == depot)
            {
                med.state = Medic::State::GoingToPatient;
                std::cout << "[MEDIC] Reached depot, now going to patient\n";
            }
            else
            {
                auto path = aStarPath(g, med.pos, depot, risk, 0.3f);
                if (path.size() > 1) {
                    med.pos = path[1];
                }
            }
            break;

        case Medic::State::GoingToPatient:
        {
            // Find the warrior we're trying to heal
            Warrior* patient = nullptr;
            for (auto& w : warriors)
            {
                if (w.pos == med.targetPatient) {
                    patient = &w;
                    break;
                }
            }

            if (!patient || !patient->alive)
            {
                // Patient died or moved, abort mission
                med.state = Medic::State::Idle;
                std::cout << "[MEDIC] Patient lost, returning to idle\n";
                break;
            }

            int dist = med.pos.manhattan(patient->pos);
            if (dist <= 1)
            {
                // Start healing
                med.state = Medic::State::Healing;
                patient->hp = 100;
                std::cout << "[MEDIC] Healed warrior to HP=100!\n";
            }
            else
            {
                auto path = aStarPath(g, med.pos, patient->pos, risk, 0.3f);
                if (path.size() > 1) {
                    med.pos = path[1];
                }
            }
            break;
        }

        case Medic::State::Healing:
            // Healing complete, go idle
            med.state = Medic::State::Idle;
            std::cout << "[MEDIC] Mission complete, going idle\n";
            break;
        }
    }

    // ========================================
    // 2. RESUPPLY - Check ALL warriors
    // ========================================
    bool porterBusy = false;

    for (auto& w : warriors)
    {
        if (!w.alive || porterBusy) continue;

        if (w.ammo < 5 && port.alive)
        {
            IVec2 depot = (c.team == Team::Blue ? g.blueAmmo : g.orangeAmmo);

            // Go to depot first
            if (port.pos != depot)
            {
                auto path = aStarPath(g, port.pos, depot, risk, 0.3f);
                if (path.size() > 1) {
                    port.pos = path[1];
                    porterBusy = true;
                    std::cout << "📦 Porter (" << teamName(c.team)
                        << ") moving to depot\n";
                }
            }
            // Then go to warrior
            else if (port.pos != w.pos)
            {
                auto path = aStarPath(g, port.pos, w.pos, risk, 0.3f);
                if (path.size() > 1) {
                    port.pos = path[1];
                    porterBusy = true;
                    std::cout << "📦 Porter (" << teamName(c.team)
                        << ") moving to warrior\n";
                }

                // Resupply when adjacent or same position
                if (port.pos.manhattan(w.pos) <= 1)
                {
                    w.ammo += 15;
                    w.grenades += 2;
                    porterBusy = true;
                    std::cout << "🔫 Porter resupplied warrior: Ammo="
                        << w.ammo << ", Grenades=" << w.grenades << "\n";
                }
            }
            break; // Only resupply one warrior at a time
        }
    }

    // ========================================
    // 3. DEFEND - Warriors find cover
    // ========================================
    for (auto& w : warriors)
    {
        if (!w.alive) continue;

        // Only defend if BOTH low HP AND high risk
        float currentRisk = riskAt(risk, g, w.pos);
        bool shouldDefend = (w.hp < 25) && (currentRisk > 0.6f);  // Changed from 35 HP and 0.5 risk

        if (shouldDefend)
        {
            auto safeOpt = bfsFindSafe(g, w.pos, risk, 0.35f, 8);

            if (safeOpt && *safeOpt != w.pos)
            {
                auto path = aStarPath(g, w.pos, *safeOpt, risk, 0.7f);
                if (path.size() > 1)
                {
                    w.pos = path[1];
                    std::cout << "Shield Warrior moving to cover\n";
                }
            }
        }
    }

    // ========================================
    // 4. ADVANCE - Move toward enemies
    // ========================================
    if (!enemySpots.empty())
    {
        for (auto& w : warriors)
        {
            if (!w.alive) continue;

            // Find closest enemy to this warrior
            IVec2 closestEnemy = enemySpots[0];
            int minDist = 9999;

            for (auto enemy : enemySpots) {
                int dist = w.pos.manhattan(enemy);
                if (dist < minDist) {
                    minDist = dist;
                    closestEnemy = enemy;
                }
            }

            // Advance if healthy OR close to enemy
            float currentRisk = riskAt(risk, g, w.pos);
            bool shouldAdvance = (w.hp > 30) || (minDist <= 8);  // More aggressive

            if (shouldAdvance && minDist > 2)  // Don't walk into enemy
            {
                auto path = aStarPath(g, w.pos, closestEnemy, risk, 0.2f);  // Less risk-averse
                if (path.size() > 1)
                {
                    w.pos = path[1];
                    std::cout << "Sword Warrior advancing\n";
                }
            }
        }
    }
    // ========================================
// 5. COMMANDER COMBAT (Last Resort)
// ========================================

// Count alive warriors
    int aliveWarriors = 0;
    for (auto& w : warriors)
        if (w.alive) aliveWarriors++;

    // If all warriors are dead, commander fights!
    if (aliveWarriors == 0 && !enemySpots.empty()) {
        // Find closest enemy
        IVec2 closestEnemy = enemySpots[0];
        int minDist = 9999;

        for (auto enemy : enemySpots) {
            int dist = c.pos.manhattan(enemy);
            if (dist < minDist) {
                minDist = dist;
                closestEnemy = enemy;
            }
        }

        // Commander advances toward enemy
        if (minDist > 2) {
            auto path = aStarPath(g, c.pos, closestEnemy, risk, 0.5f);
            if (path.size() > 1) {
                c.pos = path[1];
                std::cout << "[COMMANDER] All warriors dead - engaging enemy!\n";
            }
        }
    }
}