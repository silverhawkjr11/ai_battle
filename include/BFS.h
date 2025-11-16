#pragma once
#include "Types.h" 
#include "Grid.h"
#include <vector> 
#include <optional>
std::optional<IVec2> bfsFindSafe(const Grid& g, IVec2 start, const std::vector<float>& risk, float maxRisk, int radius);
