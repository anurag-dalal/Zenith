#include "BlockRegistryReader.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace Zenith {

BlockRegistryReader::BlockRegistryReader() 
    : m_assetsPath(std::string(ASSETS_DIR) + "/minecraft/textures/blocks/")
    , m_isLoaded(false) 
{
}

bool BlockRegistryReader::loadRegistry() {
    try {
        // Open the BlockRegistry.json file
        std::ifstream file(std::string(CONFIG_DIR) + "/BlockRegistry.json");
        if (!file.is_open()) {
            std::cerr << "Failed to open BlockRegistry.json" << std::endl;
            return false;
        }

        // Parse the JSON file
        nlohmann::json registry;
        file >> registry;

        // Clear any existing data
        m_blockTextures.clear();

        // Check if the JSON has a "blocks" array
        if (registry.contains("blocks") && registry["blocks"].is_array()) {
            // Process each block in the blocks array
            for (const auto& blockData : registry["blocks"]) {
                if (blockData.contains("id") && blockData.contains("textures")) {
                    std::string blockId = blockData["id"].get<std::string>();
                    processBlockEntry(blockId, blockData["textures"]);
                }
            }
        } else {
            // Fallback to the old format where blocks are direct keys
            for (auto it = registry.begin(); it != registry.end(); ++it) {
                processBlockEntry(it.key(), it.value());
            }
        }

        m_isLoaded = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading block registry: " << e.what() << std::endl;
        return false;
    }
}

const BlockTextures* BlockRegistryReader::getBlockTextures(const std::string& blockId) const {
    auto it = m_blockTextures.find(blockId);
    if (it != m_blockTextures.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool BlockRegistryReader::hasBlock(const std::string& blockId) const {
    return m_blockTextures.find(blockId) != m_blockTextures.end();
}

size_t BlockRegistryReader::getBlockCount() const {
    return m_blockTextures.size();
}

void BlockRegistryReader::forEachBlock(const std::function<void(const std::string&, const BlockTextures&)>& callback) const {
    for (const auto& [blockId, textures] : m_blockTextures) {
        callback(blockId, textures);
    }
}

void BlockRegistryReader::processBlockEntry(const std::string& id, const nlohmann::json& blockData) {
    BlockTextures textures;

    // Check if the 'all' property is present, which means all faces use the same texture
    if (blockData.contains("all")) {
        std::string allTexture = blockData["all"].get<std::string>();
        std::string fullPath = buildTexturePath(allTexture);
        
        // Set all faces to the same texture
        textures.top = fullPath;
        textures.bottom = fullPath;
        textures.front = fullPath;
        textures.back = fullPath;
        textures.left = fullPath;
        textures.right = fullPath;
    } else {
        // Process individual face textures
        if (blockData.contains("top")) {
            textures.top = buildTexturePath(blockData["top"].get<std::string>());
        }
        
        if (blockData.contains("bottom")) {
            textures.bottom = buildTexturePath(blockData["bottom"].get<std::string>());
        }
        
        if (blockData.contains("front")) {
            textures.front = buildTexturePath(blockData["front"].get<std::string>());
        }
        
        if (blockData.contains("back")) {
            textures.back = buildTexturePath(blockData["back"].get<std::string>());
        }
        
        if (blockData.contains("left")) {
            textures.left = buildTexturePath(blockData["left"].get<std::string>());
        }
        
        if (blockData.contains("right")) {
            textures.right = buildTexturePath(blockData["right"].get<std::string>());
        }
    }

    // Store the textures in the map
    m_blockTextures[id] = textures;
}

std::string BlockRegistryReader::buildTexturePath(const std::string& texturePath) const {
    std::string fullPath = m_assetsPath + texturePath;
    return fullPath;
}

std::pair<std::string, BlockTextures> BlockRegistryReader::getBlockById(const std::string& blockId) const {
    auto it = m_blockTextures.find(blockId);
    if (it != m_blockTextures.end()) {
        return std::make_pair(blockId, it->second);
    }
    // Return empty pair if block not found
    return std::make_pair("", BlockTextures{});
}

} // namespace Zenith