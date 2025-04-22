#include "Block.h"
#include <iostream>
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Zenith {

Block::Block(const std::string& blockId, const BlockRegistryReader& registry, float halfWidth)
    : m_blockId(blockId)
    , m_halfWidth(halfWidth)
    , m_VAO(0)
    , m_VBO(0)
    , m_meshCreated(false)
{
    // Initialize texture IDs to 0
    m_textureIds.fill(0);
    
    // Get the block textures from the registry
    const BlockTextures* textures = registry.getBlockTextures(blockId);
    if (textures) {
        // Set texture paths from the registry
        m_texturePaths[0] = textures->front;  // FRONT
        m_texturePaths[1] = textures->back;   // BACK
        m_texturePaths[2] = textures->left;   // LEFT
        m_texturePaths[3] = textures->right;  // RIGHT
        m_texturePaths[4] = textures->bottom; // BOTTOM
        m_texturePaths[5] = textures->top;    // TOP
        
        // Debug: Print texture paths
        std::cout << "Loaded texture paths for block " << blockId << ":" << std::endl;
        std::cout << "  Top:    " << m_texturePaths[5] << std::endl;
        std::cout << "  Bottom: " << m_texturePaths[4] << std::endl;
        std::cout << "  Front:  " << m_texturePaths[0] << std::endl;
        std::cout << "  Back:   " << m_texturePaths[1] << std::endl;
        std::cout << "  Left:   " << m_texturePaths[2] << std::endl;
        std::cout << "  Right:  " << m_texturePaths[3] << std::endl;
    } else {
        std::cerr << "Block type not found in registry: " << blockId << std::endl;
        // Set default texture paths
        m_texturePaths.fill("");
    }
}

Block::Block(const std::string& blockId, const std::array<std::string, 6>& texturePaths, float halfWidth)
    : m_blockId(blockId)
    , m_halfWidth(halfWidth)
    , m_texturePaths(texturePaths)
    , m_VAO(0)
    , m_VBO(0)
    , m_meshCreated(false)
{
    // Initialize texture IDs to 0
    m_textureIds.fill(0);
}

Block::Block(const std::string& blockId, const std::string& allFacesTexturePath, float halfWidth)
    : m_blockId(blockId)
    , m_halfWidth(halfWidth)
    , m_VAO(0)
    , m_VBO(0)
    , m_meshCreated(false)
{
    // Set the same texture path for all faces
    m_texturePaths.fill(allFacesTexturePath);
    
    // Initialize texture IDs to 0
    m_textureIds.fill(0);
}

Block::~Block() {
    // Clean up OpenGL resources
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
    }
    
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
    }
    
    // Delete textures
    for (unsigned int textureId : m_textureIds) {
        if (textureId != 0) {
            glDeleteTextures(1, &textureId);
        }
    }
}

const std::string& Block::getBlockId() const {
    return m_blockId;
}

void Block::createMesh() {
    // Release previous mesh if it exists
    if (m_meshCreated) {
        glDeleteBuffers(1, &m_VBO);
        glDeleteVertexArrays(1, &m_VAO);
    }
    
    // Define the vertices for our cube with custom half width
    float hw = m_halfWidth; // Half width
    
    float vertices[] = {
        // Positions           // Normals           // Texture Coords
        // Front face (0)
        -hw, -hw,  hw,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f, // Bottom-left
         hw, -hw,  hw,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f, // Bottom-right
         hw,  hw,  hw,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f, // Top-right
         hw,  hw,  hw,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f, // Top-right
        -hw,  hw,  hw,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f, // Top-left
        -hw, -hw,  hw,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f, // Bottom-left
        
        // Back face (1)
        -hw, -hw, -hw,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, // Bottom-left
         hw, -hw, -hw,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f, // Bottom-right
         hw,  hw, -hw,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f, // Top-right
         hw,  hw, -hw,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f, // Top-right
        -hw,  hw, -hw,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f, // Top-left
        -hw, -hw, -hw,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, // Bottom-left
        
        // Left face (2)
        -hw,  hw,  hw,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f, // Top-front
        -hw,  hw, -hw,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Top-back
        -hw, -hw, -hw,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, // Bottom-back
        -hw, -hw, -hw,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, // Bottom-back
        -hw, -hw,  hw,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Bottom-front
        -hw,  hw,  hw,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f, // Top-front
        
        // Right face (3)
         hw,  hw,  hw,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Top-front
         hw,  hw, -hw,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f, // Top-back
         hw, -hw, -hw,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Bottom-back
         hw, -hw, -hw,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Bottom-back
         hw, -hw,  hw,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, // Bottom-front
         hw,  hw,  hw,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Top-front
        
        // Bottom face (4)
        -hw, -hw, -hw,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f, // Top-right
         hw, -hw, -hw,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f, // Top-left
         hw, -hw,  hw,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, // Bottom-left
         hw, -hw,  hw,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, // Bottom-left
        -hw, -hw,  hw,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f, // Bottom-right
        -hw, -hw, -hw,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f, // Top-right
        
        // Top face (5)
        -hw,  hw, -hw,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f, // Top-left
         hw,  hw, -hw,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f, // Top-right
         hw,  hw,  hw,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, // Bottom-right
         hw,  hw,  hw,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, // Bottom-right
        -hw,  hw,  hw,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f, // Bottom-left
        -hw,  hw, -hw,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f  // Top-left
    };
    
    // Create VAO and VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    // Bind VAO
    glBindVertexArray(m_VAO);
    
    // Bind VBO and copy vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Configure vertex attributes
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Unbind VAO to prevent accidental modifications
    glBindVertexArray(0);
    
    m_meshCreated = true;
}

void Block::draw(unsigned int shaderID) const {
    if (!m_meshCreated) {
        std::cerr << "Cannot draw block: mesh not created" << std::endl;
        return;
    }
    
    // Bind VAO
    glBindVertexArray(m_VAO);
    
    // Render each face with its correct texture
    for (int face = 0; face < 6; face++) {
        // Activate texture unit and bind the texture for this face
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[face]);
        
        // Set the texture sampler uniform to use texture unit 0
        // Use "blockTexture" which is the actual uniform name in your shader
        GLint textureSamplerLocation = glGetUniformLocation(shaderID, "blockTexture");
        if (textureSamplerLocation >= 0) {
            glUniform1i(textureSamplerLocation, 0);
        } else {
            // Fallback to other common names if needed
            static bool warnedOnce = false;
            if (!warnedOnce) {
                std::cerr << "Warning: Could not find 'blockTexture' uniform in shader, trying alternatives" << std::endl;
                warnedOnce = true;
            }
            
            // Try common alternative names for texture sampler
            const char* alternativeNames[] = {"texture_diffuse", "diffuseTexture", "material.diffuse", "texture1"};
            for (const char* name : alternativeNames) {
                textureSamplerLocation = glGetUniformLocation(shaderID, name);
                if (textureSamplerLocation >= 0) {
                    glUniform1i(textureSamplerLocation, 0);
                    break;
                }
            }
            
            if (textureSamplerLocation < 0) {
                static bool errorReported = false;
                if (!errorReported) {
                    std::cerr << "Error: Could not find any texture sampler uniform in shader" << std::endl;
                    errorReported = true;
                }
            }
        }
        
        // Draw the appropriate vertices for this face (6 vertices per face)
        glDrawArrays(GL_TRIANGLES, face * 6, 6);
    }
    
    // Unbind VAO
    glBindVertexArray(0);
}

void Block::setHalfWidth(float halfWidth) {
    // Only update if value has changed
    if (m_halfWidth != halfWidth) {
        m_halfWidth = halfWidth;
        
        // Recreate mesh with new dimensions
        if (m_meshCreated) {
            createMesh();
        }
    }
}

unsigned int Block::getTextureId(BlockFace face) const {
    // Map enum to array index
    int index;
    switch (face) {
        case BlockFace::TOP:    index = 5; break;
        case BlockFace::BOTTOM: index = 4; break;
        case BlockFace::FRONT:  index = 0; break;
        case BlockFace::BACK:   index = 1; break;
        case BlockFace::LEFT:   index = 2; break;
        case BlockFace::RIGHT:  index = 3; break;
        default:                index = 0; break;
    }
    
    return m_textureIds[index];
}

void Block::loadTextures() {
    // Load texture for each face
    for (int i = 0; i < 6; i++) {
        // Delete previous texture if it exists
        if (m_textureIds[i] != 0) {
            glDeleteTextures(1, &m_textureIds[i]);
        }
        
        // Load the texture from file
        m_textureIds[i] = loadTextureFromFile(m_texturePaths[i]);
    }
}

unsigned int Block::loadTextureFromFile(const std::string& path) const {
    // Generate texture ID
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    // Skip loading if path is empty
    if (path.empty()) {
        std::cerr << "Empty texture path, using default texture" << std::endl;
        unsigned char defaultData[] = {255, 0, 255, 255}; // Pink for debugging
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, defaultData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return textureID;
    }
    
    // Load image
    std::cout << "Loading texture: " << path << std::endl;
    int width, height, nrChannels;
    
    // Set stbi to flip images vertically (OpenGL expects 0,0 to be at bottom left)
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cerr << "Unsupported number of channels: " << nrChannels << " in texture: " << path << std::endl;
            format = GL_RGB;
        }
        
        std::cout << "Texture loaded successfully: " << path << " (" << width << "x" << height 
                  << ", " << nrChannels << " channels)" << std::endl;
        
        // Bind and set texture parameters
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Free image data
        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << path << " - " << stbi_failure_reason() << std::endl;
        // Generate a default texture (pink)
        unsigned char defaultData[] = {255, 0, 255, 255};
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, defaultData);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    return textureID;
}

} // namespace Zenith