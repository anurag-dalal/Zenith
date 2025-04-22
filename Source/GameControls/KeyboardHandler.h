#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <GLFW/glfw3.h>
#include "../Camera/FreeCamera.h"

class KeyboardHandler {
public:
    KeyboardHandler() = default;
    ~KeyboardHandler() = default;

    // Process keyboard input for camera movement
    void processInput(GLFWwindow* window, FreeCamera& camera, float deltaTime);

    // Process escape key for window closing
    static void processEscapeKey(GLFWwindow* window);
    
    // Toggle mouse cursor with Alt key
    bool processAltKey(GLFWwindow* window);

private:
    bool mouseLocked = true;
    bool altKeyPressed = false;
};

#endif // KEYBOARDHANDLER_H