#include "KeyboardHandler.h"
#include "imgui.h"
void KeyboardHandler::processInput(GLFWwindow* window, FreeCamera& camera, float deltaTime) {
    // Camera keyboard controls are processed in the camera class
    camera.processKeyboard(window, deltaTime);
}

void KeyboardHandler::processEscapeKey(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

bool KeyboardHandler::processAltKey(GLFWwindow* window) {
    // Check if ImGui wants keyboard input
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        return mouseLocked;
    }

    // Check the state of the Alt key
    int altKeyState = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
    
    // Detect alt key press (transition from released to pressed)
    if (altKeyState == GLFW_PRESS && !altKeyPressed) {
        altKeyPressed = true;
        // Toggle mouse lock
        mouseLocked = !mouseLocked;
        
        // Update cursor mode based on mouseLocked state
        if (!io.WantCaptureMouse) {
            glfwSetInputMode(window, GLFW_CURSOR, 
                           mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }
    else if (altKeyState == GLFW_RELEASE) {
        altKeyPressed = false;
    }
    
    return mouseLocked;
}