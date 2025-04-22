#include <iostream>
#include "Blocks/BlockRegistryReader.h"

// Path macros defined by CMake
#ifndef ASSETS_DIR
#define ASSETS_DIR "Assets"
#endif

#ifndef CONFIG_DIR
#define CONFIG_DIR "Configs"
#endif

int main(int argc, char* argv[]) {
    std::cout << "Zenith Block Registry Reader" << std::endl;
    std::cout << "============================" << std::endl;

    // Create and load the block registry
    Zenith::BlockRegistryReader registry;
    if (!registry.loadRegistry()) {
        std::cerr << "Failed to load block registry!" << std::endl;
        return 1;
    }

    std::cout << "Successfully loaded " << registry.getBlockCount() << " blocks." << std::endl;
    std::cout << "Block types and their texture paths:" << std::endl;
    std::cout << "====================================" << std::endl;

    // Iterate through all blocks and print their information
    registry.forEachBlock([](const std::string& blockId, const Zenith::BlockTextures& textures) {
        std::cout << "Block ID: " << blockId << std::endl;
        std::cout << "  Top:    " << textures.top << std::endl;
        std::cout << "  Bottom: " << textures.bottom << std::endl;
        std::cout << "  Front:  " << textures.front << std::endl;
        std::cout << "  Back:   " << textures.back << std::endl;
        std::cout << "  Left:   " << textures.left << std::endl;
        std::cout << "  Right:  " << textures.right << std::endl;
        std::cout << std::endl;
    });

    return 0;
}