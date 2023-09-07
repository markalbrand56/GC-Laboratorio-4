// uniform.h
#pragma once

#include <glm/glm.hpp>

struct Uniforms {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 viewport;
};

glm::mat4 createModelMatrix(glm::vec3 translationVector, glm::vec3 scaleVector, glm::vec3 rotationVector, float angle)  {
    glm::mat4 translate = glm::translate(glm::mat4(1), translationVector);
    glm::mat4 scale = glm::scale(glm::mat4(1), scaleVector);
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(angle), rotationVector);

    return translate * scale * rotation;
}

glm::mat4 createProjectionMatrix() {
    float fovInDegrees = 45.0f;
    float aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
    float nearClip = 0.1f;
    float farClip = 100.0f;

    return glm::perspective(glm::radians(fovInDegrees), aspectRatio, nearClip, farClip);
}

glm::mat4 createViewportMatrix() {
    glm::mat4 viewport = glm::mat4(1.0f);

    // Scale
    viewport = glm::scale(viewport, glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.5f));

    // Translate
    viewport = glm::translate(viewport, glm::vec3(1.0f, 1.0f, 0.5f));

    return viewport;
}