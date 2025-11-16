
#include "Bullets.h"
#include "Grid.h"
#include <cmath>
#include <algorithm>

//////////////////////////////////////////////////////////
// BULLET
//////////////////////////////////////////////////////////

Bullet::Bullet(float sx, float sy, float tx, float ty)
{
    x = sx;
    y = sy;

    float vx = tx - sx;
    float vy = ty - sy;

    float len = std::sqrt(vx * vx + vy * vy);
    dx = vx / len;
    dy = vy / len;
}

void Bullet::update(const Grid& g)
{
    // Trail
    trail.emplace_back(x, y);
    if (trail.size() > 10)
        trail.erase(trail.begin());

    float nx = x + dx * speed;
    float ny = y + dy * speed;

    // Bounces
    if (g.isBlocked(nx, y))
    {
        dx *= -1;
        bounces--;
    }

    if (g.isBlocked(x, ny))
    {
        dy *= -1;
        bounces--;
    }

    if (bounces <= 0)
        alive = false;

    x += dx * speed;
    y += dy * speed;

    if (x < 0 || y < 0 || x > g.w || y > g.h)
        alive = false;
}

//////////////////////////////////////////////////////////
// BULLET SYSTEM
//////////////////////////////////////////////////////////

void BulletSystem::addBullet(float sx, float sy, float tx, float ty)
{
    bullets.emplace_back(sx, sy, tx, ty);
}

void BulletSystem::update(const Grid& g)
{
    for (auto& b : bullets)
        if (b.alive)
            b.update(g);

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.alive; }),
        bullets.end());
}

//////////////////////////////////////////////////////////
// GRENADE
//////////////////////////////////////////////////////////

void Grenade::update()
{
    t += speed;

    if (t >= 1.0f)
    {
        alive = false;
        return;
    }

    x = x * (1 - t) + tx * t;
    y = y * (1 - t) + ty * t;
}
