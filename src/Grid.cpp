#include "Grid.h"
#include <fstream>
#include <vector>
Grid Grid::loadFromTxt(const std::string& path){
    Grid g; std::ifstream in(path); std::vector<std::string> lines; std::string s;
    if(!in){
        lines = {
            "................................................",
            "................................................",
            "....######.............TTTTT....................",
            "....#....#.............T...T......~~~~~~~.......",
            "....#....#.............TTTTT......~~~~~~~.......",
            "....######........................~~~~~~~.......",
            "....A.M.........................................",
            "..............................................a.m",
            "................................................"
        };
    }else while(std::getline(in,s)) if(!s.empty()) lines.push_back(s);
    g.h=(int)lines.size(); g.w=(int)(lines.empty()?0:lines[0].size());
    g.cells.assign(g.w*g.h, Tile::Open);
    for(int y=0;y<g.h;++y) for(int x=0;x<g.w;++x){
        char c=lines[y][x]; Tile t=Tile::Open;
        switch(c){
            case '#': t=Tile::Rock; break; case 'T': t=Tile::Tree; break;
            case '~': t=Tile::Water; break; case 'A': t=Tile::DepotAmmo; g.blueAmmo={x,y}; break;
            case 'M': t=Tile::DepotMed; g.blueMed={x,y}; break; case 'a': t=Tile::DepotAmmo; g.orangeAmmo={x,y}; break;
            case 'm': t=Tile::DepotMed; g.orangeMed={x,y}; break; default: t=Tile::Open; break; }
        g.cells[y*g.w+x]=t;
    }
    if(g.orangeAmmo==IVec2{0,0}) g.orangeAmmo={g.w-2,g.h-2};
    if(g.orangeMed==IVec2{0,0}) g.orangeMed={g.w-2,g.h-3};
    return g;
}
