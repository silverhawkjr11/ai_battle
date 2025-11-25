// CommanderAI.cpp - AI commander decision making
// Implements:
// - Healing coordination (medic dispatch)
// - Resupply coordination (porter dispatch)  
// - Tactical positioning (defend/advance)
// - Commander safety management
// - Visibility map construction

#include "CommanderAI.h"
#include "Visibility.h"
#include <algorithm>
#include <iostream>

void CommanderAI::step(const Grid& g,
    Commander& c,
    std::vector<Warrior>& warriors,
    Medic& med,
    Porter& port,
    const std::vector<IVec2>& enemySpots,
    int tick)
{
    if (!c.alive) return;

    auto risk = makeRisk(g, enemySpots, 0.05f);

    // ========================================
    // 1. HEALING - Check ALL warriors
    // ========================================

    // If medic is already busy, skip
    if (med.alive && med.state == Medic::State::Idle)
    {
        // Find most injured warrior that needs healing (including incapacitated ones at 0 HP)
        Warrior* targetWarrior = nullptr;
        int lowestHP = kMedCallHP;  // Only heal if below 60 HP

        for (auto& w : warriors)
        {
            // Check incapacitated warriors first (HP = 0) - PRIORITY!
            if (w.incapacitated) {
                targetWarrior = &w;
                lowestHP = 0;
                break; // Reviving is top priority
            }

            // Otherwise check injured warriors
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
            std::cout << "[MEDIC] " << teamName(c.team) << " dispatching medic (warrior HP=" << lowestHP << ")\n";
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
            // Find the warrior that needs healing (closest injured one)
            Warrior* patient = nullptr;
            int minDist = 9999;
            
            for (auto& w : warriors)
            {
                // Prioritize incapacitated warriors even though alive=false
                if (w.incapacitated) {
                    int dist = med.pos.manhattan(w.pos);
                    if (dist < minDist) {
                        minDist = dist;
                        patient = &w;
                    }
                    continue;
                }

                if (!w.alive || w.hp >= 60) continue;

                int dist = med.pos.manhattan(w.pos);
                if (dist < minDist) {
                    minDist = dist;
                    patient = &w;
                }
            }

            if (!patient)
            {
                // No injured warriors found, abort mission
                med.state = Medic::State::Idle;
                break;
            }

            int dist = med.pos.manhattan(patient->pos);
            if (dist <= 1)
            {
                // Start healing
                med.state = Medic::State::Healing;
                if (patient->incapacitated) {
                    // Revive incapacitated warrior
                    patient->revive(100);
                    std::cout << "[MEDIC] REVIVED " << teamName(c.team) << " warrior from 0 HP to 100 HP!\n";
                } else {
                    // Regular healing
                    patient->hp = 100;
                    std::cout << "[MEDIC] Healed " << teamName(c.team) << " warrior to HP=100!\n";
                }
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
            break;
        }
    }

    // ========================================
    // 2. RESUPPLY - Check ALL warriors  
    // ========================================
    bool porterBusy = false;

    // First pass: Find warriors that are COMPLETELY out of ammo (priority)
    Warrior* urgentWarrior = nullptr;
    for (auto& w : warriors)
    {
        if (!w.alive || w.incapacitated) continue;
        if ((w.ammo == 0 && w.grenades == 0) && (tick - w.lastResupplyTick) >= kPorterCooldown) {
            urgentWarrior = &w;
            break; // Found urgent case
        }
    }

    // Second pass: If no urgent case, check for low ammo
    if (!urgentWarrior) {
        for (auto& w : warriors)
        {
            if (!w.alive || w.incapacitated) continue;
            bool needsResupply = (w.ammo == 0 || w.grenades == 0) || (w.ammo < kLowAmmo);
            if (needsResupply && (tick - w.lastResupplyTick) >= kPorterCooldown) {
                urgentWarrior = &w;
                break;
            }
        }
    }

    // Execute resupply mission if we found a warrior
    if (urgentWarrior && port.alive)
    {
        IVec2 depot = (c.team == Team::Blue ? g.blueAmmo : g.orangeAmmo);
        int distToDepot = port.pos.manhattan(depot);
        int distToWarrior = port.pos.manhattan(urgentWarrior->pos);

        // If near depot (within 10 tiles), can resupply from long distance (50 tiles)
        if (distToDepot <= 10 && distToWarrior <= 50)
        {
            urgentWarrior->ammo = 20;  // Full resupply
            urgentWarrior->grenades = 2;
            urgentWarrior->lastResupplyTick = tick; // Mark resupply time
            porterBusy = true;
            std::cout << "🔫 Porter resupplied " << teamName(c.team) 
                      << " warrior at tick " << tick << " (next at " << (tick + kPorterCooldown) << ")\n";
        }
        // Move toward depot to get supplies
        else if (distToDepot > 5)
        {
            auto path = aStarPath(g, port.pos, depot, risk, 0.3f);
            if (path.size() > 1) {
                port.pos = path[1];
                porterBusy = true;
            }
        }
        // At depot, move toward warrior
        else
        {
            auto path = aStarPath(g, port.pos, urgentWarrior->pos, risk, 0.3f);
            if (path.size() > 1) {
                port.pos = path[1];
                porterBusy = true;
            }
        }
    }

    // ========================================
    // 3. WARRIOR TACTICAL MOVEMENT
    // ========================================
    // Warriors decide: Defend (if low HP/high risk) OR Advance (if healthy) OR Hold position (in combat range)
    
    static int moveLogCounter = 0;
    moveLogCounter++;
    bool shouldLog = (moveLogCounter % 50 == 0); // Log movement every 50 moves
    
    for (auto& w : warriors)
    {
        if (!w.alive || w.incapacitated) continue; // Skip dead and incapacitated warriors

        float currentRisk = riskAt(risk, g, w.pos);
        
        // Check if we're in combat range of an enemy
        bool inCombatRange = false;
        int closestEnemyDist = 9999;
        IVec2 closestEnemy;

        for (auto enemy : enemySpots) {
            int dist = w.pos.manhattan(enemy);
            if (dist < closestEnemyDist) {
                closestEnemyDist = dist;
                closestEnemy = enemy;
            }
            bool hasLOS = los(g, w.pos, enemy);
            // Only hold position if ACTUALLY within gun range (can shoot)
            if (dist <= kGunRange && hasLOS) {
                inCombatRange = true;
            }
        }
        
        // PRIORITY 1: Defend if critically low HP or extremely high risk
        bool criticalDanger = (w.hp <= 25);  // Only retreat if actually low HP, ignore risk
        
        if (criticalDanger)
        {
            auto safeOpt = bfsFindSafe(g, w.pos, risk, 0.35f, 8);

            if (safeOpt && *safeOpt != w.pos)
            {
                auto path = aStarPath(g, w.pos, *safeOpt, risk, 0.7f);
                if (path.size() > 1)
                {
                    w.pos = path[1];
                }
            }
        }
        
        // PRIORITY 2: Stay and fight if in combat range
        if (inCombatRange && w.ammo > 0 && w.hp > 25)
        {
            // Don't log every tick - too spammy
            continue; // Don't move, stay and shoot
        }
        
        // PRIORITY 3: Advance toward enemies if not in critical danger and not in combat range
        if (!enemySpots.empty())
        {
            // Find home base position
            IVec2 homeBase = (c.team == Team::Blue) ? IVec2{5, 5} : IVec2{74, 44};
            int distFromHome = w.pos.manhattan(homeBase);
            
            // Warriors can advance up to 60 tiles from home (increased for 80-tile map)
            bool tooFarFromHome = (distFromHome > 100);  // Allow 100 tiles from home
            
            // Advance logic:
            // - Keep advancing if far away (>9)
            // - OR if we're out of grenades and not yet in gun range
            // - BUT NOT if totally out of ammo (hold position and wait for resupply)
            bool needToCloseIn = (w.grenades == 0 && closestEnemyDist > kGunRange && w.ammo > 0);
            bool totallyOutOfAmmo = (w.ammo == 0 && w.grenades == 0);
            // Change: Advance if distance > kGrenadeRange (10) OR if out of grenades and not in gun range
            // BUT: Don't advance if totally out of ammo - stay put and wait for resupply
            bool shouldAdvance = (w.hp > 25) && !totallyOutOfAmmo && ((closestEnemyDist > kGrenadeRange) || needToCloseIn);
            
            if (tick % 500 == 0) {
                std::cout << "[MOVE] " << teamName(c.team) << " warrior at (" << w.pos.x << "," << w.pos.y << ")"
                          << " distToEnemy=" << closestEnemyDist 
                          << " HP=" << w.hp 
                          << " Ammo=" << w.ammo
                          << " Grenades=" << w.grenades
                          << " distFromHome=" << distFromHome
                          << " shouldAdvance=" << (shouldAdvance ? "YES" : "NO") << "\n";
            }
            
            if (shouldAdvance)
            {
                auto path = aStarPath(g, w.pos, closestEnemy, risk, 0.3f);
                if (tick % 500 == 0) {
                    std::cout << "  -> Path found: " << (path.size() > 1 ? "YES" : "NO") 
                              << " (size=" << path.size() << ")\n";
                }
                if (path.size() > 1)
                {
                    w.pos = path[1];
                }
            }
        }
    }
    // ========================================
    // 5. COMMANDER SURVIVAL (Retreat to Safety)
    // ========================================
    
    // Commander cannot attack per requirements, only move to safety
    if (!enemySpots.empty()) {
        float commanderRisk = riskAt(risk, g, c.pos);
        
        // If commander is in danger, find safer position
        if (commanderRisk > 0.5f) {
            auto safeOpt = bfsFindSafe(g, c.pos, risk, 0.3f, 10);
            
            if (safeOpt && *safeOpt != c.pos) {
                auto path = aStarPath(g, c.pos, *safeOpt, risk, 0.8f);
                if (path.size() > 1) {
                    c.pos = path[1];
                    std::cout << "[COMMANDER] Moving to safer position!\n";
                }
            }
        }
    }
    
    // ========================================
    // 6. BUILD VISIBILITY MAP
    // ========================================
    
    // Commander combines all soldiers' visibility
    c.visibilityMap.clear();
    for (auto& w : warriors) {
        if (w.alive) {
            c.visibilityMap.push_back(w.pos);
        }
    }
    if (med.alive) c.visibilityMap.push_back(med.pos);
    if (port.alive) c.visibilityMap.push_back(port.pos);
}