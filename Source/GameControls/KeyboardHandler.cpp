#include "KeyboardHandler.h"

void KeyboardHandler::processInput(GLFWwindow* window, FreeCamera& camera, float deltaTime) {
    // Camera keyboard controls are processed in the camera class
    camera.processKeyboard(window, deltaTime);
}

void KeyboardHandler::processEscapeKey(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}