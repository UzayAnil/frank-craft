#pragma once

#include <glm/glm.hpp>

struct LightSource {
    glm::vec3 position;
    glm::vec3 rgbIntensity;
};

struct Controls {
    Controls() : W( false ), A( false ), S( false ), D( false ),
        Space( false ),
        mouse_left_down( false ), mouse_right( false ){}
    bool W; bool A; bool S; bool D;
    bool Space;

    bool mouse_left_down;
    bool mouse_right;
    float xPos;
    float yPos;
};

double lerp(double t, double a, double b);
double getTime();
