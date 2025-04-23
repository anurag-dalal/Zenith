// filepath: /home/anurag/Codes/Learnings/opengl/Zenith/Source/HutModelViewer.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

// ImGui headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Camera/WindowManager.h"
#include "Camera/FreeCamera.h"
#include "GameControls/KeyboardHandler.h"
#include "GameControls/MouseHandler.h"
#include "ConfigManager/ConfigReader.h"
#include "Blocks/BlockRegistryReader.h"
#include "World/Models/HutModel.h"

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
    
    // Initially start with mouse unlocked for ImGui interaction
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    // Register window callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);
    
    // Create camera
    FreeCamera camera(config.camera.position, config.camera.up);
    
    // Create input handlers
    KeyboardHandler keyHandler;
    MouseHandler mouseHandler;
    
    // Set up mouse callbacks - COMMENTED OUT to avoid conflict with ImGui
    // mouseHandler.setupMouseCallbacks(window);
    mouseHandler.processMouseMovement(window, camera);
    
    // Load block registry
    Zenith::BlockRegistryReader blockRegistry;
    if (!blockRegistry.loadRegistry()) {
        std::cerr << "Failed to load block registry" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Create a HutModel with max dimensions
    int maxWidth = 20;
    int maxHeight = 20;
    int maxDepth = 20;
    std::shared_ptr<Zenith::HutModel> hutModel = std::make_shared<Zenith::HutModel>(maxWidth, maxHeight, maxDepth);
    
    // Generate an initial hut
    hutModel->generateHut(Zenith::HutType::BASIC, true);
    hutModel->createVoxelObjects(blockRegistry);
    
    // Store current hut type for UI
    Zenith::HutType currentHutType = Zenith::HutType::BASIC;
    bool withFurnishings = true;
    
    // Position the hut model in the world
    hutModel->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Hut type names for ImGui
    const char* hutTypeNames[] = {
        "Basic Hut", "Round Hut", "Longhouse", "Tiered Hut"
    };
    
    // Lighting setup
    glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    
    // Random seed for hut generation
    unsigned int seed = 0;
    bool useCustomSeed = false;
    
    // Timing variables
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    // Mouse lock state
    bool mouseLocked = false;
    bool altKeyPressed = false;
    
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::GetIO().FontGlobalScale = 2.5f;  // Scale up the ImGui font size
        ImGui::NewFrame();
        
        // Check if ImGui wants to capture mouse input before processing Alt key
        bool imguiWantsMouse = ImGui::GetIO().WantCaptureMouse;
        
        // Only process Alt key if ImGui is not capturing mouse
        if (!imguiWantsMouse) {
            // Process Alt key to toggle mouse lock
            if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && !altKeyPressed) {
                altKeyPressed = true;
                mouseLocked = !mouseLocked;
                glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                
                // Reset first mouse to avoid camera jump when toggling
                if (mouseLocked) {
                    double xpos, ypos;
                    glfwGetCursorPos(window, &xpos, &ypos);
                    mouseHandler.setLastX(static_cast<float>(xpos));
                    mouseHandler.setLastY(static_cast<float>(ypos));
                    mouseHandler.setFirstMouse(true);
                }
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE) {
                altKeyPressed = false;
            }
        }
        
        // Create ImGui window for hut model control
        ImGui::SetNextWindowSize(ImVec2(800, 1000), ImGuiCond_Always);
        ImGui::Begin("Hut Model Viewer");
        
        // Add a hint about Alt key functionality
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), 
                          "Press [ALT] to %s mouse | Mouse is %s | ImGui wants mouse: %s", 
                          mouseLocked ? "unlock" : "lock",
                          mouseLocked ? "locked" : "unlocked",
                          imguiWantsMouse ? "Yes" : "No");
        ImGui::Separator();
        
        // Process camera movement only when mouse is locked and ImGui is not capturing input
        if (mouseLocked && !imguiWantsMouse) {
            // Handle keyboard input for camera movement
            keyHandler.processInput(window, camera, deltaTime);
            
            // Handle mouse movement for camera
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            
            float xposf = static_cast<float>(xpos);
            float yposf = static_cast<float>(ypos);
            
            if (mouseHandler.isFirstMouse()) {
                mouseHandler.setLastX(xposf);
                mouseHandler.setLastY(yposf);
                mouseHandler.setFirstMouse(false);
            }
            
            float xoffset = xposf - mouseHandler.getLastX();
            float yoffset = mouseHandler.getLastY() - yposf;
            
            mouseHandler.setLastX(xposf);
            mouseHandler.setLastY(yposf);
            
            camera.processMouseMovement(xoffset, yoffset);
        }
        
        // Hut type selection
        ImGui::Text("Hut Type:");
        bool hutTypeChanged = false;
        int hutTypeIndex = static_cast<int>(currentHutType);
        
        if (ImGui::BeginCombo("##HutType", hutTypeNames[hutTypeIndex])) {
            for (int i = 0; i < IM_ARRAYSIZE(hutTypeNames); i++) {
                bool isSelected = (hutTypeIndex == i);
                if (ImGui::Selectable(hutTypeNames[i], isSelected)) {
                    hutTypeIndex = i;
                    currentHutType = static_cast<Zenith::HutType>(i);
                    hutTypeChanged = true;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        
        // Furnishings checkbox
        bool furnishingsChanged = ImGui::Checkbox("Add Furnishings", &withFurnishings);
        
        // Random seed
        ImGui::Text("Random Seed:");
        ImGui::SameLine();
        ImGui::Checkbox("Use Custom Seed", &useCustomSeed);
        
        bool seedChanged = false;
        if (useCustomSeed) {
            seedChanged = ImGui::InputInt("##RandomSeed", (int*)&seed);
        }
        
        // Regenerate button
        if (ImGui::Button("Regenerate Hut") || hutTypeChanged || furnishingsChanged || seedChanged) {
            hutModel->clear();
            
            if (useCustomSeed) {
                hutModel->setRandomSeed(seed);
            }
            
            hutModel->generateHut(currentHutType, withFurnishings);
            hutModel->createVoxelObjects(blockRegistry);
            
            // Output some info
            int p, q, r;
            hutModel->getDimensions(p, q, r);
            size_t blockCount = hutModel->getVoxelCount();
            
            std::cout << "Generated " << hutTypeNames[hutTypeIndex] << " with " 
                      << blockCount << " blocks in a " << p << "x" << q << "x" << r << " volume" << std::endl;
        }
        
        // Display model information
        ImGui::Separator();
        ImGui::Text("Hut Model Information:");
        
        int p, q, r;
        hutModel->getDimensions(p, q, r);
        size_t blockCount = hutModel->getVoxelCount();
        
        ImGui::Text("Model Dimensions: %d x %d x %d", p, q, r);
        ImGui::Text("Total Blocks: %zu", blockCount);
        
        ImGui::End();
        
        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Create transformation matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.getZoom()), 
            (float)config.window.width / (float)config.window.height, 
            0.1f, 
            100.0f
        );
        
        // Render the hut model
        hutModel->render(view, projection, lightDir, lightColor, camera.getPosition());
        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // Clean up
    glfwTerminate();
    return 0;
}