#include "Game.h" 
#include "Renderer.h" 
#include <iostream>
int main(){
    auto grid=Grid::loadFromTxt("assets/sample_map_80x50.txt");
    Game game(grid);
#ifdef USE_CONSOLE
    std::cout<<"Running CONSOLE fallback. Define USE_CONSOLE off to enable graphics.\n"; runConsole(game);
#else
    runGraphics(game);
#endif
    return 0;
}
