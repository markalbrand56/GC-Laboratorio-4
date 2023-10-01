// shaders.h
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "fragment.h"
#include "uniforms.h"
#include "gl.h"
#include "FastNoise.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <random>

const glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);  // 1, 1, 1: White
const glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);  // 0, 0, 0: Black

Vertex vertexShader(const Vertex& vertex, const Uniforms& uniforms) {
    // Apply transformations to the input vertex using the matrices from the uniforms
    glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

    // Perspective divide
    glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

    // Apply the viewport transform
    glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

    // Transform the normal
    glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
    transformedNormal = glm::normalize(transformedNormal);

    glm::vec3 transformedWorldPosition = glm::vec3(uniforms.model * glm::vec4(vertex.position, 1.0f));

    // Return the transformed vertex as a vec3
    return Vertex{
            glm::vec3(screenVertex),
            transformedNormal,
            vertex.tex,
            transformedWorldPosition,
            vertex.position
    };
}

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

Fragment sunFragmentShader(Fragment& fragment) {
    Color color;

    // 235 127 33
    glm::vec3 mainColor = glm::vec3(235.0f/255.0f, 127.0f/255.0f, 33.0f/255.0f);  // 235, 127, 33: Orange
    // 194 77 14
    glm::vec3 secondColor = glm::vec3(194.0f/255.0f, 77.0f/255.0f, 14.0f/255.0f);  // 194, 77, 14: Dark orange

    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 3200.0f;
    float oy = 3000.0f;
    float z = 1800.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * z, (uv.y + oy) * z);

    glm::vec3 tempColor;

    tempColor = glm::mix(secondColor, mainColor,glm::smoothstep(0.16f, 0.6f, abs(noiseValue)));
//    tempColor = (abs(noiseValue) < 0.6f) ? mainColor : secondColor;

    color = Color(tempColor.x, tempColor.y, tempColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment earthFragmentShader(Fragment& fragment) {
    Color color;

    glm::vec3 forestColor = glm::vec3(0.44f, 0.51f, 0.33f);
    glm::vec3 dirtColor = glm::vec3(179/255.0f, 147/255.0f, 120/255.0f);
    glm::vec3 oceanColor = glm::vec3(0.12f, 0.38f, 0.57f);
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    glm::vec3 tmpColor;

    if (noiseValue < 0.05f) {
        tmpColor = oceanColor;
    } else {
        tmpColor = glm::mix(forestColor, dirtColor, glm::smoothstep(0.15f, 0.96f, noiseValue));
    }

    float oxc = 5500.0f;
    float oyc = 6900.0f;
    float zoomc = 300.0f;

    float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc);

    if (noiseValueC > 0.5f) {
        tmpColor = cloudColor;
    }

    color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment moonFragmentShader(Fragment& fragment) {
    Color color;

    // 185 185 185
    glm::vec3 mainColor = glm::vec3(185.0f/255.0f, 185.0f/255.0f, 185.0f/255.0f);  // 185, 185, 185: Gray
    // 140 140 140
    glm::vec3 secondColor = glm::vec3(140.0f/255.0f, 140.0f/255.0f, 140.0f/255.0f);  // 140, 140, 140: Dark gray

    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 2000.0f;
    float oy = 2000.0f;
    float z = 350.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * z, (uv.y + oy) * z);

    glm::vec3 tmpColor;
    tmpColor = (noiseValue < 0.4f) ? mainColor : secondColor;

    color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment jupiterFragmentShader(Fragment& fragment){
    Color color;

    // 214 181 144
    glm::vec3 mainColor = glm::vec3(214.0f/255.0f, 181.0f/255.0f, 144.0f/255.0f);  // 214, 181, 144: Light brown
    // white
    glm::vec3 secondColor = glm::vec3(1.0f, 1.0f, 1.0f);  // 255, 255, 255: White
    // 214 131 36
    glm::vec3 thirdColor = glm::vec3(214.0f/255.0f, 131.0f/255.0f, 36.0f/255.0f);  // 214, 131, 36: Dark brown

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0 , fragment.originalPos.y * 2.0 - 1.0);

    // Frecuencia y amplitud de las ondas en el planeta
    float frequency = 13.0; // Ajusta la frecuencia de las líneas
    float amplitude = 0.1; // Ajusta la amplitud de las líneas

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float offsetX = 1000.0f;
    float offsetY = 200.0f;
    float scale = 100.0f;

    // Genera el valor de ruido
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);
    noiseValue = (noiseValue + 1.0f) * 0.5f;

    //aplicar ruido como hacer piedras
    noiseValue = glm::smoothstep(0.0f, 1.0f, noiseValue);

    // Interpola entre el color base y el color secundario basado en el valor de ruido
    secondColor = glm::mix(mainColor, secondColor, noiseValue);

    // Calcula el valor sinusoide para crear líneas
    float sinValue = glm::sin(uv.y * frequency) * amplitude;

    // Combina el color base con las líneas sinusoide
    secondColor = glm::mix(secondColor, thirdColor, sinValue);

    color = Color(secondColor.x, secondColor.y, secondColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
}

// MAKE A SHADER TO DISPLAY PLAIN NOISE
Fragment noiseFragmentShader(Fragment& fragment) {
    Color color;

    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 2000.0f;
    float oy = 2000.0f;
    float z = 350.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * z, (uv.y + oy) * z);

    color = Color(noiseValue, noiseValue, noiseValue);

    fragment.color = color * fragment.intensity;

    return fragment;
}