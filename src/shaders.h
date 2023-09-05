// shaders.h
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "fragment.h"
#include "uniforms.h"
#include "gl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <random>

Vertex vertexShader(const Vertex& vertex, const Uniforms& u) {
    glm::vec4 v = glm::vec4(vertex.position.x, vertex.position.y, vertex.position.z, 1);
    glm::vec4 r = u.viewport * u.projection * u.view * u.model * v;

    return Vertex{
        glm::vec3(r.x/r.w, r.y/r.w, r.z/r.w),
        vertex.color
    };
};

std::vector<std::vector<Vertex>> primitiveAssembly (
    const std::vector<Vertex>& transformedVertices
) {
    // Assemble the transformed vertices into triangles
    // Return a vector of triangles, where each triangle is a vector of 3 vertices (vec3)

    std::vector<std::vector<Vertex>> groupedVertices;

    for (int i = 0; i < transformedVertices.size(); i += 3) {
        std::vector<Vertex> vertexGroup;
        vertexGroup.push_back(transformedVertices[i]);
        vertexGroup.push_back(transformedVertices[i+1]);
        vertexGroup.push_back(transformedVertices[i+2]);

        groupedVertices.push_back(vertexGroup);
    }

    return groupedVertices;
}

Fragment fragmentShader(Fragment fragment) {
    fragment.color = fragment.color * fragment.intensity;
    return fragment;
}