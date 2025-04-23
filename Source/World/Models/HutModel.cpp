// filepath: /home/anurag/Codes/Learnings/opengl/Zenith/Source/World/Models/HutModel.cpp
#include "HutModel.h"
#include <iostream>
#include <chrono>
#include <cmath>

namespace Zenith {

HutModel::HutModel(int maxWidth, int maxHeight, int maxDepth)
    : BaseModel(maxWidth, maxHeight, maxDepth), 
      m_hasCustomSeed(false)
{
    // Seed the random number generator with the current time
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_rng.seed(seed);
}

void HutModel::setRandomSeed(unsigned int seed) {
    m_rng.seed(seed);
    m_hasCustomSeed = true;
}

void HutModel::generateHut(HutType type, bool withFurnishings) {
    // Clear any existing model data
    clear();
    
    // Generate the appropriate type of hut
    switch (type) {
        case HutType::BASIC:
            generateBasicHut(withFurnishings);
            break;
        case HutType::ROUND:
            generateRoundHut(withFurnishings);
            break;
        case HutType::LONGHOUSE:
            generateLonghouse(withFurnishings);
            break;
        case HutType::TIERED:
            generateTieredHut(withFurnishings);
            break;
    }
}

void HutModel::generateBasicHut(bool withFurnishings) {
    // Dimensions for a small square hut
    int width = 7;
    int depth = 7;
    int height = 5;
    
    // Base dimensions - center the hut in the model
    int startX = (m_width - width) / 2;
    int startZ = (m_depth - depth) / 2;
    int startY = 0;  // Start from the ground
    
    std::string wallMaterial = getWallMaterial();
    std::string floorMaterial = getFloorMaterial();
    std::string roofMaterial = getRoofMaterial();
    
    // Create the floor
    for (int x = startX; x < startX + width; x++) {
        for (int z = startZ; z < startZ + depth; z++) {
            addVoxel(x, startY, z, floorMaterial);
        }
    }
    
    // Create the walls
    for (int y = startY + 1; y < startY + height; y++) {
        for (int x = startX; x < startX + width; x++) {
            // Front and back walls
            addVoxel(x, y, startZ, wallMaterial);
            addVoxel(x, y, startZ + depth - 1, wallMaterial);
        }
        
        for (int z = startZ + 1; z < startZ + depth - 1; z++) {
            // Left and right walls
            addVoxel(startX, y, z, wallMaterial);
            addVoxel(startX + width - 1, y, z, wallMaterial);
        }
    }
    
    // Add a door in the middle of the front wall
    int doorX = startX + width / 2;
    int doorZ = startZ;
    // Remove blocks for door
    removeVoxel(doorX, startY + 1, doorZ);
    removeVoxel(doorX, startY + 2, doorZ);
    
    // Add windows
    int windowHeight = startY + 3;
    // Front windows
    addWindow(startX + 2, windowHeight, startZ, 0);
    addWindow(startX + width - 3, windowHeight, startZ, 0);
    // Back window
    addWindow(startX + width / 2, windowHeight, startZ + depth - 1, 2);
    // Side windows
    addWindow(startX, windowHeight, startZ + depth / 2, 3);
    addWindow(startX + width - 1, windowHeight, startZ + depth / 2, 1);
    
    // Generate a pitched roof
    generatePitchedRoof(startX, startY + height, startZ, width, depth, roofMaterial);
    
    // Add furnishings if requested
    if (withFurnishings) {
        addFurnishings(startX + width / 2, startY, startZ + depth / 2, width, depth, HutType::BASIC);
    }
}

void HutModel::generateRoundHut(bool withFurnishings) {
    // Dimensions for a round hut
    int radius = 5;
    int height = 5;
    
    // Base dimensions - center the hut in the model
    int centerX = m_width / 2;
    int centerZ = m_depth / 2;
    int startY = 0;  // Start from the ground
    
    std::string wallMaterial = getWallMaterial();
    std::string floorMaterial = getFloorMaterial();
    std::string roofMaterial = getRoofMaterial();
    
    // Create the circular floor
    for (int x = centerX - radius; x <= centerX + radius; x++) {
        for (int z = centerZ - radius; z <= centerZ + radius; z++) {
            float dx = x - centerX;
            float dz = z - centerZ;
            float distance = std::sqrt(dx*dx + dz*dz);
            
            if (distance <= radius) {
                addVoxel(x, startY, z, floorMaterial);
            }
        }
    }
    
    // Create the circular walls
    for (int y = startY + 1; y < startY + height; y++) {
        for (int x = centerX - radius; x <= centerX + radius; x++) {
            for (int z = centerZ - radius; z <= centerZ + radius; z++) {
                float dx = x - centerX;
                float dz = z - centerZ;
                float distance = std::sqrt(dx*dx + dz*dz);
                
                // Create a hollow circle for the walls (use 0.5 tolerance)
                if (distance <= radius && distance >= radius - 1) {
                    addVoxel(x, y, z, wallMaterial);
                }
            }
        }
    }
    
    // Add a door to the south
    int doorZ = centerZ - radius;
    // Remove blocks for door
    removeVoxel(centerX, startY + 1, doorZ);
    removeVoxel(centerX, startY + 2, doorZ);
    
    // Add windows around the hut
    int windowHeight = startY + 3;
    // Add windows at cardinal directions
    addWindow(centerX, windowHeight, centerZ + radius, 2); // North
    addWindow(centerX + radius, windowHeight, centerZ, 1); // East
    addWindow(centerX - radius, windowHeight, centerZ, 3); // West
    
    // Generate a conical roof
    generateConicalRoof(centerX, startY + height, centerZ, radius, 4, roofMaterial);
    
    // Add furnishings if requested
    if (withFurnishings) {
        addFurnishings(centerX, startY, centerZ, radius * 2, radius * 2, HutType::ROUND);
    }
}

void HutModel::generateLonghouse(bool withFurnishings) {
    // Dimensions for a longhouse
    int width = 7;
    int depth = 15;
    int height = 6;
    
    // Base dimensions - center the hut in the model
    int startX = (m_width - width) / 2;
    int startZ = (m_depth - depth) / 2;
    int startY = 0;  // Start from the ground
    
    std::string wallMaterial = getWallMaterial();
    std::string floorMaterial = getFloorMaterial();
    std::string roofMaterial = getRoofMaterial();
    
    // Create the floor
    for (int x = startX; x < startX + width; x++) {
        for (int z = startZ; z < startZ + depth; z++) {
            addVoxel(x, startY, z, floorMaterial);
        }
    }
    
    // Create the walls
    for (int y = startY + 1; y < startY + height; y++) {
        for (int x = startX; x < startX + width; x++) {
            // Front and back walls
            addVoxel(x, y, startZ, wallMaterial);
            addVoxel(x, y, startZ + depth - 1, wallMaterial);
        }
        
        for (int z = startZ + 1; z < startZ + depth - 1; z++) {
            // Left and right walls
            addVoxel(startX, y, z, wallMaterial);
            addVoxel(startX + width - 1, y, z, wallMaterial);
        }
    }
    
    // Add doors on both ends
    int doorX1 = startX + width / 2;
    int doorZ1 = startZ;
    int doorX2 = startX + width / 2;
    int doorZ2 = startZ + depth - 1;
    
    // Remove blocks for doors
    removeVoxel(doorX1, startY + 1, doorZ1);
    removeVoxel(doorX1, startY + 2, doorZ1);
    removeVoxel(doorX2, startY + 1, doorZ2);
    removeVoxel(doorX2, startY + 2, doorZ2);
    
    // Add windows along the sides
    int windowHeight = startY + 3;
    for (int z = startZ + 3; z < startZ + depth - 3; z += 3) {
        addWindow(startX, windowHeight, z, 3);
        addWindow(startX + width - 1, windowHeight, z, 1);
    }
    
    // Generate a pitched roof
    generatePitchedRoof(startX, startY + height, startZ, width, depth, roofMaterial);
    
    // Add furnishings if requested
    if (withFurnishings) {
        addFurnishings(startX + width / 2, startY, startZ + depth / 2, width, depth, HutType::LONGHOUSE);
    }
}

void HutModel::generateTieredHut(bool withFurnishings) {
    // Dimensions for a tiered hut
    int baseWidth = 11;
    int baseDepth = 11;
    int midWidth = 7;
    int midDepth = 7;
    int topWidth = 5;
    int topDepth = 5;
    int floorHeight = 4;
    
    // Base dimensions - center the hut in the model
    int startX = (m_width - baseWidth) / 2;
    int startZ = (m_depth - baseDepth) / 2;
    int startY = 0;  // Start from the ground
    
    std::string wallMaterial = getWallMaterial();
    std::string floorMaterial = getFloorMaterial();
    std::string roofMaterial = getRoofMaterial();
    
    // First tier (base)
    // Create the floor
    for (int x = startX; x < startX + baseWidth; x++) {
        for (int z = startZ; z < startZ + baseDepth; z++) {
            addVoxel(x, startY, z, floorMaterial);
        }
    }
    
    // Create the walls for first tier
    for (int y = startY + 1; y < startY + floorHeight; y++) {
        for (int x = startX; x < startX + baseWidth; x++) {
            // Front and back walls
            addVoxel(x, y, startZ, wallMaterial);
            addVoxel(x, y, startZ + baseDepth - 1, wallMaterial);
        }
        
        for (int z = startZ + 1; z < startZ + baseDepth - 1; z++) {
            // Left and right walls
            addVoxel(startX, y, z, wallMaterial);
            addVoxel(startX + baseWidth - 1, y, z, wallMaterial);
        }
    }
    
    // Add a door in the middle of the front wall
    int doorX = startX + baseWidth / 2;
    int doorZ = startZ;
    // Remove blocks for door
    removeVoxel(doorX, startY + 1, doorZ);
    removeVoxel(doorX, startY + 2, doorZ);
    
    // Add windows on the first tier
    int windowHeight = startY + 3;
    // Front windows
    addWindow(startX + 3, windowHeight, startZ, 0);
    addWindow(startX + baseWidth - 4, windowHeight, startZ, 0);
    // Back windows
    addWindow(startX + 3, windowHeight, startZ + baseDepth - 1, 2);
    addWindow(startX + baseWidth - 4, windowHeight, startZ + baseDepth - 1, 2);
    // Side windows
    addWindow(startX, windowHeight, startZ + 3, 3);
    addWindow(startX, windowHeight, startZ + baseDepth - 4, 3);
    addWindow(startX + baseWidth - 1, windowHeight, startZ + 3, 1);
    addWindow(startX + baseWidth - 1, windowHeight, startZ + baseDepth - 4, 1);
    
    // Add first tier flat roof / second tier floor
    for (int x = startX; x < startX + baseWidth; x++) {
        for (int z = startZ; z < startZ + baseDepth; z++) {
            addVoxel(x, startY + floorHeight, z, floorMaterial);
        }
    }
    
    // Second tier
    int midStartX = startX + (baseWidth - midWidth) / 2;
    int midStartZ = startZ + (baseDepth - midDepth) / 2;
    int midStartY = startY + floorHeight;
    
    // Create the walls for second tier
    for (int y = midStartY + 1; y < midStartY + floorHeight; y++) {
        for (int x = midStartX; x < midStartX + midWidth; x++) {
            // Front and back walls
            addVoxel(x, y, midStartZ, wallMaterial);
            addVoxel(x, y, midStartZ + midDepth - 1, wallMaterial);
        }
        
        for (int z = midStartZ + 1; z < midStartZ + midDepth - 1; z++) {
            // Left and right walls
            addVoxel(midStartX, y, z, wallMaterial);
            addVoxel(midStartX + midWidth - 1, y, z, wallMaterial);
        }
    }
    
    // Add windows on the second tier
    windowHeight = midStartY + 2;
    // Windows on each side
    addWindow(midStartX + midWidth / 2, windowHeight, midStartZ, 0);
    addWindow(midStartX + midWidth / 2, windowHeight, midStartZ + midDepth - 1, 2);
    addWindow(midStartX, windowHeight, midStartZ + midDepth / 2, 3);
    addWindow(midStartX + midWidth - 1, windowHeight, midStartZ + midDepth / 2, 1);
    
    // Add second tier flat roof / third tier floor
    for (int x = midStartX; x < midStartX + midWidth; x++) {
        for (int z = midStartZ; z < midStartZ + midDepth; z++) {
            addVoxel(x, midStartY + floorHeight, z, floorMaterial);
        }
    }
    
    // Third tier (top)
    int topStartX = midStartX + (midWidth - topWidth) / 2;
    int topStartZ = midStartZ + (midDepth - topDepth) / 2;
    int topStartY = midStartY + floorHeight;
    
    // Create the walls for third tier
    for (int y = topStartY + 1; y < topStartY + floorHeight - 1; y++) {
        for (int x = topStartX; x < topStartX + topWidth; x++) {
            // Front and back walls
            addVoxel(x, y, topStartZ, wallMaterial);
            addVoxel(x, y, topStartZ + topDepth - 1, wallMaterial);
        }
        
        for (int z = topStartZ + 1; z < topStartZ + topDepth - 1; z++) {
            // Left and right walls
            addVoxel(topStartX, y, z, wallMaterial);
            addVoxel(topStartX + topWidth - 1, y, z, wallMaterial);
        }
    }
    
    // Generate a pitched roof for the top tier
    generatePitchedRoof(topStartX, topStartY + floorHeight - 1, topStartZ, topWidth, topDepth, roofMaterial);
    
    // Add furnishings if requested
    if (withFurnishings) {
        // Add furnishings to the main floor
        addFurnishings(startX + baseWidth / 2, startY, startZ + baseDepth / 2, baseWidth, baseDepth, HutType::TIERED);
    }
}

void HutModel::addWindow(int x, int y, int z, int facingDirection) {
    // Remove the block at the window position
    removeVoxel(x, y, z);
    
    // Add glass pane
    addVoxel(x, y, z, "GLASS");
}

void HutModel::addDoor(int x, int y, int z, int facingDirection) {
    // Simply remove blocks for the door (simplified for this example)
    removeVoxel(x, y, z);
    removeVoxel(x, y + 1, z);
}

void HutModel::addFurnishings(int centerX, int centerY, int centerZ, int width, int depth, HutType type) {
    std::string furnishingMaterial;
    
    // Add a bed
    int bedX = centerX - width / 4;
    int bedZ = centerZ + depth / 4;
    furnishingMaterial = "WOOL_RED";  // Red wool for the bed
    addVoxel(bedX, centerY + 1, bedZ, furnishingMaterial);
    addVoxel(bedX + 1, centerY + 1, bedZ, furnishingMaterial);
    
    // Add a crafting table
    addVoxel(centerX + width / 4, centerY + 1, centerZ - depth / 4, "CRAFTING_TABLE");
    
    // Add a chest (using bookshelf texture as a stand-in)
    addVoxel(centerX - width / 4, centerY + 1, centerZ - depth / 4, "BOOKSHELF");
    
    // Add a furnace
    if (type != HutType::ROUND) {
        // For square huts, place against a wall
        addVoxel(centerX + width / 4, centerY + 1, centerZ + depth / 3, "FURNACE");
    } else {
        // For round huts, place closer to the center
        addVoxel(centerX, centerY + 1, centerZ + depth / 4, "FURNACE");
    }
    
    // Add table (cauldron)
    addVoxel(centerX, centerY + 1, centerZ, "CAULDRON");
    
    // If it's a tiered or longhouse, add more furnishings
    if (type == HutType::TIERED || type == HutType::LONGHOUSE) {
        // Add a jukebox
        addVoxel(centerX - width / 3, centerY + 1, centerZ + depth / 3, "JUKEBOX");
        
        // Add bookshelves
        addVoxel(centerX + width / 3, centerY + 1, centerZ + depth / 3, "BOOKSHELF");
        addVoxel(centerX + width / 3, centerY + 2, centerZ + depth / 3, "BOOKSHELF");
    }
}

void HutModel::generateFlatRoof(int startX, int startY, int startZ, int width, int depth, const std::string& roofMaterial) {
    // Create a simple flat roof
    for (int x = startX; x < startX + width; x++) {
        for (int z = startZ; z < startZ + depth; z++) {
            addVoxel(x, startY, z, roofMaterial);
        }
    }
}

void HutModel::generatePitchedRoof(int startX, int startY, int startZ, int width, int depth, const std::string& roofMaterial) {
    // Create a pitched roof
    int peakHeight = 3;  // Height of the roof peak
    
    for (int y = 0; y < peakHeight; y++) {
        for (int x = startX + y; x < startX + width - y; x++) {
            for (int z = startZ + y; z < startZ + depth - y; z++) {
                // Only add blocks around the perimeter for each level
                if (x == startX + y || x == startX + width - y - 1 ||
                    z == startZ + y || z == startZ + depth - y - 1) {
                    addVoxel(x, startY + y, z, roofMaterial);
                }
            }
        }
    }
    
    // Add the peak blocks
    int centerX = startX + width / 2;
    int centerZ = startZ + depth / 2;
    
    // If width is even, we need two center blocks
    if (width % 2 == 0) {
        addVoxel(centerX - 1, startY + peakHeight, centerZ, roofMaterial);
        addVoxel(centerX, startY + peakHeight, centerZ, roofMaterial);
    } else {
        addVoxel(centerX, startY + peakHeight, centerZ, roofMaterial);
    }
    
    // If depth is even, we need two center blocks in z direction
    if (depth % 2 == 0) {
        addVoxel(centerX, startY + peakHeight, centerZ - 1, roofMaterial);
        // If both width and depth are even, add the diagonal
        if (width % 2 == 0) {
            addVoxel(centerX - 1, startY + peakHeight, centerZ - 1, roofMaterial);
        }
    }
}

void HutModel::generateConicalRoof(int centerX, int startY, int centerZ, int radius, int height, const std::string& roofMaterial) {
    // Create a conical roof
    for (int y = 0; y < height; y++) {
        int currentRadius = radius - (y * radius / height);
        
        for (int x = centerX - currentRadius; x <= centerX + currentRadius; x++) {
            for (int z = centerZ - currentRadius; z <= centerZ + currentRadius; z++) {
                float dx = x - centerX;
                float dz = z - centerZ;
                float distance = std::sqrt(dx*dx + dz*dz);
                
                // Create a circle for each level of the cone
                if (distance <= currentRadius && distance >= currentRadius - 1) {
                    addVoxel(x, startY + y, z, roofMaterial);
                }
            }
        }
    }
    
    // Add the peak block
    addVoxel(centerX, startY + height, centerZ, roofMaterial);
}

std::string HutModel::getWallMaterial() const {
    // Randomly choose a wall material
    int choice = randomInt(0, 4);
    switch (choice) {
        case 0:
            return "PLANKS_OAK";
        case 1:
            return "PLANKS_SPRUCE";
        case 2:
            return "PLANKS_BIRCH";
        case 3:
            return "PLANKS_ACACIA";
        case 4:
            return "STONEBRICK";
        default:
            return "PLANKS_OAK";
    }
}

std::string HutModel::getFloorMaterial() const {
    // Randomly choose a floor material
    int choice = randomInt(0, 3);
    switch (choice) {
        case 0:
            return "PLANKS_OAK";
        case 1:
            return "PLANKS_SPRUCE";
        case 2:
            return "PLANKS_BIRCH";
        case 3:
            return "STONEBRICK";
        default:
            return "PLANKS_OAK";
    }
}

std::string HutModel::getRoofMaterial() const {
    // Randomly choose a roof material
    int choice = randomInt(0, 4);
    switch (choice) {
        case 0:
            return "PLANKS_OAK";
        case 1:
            return "PLANKS_SPRUCE";
        case 2:
            return "PLANKS_BIRCH";
        case 3:
            return "HARDENED_CLAY_RED";
        case 4:
            return "COBBLESTONE";
        default:
            return "PLANKS_OAK";
    }
}

int HutModel::randomInt(int min, int max) const {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(m_rng);
}

} // namespace Zenith