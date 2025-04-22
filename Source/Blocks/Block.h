#pragma once

#include <string>
#include <array>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "BlockRegistryReader.h"

namespace Zenith {

// Enum for specifying block faces
enum class BlockFace {
    TOP,
    BOTTOM,
    FRONT,
    BACK,
    LEFT,
    RIGHT
};

class Block {
public:
    // Constructor with block ID from registry
    Block(const std::string& blockId, const BlockRegistryReader& registry, float halfWidth = 0.5f);
    
    // Constructor with explicit texture paths
    Block(const std::string& blockId, const std::array<std::string, 6>& texturePaths, float halfWidth = 0.5f);
    
    // Constructor with same texture path for all faces
    Block(const std::string& blockId, const std::string& allFacesTexturePath, float halfWidth = 0.5f);
    
    // Destructor
    ~Block();
    
    // Generate the mesh for the block
    void createMesh();
    
    // Draw the block
    void draw(unsigned int shader) const;
    
    // Set half width of the block
    void setHalfWidth(float halfWidth);
    
    // Get block ID
    const std::string& getBlockId() const;
    
    // Get texture ID for a specific face
    unsigned int getTextureId(BlockFace face) const;
    
    // Load textures for the block
    void loadTextures();
    
private:
    std::string m_blockId;
    float m_halfWidth;
    std::array<std::string, 6> m_texturePaths;
    std::array<unsigned int, 6> m_textureIds;
    unsigned int m_VAO;
    unsigned int m_VBO;
    bool m_meshCreated;
    
    // Helper function to load a texture from file
    unsigned int loadTextureFromFile(const std::string& path) const;
};

} // namespace Zenith