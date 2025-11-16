#include "AStar.h"
#include <queue>
#include <limits>
#include <cmath> 
#include <algorithm>
static int idx(const Grid& g, IVec2 p){ return p.y*g.w + p.x; }
std::vector<IVec2> aStarPath(const Grid& g, IVec2 start, IVec2 goal, const std::vector<float>& risk, float alpha){
    auto h=[&](IVec2 a){ return std::abs(a.x-goal.x)+std::abs(a.y-goal.y); };
    struct Node{IVec2 p; float f;}; struct Cmp{bool operator()(const Node&a,const Node&b)const{return a.f>b.f;}};
    std::priority_queue<Node,std::vector<Node>,Cmp> open;
    std::vector<float> gscore(g.w*g.h, std::numeric_limits<float>::infinity());
    std::vector<int> came(g.w*g.h, -1);
    auto inb=[&](IVec2 p){ return g.inBounds(p)&&g.passable(p); };
    gscore[idx(g,start)]=0.f; open.push({start,(float)h(start)});
    auto neigh=[&](IVec2 p){ static const int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1}; std::vector<IVec2> out;
        for(int i=0;i<4;++i){ IVec2 q{p.x+dx[i],p.y+dy[i]}; if(inb(q)) out.push_back(q);} return out; };
    while(!open.empty()){
        auto cur=open.top(); open.pop(); if(cur.p==goal) break; int ci=idx(g,cur.p);
        for(auto q:neigh(cur.p)){ int qi=idx(g,q); float tentative=gscore[ci]+(1.0f+alpha*risk[qi]);
            if(tentative<gscore[qi]){ gscore[qi]=tentative; came[qi]=ci; open.push({q, tentative+(float)h(q)}); } }
    }
    std::vector<IVec2> path; int gi=idx(g,goal); if(came[gi]==-1){ path.push_back(start); return path; }
    for(int i=gi;i!=-1;){ int y=i/g.w, x=i%g.w; path.push_back({x,y}); i=came[i]; } std::reverse(path.begin(),path.end()); return path;
}
