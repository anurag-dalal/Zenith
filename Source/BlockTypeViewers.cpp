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
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
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
    
    // Set up mouse callbacks - Important: Don't override ImGui mouse callbacks
    // We'll handle mouse lock state directly in the main loop
    
    // Update the camera reference (but don't set callbacks)
    mouseHandler.processMouseMovement(window, camera);
    
    // Initially start with mouse unlocked for ImGui interaction
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    // Load block registry
    Zenith::BlockRegistryReader blockRegistry;
    if (!blockRegistry.loadRegistry()) {
        std::cerr << "Failed to load block registry" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Store all block IDs and create dropdown
    std::vector<std::string> blockIds;
    std::vector<std::string> blockCategories; // Store potential block categories
    
    blockRegistry.forEachBlock([&blockIds](const std::string& blockId, const Zenith::BlockTextures&) {
        blockIds.push_back(blockId);
    });
    
    // Sort block IDs alphabetically for easier navigation
    std::sort(blockIds.begin(), blockIds.end());
    
    // Try to identify categories (like GRASS, STONE, WOOD, etc.)
    for (const auto& blockId : blockIds) {
        size_t underscorePos = blockId.find('_');
        if (underscorePos != std::string::npos) {
            std::string category = blockId.substr(0, underscorePos);
            if (std::find(blockCategories.begin(), blockCategories.end(), category) == blockCategories.end()) {
                blockCategories.push_back(category);
            }
        }
    }
    std::sort(blockCategories.begin(), blockCategories.end());
    
    // Initially set to GRASS or first available block
    std::string currentBlockId = "BEDROCK";
    if (std::find(blockIds.begin(), blockIds.end(), currentBlockId) == blockIds.end() && !blockIds.empty()) {
        currentBlockId = blockIds[0];
    }
    
    // Search filter for blocks
    char searchBuffer[256] = "";
    std::string currentCategory = "ALL";
    
    // Get the initial block textures
    const Zenith::BlockTextures* blockTextures = blockRegistry.getBlockTextures(currentBlockId);
    if (!blockTextures) {
        std::cerr << "Failed to find block textures for " << currentBlockId << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Create voxel with initial textures
    std::shared_ptr<Zenith::Voxel> currentVoxel = Zenith::Voxel::create(
        blockTextures->top,
        blockTextures->bottom,
        blockTextures->front,
        blockTextures->back,
        blockTextures->left,
        blockTextures->right
    );
    
    if (!currentVoxel) {
        std::cerr << "Failed to create voxel" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Set voxel position
    currentVoxel->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Lighting setup
    glm::vec3 lightDir(-0.2f, -1.0f, -0.3f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    
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
        
        // Create ImGui window for block selection
        ImGui::SetNextWindowSize(ImVec2(800, 1000), ImGuiCond_Always);
        ImGui::Begin("Block Registry Viewer");
        
        // Process Alt key to toggle mouse lock
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && !altKeyPressed) {
            altKeyPressed = true;
            mouseLocked = !mouseLocked;
            glfwSetInputMode(window, GLFW_CURSOR, mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            std::cout << "Mouse lock toggled: " << (mouseLocked ? "Locked" : "Unlocked") << std::endl;
            
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
        
        // Process camera movement when mouse is locked
        if (mouseLocked) {
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
            float yoffset = mouseHandler.getLastY() - yposf; // Reversed since y-coordinates range from bottom to top
            
            mouseHandler.setLastX(xposf);
            mouseHandler.setLastY(yposf);
            
            camera.processMouseMovement(xoffset, yoffset);
            
            // Handle keyboard input for scrolling (zoom)
            if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
                camera.processMouseScroll(0.1f);
            }
            if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
                camera.processMouseScroll(-0.1f);
            }
        }
        
        // Check if ImGui is being interacted with
        bool imguiWantsMouse = ImGui::GetIO().WantCaptureMouse;
        
        // Add a hint about Alt key functionality
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), 
                          "Press [ALT] to %s mouse | Mouse is %s | ImGui wants mouse: %s", 
                          mouseLocked ? "unlock" : "lock",
                          mouseLocked ? "locked" : "unlocked",
                          imguiWantsMouse ? "Yes" : "No");
        ImGui::Separator();
        
        // Display block count
        ImGui::Text("Available Blocks: %zu", blockIds.size());
        
        // Search filter
        ImGui::Separator();
        ImGui::Text("Search Blocks:");
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##searchbox", searchBuffer, IM_ARRAYSIZE(searchBuffer));
        ImGui::PopItemWidth();
        
        // Category filter
        if (!blockCategories.empty()) {
            ImGui::Text("Filter by Category:");
            
            // All categories option
            if (ImGui::RadioButton("ALL", currentCategory == "ALL")) {
                currentCategory = "ALL";
            }
            
            // Display category radio buttons
            for (const auto& category : blockCategories) {
                ImGui::SameLine();
                if (ImGui::RadioButton(category.c_str(), currentCategory == category)) {
                    currentCategory = category;
                }
            }
        }
        
        ImGui::Separator();
        
        // Create filtered block list
        std::string searchStr = searchBuffer;
        std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::toupper);
        
        // Display blocks in a child window with scrolling
        ImGui::BeginChild("BlocksListRegion", ImVec2(0, 200), true);
        
        for (const auto& blockId : blockIds) {
            // Check if the block passes the category filter
            bool passesCategory = (currentCategory == "ALL" || 
                                 (blockId.find(currentCategory + "_") == 0));
            
            // Check if block passes the search filter
            std::string upperBlockId = blockId;
            std::transform(upperBlockId.begin(), upperBlockId.end(), upperBlockId.begin(), ::toupper);
            bool passesSearch = searchStr.empty() || upperBlockId.find(searchStr) != std::string::npos;
            
            // Only display blocks that pass all filters
            if (passesCategory && passesSearch) {
                // Use Selectable with proper highlighting
                bool selected = (currentBlockId == blockId);
                if (ImGui::Selectable(blockId.c_str(), selected)) {
                    // Store the new selection
                    currentBlockId = blockId;
                    
                    // Get new block textures
                    const Zenith::BlockTextures* newTextures = blockRegistry.getBlockTextures(currentBlockId);
                    if (newTextures) {
                        // Create new voxel with selected block textures
                        currentVoxel = Zenith::Voxel::create(
                            newTextures->top,
                            newTextures->bottom,
                            newTextures->front,
                            newTextures->back,
                            newTextures->left,
                            newTextures->right
                        );
                        
                        if (currentVoxel) {
                            currentVoxel->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
                            // Print debug info when selection changes
                            std::cout << "Selected block: " << currentBlockId << std::endl;
                            std::cout << "  Top texture: " << newTextures->top << std::endl;
                        } else {
                            std::cerr << "Failed to create voxel for " << currentBlockId << std::endl;
                        }
                    } else {
                        std::cerr << "Failed to get textures for " << currentBlockId << std::endl;
                    }
                }
                
                // If this item is selected, ensure it's visible in the list
                if (selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }
        ImGui::EndChild();
        
        // Display currently selected block information
        ImGui::Separator();
        ImGui::Text("Selected Block: %s", currentBlockId.c_str());
        
        // Display texture paths for the selected block
        if (const Zenith::BlockTextures* textures = blockRegistry.getBlockTextures(currentBlockId)) {
            ImGui::Separator();
            ImGui::Text("Texture Paths:");
            
            ImGui::BeginChild("TexturePaths", ImVec2(0, 120), true);
            ImGui::Text("Top:    %s", textures->top.c_str());
            ImGui::Text("Bottom: %s", textures->bottom.c_str());
            ImGui::Text("Front:  %s", textures->front.c_str());
            ImGui::Text("Back:   %s", textures->back.c_str());
            ImGui::Text("Left:   %s", textures->left.c_str());
            ImGui::Text("Right:  %s", textures->right.c_str());
            ImGui::EndChild();
        }
        
        ImGui::End();
        
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
        
        // Render the current voxel
        if (currentVoxel) {
            currentVoxel->render(model, view, projection, lightDir, lightColor, camera.getPosition());
        }
        
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