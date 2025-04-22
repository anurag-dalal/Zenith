#include "BaseModel.h"
#include <iostream>

namespace Zenith {

BaseModel::BaseModel(int p, int q, int r)
    : m_width(p), m_height(q), m_depth(r), m_position(0.0f, 0.0f, 0.0f)
{
    // Initialize with empty dimensions
}

bool BaseModel::addVoxel(int x, int y, int z, const std::string& blockType) {
    if (!isWithinBounds(x, y, z)) {
        std::cerr << "Error: Attempted to add voxel outside model bounds (" << x << ", " << y << ", " << z << ")" << std::endl;
        return false;
    }
    
    // Add or update the block at this position
    m_blocks[VoxelPosition(x, y, z)] = blockType;
    return true;
}

bool BaseModel::removeVoxel(int x, int y, int z) {
    if (!isWithinBounds(x, y, z)) {
        return false;
    }
    
    auto it = m_blocks.find(VoxelPosition(x, y, z));
    if (it != m_blocks.end()) {
        m_blocks.erase(it);
        // Also remove the voxel object if it exists
        m_voxels.erase(VoxelPosition(x, y, z));
        return true;
    }
    
    return false; // No voxel at this position
}

bool BaseModel::isWithinBounds(int x, int y, int z) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height && z >= 0 && z < m_depth;
}

std::string BaseModel::getBlockType(int x, int y, int z) const {
    if (!isWithinBounds(x, y, z)) {
        return ""; // Return empty string for out-of-bounds positions
    }
    
    auto it = m_blocks.find(VoxelPosition(x, y, z));
    if (it != m_blocks.end()) {
        return it->second;
    }
    
    return ""; // No block at this position
}

void BaseModel::setPosition(const glm::vec3& position) {
    m_position = position;
}

glm::vec3 BaseModel::getPosition() const {
    return m_position;
}

void BaseModel::getDimensions(int& p, int& q, int& r) const {
    p = m_width;
    q = m_height;
    r = m_depth;
}

bool BaseModel::createVoxelObjects(const BlockRegistryReader& blockRegistry) {
    // Clear any existing voxel objects
    m_voxels.clear();
    
    // Iterate through all blocks in the model
    for (const auto& [pos, blockType] : m_blocks) {
        // Skip AIR blocks
        if (blockType == "AIR") {
            continue;
        }
        
        // Get textures for this block type
        const BlockTextures* textures = blockRegistry.getBlockTextures(blockType);
        if (!textures) {
            std::cerr << "Error: Could not find textures for block type: " << blockType << std::endl;
            continue;
        }
        
        // Create a voxel object for this block
        std::shared_ptr<Voxel> voxel = Voxel::create(
            textures->top,
            textures->bottom,
            textures->front,
            textures->back,
            textures->left,
            textures->right
        );
        
        if (!voxel) {
            std::cerr << "Error: Failed to create voxel for block type: " << blockType << std::endl;
            continue;
        }
        
        // Position the voxel at its grid position plus the model's offset
        // Note: Each voxel is 1x1x1 unit, so we can use grid coordinates directly
        voxel->setPosition(glm::vec3(
            m_position.x + static_cast<float>(pos.x),
            m_position.y + static_cast<float>(pos.y),
            m_position.z + static_cast<float>(pos.z)
        ));
        
        // Store the voxel object
        m_voxels[pos] = voxel;
    }
    
    return true;
}

void BaseModel::render(const glm::mat4& view, const glm::mat4& projection, 
                       const glm::vec3& lightDir, const glm::vec3& lightColor, 
                       const glm::vec3& viewPos) {
    // Identity model matrix (position is already set for each voxel)
    glm::mat4 model = glm::mat4(1.0f);
    
    // Render each voxel
    for (const auto& [pos, voxel] : m_voxels) {
        voxel->render(model, view, projection, lightDir, lightColor, viewPos);
    }
}

void BaseModel::clear() {
    m_blocks.clear();
    m_voxels.clear();
}

size_t BaseModel::getVoxelCount() const {
    return m_blocks.size();
}

std::vector<VoxelPosition> BaseModel::getOccupiedPositions() const {
    std::vector<VoxelPosition> positions;
    positions.reserve(m_blocks.size());
    
    for (const auto& [pos, blockType] : m_blocks) {
        positions.push_back(pos);
    }
    
    return positions;
}

} // namespace Zenith