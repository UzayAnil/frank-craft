#pragma once

#include <glm/glm.hpp>

struct LightSource {
    glm::vec3 position;
    glm::vec3 rgbIntensity;
};

struct Controls {
    Controls() : W( false ), A( false ), S( false ), D( false ),
        Space( false ) {}
    bool W; bool A; bool S; bool D;
    bool Space;
};

double lerp(double t, double a, double b);
double getTime();
