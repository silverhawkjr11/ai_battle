#pragma once
#include "Types.h"
#include "Grid.h"
#include <vector>
std::vector<IVec2> aStarPath(const Grid& g, IVec2 start, IVec2 goal, const std::vector<float>& risk, float alpha);
