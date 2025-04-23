// filepath: /home/anurag/Codes/Learnings/opengl/Zenith/Source/World/Models/HutModel.h
#ifndef HUT_MODEL_H
#define HUT_MODEL_H

#include "BaseModel.h"
#include <random>

namespace Zenith {

// Enum for different hut types
enum class HutType {
    BASIC,          // Simple square hut
    ROUND,          // Round hut with conical roof
    LONGHOUSE,      // Rectangular longhouse style
    TIERED          // Multi-tiered hut with multiple roofs
};

class HutModel : public BaseModel {
public:
    // Constructor
    HutModel(int maxWidth, int maxHeight, int maxDepth);
    
    // Generate a hut at the given position
    void generateHut(HutType type, bool withFurnishings = true);
    
    // Set random seed for hut generation
    void setRandomSeed(unsigned int seed);
    
private:
    // Random number generator
    mutable std::mt19937 m_rng;
    bool m_hasCustomSeed;
    
    // Helper methods for hut generation
    void generateBasicHut(bool withFurnishings);
    void generateRoundHut(bool withFurnishings);
    void generateLonghouse(bool withFurnishings);
    void generateTieredHut(bool withFurnishings);
    
    // Helper methods for common hut features
    void addDoor(int x, int y, int z, int facingDirection);
    void addWindow(int x, int y, int z, int facingDirection);
    void addFurnishings(int centerX, int centerY, int centerZ, int width, int depth, HutType type);
    
    // Generate a flat roof
    void generateFlatRoof(int startX, int startY, int startZ, int width, int depth, const std::string& roofMaterial);
    
    // Generate a pitched roof
    void generatePitchedRoof(int startX, int startY, int startZ, int width, int depth, const std::string& roofMaterial);
    
    // Generate a conical roof
    void generateConicalRoof(int centerX, int startY, int centerZ, int radius, int height, const std::string& roofMaterial);
    
    // Helper to get block type strings based on wall, floor and roof materials
    std::string getWallMaterial() const;
    std::string getFloorMaterial() const;
    std::string getRoofMaterial() const;
    
    // Generate a random number between min and max (inclusive)
    int randomInt(int min, int max) const;
};

} // namespace Zenith

#endif // HUT_MODEL_H