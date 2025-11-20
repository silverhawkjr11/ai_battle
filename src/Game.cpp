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

    constexpr int FIRE_RANGE = 6;
    constexpr int GRENADE_RANGE = 9;
    constexpr int FIRE_DAMAGE = 20;
    constexpr int GRENADE_DAMAGE = 40;

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

    if (tick % 100 == 0) {  // Print every 100 ticks
        std::cout << "=== TICK " << tick << " ===\n";
    }

    if (tick > 5000) {
        std::cout << "Game timeout!\n";
        running = false;
        return;
    }

    // --- תנועה אוטומטית (A*) ---
    //moveWarriors(grid, blue, orange);
    //moveWarriors(grid, orange, blue);

    auto spotsForBlue = enemySpots(Team::Blue);
    auto spotsForOrange = enemySpots(Team::Orange);
    CommanderAI::step(grid, blue.commander, blue.warriors,
        blue.medic, blue.porter, spotsForBlue);

    CommanderAI::step(grid, orange.commander, orange.warriors,
        orange.medic, orange.porter, spotsForOrange);

    //---------------------------------------------
 //    GRENADE UPDATE + EXPLOSION DAMAGE
 //---------------------------------------------
    grenades.updateAndExplode([&](float gx, float gy, float radius)
        {
            auto hit = [&](Agent& A)
                {
                    if (!A.alive) return;

                    float dx = A.pos.x - gx;
                    float dy = A.pos.y - gy;
                    if (dx * dx + dy * dy <= radius * radius)
                        A.takeDamage(40);
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



    // ----------------- ירי של כחולים -----------------
    for (auto& w : blue.warriors)
    {
        if (!w.alive) continue;

        Perception per = w.look(grid, spotsForBlue);
        if (per.seesEnemy)
        {
            IVec2 targetPos = *per.enemyPos;
            int   dist = manhattan(w.pos, targetPos);

            if (dist <= FIRE_RANGE && w.ammo > 0)
            {
                w.ammo--;
                bullets.addBullet(
                    w.pos.x + 0.5f, w.pos.y + 0.5f,
                    targetPos.x + 0.5f, targetPos.y + 0.5f);

                Agent* victim = findAgentAt(Team::Orange, targetPos);
                if (victim)
                {
                    victim->takeDamage(FIRE_DAMAGE);
                    std::cout << "💥 Blue warrior hit Orange "
                        << victim->role << " hp=" << victim->hp << "\n";
                }
            }
            else if (dist <= GRENADE_RANGE && w.grenades > 0)
            {
                w.grenades--;
                Agent* victim = findAgentAt(Team::Orange, targetPos);
                if (victim)
                {
                    victim->takeDamage(GRENADE_DAMAGE);
                    std::cout << "💥💥 Blue grenade on Orange "
                        << victim->role << " hp=" << victim->hp << "\n";
                }
            }
        }
    }

    // ----------------- ירי של כתומים -----------------
    for (auto& w : orange.warriors)
    {
        if (!w.alive) continue;

        Perception per = w.look(grid, spotsForOrange);
        if (per.seesEnemy)
        {
            IVec2 targetPos = *per.enemyPos;
            int   dist = manhattan(w.pos, targetPos);

            if (dist <= FIRE_RANGE && w.ammo > 0)
            {
                w.ammo--;
                bullets.addBullet(
                    w.pos.x + 0.5f, w.pos.y + 0.5f,
                    targetPos.x + 0.5f, targetPos.y + 0.5f);

                Agent* victim = findAgentAt(Team::Blue, targetPos);
                if (victim)
                {
                    victim->takeDamage(FIRE_DAMAGE);
                    std::cout << "💥 Orange warrior hit Blue "
                        << victim->role << " hp=" << victim->hp << "\n";
                }
            }
            else if (dist <= GRENADE_RANGE && w.grenades > 0)
            {
                w.grenades--;
                Agent* victim = findAgentAt(Team::Blue, targetPos);
                if (victim)
                {
                    victim->takeDamage(GRENADE_DAMAGE);
                    std::cout << "💥💥 Orange grenade on Blue "
                        << victim->role << " hp=" << victim->hp << "\n";
                }
            }
        }
    }
    // ----------------- Commander shooting (when warriors are dead) -----------------

// Blue commander combat
    if (blue.commander.alive) {
        int blueWarriorsAlive = 0;
        for (auto& w : blue.warriors) if (w.alive) blueWarriorsAlive++;

        if (blueWarriorsAlive == 0) {
            Perception per;
            // Commander uses same vision as warrior
            for (auto e : spotsForBlue) {
                if (los(grid, blue.commander.pos, e)) {
                    per.seesEnemy = true;
                    per.enemyPos = e;
                    break;
                }
            }

            if (per.seesEnemy && blue.commander.ammo > 0) {
                IVec2 targetPos = *per.enemyPos;
                int dist = manhattan(blue.commander.pos, targetPos);

                if (dist <= FIRE_RANGE) {
                    blue.commander.ammo--;
                    bullets.addBullet(
                        blue.commander.pos.x + 0.5f, blue.commander.pos.y + 0.5f,
                        targetPos.x + 0.5f, targetPos.y + 0.5f);

                    Agent* victim = findAgentAt(Team::Orange, targetPos);
                    if (victim) {
                        victim->takeDamage(FIRE_DAMAGE);
                        std::cout << "💥 Blue COMMANDER hit Orange "
                            << victim->role << " hp=" << victim->hp << "\n";
                    }
                }
            }
        }
    }

    // Orange commander combat
    if (orange.commander.alive) {
        int orangeWarriorsAlive = 0;
        for (auto& w : orange.warriors) if (w.alive) orangeWarriorsAlive++;

        if (orangeWarriorsAlive == 0) {
            Perception per;
            for (auto e : spotsForOrange) {
                if (los(grid, orange.commander.pos, e)) {
                    per.seesEnemy = true;
                    per.enemyPos = e;
                    break;
                }
            }

            if (per.seesEnemy && orange.commander.ammo > 0) {
                IVec2 targetPos = *per.enemyPos;
                int dist = manhattan(orange.commander.pos, targetPos);

                if (dist <= FIRE_RANGE) {
                    orange.commander.ammo--;
                    bullets.addBullet(
                        orange.commander.pos.x + 0.5f, orange.commander.pos.y + 0.5f,
                        targetPos.x + 0.5f, targetPos.y + 0.5f);

                    Agent* victim = findAgentAt(Team::Blue, targetPos);
                    if (victim) {
                        victim->takeDamage(FIRE_DAMAGE);
                        std::cout << "💥 Orange COMMANDER hit Blue "
                            << victim->role << " hp=" << victim->hp << "\n";
                    }
                }
            }
        }
    }
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

    // Check if commanders are alive - COMMANDER DEATH = GAME OVER
    if (!blue.commander.alive) {
        std::cout << "\n🏆🏆🏆 ORANGE TEAM WINS! 🏆🏆🏆\n";
        std::cout << "Blue Commander eliminated!\n";
        running = false;
    }
    else if (!orange.commander.alive) {
        std::cout << "\n🏆🏆🏆 BLUE TEAM WINS! 🏆🏆🏆\n";
        std::cout << "Orange Commander eliminated!\n";
        running = false;
    }

    tick++;
}
