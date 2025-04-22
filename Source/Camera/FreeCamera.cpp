#include "FreeCamera.h"
#include <glm/gtc/matrix_transform.hpp>

FreeCamera::FreeCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) 
    : position(position), worldUp(up), yaw(yaw), pitch(pitch), 
      front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(2.5f), 
      mouseSensitivity(0.1f), zoom(45.0f) {
    updateCameraVectors();
}

glm::mat4 FreeCamera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void FreeCamera::processKeyboard(GLFWwindow* window, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += worldUp * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= worldUp * velocity;
}

void FreeCamera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // Update front, right and up Vectors using the updated Euler angles
    updateCameraVectors();
}

void FreeCamera::processMouseScroll(float yoffset) {
    zoom -= yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void FreeCamera::updateCameraVectors() {
    // Calculate the new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    // Also re-calculate the right and up vector
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}