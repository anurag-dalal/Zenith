#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ConfigManager/ConfigReader.h" // For loadConfig and Config struct
#include "Camera/WindowManager.h"     // For WindowManager class

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set GLFW window hints (optional, but good practice)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Load configuration
    Config config = loadConfig("Configs/config.json");

    // Create WindowManager instance
    WindowManager windowManager;

    // Create the window
    GLFWwindow* window = windowManager.createWindow(config);
    if (window == nullptr) {
        // Error handling is done inside createWindow, just return
        return -1;
    }

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set the viewport (optional here, but usually needed)
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // --- Main Render Loop ---
    while (!glfwWindowShouldClose(window)) {
        // Input handling (optional for an empty window)
        // processInput(window);

        // Rendering commands here (just clear for now)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set clear color
        glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}