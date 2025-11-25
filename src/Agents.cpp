// Agents.cpp
#include "Agents.h"
#include "Visibility.h"

Perception Warrior::look(const Grid& g,
	const std::vector<IVec2>& enemySpots) const
{
	Perception p;

	// Priority 1: Look for closest enemy with line of sight
	int closestDist = 9999;
	
	for (auto e : enemySpots)
	{
		if (los(g, pos, e))
		{
			int dist = pos.manhattan(e);
			if (dist < closestDist)
			{
				closestDist = dist;
				p.seesEnemy = true;
				p.enemyPos = e;
			}
		}
	}

	return p;
}
