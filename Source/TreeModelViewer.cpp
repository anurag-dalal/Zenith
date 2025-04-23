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
#include "World/Models/TreeModel.h"

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
    
    // Create a TreeModel with max dimensions
    int maxTreeHeight = 20;
    int maxTreeWidth = 15;
    std::shared_ptr<Zenith::TreeModel> treeModel = std::make_shared<Zenith::TreeModel>(maxTreeHeight, maxTreeWidth);
    
    // Generate an initial oak tree
    treeModel->generateTree(Zenith::TreeType::BIRCH);
    treeModel->createVoxelObjects(blockRegistry);
    
    // Store current tree type and height for UI
    Zenith::TreeType currentTreeType = Zenith::TreeType::BIRCH;
    int currentTreeHeight = 0;  // 0 means use the default random height for the tree type
    
    // Position the tree model in the world
    treeModel->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Tree type names for ImGui
    const char* treeTypeNames[] = {
        "Oak", "Spruce", "Birch", "Jungle", "Acacia", "Dark Oak"
    };
    
    // Lighting setup
    glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    
    // Random seed for tree generation
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
        
        // Create ImGui window for tree model control
        ImGui::SetNextWindowSize(ImVec2(1500, 1000), ImGuiCond_Always);
        ImGui::Begin("Tree Model Viewer");
        
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
        
        // Tree type selection
        ImGui::Text("Tree Type:");
        bool treeTypeChanged = false;
        int treeTypeIndex = static_cast<int>(currentTreeType);
        
        if (ImGui::BeginCombo("##TreeType", treeTypeNames[treeTypeIndex])) {
            for (int i = 0; i < IM_ARRAYSIZE(treeTypeNames); i++) {
                bool isSelected = (treeTypeIndex == i);
                if (ImGui::Selectable(treeTypeNames[i], isSelected)) {
                    treeTypeIndex = i;
                    currentTreeType = static_cast<Zenith::TreeType>(i);
                    treeTypeChanged = true;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        
        // Tree height slider
        ImGui::Text("Tree Height (0 = random):");
        bool heightChanged = ImGui::SliderInt("##TreeHeight", &currentTreeHeight, 0, maxTreeHeight - 2);
        
        // Random seed
        ImGui::Text("Random Seed:");
        ImGui::SameLine();
        ImGui::Checkbox("Use Custom Seed", &useCustomSeed);
        
        bool seedChanged = false;
        if (useCustomSeed) {
            seedChanged = ImGui::InputInt("##RandomSeed", (int*)&seed);
        }
        
        // Regenerate button
        if (ImGui::Button("Regenerate Tree") || treeTypeChanged || heightChanged || seedChanged) {
            treeModel->clear();
            
            if (useCustomSeed) {
                treeModel->setRandomSeed(seed);
            }
            
            treeModel->generateTree(currentTreeType, currentTreeHeight);
            treeModel->createVoxelObjects(blockRegistry);
            
            // Output some info
            int p, q, r;
            treeModel->getDimensions(p, q, r);
            size_t blockCount = treeModel->getVoxelCount();
            
            std::cout << "Generated " << treeTypeNames[treeTypeIndex] << " tree with " 
                      << blockCount << " blocks in a " << p << "x" << q << "x" << r << " volume" << std::endl;
        }
        
        // Display model information
        ImGui::Separator();
        ImGui::Text("Tree Model Information:");
        
        int p, q, r;
        treeModel->getDimensions(p, q, r);
        size_t blockCount = treeModel->getVoxelCount();
        
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
        
        // Render the tree model
        treeModel->render(view, projection, lightDir, lightColor, camera.getPosition());
        
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