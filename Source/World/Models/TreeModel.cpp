#include "TreeModel.h"
#include <iostream>
#include <chrono>

namespace Zenith {

TreeModel::TreeModel(int maxHeight, int maxWidth)
    : BaseModel(maxWidth, maxHeight, maxWidth), 
      m_hasCustomSeed(false)
{
    // Seed the random number generator with the current time
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_rng.seed(seed);
}

void TreeModel::setRandomSeed(unsigned int seed) {
    m_rng.seed(seed);
    m_hasCustomSeed = true;
}

void TreeModel::generateTree(TreeType type, int height) {
    // Clear any existing tree data
    clear();
    
    // If no height specified, generate a random height based on tree type
    if (height <= 0) {
        switch (type) {
            case TreeType::OAK:
                height = randomInt(4, 6);
                break;
            case TreeType::SPRUCE:
                height = randomInt(7, 11);
                break;
            case TreeType::BIRCH:
                height = randomInt(5, 7);
                break;
            case TreeType::JUNGLE:
                height = randomInt(8, 12);
                break;
            case TreeType::ACACIA:
                height = randomInt(5, 8);
                break;
            case TreeType::DARK_OAK:
                height = randomInt(6, 9);
                break;
        }
    }
    
    // Make sure the tree fits within our model's dimensions
    int maxAllowedHeight = m_height - 2; // Leave some margin
    if (height > maxAllowedHeight) {
        height = maxAllowedHeight;
    }
    
    // Generate the appropriate type of tree
    switch (type) {
        case TreeType::OAK:
            generateOakTree(height);
            break;
        case TreeType::SPRUCE:
            generateSpruceTree(height);
            break;
        case TreeType::BIRCH:
            generateBirchTree(height);
            break;
        case TreeType::JUNGLE:
            generateJungleTree(height);
            break;
        case TreeType::ACACIA:
            generateAcaciaTree(height);
            break;
        case TreeType::DARK_OAK:
            generateDarkOakTree(height);
            break;
    }
}

void TreeModel::generateOakTree(int height) {
    // Base dimensions - center the tree in the model
    int centerX = m_width / 2;
    int centerZ = m_depth / 2;
    int baseY = 0;  // Start from the ground
    
    std::string woodType = getWoodType(TreeType::OAK);
    std::string leavesType = getLeavesType(TreeType::OAK);
    
    // Generate the trunk
    for (int y = baseY; y < baseY + height; y++) {
        addVoxel(centerX, y, centerZ, woodType);
    }
    
    // Generate the leaves (spherical shape)
    int leavesRadius = 2;
    int leavesBottom = baseY + height - 3; // Start leaves a bit below the top
    int leavesTop = baseY + height + 1;    // Extend leaves above the top
    
    for (int y = leavesBottom; y <= leavesTop; y++) {
        for (int x = centerX - leavesRadius; x <= centerX + leavesRadius; x++) {
            for (int z = centerZ - leavesRadius; z <= centerZ + leavesRadius; z++) {
                // Calculate distance from the trunk for this leaf block
                float dx = x - centerX;
                float dy = y - (leavesBottom + (leavesTop - leavesBottom) / 2.0f);
                float dz = z - centerZ;
                float distance = std::sqrt(dx*dx + dy*dy*1.5f + dz*dz);
                
                // If within the spherical leaf radius, add a leaf block
                if (distance <= leavesRadius + 0.5f) {
                    // Don't overwrite the trunk
                    if (!(x == centerX && z == centerZ && y < baseY + height)) {
                        addVoxel(x, y, z, leavesType);
                    }
                }
            }
        }
    }
}

void TreeModel::generateSpruceTree(int height) {
    // Base dimensions - center the tree in the model
    int centerX = m_width / 2;
    int centerZ = m_depth / 2;
    int baseY = 0;  // Start from the ground
    
    std::string woodType = getWoodType(TreeType::SPRUCE);
    std::string leavesType = getLeavesType(TreeType::SPRUCE);
    
    // Generate the trunk
    for (int y = baseY; y < baseY + height; y++) {
        addVoxel(centerX, y, centerZ, woodType);
    }
    
    // Generate the leaves (conical shape)
    int baseRadius = 3; // Radius at the bottom of the cone
    int topOffset = 2;  // How far the top of the cone extends above the trunk
    int leavesBottom = baseY + height / 3; // Start leaves a third up the trunk
    int leavesTop = baseY + height + topOffset;
    
    for (int y = leavesBottom; y <= leavesTop; y++) {
        // Calculate radius at this height level (linear decrease from bottom to top)
        float levelRatio = 1.0f - (float)(y - leavesBottom) / (leavesTop - leavesBottom);
        int levelRadius = std::max(0, (int)(baseRadius * levelRatio));
        
        for (int x = centerX - levelRadius; x <= centerX + levelRadius; x++) {
            for (int z = centerZ - levelRadius; z <= centerZ + levelRadius; z++) {
                // Calculate distance from trunk for this block
                float dx = x - centerX;
                float dz = z - centerZ;
                float distance = std::sqrt(dx*dx + dz*dz);
                
                // If within the circular radius at this level, add a leaf block
                if (distance <= levelRadius) {
                    // Don't overwrite the trunk
                    if (!(x == centerX && z == centerZ)) {
                        addVoxel(x, y, z, leavesType);
                    }
                }
            }
        }
    }
    
    // Add a single leaf on top of the tree
    addVoxel(centerX, leavesTop + 1, centerZ, leavesType);
}

void TreeModel::generateBirchTree(int height) {
    // Similar to oak but with a more slender profile
    int centerX = m_width / 2;
    int centerZ = m_depth / 2;
    int baseY = 0;
    
    std::string woodType = getWoodType(TreeType::BIRCH);
    std::string leavesType = getLeavesType(TreeType::BIRCH);
    
    // Generate the trunk
    for (int y = baseY; y < baseY + height; y++) {
        addVoxel(centerX, y, centerZ, woodType);
    }
    
    // Generate the leaves (smaller spherical shape)
    int leavesRadius = 2;
    int leavesBottom = baseY + height - 2;
    int leavesTop = baseY + height;
    
    for (int y = leavesBottom; y <= leavesTop; y++) {
        for (int x = centerX - leavesRadius; x <= centerX + leavesRadius; x++) {
            for (int z = centerZ - leavesRadius; z <= centerZ + leavesRadius; z++) {
                float dx = x - centerX;
                float dy = y - (leavesBottom + (leavesTop - leavesBottom) / 2.0f);
                float dz = z - centerZ;
                float distance = std::sqrt(dx*dx + dy*dy*2.0f + dz*dz);
                
                if (distance <= leavesRadius) {
                    if (!(x == centerX && z == centerZ && y < baseY + height)) {
                        addVoxel(x, y, z, leavesType);
                    }
                }
            }
        }
    }
}

void TreeModel::generateJungleTree(int height) {
    // Jungle trees have a large canopy
    int centerX = m_width / 2;
    int centerZ = m_depth / 2;
    int baseY = 0;
    
    std::string woodType = getWoodType(TreeType::JUNGLE);
    std::string leavesType = getLeavesType(TreeType::JUNGLE);
    
    // Generate the trunk
    for (int y = baseY; y < baseY + height; y++) {
        addVoxel(centerX, y, centerZ, woodType);
    }
    
    // Generate the leaves (large canopy)
    int leavesRadius = 3;
    int leavesBottom = baseY + height - 4;
    int leavesTop = baseY + height + 1;
    
    for (int y = leavesBottom; y <= leavesTop; y++) {
        for (int x = centerX - leavesRadius; x <= centerX + leavesRadius; x++) {
            for (int z = centerZ - leavesRadius; z <= centerZ + leavesRadius; z++) {
                float dx = x - centerX;
                float dy = y - (leavesBottom + (leavesTop - leavesBottom) / 2.0f);
                float dz = z - centerZ;
                float distance = std::sqrt(dx*dx + dy*dy + dz*dz);
                
                // Add some randomness to the leaf placement for a more natural look
                if (distance <= leavesRadius + 0.5f && randomInt(0, 10) > 2) {
                    if (!(x == centerX && z == centerZ && y < baseY + height)) {
                        addVoxel(x, y, z, leavesType);
                    }
                }
            }
        }
    }
    
    // Add some hanging leaves/vines (just a visual effect)
    for (int x = centerX - leavesRadius; x <= centerX + leavesRadius; x++) {
        for (int z = centerZ - leavesRadius; z <= centerZ + leavesRadius; z++) {
            if (getBlockType(x, leavesBottom, z) == leavesType && randomInt(0, 10) > 6) {
                // Add a hanging leaf/vine
                int hangLength = randomInt(1, 2);
                for (int y = leavesBottom - 1; y >= leavesBottom - hangLength && y >= 0; y--) {
                    if (getBlockType(x, y, z).empty()) {
                        addVoxel(x, y, z, leavesType);
                    }
                }
            }
        }
    }
}

void TreeModel::generateAcaciaTree(int height) {
    // Acacia trees have a distinctive Y-shaped trunk and a flat canopy
    int centerX = m_width / 2;
    int centerZ = m_depth / 2;
    int baseY = 0;
    
    std::string woodType = getWoodType(TreeType::ACACIA);
    std::string leavesType = getLeavesType(TreeType::ACACIA);
    
    // Generate the main trunk
    int trunkHeight = height - 2;
    for (int y = baseY; y < baseY + trunkHeight; y++) {
        addVoxel(centerX, y, centerZ, woodType);
    }
    
    // Generate the Y-shaped branches
    int branchDir1X = randomInt(0, 1) == 0 ? -1 : 1;
    int branchDir1Z = randomInt(0, 1) == 0 ? -1 : 1;
    int branchDir2X = -branchDir1X;
    int branchDir2Z = -branchDir1Z;
    
    // First branch
    int branch1X = centerX;
    int branch1Z = centerZ;
    for (int i = 0; i < 3; i++) {
        branch1X += branchDir1X;
        branch1Z += branchDir1Z;
        int branchY = baseY + trunkHeight + i;
        addVoxel(branch1X, branchY, branch1Z, woodType);
    }
    
    // Second branch
    int branch2X = centerX;
    int branch2Z = centerZ;
    for (int i = 0; i < 3; i++) {
        branch2X += branchDir2X;
        branch2Z += branchDir2Z;
        int branchY = baseY + trunkHeight + i;
        addVoxel(branch2X, branchY, branch2Z, woodType);
    }
    
    // Generate the flat canopy above each branch
    generateAcaciaCanopy(branch1X, baseY + trunkHeight + 2, branch1Z, leavesType);
    generateAcaciaCanopy(branch2X, baseY + trunkHeight + 2, branch2Z, leavesType);
}

// Helper for Acacia tree generation
void TreeModel::generateAcaciaCanopy(int centerX, int centerY, int centerZ, const std::string& leavesType) {
    int canopyRadius = 2;
    
    for (int x = centerX - canopyRadius; x <= centerX + canopyRadius; x++) {
        for (int z = centerZ - canopyRadius; z <= centerZ + canopyRadius; z++) {
            float dx = x - centerX;
            float dz = z - centerZ;
            float distance = std::sqrt(dx*dx + dz*dz);
            
            if (distance <= canopyRadius + 0.5f) {
                // Add a flat layer of leaves
                addVoxel(x, centerY, z, leavesType);
                
                // Add some scattered leaves above and below
                if (randomInt(0, 10) > 7) {
                    addVoxel(x, centerY + 1, z, leavesType);
                }
                if (randomInt(0, 10) > 8) {
                    addVoxel(x, centerY - 1, z, leavesType);
                }
            }
        }
    }
}

void TreeModel::generateDarkOakTree(int height) {
    // Dark oak trees have thick trunks (2x2) and a large canopy
    int centerX = m_width / 2;
    int centerZ = m_depth / 2;
    int baseY = 0;
    
    std::string woodType = getWoodType(TreeType::DARK_OAK);
    std::string leavesType = getLeavesType(TreeType::DARK_OAK);
    
    // Generate the thick trunk (2x2)
    for (int y = baseY; y < baseY + height; y++) {
        addVoxel(centerX, y, centerZ, woodType);
        addVoxel(centerX + 1, y, centerZ, woodType);
        addVoxel(centerX, y, centerZ + 1, woodType);
        addVoxel(centerX + 1, y, centerZ + 1, woodType);
    }
    
    // Generate the large, dense canopy
    int leavesRadius = 4;
    int leavesBottom = baseY + height - 3;
    int leavesTop = baseY + height + 1;
    
    // Offset the center of the canopy to account for the 2x2 trunk
    float canopyCenterX = centerX + 0.5f;
    float canopyCenterZ = centerZ + 0.5f;
    
    for (int y = leavesBottom; y <= leavesTop; y++) {
        for (int x = centerX - leavesRadius; x <= centerX + leavesRadius + 1; x++) {
            for (int z = centerZ - leavesRadius; z <= centerZ + leavesRadius + 1; z++) {
                float dx = x - canopyCenterX;
                float dy = y - (leavesBottom + (leavesTop - leavesBottom) / 2.0f);
                float dz = z - canopyCenterZ;
                float distance = std::sqrt(dx*dx + dy*dy*1.2f + dz*dz);
                
                // Add randomness to make a more natural, less perfect shape
                float randomOffset = (randomInt(0, 100) / 100.0f) * 0.8f;
                
                if (distance <= leavesRadius - randomOffset) {
                    // Skip if it's the trunk position
                    if (!(x >= centerX && x <= centerX + 1 &&
                          z >= centerZ && z <= centerZ + 1 &&
                          y < baseY + height)) {
                        addVoxel(x, y, z, leavesType);
                    }
                }
            }
        }
    }
}

std::string TreeModel::getWoodType(TreeType type) const {
    switch (type) {
        case TreeType::OAK:
            return "WOOD_OAK";
        case TreeType::SPRUCE:
            return "WOOD_SPRUCE";
        case TreeType::BIRCH:
            return "WOOD_BIRCH";
        case TreeType::JUNGLE:
            return "WOOD_JUNGLE";
        case TreeType::ACACIA:
            return "WOOD_ACACIA";
        case TreeType::DARK_OAK:
            return "WOOD_BIG_OAK";
        default:
            return "WOOD_OAK";
    }
}

std::string TreeModel::getLeavesType(TreeType type) const {
    switch (type) {
        case TreeType::OAK:
            return "LEAVES_OAK";
        case TreeType::SPRUCE:
            return "LEAVES_SPRUCE";
        case TreeType::BIRCH:
            return "LEAVES_BIRCH";
        case TreeType::JUNGLE:
            return "LEAVES_JUNGLE";
        case TreeType::ACACIA:
            return "LEAVES_ACACIA";
        case TreeType::DARK_OAK:
            return "LEAVES_BIG_OAK";
        default:
            return "LEAVES_OAK";
    }
}

int TreeModel::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(m_rng);
}

} // namespace Zenith