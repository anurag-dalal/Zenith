cmake_minimum_required(VERSION 3.10)
project(Zenith VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define paths for shaders, assets, and configs
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/Source/Shaders")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/Assets")
set(CONFIG_DIR "${CMAKE_SOURCE_DIR}/Configs")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Zenith")

# Create folder if it doesn't exist and clean up
add_custom_target(create_output_dir ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configs"
)

# Copy only .glsl files from the Shaders directory
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SHADER_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders"
    COMMAND find "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders" -type f ! -name "*.glsl" -delete
    DEPENDS create_output_dir
)

# Copy assets to build directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
    DEPENDS create_output_dir
)

# Copy configs to build directory
add_custom_target(copy_configs ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CONFIG_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configs"
    DEPENDS create_output_dir
)

# Add GLM
set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Include/glm")
include_directories(${GLM_INCLUDE_DIR})

# Add ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/Include/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR})

# Add GLAD
add_subdirectory(Include/glad)
include_directories(Include)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Include
    ${CMAKE_SOURCE_DIR}/Source
)

# Add the JSON library include path
include_directories(${CMAKE_SOURCE_DIR}/Include/json/single_include)

# Source files - collect all directory sources
file(GLOB_RECURSE BLOCKS_SOURCES
    "Source/Blocks/*.cpp"
    "Source/Blocks/*.h"
)

file(GLOB_RECURSE CONFIG_MANAGER_SOURCES
    "Source/ConfigManager/*.cpp"
    "Source/ConfigManager/*.h"
)

file(GLOB_RECURSE SHADERS_SOURCES
    "Source/Shaders/*.cpp"
    "Source/Shaders/*.h"
)

file(GLOB_RECURSE UTILS_SOURCES
    "Source/Utils/*.cpp"
    "Source/Utils/*.h"
)

# Application to print all Block registry
add_executable(PrintAllBlockTypes 
    Source/PrintAllBlockTypes.cpp
    ${BLOCKS_SOURCES}
    ${CONFIG_MANAGER_SOURCES}
    ${SHADERS_SOURCES}
    ${UTILS_SOURCES}
)

# Define paths for resources
target_compile_definitions(PrintAllBlockTypes PRIVATE 
    SHADER_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders"
    ASSETS_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
    CONFIG_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configs"
)

# Link libraries
target_link_libraries(PrintAllBlockTypes
    glad
    glfw
    imgui
    ${OPENGL_gl_LIBRARY}
)

# Add dependencies to ensure assets, shaders, and configs are copied before running
add_dependencies(PrintAllBlockTypes copy_assets copy_shaders copy_configs)

# Main application for the block viewer
add_executable(BlockTypeViewers 
    Source/BlockTypeViewers.cpp
    ${BLOCKS_SOURCES}
    ${CONFIG_MANAGER_SOURCES}
    ${SHADERS_SOURCES}
    ${UTILS_SOURCES}
)

# Define paths for resources
target_compile_definitions(BlockTypeViewers PRIVATE 
    SHADER_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders"
    ASSETS_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
    CONFIG_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configs"
)

# Link libraries
target_link_libraries(BlockTypeViewers
    glad
    glfw
    imgui
    ${OPENGL_gl_LIBRARY}
)

# Add dependencies to ensure assets, shaders, and configs are copied before running
add_dependencies(BlockTypeViewers copy_assets copy_shaders copy_configs)
