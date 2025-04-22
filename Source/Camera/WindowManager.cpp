#include "WindowManager.h"

WindowManager::WindowManager() {
    // Constructor implementation, if needed
    // Initialize GLFW or other setup tasks can go here if they are specific to the manager
    // However, GLFW initialization is often done globally once.
}

WindowManager::~WindowManager() {
    // Destructor implementation
    // Cleanup resources managed by this class, if any
    // Note: Window destruction might be handled elsewhere depending on ownership
}

GLFWwindow* WindowManager::createWindow(const Config& config) { // Changed ConfigReader to Config
    // Window size - Note: SCR_WIDTH and SCR_HEIGHT are not used directly in the provided logic
    // const unsigned int SCR_WIDTH = config.window.width;
    // const unsigned int SCR_HEIGHT = config.window.height;

    // Set up window
    GLFWwindow *window;
    if (config.fullscreen.enabled)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        
        // Set window hints for fullscreen
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        
        if (config.fullscreen.borderless)
        {
            // Borderless fullscreen (windowed fullscreen)
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            window = glfwCreateWindow(mode->width, mode->height, config.window.title.c_str(), nullptr, nullptr);
            if (window) { // Check if window creation was successful before setting position
                glfwSetWindowPos(window, 0, 0);
            }
        }
        else
        {
            // True fullscreen
            window = glfwCreateWindow(mode->width, mode->height, config.window.title.c_str(), monitor, nullptr);
        }
    }
    else
    {
        // Windowed mode
        window = glfwCreateWindow(config.window.width, config.window.height, config.window.title.c_str(), nullptr, nullptr);
    }

    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl; // Use cerr for errors
        glfwTerminate(); // Consider if terminating GLFW here is appropriate or should be handled by the caller
        return nullptr; // Return nullptr instead of -1 for a pointer type
    }
    glfwMakeContextCurrent(window);
    
    // Configure VSync based on settings
    glfwSwapInterval(config.performance.vsync ? 1 : 0);

    return window;
}
