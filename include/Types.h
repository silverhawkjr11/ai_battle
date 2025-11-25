#pragma once
#include <vector>
#include <string>
#include <optional>
#include <cstdint>
#include <cmath>

struct IVec2 {
    int x{ 0 }, y{ 0 };

    IVec2() = default;
    IVec2(int X, int Y) : x(X), y(Y) {}

    bool operator==(const IVec2& o) const { return x == o.x && y == o.y; }
    bool operator!=(const IVec2& o) const { return !(*this == o); }
    bool operator<(const IVec2& o) const { return (y < o.y) || (y == o.y && x < o.x); }

    IVec2 operator+(const IVec2& other) const { return IVec2(x + other.x, y + other.y); }
    IVec2 operator-(const IVec2& other) const { return IVec2(x - other.x, y - other.y); }
    IVec2 operator*(int k) const { return IVec2(x * k, y * k); }

    int manhattan(const IVec2& o) const {
        return std::abs(x - o.x) + std::abs(y - o.y);
    }
};

enum class Tile : std::uint8_t {
    Open = 0, Rock, Tree, Water, DepotAmmo, DepotMed
};

enum class Team : std::uint8_t {
    Blue = 0,
    Orange = 1
};

inline const char* teamName(Team t)
{
    return t == Team::Blue ? "Blue" : "Orange";
}

// CONSTANTS
constexpr int  kSightRange = 10;
constexpr int  kGunRange = 6;
constexpr int  kGrenadeRange = 4;
constexpr int  kGrenadeDamage = 15;       // Reduced from 45-60 to 15
constexpr int  kMaxHP = 100;
constexpr int  kLowHPThreshold = 25;      // 25% of max HP
constexpr int  kMedCallHP = 60;           // Call medic if HP below this
constexpr int  kLowAmmo = 5;              // Low ammo threshold
constexpr int  kSafeSearchRadius = 8;
constexpr float kMaxSafeRisk = 0.25f;
constexpr int  kPorterCooldown = 50;  // Ticks between resupplies
// NEW: Force aggression after this tick regardless of remaining enemy warriors
constexpr int  kForceCommanderFocusTick = 600;
// NEW: Limit number of revives per warrior to avoid infinite loops
constexpr int  kMaxWarriorRevives = 1;
// NEW: Limit number of resupplies per warrior to avoid infinite sustain
constexpr int  kMaxResuppliesPerWarrior = 6;

// Game configuration
struct GameConfig {
    std::string name;
    
    // Team advantages
    int blueExtraHP = 0;        // Extra HP for Blue warriors
    int orangeExtraHP = 0;      // Extra HP for Orange warriors
    int blueExtraAmmo = 0;      // Extra ammo for Blue warriors
    int orangeExtraAmmo = 0;    // Extra ammo for Orange warriors
    int blueExtraGrenades = 0;  // Extra grenades for Blue warriors
    int orangeExtraGrenades = 0; // Extra grenades for Orange warriors
    
    // Named configurations
    static GameConfig Balanced() {
        GameConfig cfg;
        cfg.name = "Balanced (Equal teams)";
        return cfg;
    }
    
    static GameConfig BlueAdvantage() {
        GameConfig cfg;
        cfg.name = "Blue Advantage (+20 HP, +5 ammo)";
        cfg.blueExtraHP = 20;
        cfg.blueExtraAmmo = 5;
        cfg.blueExtraGrenades = 1;
        return cfg;
    }
    
    static GameConfig OrangeAdvantage() {
        GameConfig cfg;
        cfg.name = "Orange Advantage (+20 HP, +5 ammo)";
        cfg.orangeExtraHP = 20;
        cfg.orangeExtraAmmo = 5;
        cfg.orangeExtraGrenades = 1;
        return cfg;
    }
};
