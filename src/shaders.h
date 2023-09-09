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
    // Define the colors for the ocean, the ground, and the spots with direct values
    // 244, 140, 6
    glm::vec3 sunColor = glm::vec3(244.0f/255.0f, 140.0f/255.0f, 6.0f/255.0f);
    // 191, 110, 6
    glm::vec3 sunSpotColor = glm::vec3(191.0f/255.0f, 110.0f/255.0f, 6.0f/255.0f);

    // Sample the Perlin noise map at the fragment's position
    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.z);
    uv = glm::clamp(uv, 0.0f, 1.0f);  // make sure the uv coordinates are in [0, 1] range

    // Set up the noise generator
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float z = 1000.0f;
    // Generate the noise value
    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * z, (uv.y + oy) * z);

    // Decide the spot color based on the noise value
    float normalizedNoise = (noiseValue + 1.0f) * 0.5f;
    glm::vec3 c = glm::mix(sunColor, sunSpotColor, normalizedNoise);

    // Convert glm::vec3 color to your Color class
    fragment.color = Color(c.x, c.y, c.z);

    return fragment;
}

Fragment earthFragmentShader(Fragment& fragment) {
    Color color;

    glm::vec3 groundColor = glm::vec3(0.44f, 0.51f, 0.33f);
    glm::vec3 oceanColor = glm::vec3(0.12f, 0.38f, 0.57f);
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y);
    /* glm::vec2 uv = glm::vec2(fragment.texCoords.x, fragment.texCoords.y) */;

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    glm::vec3 tmpColor = (noiseValue < 0.5f) ? oceanColor : groundColor;

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
    // Define the colors for the ocean, the ground, and the spots with direct values
    glm::vec3 spotColor = glm::vec3(0.8f, 0.8f, 0.8f);  // Gray

    // Sample the Perlin noise map at the fragment's position
    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.z);
    uv = glm::clamp(uv, 0.0f, 1.0f);  // make sure the uv coordinates are in [0, 1] range

    // Set up the noise generator
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float z = 1000.0f;
    // Generate the noise value
    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * z, (uv.y + oy) * z);

    // Ajusta la escala y el sesgo del valor de ruido
    float scale = 0.2f; // Ajusta este valor según tus preferencias
    float bias = 0.8f;  // Ajusta este valor según tus preferencias

    float adjustedNoise = (noiseValue * scale) + bias;


    // Asegúrate de que el valor esté en el rango [0.8, 1.0]
    adjustedNoise = glm::clamp(adjustedNoise, 0.6f, 0.8f);

    // Usa el valor ajustado para definir el color
    glm::vec3 c = glm::vec3(adjustedNoise);

    // Convierte glm::vec3 color a tu clase Color
    fragment.color = Color(c.r, c.g, c.b);

    // Interpolate between the ocean color and the ground color based on the water/ground transition
    // Then, interpolate between this color and the spot color

    // Convert glm::vec3 color to your Color class
    fragment.color = Color(c.r, c.g, c.b);

    return fragment;
}

Fragment jupiterFragmentShader(Fragment& fragment){
    Color color;

    glm::vec3 mainColor = glm::vec3(204.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);  // 204, 255, 255: Light blue
    glm::vec3 secondColor = glm::vec3(0.0f/255.0f, 153.0f/255.0f, 153.0f/255.0f);  // 0, 153, 153: Dark blue
    glm::vec3 thirdColor = glm::vec3(204.0f/255.0f, 255.0f/255.0f, 229.0f/255.0f);  // 204, 255, 229: Light green

    glm::vec2 uv = glm::vec2(fragment.originalPos.x * 2.0 - 1.0 , fragment.originalPos.y * 2.0 - 1.0);

    // Frecuencia y amplitud de las ondas en el planeta
    float frequency = 9.0; // Ajusta la frecuencia de las líneas
    float amplitude = 0.1; // Ajusta la amplitud de las líneas

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);

    float offsetX = 800.0f;
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

    float ox = 5000.0f;
    float oy = 3000.0f;
    float z = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * z, (uv.y + oy) * z);

    color = Color(noiseValue, noiseValue, noiseValue);

    fragment.color = color * fragment.intensity;

    return fragment;
}