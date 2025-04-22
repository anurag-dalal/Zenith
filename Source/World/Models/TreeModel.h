#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include "BaseModel.h"
#include <random>

namespace Zenith {

// Enum for different tree types
enum class TreeType {
    OAK,
    SPRUCE,
    BIRCH,
    JUNGLE,
    ACACIA,
    DARK_OAK
};

class TreeModel : public BaseModel {
public:
    // Constructor
    TreeModel(int maxHeight, int maxWidth);
    
    // Generate a tree at the given position
    void generateTree(TreeType type, int height = 0);
    
    // Set random seed for tree generation
    void setRandomSeed(unsigned int seed);
    
private:
    // Random number generator
    std::mt19937 m_rng;
    bool m_hasCustomSeed;
    
    // Helper methods for tree generation
    void generateOakTree(int height);
    void generateSpruceTree(int height);
    void generateBirchTree(int height);
    void generateJungleTree(int height);
    void generateAcaciaTree(int height);
    void generateDarkOakTree(int height);
    
    // Helper method for acacia tree canopy generation
    void generateAcaciaCanopy(int centerX, int centerY, int centerZ, const std::string& leavesType);
    
    // Helper to get block type strings based on tree type
    std::string getWoodType(TreeType type) const;
    std::string getLeavesType(TreeType type) const;
    
    // Generate a random number between min and max (inclusive)
    int randomInt(int min, int max);
};

} // namespace Zenith

#endif // TREE_MODEL_H