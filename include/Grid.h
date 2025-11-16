#pragma once
#include "Types.h"
#include <vector>
#include <string>

struct Grid {
    int w{ 0 }, h{ 0 };
    std::vector<Tile> cells;

    IVec2 blueAmmo{ 1,1 }, blueMed{ 1,2 };
    IVec2 orangeAmmo{ 0,0 }, orangeMed{ 0,0 };

    bool inBounds(IVec2 p) const {
        return p.x >= 0 && p.y >= 0 && p.x < w && p.y < h;
    }

    Tile at(IVec2 p) const {
        return cells[p.y * w + p.x];
    }

    bool passable(IVec2 p) const {
        Tile t = at(p);
        return t != Tile::Rock && t != Tile::Water;
    }

    bool blocksLOS(IVec2 p) const {
        Tile t = at(p);
        return t == Tile::Rock || t == Tile::Tree;
    }

    bool isBlocked(int x, int y) const {
        if (x < 0 || y < 0 || x >= w || y >= h)
            return true;
        Tile t = cells[y * w + x];
        return (t == Tile::Rock ||
            t == Tile::Tree ||
            t == Tile::Water);
    }

    static Grid loadFromTxt(const std::string& path);
};
