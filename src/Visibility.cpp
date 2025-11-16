#include "Visibility.h"
#include <cmath>
bool los(const Grid& g, IVec2 a, IVec2 b){
    int x0=a.x,y0=a.y,x1=b.x,y1=b.y;
    int dx=std::abs(x1-x0), sx=x0<x1?1:-1;
    int dy=-std::abs(y1-y0), sy=y0<y1?1:-1;
    int err=dx+dy, e2; IVec2 p=a;
    while(true){
        if(!(p==a)&&!(p==b)&&g.blocksLOS(p)) return false;
        if(p.x==x1&&p.y==y1) break;
        e2=2*err; if(e2>=dy){err+=dy; p.x+=sx;} if(e2<=dx){err+=dx; p.y+=sy;}
        if(!g.inBounds(p)) return false;
    } return true;
}

bool rayLine(const Grid& g, IVec2 a, IVec2 b)
{
    int x0 = a.x, y0 = a.y;
    int x1 = b.x, y1 = b.y;

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        if (g.isBlocked(x0, y0))
            return false;

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    return true;
}
