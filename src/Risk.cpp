// Risk.cpp - Risk map generation for tactical decision making
// Higher risk near enemies, reduced behind cover (trees/rocks)

#include "Risk.h"
#include <cmath> 
#include <algorithm>

std::vector<float> makeRisk(const Grid& g, const std::vector<IVec2>& enemyHints, float base)
{
    std::vector<float> r(g.w * g.h, base);
    
    // Add risk radiating from enemy positions
    for (auto e : enemyHints) {
        for (int y = 0; y < g.h; ++y) {
            for (int x = 0; x < g.w; ++x) {
                float d = std::hypot(float(e.x - x), float(e.y - y));
                
                // Risk falls off with distance
                float add = d < 1.0f ? 1.0f : std::max(0.0f, 1.5f - d * 0.15f);
                r[y * g.w + x] += add;
            }
        }
    }
    
    // Reduce risk behind cover
    for (int y = 0; y < g.h; ++y) {
        for (int x = 0; x < g.w; ++x) {
            auto t = g.cells[y * g.w + x];
            if (t == Tile::Tree || t == Tile::Rock)
                r[y * g.w + x] *= 0.7f;  // 30% risk reduction for cover
        }
    }
    
    return r;
}
