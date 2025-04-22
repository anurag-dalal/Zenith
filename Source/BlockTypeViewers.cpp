#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>

#include "Camera/WindowManager.h"
#include "Camera/FreeCamera.h"
#include "GameControls/KeyboardHandler.h"
#include "GameControls/MouseHandler.h"
#include "ConfigManager/ConfigReader.h"
#include "Blocks/BlockRegistryReader.h"
#include "Blocks/Voxel.h"

// Callback function for window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Load configuration
    Config config = loadConfig(std::string(CONFIG_DIR) + "/config.json");
    
    // Create window
    WindowManager windowManager;
    GLFWwindow* window = windowManager.createWindow(config);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Register window callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);  // Changed from GL_CCW to GL_CW
    
    // Create camera
    FreeCamera camera(config.camera.position, config.camera.up);
    
    // Create input handlers
    KeyboardHandler keyHandler;
    MouseHandler mouseHandler;
    
    // Set up mouse callbacks
    mouseHandler.setupMouseCallbacks(window);
    mouseHandler.processMouseMovement(window, camera);
    
    // Load block registry
    Zenith::BlockRegistryReader blockRegistry;
    if (!blockRegistry.loadRegistry()) {
        std::cerr << "Failed to load block registry" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Get the GRASS block textures
    const Zenith::BlockTextures* grassTextures = blockRegistry.getBlockTextures("GRASS");
    if (!grassTextures) {
        std::cerr << "Failed to find GRASS block textures" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Create a voxel with grass textures
    std::shared_ptr<Zenith::Voxel> grassVoxel = Zenith::Voxel::create(
        grassTextures->top,     // Top texture
        grassTextures->bottom,  // Bottom texture
        grassTextures->front,   // Front texture
        grassTextures->back,    // Back texture
        grassTextures->left,    // Left texture
        grassTextures->right    // Right texture
    );
    
    if (!grassVoxel) {
        std::cerr << "Failed to create grass voxel" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Set voxel position
    grassVoxel->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Lighting setup
    glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    
    // Timing variables
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Process input
        keyHandler.processEscapeKey(window);
        keyHandler.processInput(window, camera, deltaTime);
        
        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Create transformation matrices
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix for model
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.getZoom()), 
            (float)config.window.width / (float)config.window.height, 
            0.1f, 
            100.0f
        );
        
        // Render the grass voxel
        grassVoxel->render(model, view, projection, lightDir, lightColor, camera.getPosition());
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up
    glfwTerminate();
    return 0;
}