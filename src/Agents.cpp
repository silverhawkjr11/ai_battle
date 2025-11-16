// Agents.cpp
#include "Agents.h"
#include "Visibility.h"

Perception Warrior::look(const Grid& g,
    const std::vector<IVec2>& enemySpots) const
{
    Perception p;

    for (auto e : enemySpots)
    {
        // אם יש קו ראייה
        if (los(g, pos, e))
        {
            p.seesEnemy = true;
            p.enemyPos = e;
            return p;
        }
    }

    return p;
}
