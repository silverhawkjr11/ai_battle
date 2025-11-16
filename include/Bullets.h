#pragma once
#include "Types.h"
#include "Grid.h"
#include <vector>
#include <cmath>
#include <algorithm>

struct BulletTrail {
    float x, y;
    float life = 1.0f;
    BulletTrail(float X, float Y) : x(X), y(Y) {}
};

struct Bullet {
    float x, y;
    float dx, dy;
    float speed = 0.35f;
    bool alive = true;
    int damage = 20;
    int bounces = 2;
    std::vector<BulletTrail> trail;

    Bullet(float sx, float sy, float tx, float ty);
    void update(const Grid& g);
};

struct BulletSystem {
    std::vector<Bullet> bullets;
    void addBullet(float sx, float sy, float tx, float ty);
    void update(const Grid& g);
};

struct Grenade {
    float x, y;
    float tx, ty;
    float t = 0;
    float speed = 0.02f;
    float explodeRadius = 3;
    bool alive = true;

    Grenade(float sx, float sy, float tx_, float ty_)
        : x(sx), y(sy), tx(tx_), ty(ty_) {}

    void update();
};

struct GrenadeSystem {
    std::vector<Grenade> grenades;

    void addGrenade(float sx, float sy, float tx, float ty) {
        grenades.emplace_back(sx, sy, tx, ty);
    }

    template<typename Callback>
    void updateAndExplode(Callback onExplode) {
        for (auto& g : grenades) {
            bool wasAlive = g.alive;
            g.update();

            if (wasAlive && !g.alive)
                onExplode(g.tx, g.ty, g.explodeRadius);
        }

        grenades.erase(
            std::remove_if(
                grenades.begin(), grenades.end(),
                [](const Grenade& gr) { return !gr.alive; }),
            grenades.end()
        );
    }
};
