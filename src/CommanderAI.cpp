#include "CommanderAI.h"
#include <algorithm>

void CommanderAI::step(const Grid& g,
    Commander& c,
    std::vector<Warrior>& warriors,
    Medic& med,
    Porter& port,
    const std::vector<IVec2>& enemySpots)
{
    auto risk = makeRisk(g, enemySpots, 0.05f);

    // 1. ריפוי – אם יש לוחם מתחת ל־40 HP שולחים חובש
    for (auto& w : warriors)
    {
        if (!w.alive) continue;
        if (w.hp < 40 && med.alive)
        {
            IVec2 depot = (c.team == Team::Blue ? g.blueMed : g.orangeMed);

            // קודם להגיע למחסן תרופות, אחר-כך ללוחם
            if (med.pos != depot)
            {
                auto path = aStarPath(g, med.pos, depot, risk, 0.3f);
                if (path.size() > 1) med.pos = path[1];
                return;
            }
            else if (med.pos != w.pos)
            {
                auto path = aStarPath(g, med.pos, w.pos, risk, 0.3f);
                if (path.size() > 1) med.pos = path[1];
                else med.pos = w.pos;

                if (med.pos == w.pos)
                    w.hp = 100; // ריפוי מלא

                return;
            }
        }
    }

    // 2. הספקה – אם ללוחם יש פחות מ־5 כדורים, שולחים חייל הספקה
    for (auto& w : warriors)
    {
        if (!w.alive) continue;
        if (w.ammo < 5 && port.alive)
        {
            IVec2 depot = (c.team == Team::Blue ? g.blueAmmo : g.orangeAmmo);

            if (port.pos != depot)
            {
                auto path = aStarPath(g, port.pos, depot, risk, 0.3f);
                if (path.size() > 1) port.pos = path[1];
                return;
            }
            else if (port.pos != w.pos)
            {
                auto path = aStarPath(g, port.pos, w.pos, risk, 0.3f);
                if (path.size() > 1) port.pos = path[1];

                if (port.pos == w.pos)
                    w.ammo += 10;

                return;
            }
        }
    }

    // 3. הגנה – לחפש משבצת בעלת סיכון נמוך יותר (BFS + A*)
    for (auto& w : warriors)
    {
        if (!w.alive) continue;

        auto safeOpt = bfsFindSafe(g, w.pos, risk, /*maxRisk*/0.35f, /*radius*/6);
        if (safeOpt && *safeOpt != w.pos)
        {
            auto path = aStarPath(g, w.pos, *safeOpt, risk, 0.1f);
            if (path.size() > 1)
                w.pos = path[1];

            return;
        }
    }

    // 4. התקדמות – לשפר מיקום לכיוון מרכז המפה
    IVec2 center{ g.w / 2, g.h / 2 };
    for (auto& w : warriors)
    {
        if (!w.alive) continue;
        auto path = aStarPath(g, w.pos, center, risk, 0.05f);
        if (path.size() > 1)
            w.pos = path[1];
    }
}
