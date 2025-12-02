#!/bin/bash
# Canvas Project Setup Script
# Run this to download and set up third-party dependencies

set -e

echo "Setting up Canvas project dependencies..."

# Create external directory
mkdir -p external
cd external

# Clone ImGui
if [ ! -d "imgui" ]; then
    echo "Downloading ImGui..."
    git clone --depth 1 https://github.com/ocornut/imgui.git
fi

# Clone stb (header-only libraries)
if [ ! -d "stb" ]; then
    echo "Downloading stb..."
    git clone --depth 1 https://github.com/nothings/stb.git
fi

# Clone glad (OpenGL loader generator)
if [ ! -d "glad" ]; then
    echo "Downloading glad..."
    git clone --depth 1 https://github.com/Dav1dde/glad.git glad-repo
    mkdir -p glad/include glad/src
    # We'll need to generate glad files - for now, placeholder
    echo "NOTE: You need to generate glad files from https://glad.dav1d.de/"
    echo "      Settings: Language=C/C++, Specification=OpenGL, Profile=Core, Version=3.3"
    echo "      Download and extract to external/glad/"
fi

cd ..

echo ""
echo "Setup complete! Next steps:"
echo "1. Generate glad files from https://glad.dav1d.de/ (if not done)"
echo "2. Install system dependencies:"
echo "   - macOS: brew install glfw python3"
echo "   - Ubuntu: sudo apt install libglfw3-dev python3-dev python3-numpy"
echo "3. Build the project:"
echo "   mkdir build && cd build"
echo "   cmake .."
echo "   make"

