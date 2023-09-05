// camera.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 cameraPosition;
    glm::vec3 targetPosition;
    glm::vec3 upVector;
};


glm::mat4 createViewMatrix(const Camera& camera) {
    return glm::lookAt(camera.cameraPosition, camera.targetPosition, camera.upVector);
}
