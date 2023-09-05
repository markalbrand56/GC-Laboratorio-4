#include <SDL.h>
#include "gl.h"
#include "camera.h"
#include "uniforms.h"
#include "shaders.h"
#include "object.h"
#include "triangle.h"
#include <iostream>
#include <vector>

Uniforms uniform;
Camera camera;

void render(const std::vector<glm::vec3>& vertices) {
    // 1. Vertex Shader
    // vertex -> trasnformedVertices
    std::vector<Vertex> transformedVertices;

    for (int i = 0; i < vertices.size(); i+=2) {
        glm::vec3 v = vertices[i];
        glm::vec3 c = vertices[i + 1];

        Vertex vertex = {v, Color(c.x, c.y, c.z)};

        Vertex transformedVertex = vertexShader(vertex, uniform);
        transformedVertices.push_back(transformedVertex);
    }

    // 2. Primitive Assembly
    // transformedVertices -> triangles
    std::vector<std::vector<Vertex>> triangles = primitiveAssembly(transformedVertices);

    // 3. Rasterize
    // triangles -> Fragments
    std::vector<Fragment> fragments;
    for (const std::vector<Vertex>& triangleVertices : triangles) {
        std::vector<Fragment> rasterizedTriangle = triangle(
                triangleVertices[0],
                triangleVertices[1],
                triangleVertices[2]
        );

        fragments.insert(
                fragments.end(),
                rasterizedTriangle.begin(),
                rasterizedTriangle.end()
        );
    }

    // 4. Fragment Shader
    // Fragments -> colors

    for (Fragment fragment : fragments) {
        point(fragmentShader(fragment));
    }
}

std::vector<glm::vec3> setupVertexFromObject(const std::vector<glm::vec3>& vertices, const std::vector<Face>& faces){
    std::vector<glm::vec3> vertexBufferObject;

    for (const Face& face : faces) {
        for (const std::array<int, 3>& vertexIndices : face.vertexIndices) {
            glm::vec3 vertex = vertices[vertexIndices[0] - 1];
            glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.8f);

            vertexBufferObject.push_back(vertex);
            vertexBufferObject.push_back(color);
        }
    }

    return vertexBufferObject;
}

int main(int argc, char** argv) {
    init();

    std::vector<glm::vec3> sphereVertices;
    std::vector<Face> sphereFaces;

    // Load the OBJ file
    bool success = loadOBJ("../model/sphere.obj", sphereVertices, sphereFaces);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }

    std::vector<glm::vec3> vertexBufferObject = setupVertexFromObject(sphereVertices, sphereFaces);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Camera
        camera.cameraPosition = glm::vec3(0.0f, 0.0f, 1.5f);
        camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        camera.upVector = glm::vec3(0.0f, -1.0f, 0.0f);  // Mira hacia abajo porque el modelo está al revés

        // Create uniform
        uniform.model = createModelMatrix();
        uniform.view = createViewMatrix(camera);
        uniform.projection = createProjectionMatrix();
        uniform.viewport = createViewportMatrix();

        clear();

        // Render
        render(vertexBufferObject);

        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
