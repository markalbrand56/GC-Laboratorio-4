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
std::string planet = "";
bool hasMoon = false;
float planetSize = 1.0f;

using namespace std;
void render() {
    for (int i = 0; i < models.size(); i++) {
        Model model = models[i];
        Uniforms uniform = model.uniforms;
        uniform.model = model.modelMatrix;

        // 1. Vertex Shader
        // vertex -> trasnformedVertices
        std::vector<Vertex> transformedVertices;

        for (int i = 0; i < model.vertices.size(); i+=3) {
            glm::vec3 v = model.vertices[i];
            glm::vec3 n = model.vertices[i+1];
            glm::vec3 t = model.vertices[i+2];

            Vertex vertex = Vertex{v, n, t};

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
            switch (model.shader) {
                case Shader::Earth:
                    point(earthFragmentShader(fragment));
                    break;
                case Shader::Sun:
                    point(sunFragmentShader(fragment));
                    break;
                case Shader::Moon:
                    point(moonFragmentShader(fragment));
                    break;
                case Shader::Jupiter:
                    point(jupiterFragmentShader(fragment));
                    break;
                case Shader::Uranus:
                    point(uranusFragmentShader(fragment));
                    break;
                case Shader::Mars:
                    point(plutoFragmentShader(fragment));
                    break;
                case Shader::Kepler186f:
                    point(keplerFragmentShader(fragment));
                    break;
                case Shader::Noise:
                    point(noiseFragmentShader(fragment));
                    break;
                default:
                    point(fragmentShader(fragment));
                    break;
            }
        }
    }
}

std::vector<glm::vec3> setupVertexFromObject(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& texCoords){
    std::vector<glm::vec3> vertexBufferObject;

    for (const auto& face : faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            // Get the vertex position
            glm::vec3 vertexPosition = vertices[face.vertexIndices[i]];

            // Get the normal for the current vertex
            glm::vec3 vertexNormal = normals[face.normalIndices[i]];

            // Get the texture for the current vertex
            glm::vec3 vertexTexture = texCoords[face.texIndices[i]];

            // Add the vertex position and normal to the vertex array
            vertexBufferObject.push_back(vertexPosition);
            vertexBufferObject.push_back(vertexNormal);
            vertexBufferObject.push_back(vertexTexture);
        }
    }

    return vertexBufferObject;
}

Shader getNextPlanetTexture(Shader currentTexture) {
    // Implementa la lógica para cambiar a la siguiente textura
    switch (currentTexture) {
        case Shader::Earth:
            planet = "Sun";
            hasMoon = false;
            planetSize = 1.6f;
            return Shader::Sun;
        case Shader::Sun:
            planet = "Jupiter";
            hasMoon = false;
            planetSize = 1.3f;
            return Shader::Jupiter;
        case Shader::Jupiter:
            planet = "Uranus";
            hasMoon = false;
            planetSize = 1.15f;
            return Shader::Uranus;
        case Shader::Uranus:
            planet = "Mars";
            hasMoon = false;
            planetSize = 1.0f;
            return Shader::Mars;
        case Shader::Mars:
            planet = "Kepler 186f";
            hasMoon = false;
            planetSize = 1.0f;
            return Shader::Kepler186f;
        default:
            planet = "Earth";
            hasMoon = true;
            planetSize = 1.0f;
            return Shader::Earth;
    }
}

int main(int argc, char** argv) {
    if (!init()) {
        return 1;
    }

    // Sol
    std::vector<glm::vec3> planetVertices;
    std::vector<Face> planetFaces;
    std::vector<glm::vec3> planetNormals;
    std::vector<glm::vec3> planetTexCoords;

    // Uranus
    std::vector<glm::vec3> moonVertices;
    std::vector<Face> moonFaces;
    std::vector<glm::vec3> moonNormals;
    std::vector<glm::vec3> moonTexCoords;

    // Load the OBJ file
    bool success = loadOBJ("../model/sphere.obj", planetVertices, planetFaces, planetNormals, planetTexCoords);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }
    success = loadOBJ("../model/sphere.obj", moonVertices, moonFaces, moonNormals, moonTexCoords);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }

    std::vector<glm::vec3> planetVBO = setupVertexFromObject(planetFaces, planetVertices, planetNormals, planetTexCoords);
    std::vector<glm::vec3> moonVBO = setupVertexFromObject(moonFaces, moonVertices, moonNormals, moonTexCoords);

    Uint32 frameStart, frameTime;
    std::string title = "FPS: ";
    float a = 45.0f;
    float b = 0.0f;

    // Camera
    camera.cameraPosition = glm::vec3(0.0f, 0.0f, 2.5f);
    camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // Create Sun planetUniform
    Uniforms planetUniform;

    glm::vec3 translationVector(0.0f, 0.0f, 0.0f);
    glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
    glm::vec3 scaleFactor(1.0f, 1.0f, 1.0f);

    planetUniform.view = createViewMatrix(camera);
    planetUniform.projection = createProjectionMatrix();
    planetUniform.viewport = createViewportMatrix();

    // Create Uranus planetUniform
    Uniforms moonUniform;

    glm::vec3 rotationAxisMoon(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis
    glm::vec3 scaleFactorMoon(0.27f, 0.27f, 0.27f);

    moonUniform.view = createViewMatrix(camera);
    moonUniform.projection = createProjectionMatrix();
    moonUniform.viewport = createViewportMatrix();

    // Create model
    Model planetModel;
    planetModel.vertices = planetVBO;
    planetModel.uniforms = planetUniform;
    planetModel.shader = Shader::Noise;

    Model moonModel;
    moonModel.vertices = moonVBO;
    moonModel.uniforms = moonUniform;
    moonModel.shader = Shader::Moon;

    cout << "Starting loop" << endl;

    float speed = 5.0f;
    bool running = true;
    float moonOrbitAngle = 0.0f;
    float distanceToPlanet = 1.0f;

    while (running) {
        frameStart = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        speed += -1.0f;
                        break;
                    case SDLK_RIGHT:
                        speed += 1.0f;
                        break;
                    case SDLK_SPACE:
                        planetModel.shader = getNextPlanetTexture(planetModel.shader);
                        break;
                }
            }
        }

        a += (speed/planetSize);
        b += (speed/planetSize) * 1.5f;

        // Sun
        planetUniform.model = createModelMatrix(translationVector, scaleFactor * planetSize, rotationAxis, a);
        planetModel.modelMatrix = planetUniform.model;

        // Moon
        // move the moon around the planet on the x and y axis
        moonOrbitAngle += 2.0f;
        glm::vec3 translationVectorMoon(
                distanceToPlanet * cos(glm::radians(moonOrbitAngle)),
                0.0f,
                distanceToPlanet * sin(glm::radians(moonOrbitAngle))
        );
        moonUniform.model = createModelMatrix(translationVectorMoon, scaleFactorMoon, rotationAxisMoon, b);
        moonModel.modelMatrix = moonUniform.model;

        models.push_back(planetModel);

        if (hasMoon){
            models.push_back(moonModel);
        }

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
            titleStream << planet + " FPS: " << 1000.0 / frameTime;  // Milliseconds to seconds
            SDL_SetWindowTitle(window, titleStream.str().c_str());
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
