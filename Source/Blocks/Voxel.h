#pragma once

#include <string>
#include <array>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Zenith {

class Voxel {
public:
    // Constructor and destructor
    Voxel();
    ~Voxel();

    // Texture loading
    bool loadTextures(const std::string& topPath, 
                     const std::string& bottomPath, 
                     const std::string& frontPath, 
                     const std::string& backPath, 
                     const std::string& leftPath, 
                     const std::string& rightPath);

    // Alternate constructor to load all textures at once
    static std::shared_ptr<Voxel> create(const std::string& topPath, 
                                        const std::string& bottomPath, 
                                        const std::string& frontPath, 
                                        const std::string& backPath, 
                                        const std::string& leftPath, 
                                        const std::string& rightPath);

    // Rendering
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, 
                const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& viewPos);
    
    // Position setters
    void setPosition(const glm::vec3& position);
    const glm::vec3& getPosition() const { return m_position; }

private:
    // Initialize the voxel geometry and textures
    void initialize();
    
    // Setup vertex and index buffers
    void setupBuffers();

    // Load a texture from file
    unsigned int loadTextureFromFile(const std::string& path);

private:
    // Texture IDs for each face
    std::array<unsigned int, 6> m_textureIDs;
    
    // Shader program ID
    unsigned int m_shaderProgramID;
    
    // Buffer IDs
    unsigned int m_VAO, m_VBO, m_EBO;
    
    // Voxel position
    glm::vec3 m_position;
    
    // Flag to track if the voxel is initialized
    bool m_initialized;
    
    // Vertex data for a cube
    std::vector<float> m_vertices;
    
    // Index data for a cube
    std::vector<unsigned int> m_indices;
    
    // Texture paths
    std::array<std::string, 6> m_texturePaths;
    
    // Face enum for clarity
    enum Face {
        TOP = 0,
        BOTTOM,
        FRONT,
        BACK,
        LEFT,
        RIGHT
    };
};

} // namespace Zenith