// fragment.h
#pragma once
#include "glm/glm.hpp"
#include "color.h"

struct Fragment {
    glm::vec3 position; // X and Y coordinates of the pixel (in screen space)
    Color color;
    float intensity;
    glm::vec3 worldPos;
    glm::vec3 originalPos;
};

struct Vertex {
    glm::vec3 position;
    Color color;
    glm::vec3 normal;
    glm::vec3 tex;
    glm::vec3 worldPos;
    glm::vec3 originalPos;
};

