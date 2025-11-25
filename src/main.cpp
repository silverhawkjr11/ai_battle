#include "Game.h" 
#include "Renderer.h" 
#include <iostream>

int main(int argc, char* argv[]){
    // Configuration selection
    GameConfig config;
    
    if (argc > 1) {
        // Command line argument: 1=balanced, 2=blue advantage, 3=orange advantage
        int choice = std::atoi(argv[1]);
        if (choice == 2) {
            config = GameConfig::BlueAdvantage();
            std::cout << "=== BLUE ADVANTAGE CONFIGURATION ===" << std::endl;
        } else if (choice == 3) {
            config = GameConfig::OrangeAdvantage();
            std::cout << "=== ORANGE ADVANTAGE CONFIGURATION ===" << std::endl;
        } else {
            config = GameConfig::Balanced();
            std::cout << "=== BALANCED CONFIGURATION ===" << std::endl;
        }
    } else {
        // Interactive menu
        std::cout << "\n========== AI BATTLE GAME ==========" << std::endl;
        std::cout << "Select configuration:" << std::endl;
        std::cout << "1. Balanced (Equal strength)" << std::endl;
        std::cout << "2. Blue Advantage (+20 HP, +5 Ammo, +1 Grenade)" << std::endl;
        std::cout << "3. Orange Advantage (+20 HP, +5 Ammo, +1 Grenade)" << std::endl;
        std::cout << "Enter choice (1-3): ";
        
        int choice;
        std::cin >> choice;
        
        if (choice == 2) {
            config = GameConfig::BlueAdvantage();
            std::cout << "\n=== BLUE ADVANTAGE CONFIGURATION ===" << std::endl;
        } else if (choice == 3) {
            config = GameConfig::OrangeAdvantage();
            std::cout << "\n=== ORANGE ADVANTAGE CONFIGURATION ===" << std::endl;
        } else {
            config = GameConfig::Balanced();
            std::cout << "\n=== BALANCED CONFIGURATION ===" << std::endl;
        }
    }
    
    std::cout << config.name << std::endl;
    std::cout << "===================================\n" << std::endl;
    
    auto grid=Grid::loadFromTxt("assets/sample_map_80x50.txt");
    Game game(grid, config);
#ifdef USE_CONSOLE
    std::cout<<"Running CONSOLE fallback. Define USE_CONSOLE off to enable graphics.\n"; runConsole(game);
#else
    runGraphics(game);
#endif
    return 0;
}
