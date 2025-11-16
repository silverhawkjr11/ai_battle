#pragma once
#include <algorithm>

struct Camera {
    float x = 0.0f;     // world offset X
    float y = 0.0f;     // world offset Y
    float zoom = 1.0f;  // zoom factor

    void pan(float dx, float dy) {
        x += dx / zoom;
        y += dy / zoom;
    }

    void addZoom(float z) {
        zoom = std::clamp(zoom + z, 0.25f, 4.0f);
    }
};
