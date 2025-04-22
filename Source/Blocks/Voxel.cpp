#include "Voxel.h"
#include "../Utils/ShaderUtils.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

namespace Zenith {

Voxel::Voxel()
    : m_position(0.0f)
    , m_initialized(false)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_shaderProgramID(0)
{
    // Initialize texture IDs to 0
    for (auto& texID : m_textureIDs) {
        texID = 0;
    }
}

Voxel::~Voxel() {
    // Clean up OpenGL resources - but only if they've been initialized
    // No OpenGL calls are made to check context validity to avoid segfaults
    if (m_initialized) {
        // We're intentionally skipping OpenGL cleanup to avoid segmentation faults
        // Resources will be cleaned up when the OpenGL context is destroyed
        // This is a safer approach for application shutdown
        
        // Just mark everything as uninitialized
        m_initialized = false;
        m_VAO = 0;
        m_VBO = 0;
        m_EBO = 0;
        m_shaderProgramID = 0;
        
        for (auto& texID : m_textureIDs) {
            texID = 0;
        }
    }
}

std::shared_ptr<Voxel> Voxel::create(const std::string& topPath, 
                                     const std::string& bottomPath, 
                                     const std::string& frontPath, 
                                     const std::string& backPath, 
                                     const std::string& leftPath, 
                                     const std::string& rightPath) {
    auto voxel = std::make_shared<Voxel>();
    if (!voxel->loadTextures(topPath, bottomPath, frontPath, backPath, leftPath, rightPath)) {
        return nullptr;
    }
    return voxel;
}

bool Voxel::loadTextures(const std::string& topPath, 
                         const std::string& bottomPath, 
                         const std::string& frontPath, 
                         const std::string& backPath, 
                         const std::string& leftPath, 
                         const std::string& rightPath) {
    // Store texture paths
    m_texturePaths[TOP] = topPath;
    m_texturePaths[BOTTOM] = bottomPath;
    m_texturePaths[FRONT] = frontPath;
    m_texturePaths[BACK] = backPath;
    m_texturePaths[LEFT] = leftPath;
    m_texturePaths[RIGHT] = rightPath;
    
    // Load the shader program if not already loaded
    if (m_shaderProgramID == 0) {
        m_shaderProgramID = ShaderUtils::createShaderProgram(
            std::string(SHADER_DIR) + "/voxel_vertex.glsl",
            std::string(SHADER_DIR) + "/voxel_fragment.glsl"
        );
        
        if (m_shaderProgramID == 0) {
            std::cerr << "Failed to load voxel shaders" << std::endl;
            return false;
        }
    }
    
    // Initialize the voxel if not already initialized
    if (!m_initialized) {
        initialize();
    }
    
    // Load each texture
    m_textureIDs[TOP] = loadTextureFromFile(topPath);
    m_textureIDs[BOTTOM] = loadTextureFromFile(bottomPath);
    m_textureIDs[FRONT] = loadTextureFromFile(frontPath);
    m_textureIDs[BACK] = loadTextureFromFile(backPath);
    m_textureIDs[LEFT] = loadTextureFromFile(leftPath);
    m_textureIDs[RIGHT] = loadTextureFromFile(rightPath);
    
    // Check if all textures loaded successfully
    for (auto texID : m_textureIDs) {
        if (texID == 0) {
            std::cerr << "Failed to load one or more voxel textures" << std::endl;
            return false;
        }
    }
    
    return true;
}

void Voxel::initialize() {
    if (m_initialized) return;
    
    // Define the vertices for a cube with positions, normals and texture coordinates
    // Each face has unique normals and texture coordinates
    m_vertices = {
        // Top face (y+)
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
    
        // Bottom face (y-)
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
    
        // Front face (z+), fixed mirrored texture
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
    
        // Back face (z-), UVs rotated 180°
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
    
        // Left face (x-), fixed mirrored texture
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    
        // Right face (x+), fixed mirrored texture
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 0.0f
    };
    
    
    // Define indices for drawing the cube faces
    m_indices = {
        // Top face
        0, 1, 2,
        2, 3, 0,
        
        // Bottom face
        4, 5, 6,
        6, 7, 4,
        
        // Front face
        8, 9, 10,
        10, 11, 8,
        
        // Back face
        12, 13, 14,
        14, 15, 12,
        
        // Left face
        16, 17, 18,
        18, 19, 16,
        
        // Right face
        20, 21, 22,
        22, 23, 20
    };
    
    // Set up the vertex buffers
    setupBuffers();
    
    m_initialized = true;
}

void Voxel::setupBuffers() {
    // Create VAO, VBO, and EBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    // Bind VAO
    glBindVertexArray(m_VAO);
    
    // Bind VBO and copy vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);
    
    // Bind EBO and copy index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
    
    // Set up vertex attributes
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Unbind VAO
    glBindVertexArray(0);
}

unsigned int Voxel::loadTextureFromFile(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else {
            std::cerr << "Unsupported texture format: " << path << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &textureID);
            return 0;
        }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
        return textureID;
    } else {
        std::cerr << "Texture failed to load: " << path << std::endl;
        stbi_image_free(data);
        glDeleteTextures(1, &textureID);
        return 0;
    }
}

void Voxel::setPosition(const glm::vec3& position) {
    m_position = position;
}

void Voxel::render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, 
                 const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& viewPos) {
    if (!m_initialized) {
        std::cerr << "Cannot render uninitialized voxel" << std::endl;
        return;
    }
    
    // Use the shader program
    glUseProgram(m_shaderProgramID);
    
    // Apply position transformation
    glm::mat4 modelMatrix = glm::translate(model, m_position);
    
    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(m_shaderProgramID, "lightDir"), 1, glm::value_ptr(lightDir));
    glUniform3fv(glGetUniformLocation(m_shaderProgramID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(m_shaderProgramID, "viewPos"), 1, glm::value_ptr(viewPos));
    glUniform1f(glGetUniformLocation(m_shaderProgramID, "ambientStrength"), 0.3f);
    
    // Bind VAO
    glBindVertexArray(m_VAO);
    
    // Fixed approach: Activate all textures at once
    for (int i = 0; i < 6; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textureIDs[i]);
        // Set uniform for each texture sampler
        std::string uniformName = "textureFace" + std::to_string(i);
        glUniform1i(glGetUniformLocation(m_shaderProgramID, uniformName.c_str()), i);
    }
    
    // Draw all faces at once
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    
    // Unbind VAO
    glBindVertexArray(0);
}

} // namespace Zenith