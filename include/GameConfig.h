#pragma once
#include <string>

struct GameConfig {
	std::string name;
	int blueStartHP;
	int blueStartAmmo;
	int blueStartGrenades;
	int orangeStartHP;
	int orangeStartAmmo;
	int orangeStartGrenades;

	static GameConfig balanced() {
		return GameConfig{
			"Balanced Configuration",
			100, // Blue normal HP
			20,  // Blue normal ammo
			2,   // Blue normal grenades
			100, // Orange normal HP
			20,  // Orange normal ammo
			2    // Orange normal grenades
		};
	}

	static GameConfig blueAdvantage() {
		return GameConfig{
			"Blue Advantage (+50 HP, +10 ammo, +2 grenades)",
			150, // Blue warriors start with +50 HP (50% more)
			30,  // Blue warriors start with +10 ammo (50% more)
			4,   // Blue warriors start with +2 grenades (100% more)
			100, // Orange normal HP
			20,  // Orange normal ammo
			2    // Orange normal grenades
		};
	}

	static GameConfig orangeAdvantage() {
		return GameConfig{
			"Orange Advantage (+50 HP, +10 ammo, +2 grenades)",
			100, // Blue normal HP
			20,  // Blue normal ammo
			2,   // Blue normal grenades
			150, // Orange warriors start with +50 HP (50% more)
			30,  // Orange warriors start with +10 ammo (50% more)
			4    // Orange warriors start with +2 grenades (100% more)
		};
	}
};
