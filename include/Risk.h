#pragma once
#include "Types.h"
#include "Grid.h"
#include <vector>
std::vector<float> makeRisk(const Grid& g, const std::vector<IVec2>& enemyHints, float base=0.1f);
inline float riskAt(const std::vector<float>& r, const Grid& g, IVec2 p){ return r[p.y*g.w+p.x]; }
