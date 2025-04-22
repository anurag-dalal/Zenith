#pragma once

#include <string>
#include <unordered_map>
#include <array>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>

namespace Zenith {

/**
 * Represents the texture paths for each face of a block
 */
struct BlockTextures {
    std::string top;
    std::string bottom;
    std::string front;
    std::string back;
    std::string left;
    std::string right;
};

/**
 * Reads and parses the BlockRegistry.json file and provides access to block textures
 */
class BlockRegistryReader {
public:
    /**
     * Constructor - initializes the registry but doesn't load it yet
     */
    BlockRegistryReader();

    /**
     * Loads the block registry from the config file
     * @return true if loading was successful, false otherwise
     */
    bool loadRegistry();

    /**
     * Gets the texture paths for a specific block ID
     * @param blockId The ID of the block to get textures for
     * @return A pointer to the block textures, or nullptr if the block ID doesn't exist
     */
    const BlockTextures* getBlockTextures(const std::string& blockId) const;

    /**
     * Checks if a block ID exists in the registry
     * @param blockId The ID to check
     * @return true if the block exists, false otherwise
     */
    bool hasBlock(const std::string& blockId) const;

    /**
     * Gets the number of blocks in the registry
     * @return The number of blocks
     */
    size_t getBlockCount() const;
    
    /**
     * Iterates through all blocks and calls the callback function for each
     * @param callback A function that takes a block ID and BlockTextures reference
     */
    void forEachBlock(const std::function<void(const std::string&, const BlockTextures&)>& callback) const;

    /**
     * Gets the block information by ID
     * @param blockId The ID of the block to get
     * @return A pair containing the block ID and its textures, or empty strings if the block doesn't exist
     */
    std::pair<std::string, BlockTextures> getBlockById(const std::string& blockId) const;

private:
    /**
     * Process a single block entry from the JSON
     * @param id The block ID
     * @param blockData The JSON data for the block
     */
    void processBlockEntry(const std::string& id, const nlohmann::json& blockData);

    /**
     * Builds a full texture path with the assets prefix
     * @param texturePath The relative texture path
     * @return The full texture path
     */
    std::string buildTexturePath(const std::string& texturePath) const;

    std::unordered_map<std::string, BlockTextures> m_blockTextures;
    std::string m_assetsPath;
    bool m_isLoaded;
};

} // namespace Zenith