#include "BFS.h"
#include <queue> 
#include <cmath>
std::optional<IVec2> bfsFindSafe(const Grid& g, IVec2 start, const std::vector<float>& risk, float maxRisk, int radius){
    std::queue<IVec2> q; std::vector<char> vis(g.w*g.h,0);
    auto inb=[&](IVec2 p){return g.inBounds(p)&&g.passable(p);};
    auto push=[&](IVec2 p){ if(!inb(p))return; int i=p.y*g.w+p.x; if(vis[i])return; vis[i]=1; q.push(p); };
    push(start);
    while(!q.empty()){
        IVec2 p=q.front(); q.pop(); int i=p.y*g.w+p.x;
        if(risk[i]<=maxRisk) return p;
        if(std::abs(p.x-start.x)+std::abs(p.y-start.y)>radius) continue;
        push({p.x+1,p.y}); push({p.x-1,p.y}); push({p.x,p.y+1}); push({p.x,p.y-1});
    } return std::nullopt;
}
