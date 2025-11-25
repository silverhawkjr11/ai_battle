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
