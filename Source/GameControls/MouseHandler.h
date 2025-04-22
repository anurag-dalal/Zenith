#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

#include <GLFW/glfw3.h>
#include "../Camera/FreeCamera.h"

class MouseHandler {
public:
    MouseHandler();
    ~MouseHandler() = default;

    // Setup mouse callbacks for the window
    void setupMouseCallbacks(GLFWwindow* window);
    
    // Process mouse movement for camera rotation
    void processMouseMovement(GLFWwindow* window, FreeCamera& camera);
    
    // Getters for mouse state
    bool isFirstMouse() const { return firstMouse; }
    float getLastX() const { return lastX; }
    float getLastY() const { return lastY; }
    
    // Setters for mouse state
    void setFirstMouse(bool first) { firstMouse = first; }
    void setLastX(float x) { lastX = x; }
    void setLastY(float y) { lastY = y; }

    // Static callback functions for GLFW
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    // Mouse tracking variables
    bool firstMouse;
    float lastX;
    float lastY;
    
    // Static pointer to current instance for callbacks
    static MouseHandler* instance;
    
    // Pointer to the camera for callbacks
    FreeCamera* camera;
};

#endif // MOUSEHANDLER_H