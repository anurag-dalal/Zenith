#ifndef BASE_MODEL_H
#define BASE_MODEL_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Blocks/BlockRegistryReader.h"
#include "Blocks/Voxel.h"

namespace Zenith {

// Structure to represent a position in 3D space
struct VoxelPosition {
    int x, y, z;
    
    // Constructor
    VoxelPosition(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
    
    // Hash function for VoxelPosition to be used in unordered_map
    struct Hash {
        std::size_t operator()(const VoxelPosition& pos) const {
            // Simple hash combining the coordinates
            std::size_t h1 = std::hash<int>{}(pos.x);
            std::size_t h2 = std::hash<int>{}(pos.y);
            std::size_t h3 = std::hash<int>{}(pos.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
    
    // Equality operator for VoxelPosition
    bool operator==(const VoxelPosition& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

class BaseModel {
public:
    // Constructor: Initialize a model with dimensions p x q x r
    BaseModel(int p, int q, int r);
    virtual ~BaseModel() = default;
    
    // Add a voxel at the specified position with the specified type
    bool addVoxel(int x, int y, int z, const std::string& blockType);
    
    // Remove a voxel at the specified position
    bool removeVoxel(int x, int y, int z);
    
    // Check if a position is within the model's bounds
    bool isWithinBounds(int x, int y, int z) const;
    
    // Get the block type at the specified position
    std::string getBlockType(int x, int y, int z) const;
    
    // Set the position of the model in 3D space
    void setPosition(const glm::vec3& position);
    
    // Get the position of the model
    glm::vec3 getPosition() const;
    
    // Get the dimensions of the model
    void getDimensions(int& p, int& q, int& r) const;
    
    // Create voxel objects for rendering
    bool createVoxelObjects(const BlockRegistryReader& blockRegistry);
    
    // Render the model
    void render(const glm::mat4& view, const glm::mat4& projection, 
                const glm::vec3& lightDir, const glm::vec3& lightColor, 
                const glm::vec3& viewPos);
    
    // Clear all voxels
    void clear();
    
    // Get the number of voxels in the model
    size_t getVoxelCount() const;
    
    // Get all occupied positions
    std::vector<VoxelPosition> getOccupiedPositions() const;
    
protected:
    // Dimensions of the model
    int m_width;  // p
    int m_height; // q
    int m_depth;  // r
    
    // Position of the model in 3D space
    glm::vec3 m_position;
    
    // Map from position to block type
    std::unordered_map<VoxelPosition, std::string, VoxelPosition::Hash> m_blocks;
    
    // Map from position to voxel object for rendering
    std::unordered_map<VoxelPosition, std::shared_ptr<Voxel>, VoxelPosition::Hash> m_voxels;
};

} // namespace Zenith

#endif // BASE_MODEL_H