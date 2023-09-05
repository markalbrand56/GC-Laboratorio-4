// fragment.h
#pragma once
#include "glm/glm.hpp"
#include "color.h"

struct Fragment {
    glm::vec3 position; // X and Y coordinates of the pixel (in screen space)
    Color color;
    float intensity;
};

struct Vertex {
    glm::vec3 position;
    Color color;
};

