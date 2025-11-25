// Game.cpp - Main game logic and simulation
// Implements:
// - Combat system (shooting and grenades)
// - Team coordination via CommanderAI
// - Win condition checking
// - Agent visibility and perception

#include "Game.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <Visibility.h>

namespace
{
    int manhattan(IVec2 a, IVec2 b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

    constexpr int FIRE_RANGE = 6;      // Gun range: 6 tiles
    constexpr int GRENADE_RANGE = 10;  // Grenade range: longer than guns for suppression
    constexpr int FIRE_DAMAGE = 20;    // Bullet damage: 20 (5 shots to kill)
    constexpr int GRENADE_DAMAGE = 15; // Grenade damage: 15 (less than bullets, for suppression)

    // שכנים 4 כיוונים
    std::vector<IVec2> neighbors4(const Grid& g, IVec2 p)
    {
        static const IVec2 dirs[4] = {
            {1,0},{-1,0},{0,1},{0,-1}
        };
        std::vector<IVec2> out;
        for (auto d : dirs)
        {
            IVec2 n{ p.x + d.x, p.y + d.y };
            if (g.inBounds(n) && g.passable(n))
                out.push_back(n);
        }
        return out;
    }

    int key(IVec2 p, int w) { return p.y * w + p.x; }

    std::vector<IVec2> reconstructPath(
        const Grid& g,
        IVec2 start, IVec2 goal,
        const std::unordered_map<int, IVec2>& cameFrom)
    {
        std::vector<IVec2> path;
        IVec2 cur = goal;
        path.push_back(cur);

        while (cur != start)
        {
            auto it = cameFrom.find(key(cur, g.w));
            if (it == cameFrom.end())
                break;
            cur = it->second;
            path.push_back(cur);
        }

        std::reverse(path.begin(), path.end());
        return path;
    }

    // A* בסיסי
    std::vector<IVec2> aStar(const Grid& g, IVec2 start, IVec2 goal)
    {
        struct Node
        {
            IVec2 p;
            int   g;
            int   f;
        };

        auto cmp = [](const Node& a, const Node& b) {
            return a.f > b.f;
            };

        std::priority_queue<Node,
            std::vector<Node>, decltype(cmp)> open(cmp);

        std::unordered_map<int, int> gScore;
        std::unordered_map<int, IVec2> cameFrom;

        int ks = key(start, g.w);
        gScore[ks] = 0;
        open.push({ start, 0, manhattan(start, goal) });

        while (!open.empty())
        {
            Node cur = open.top();
            open.pop();

            if (cur.p == goal)
                return reconstructPath(g, start, goal, cameFrom);

            for (auto n : neighbors4(g, cur.p))
            {
                int tent = cur.g + 1;
                int kn = key(n, g.w);

                auto it = gScore.find(kn);
                if (it != gScore.end() && tent >= it->second)
                    continue;

                gScore[kn] = tent;
                cameFrom[kn] = cur.p;
                int f = tent + manhattan(n, goal);
                open.push({ n, tent, f });
            }
        }

        return {};
    }

    // יעד פשוט: המפקד של האויב (אם חי) אחרת לוחם אחר
    IVec2 chooseTarget(const Grid& g,
        const TeamState& self,
        const TeamState& enemy)
    {
        if (enemy.commander.alive)
            return enemy.commander.pos;

        for (auto const& w : enemy.warriors)
            if (w.alive)
                return w.pos;

        return self.commander.pos; // fallback
    }

    // תנועה אוטומטית ללוחמים של קבוצה אחת
    void moveWarriors(const Grid& g,
        TeamState& self, const TeamState& enemy)
    {
        IVec2 target = chooseTarget(g, self, enemy);

        for (auto& w : self.warriors)
        {
            if (!w.alive) continue;

            // אם אין נתיב – תכנן חדש
            if (w.path.empty())
            {
                if (w.pos != target)
                {
                    w.path = aStar(g, w.pos, target);
                    w.pathIndex = 0;
                }
            }

            // בצע צעד אחד בנתיב
            if (!w.path.empty() && w.pathIndex + 1 < (int)w.path.size())
            {
                IVec2 next = w.path[w.pathIndex + 1];
                if (g.passable(next))
                {
                    w.pos = next;
                    w.pathIndex++;
                }
            }
        }
    }
}

Game::Game(const Grid& g)
    : grid(g)
    , blue(Team::Blue, g,
        IVec2{ 2, 2 },
        IVec2{ 3, 3 }, IVec2{ 3, 5 },
        IVec2{ 2, 4 }, IVec2{ 2, 6 })
    , orange(Team::Orange, g,
        IVec2{ g.w - 3, g.h - 3 },
        IVec2{ g.w - 4, g.h - 4 },
        IVec2{ g.w - 4, g.h - 6 },
        IVec2{ g.w - 3, g.h - 4 },
        IVec2{ g.w - 3, g.h - 6 })
{}

std::vector<IVec2> Game::enemySpots(Team t) const
{
    std::vector<IVec2> v;
    auto const& en = (t == Team::Blue ? orange : blue);

    if (en.commander.alive) v.push_back(en.commander.pos);
    if (en.medic.alive)     v.push_back(en.medic.pos);
    if (en.porter.alive)    v.push_back(en.porter.pos);
    for (auto const& w : en.warriors)
        if (w.alive) v.push_back(w.pos);

    return v;
}

Agent* Game::findAgentAt(Team t, IVec2 p)
{
    TeamState& ts = (t == Team::Blue ? blue : orange);

    if (ts.commander.alive && ts.commander.pos == p) return &ts.commander;
    if (ts.medic.alive && ts.medic.pos == p) return &ts.medic;
    if (ts.porter.alive && ts.porter.pos == p) return &ts.porter;
    for (auto& w : ts.warriors)
        if (w.alive && w.pos == p) return &w;

    return nullptr;
}

void Game::step()
{
    bullets.update(grid);

    if (tick % 500 == 0) {  // Print every 500 ticks
        std::cout << "\n=== TICK " << tick << " ===\n";
        
        int blueAlive = 0, orangeAlive = 0;
        for (auto& w : blue.warriors) if (w.alive) blueAlive++;
        for (auto& w : orange.warriors) if (w.alive) orangeAlive++;
        
        std::cout << "Blue: " << blueAlive << " warriors | Orange: " << orangeAlive << " warriors\n";
    }

    if (tick > 5000) {
        std::cout << "\n⏱️ Game TIMEOUT! Draw.\n";
        running = false;
        return;
    }

    // --- תנועה אוטומטית (A*) ---
    //moveWarriors(grid, blue, orange);
    //moveWarriors(grid, orange, blue);

    auto spotsForBlue = enemySpots(Team::Blue);
    auto spotsForOrange = enemySpots(Team::Orange);
    CommanderAI::step(grid, blue.commander, blue.warriors,
        blue.medic, blue.porter, spotsForBlue, tick);

    CommanderAI::step(grid, orange.commander, orange.warriors,
        orange.medic, orange.porter, spotsForOrange, tick);

    //---------------------------------------------
 //    GRENADE UPDATE + EXPLOSION DAMAGE
 //---------------------------------------------
    grenades.updateAndExplode([&](float gx, float gy, float radius)
        {
            auto hit = [&](Agent& A)
                {
                    if (!A.alive || A.hp <= 0) return;  // Don't damage dead agents

                    float dx = A.pos.x - gx;
                    float dy = A.pos.y - gy;
                    if (dx * dx + dy * dy <= radius * radius)
                        A.takeDamage(GRENADE_DAMAGE);
                };

            // פגיעה בכחולים
            hit(blue.commander);
            hit(blue.medic);
            hit(blue.porter);
            for (auto& w : blue.warriors) hit(w);

            // פגיעה בכתומים
            hit(orange.commander);
            hit(orange.medic);
            hit(orange.porter);
            for (auto& w : orange.warriors) hit(w);

            std::cout << "💥 GRENADE exploded at " << gx << "," << gy << "\n";
        });



    // ----------------- Blue team combat -----------------
    int blueShotsThisTurn = 0;
    for (auto& w : blue.warriors)
    {
        if (!w.alive) continue;

        Perception per = w.look(grid, spotsForBlue);
        
        if (per.seesEnemy)
        {
            IVec2 targetPos = *per.enemyPos;
            int   dist = manhattan(w.pos, targetPos);

            // Priority 1: Shoot if in gun range and have ammo
            if (dist <= FIRE_RANGE && w.ammo > 0)
            {
                w.ammo--;
                bullets.addBullet(
                    w.pos.x + 0.5f, w.pos.y + 0.5f,
                    targetPos.x + 0.5f, targetPos.y + 0.5f);

                Agent* victim = findAgentAt(Team::Orange, targetPos);
                if (victim && victim->hp > 0)  // Don't shoot corpses!
                {
                    victim->takeDamage(FIRE_DAMAGE);
                    blueShotsThisTurn++;
                    std::cout << "💥 Blue shot Orange " << victim->role << " (HP:" << victim->hp << ")\n";
                }
            }
            // Priority 2: Grenade if out of gun range but within grenade range
            else if (dist > FIRE_RANGE && dist <= GRENADE_RANGE && w.grenades > 0)
            {
                w.grenades--;
                grenades.addGrenade(
                    w.pos.x + 0.5f, w.pos.y + 0.5f,
                    targetPos.x + 0.5f, targetPos.y + 0.5f);
                blueShotsThisTurn++;
                std::cout << "💣 Blue threw grenade (dist=" << dist << ")!\n";
            }
        }
    }

    // ----------------- Orange team combat -----------------
    int orangeShotsThisTurn = 0;
    for (auto& w : orange.warriors)
    {
        if (!w.alive) continue;

        Perception per = w.look(grid, spotsForOrange);
        
        if (per.seesEnemy)
        {
            IVec2 targetPos = *per.enemyPos;
            int   dist = manhattan(w.pos, targetPos);

            // Priority 1: Shoot if in gun range and have ammo
            if (dist <= FIRE_RANGE && w.ammo > 0)
            {
                w.ammo--;
                bullets.addBullet(
                    w.pos.x + 0.5f, w.pos.y + 0.5f,
                    targetPos.x + 0.5f, targetPos.y + 0.5f);

                Agent* victim = findAgentAt(Team::Blue, targetPos);
                if (victim && victim->hp > 0)  // Don't shoot corpses!
                {
                    victim->takeDamage(FIRE_DAMAGE);
                    orangeShotsThisTurn++;
                    std::cout << "💥 Orange shot Blue " << victim->role << " (HP:" << victim->hp << ")\n";
                }
            }
            // Priority 2: Grenade if out of gun range but within grenade range
            else if (dist > FIRE_RANGE && dist <= GRENADE_RANGE && w.grenades > 0)
            {
                w.grenades--;
                grenades.addGrenade(
                    w.pos.x + 0.5f, w.pos.y + 0.5f,
                    targetPos.x + 0.5f, targetPos.y + 0.5f);
                orangeShotsThisTurn++;
                std::cout << "💣 Orange threw grenade (dist=" << dist << ")!\n";
            }
        }
    }
    
    if (tick % 100 == 0 && (blueShotsThisTurn == 0 && orangeShotsThisTurn == 0)) {
        std::cout << "⚠️ No combat this cycle\n";
    }
    // NOTE: Commanders CANNOT attack per project requirements
    // They can only issue orders and move to safer positions
    if (tick % 50 == 0) {  // Every 50 ticks
        int blueAlive = 0, orangeAlive = 0;
        for (auto& w : blue.warriors) if (w.alive) blueAlive++;
        for (auto& w : orange.warriors) if (w.alive) orangeAlive++;

        std::cout << "Blue: " << blueAlive << " alive | Orange: " << orangeAlive << " alive\n";
    }

    // Count warriors only (not commander)
    int blueWarriors = 0, orangeWarriors = 0;
    for (auto& w : blue.warriors)
        if (w.alive) blueWarriors++;
    for (auto& w : orange.warriors)
        if (w.alive) orangeWarriors++;

    // Check win conditions:
    // 1. Commander death = immediate loss
    // 2. Stalemate detection: if nothing changes for 500 ticks
    static int lastBlueWarriors = blueWarriors;
    static int lastOrangeWarriors = orangeWarriors;
    static int lastBlueHP = 0, lastOrangeHP = 0;
    static int stalemateTicks = 0;
    
    // Calculate total HP for both teams
    int currentBlueHP = 0, currentOrangeHP = 0;
    for (auto& w : blue.warriors) if (w.alive) currentBlueHP += w.hp;
    for (auto& w : orange.warriors) if (w.alive) currentOrangeHP += w.hp;
    
    // Check if anything changed
    if (blueWarriors == lastBlueWarriors && orangeWarriors == lastOrangeWarriors &&
        currentBlueHP == lastBlueHP && currentOrangeHP == lastOrangeHP) {
        stalemateTicks++;
    } else {
        stalemateTicks = 0;
        lastBlueWarriors = blueWarriors;
        lastOrangeWarriors = orangeWarriors;
        lastBlueHP = currentBlueHP;
        lastOrangeHP = currentOrangeHP;
    }
    
    // Check win conditions
    if (!blue.commander.alive) {
        std::cout << "\n🏆🏆🏆 ORANGE TEAM WINS! 🏆🏆🏆\n";
        std::cout << "Blue Commander eliminated!\n";
        running = false;
    }
    else if (!orange.commander.alive) {
        std::cout << "\n🏆🏆🏆 BLUE TEAM WINS! 🏆🏆🏆\n";
        std::cout << "Orange Commander eliminated!\n";
        std::cout << "Game over - stopping timer\n";
        running = false;
    }
    else if (stalemateTicks >= 500) {
        // Stalemate: count surviving warriors and HP
        std::cout << "\n⚖️ STALEMATE DETECTED (no changes for 500 ticks) ⚖️\n";
        if (blueWarriors > orangeWarriors) {
            std::cout << "\n🏆🏆🏆 BLUE TEAM WINS! 🏆🏆🏆\n";
            std::cout << "Blue has more warriors (" << blueWarriors << " vs " << orangeWarriors << ")\n";
        }
        else if (orangeWarriors > blueWarriors) {
            std::cout << "\n🏆🏆🏆 ORANGE TEAM WINS! 🏆🏆🏆\n";
            std::cout << "Orange has more warriors (" << orangeWarriors << " vs " << blueWarriors << ")\n";
        }
        else if (currentBlueHP > currentOrangeHP) {
            std::cout << "\n🏆🏆🏆 BLUE TEAM WINS! 🏆🏆🏆\n";
            std::cout << "Blue has more total HP (" << currentBlueHP << " vs " << currentOrangeHP << ")\n";
        }
        else if (currentOrangeHP > currentBlueHP) {
            std::cout << "\n🏆🏆🏆 ORANGE TEAM WINS! 🏆🏆🏆\n";
            std::cout << "Orange has more total HP (" << currentOrangeHP << " vs " << currentBlueHP << ")\n";
        }
        else {
            std::cout << "\n🤝 DRAW! 🤝\n";
            std::cout << "Both teams equal: " << blueWarriors << " warriors, " << currentBlueHP << " HP\n";
        }
        running = false;
    }
    else if (tick >= 5000) {
        // Absolute timeout failsafe
        std::cout << "\n⏰ ABSOLUTE TIMEOUT (5000 ticks) ⏰\n";
        if (blueWarriors > orangeWarriors) {
            std::cout << "\n🏆🏆🏆 BLUE TEAM WINS! 🏆🏆🏆\n";
            std::cout << "Timeout: Blue has more warriors (" << blueWarriors << " vs " << orangeWarriors << ")\n";
        }
        else if (orangeWarriors > blueWarriors) {
            std::cout << "\n🏆🏆🏆 ORANGE TEAM WINS! 🏆🏆🏆\n";
            std::cout << "Timeout: Orange has more warriors (" << orangeWarriors << " vs " << blueWarriors << ")\n";
        }
        else {
            std::cout << "\n🤝 DRAW! 🤝\n";
            std::cout << "Timeout: Both teams have " << blueWarriors << " warriors\n";
        }
        running = false;
    }

    tick++;
}
