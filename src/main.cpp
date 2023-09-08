#include <SDL.h>
#include "gl.h"
#include "camera.h"
#include "uniforms.h"
#include "shaders.h"
#include "object.h"
#include "triangle.h"
#include <iostream>
#include <vector>

Camera camera;
std::vector<Model> models;

void render() {
    for (int i = 0; i < models.size(); i++) {
        Model model = models[i];
        Uniforms uniform = model.uniforms;
        uniform.model = model.modelMatrix;

        // 1. Vertex Shader
        // vertex -> trasnformedVertices
        std::vector<Vertex> transformedVertices;

        for (int i = 0; i < model.vertices.size(); i+=2) {
            glm::vec3 v = model.vertices[i];
            glm::vec3 c = model.vertices[i + 1];

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
            switch (i) {
                case 0:
                    point(sunFragmentShader(fragment));
                    break;
                case 1:
                    point(earthFragmentShader(fragment));
                    break;
            }
        }
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
    if (!init()) {
        return 1;
    }

    // Sol
    std::vector<glm::vec3> sunVertices;
    std::vector<Face> sunFaces;
    std::vector<glm::vec3> sunNormals;
    std::vector<glm::vec3> sunTexCoords;

    // Uranus
    std::vector<glm::vec3> uranusVertices;
    std::vector<Face> uranusFaces;
    std::vector<glm::vec3> uranusNormals;
    std::vector<glm::vec3> uranusTexCoords;

    // Load the OBJ file
    bool success = loadOBJ("../model/sphere.obj", sunVertices, sunFaces, sunNormals, sunTexCoords);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }
    success = loadOBJ("../model/sphere.obj", uranusVertices, uranusFaces, uranusNormals, uranusTexCoords);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }

    std::vector<glm::vec3> sunVBO = setupVertexFromObject(sunVertices, sunFaces);
    std::vector<glm::vec3> uranusVBO = setupVertexFromObject(uranusVertices, uranusFaces);

    Uint32 frameStart, frameTime;
    std::string title = "FPS: ";
    float a = 45.0f;
    float b = 0.0f;

    // Camera
    camera.cameraPosition = glm::vec3(0.0f, 0.0f, 2.5f);
    camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // Create Sun uniform
    Uniforms uniform;

    glm::vec3 translationVector(0.0f, 0.0f, 0.0f);
    glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
    glm::vec3 scaleFactor(1.0f, 1.0f, 1.0f);

    uniform.view = createViewMatrix(camera);
    uniform.projection = createProjectionMatrix();
    uniform.viewport = createViewportMatrix();

    // Create Uranus uniform
    Uniforms uranusUniform;

    glm::vec3 translationVectorUranus(0.0f, 1.0f, 0.0f);
    glm::vec3 rotationAxisUranus(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
    glm::vec3 scaleFactorUranus(0.5f, 0.5f, 0.5f);

    uranusUniform.view = createViewMatrix(camera);
    uranusUniform.projection = createProjectionMatrix();
    uranusUniform.viewport = createViewportMatrix();

    // Create model
    Model sunModel;
    sunModel.vertices = sunVBO;
    sunModel.uniforms = uniform;

    Model uranusModel;
    uranusModel.vertices = uranusVBO;
    uranusModel.uniforms = uranusUniform;

    bool running = true;
    while (running) {
        frameStart = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        a += 15.0f;
        b += 30.0f;

        // Sun
        uniform.model = createModelMatrix(translationVector, scaleFactor, rotationAxis, a);
        sunModel.modelMatrix = uniform.model;

        // Uranus
        uranusUniform.model = createModelMatrix(translationVectorUranus, scaleFactorUranus, rotationAxisUranus, b);
        uranusModel.modelMatrix = uranusUniform.model;

        models.push_back(sunModel);
        models.push_back(uranusModel);

        clear();

        // Render
        render();

        models.clear();
        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 60);

        frameTime = SDL_GetTicks() - frameStart;

        // Calculate frames per second and update window title
        if (frameTime > 0) {
            std::ostringstream titleStream;
            titleStream << "FPS: " << 1000.0 / frameTime;  // Milliseconds to seconds
            SDL_SetWindowTitle(window, titleStream.str().c_str());
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
