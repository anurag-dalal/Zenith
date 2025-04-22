# Zenith

A modern OpenGL engine with ImGui integration.
remove cached submodule
git rm --cached Include/imgui
add submodules
git submodule add git@github.com:ocornut/imgui.git Include/imgui
git submodule add git@github.com:nlohmann/json.git Include/json

https://www.9minecraft.net/atlas-resource-pack/

## Installation

```bash
# Clone the repository with all submodules
git clone --recursive git@github.com:anurag-dalal/Zenith.git

# Build the project
mkdir build
cd build
cmake ..
make

# Run the application
./Zenith

