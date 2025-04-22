#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <glad/glad.h> // Include glad before glfw
#include <GLFW/glfw3.h>
#include "../ConfigManager/ConfigReader.h" // Adjust path as necessary
#include <iostream>
#include <string>

class WindowManager {
public:
    WindowManager(); // Default constructor if needed, or remove if not used
    ~WindowManager(); // Destructor to handle cleanup if necessary

    // Creates and returns a GLFW window based on the provided configuration
    GLFWwindow* createWindow(const Config& config); // Changed ConfigReader to Config

private:
    // Add any private members if needed, e.g., storing the window handle internally
    // GLFWwindow* window = nullptr; 
};

#endif // WINDOWMANAGER_H
