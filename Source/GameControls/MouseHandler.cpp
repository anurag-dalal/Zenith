#include "MouseHandler.h"
#include <iostream>
#include "imgui.h"

// Initialize static member
MouseHandler* MouseHandler::instance = nullptr;

MouseHandler::MouseHandler() : firstMouse(true), lastX(0.0f), lastY(0.0f), mouseLocked(true), camera(nullptr) {
    instance = this;
}

void MouseHandler::setupMouseCallbacks(GLFWwindow* window) {
    // Set GLFW callbacks
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    // Initially capture the cursor
    if (!ImGui::GetIO().WantCaptureMouse) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    
    // Get initial cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);
}

void MouseHandler::processMouseMovement(GLFWwindow* window, FreeCamera& camera) {
    // Store camera reference for callbacks
    this->camera = &camera;
}

// Static callback handlers
void MouseHandler::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!instance || !instance->camera) return;
    
    // Check if ImGui wants to capture the mouse
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return; // Let ImGui handle the mouse input
    }
    
    // Only process camera movement if mouse is locked
    if (!instance->mouseLocked) return;
    
    float xposf = static_cast<float>(xpos);
    float yposf = static_cast<float>(ypos);
    
    if (instance->firstMouse) {
        instance->lastX = xposf;
        instance->lastY = yposf;
        instance->firstMouse = false;
    }
    
    float xoffset = xposf - instance->lastX;
    float yoffset = instance->lastY - yposf; // Reversed since y-coordinates range from bottom to top
    
    instance->lastX = xposf;
    instance->lastY = yposf;
    
    instance->camera->processMouseMovement(xoffset, yoffset);
}

void MouseHandler::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!instance || !instance->camera) return;
    
    // Check if ImGui wants to capture the mouse
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return; // Let ImGui handle the scroll input
    }
    
    // Only process scroll if mouse is locked (for zooming with camera)
    if (!instance->mouseLocked) return;
    
    instance->camera->processMouseScroll(static_cast<float>(yoffset));
}