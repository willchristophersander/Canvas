# Canvas - Raster Graphics Editor

A professional-grade C++ raster graphics editor built with OpenGL, featuring a comprehensive toolset, multi-layer support, and Python-based image processing filters.

## Overview

Canvas is a fully-featured drawing application that provides a digital canvas with multiple drawing tools, layer management, and real-time interaction. Built using modern C++17 and OpenGL 3.3+, it demonstrates advanced graphics programming, event-driven architecture, and seamless C++/Python integration.

## Features

### 🎨 Drawing Tools
- **Pencil Tool** - Freehand drawing with smooth brush strokes
- **Line Tool** - Straight lines with real-time preview
- **Rectangle Tool** - Rectangles and squares (hold Shift for perfect squares)
- **Circle Tool** - Circles and ellipses (hold Shift for perfect circles)
- **Spray Tool** - Airbrush-style painting
- **Fill Tool** - Flood fill with color blending
- **Eraser Tool** - Remove pixels with adjustable size

### 📐 Advanced Features
- **Multi-Layer System** - Create, delete, reorder, and manage multiple layers
- **Layer Controls** - Adjust opacity and toggle visibility for each layer
- **Undo/Redo System** - Full command-based history (Ctrl+Z / Ctrl+Y)
- **Color Picker** - RGB color selection with preset colors
- **Adjustable Brush Size** - Dynamic brush size control for all tools
- **File I/O** - Import PNG/JPEG images and export to PNG format

### 🐍 Python Filter Integration
- **Blur Filter** - Gaussian blur for smooth image effects
- **Edge Detection** - Sobel operator for edge highlighting
- **Sharpen Filter** - Image sharpening for enhanced detail
- Extensible filter system for custom image processing

### 🖥️ User Interface
- Clean, intuitive ImGui-based interface
- Tool selection panel
- Color picker with presets
- Layer management panel
- Filter execution menu
- Real-time canvas preview

## Screenshots

*Add screenshots of the application here*

## Building

### Prerequisites

**macOS:**
```bash
brew install glfw python3
pip3 install numpy
```

**Ubuntu/Debian:**
```bash
sudo apt install libglfw3-dev python3-dev python3-numpy cmake build-essential
```

**Windows:**
- Install [CMake](https://cmake.org/download/)
- Install [GLFW](https://www.glfw.org/download.html)
- Install [Python 3](https://www.python.org/downloads/) with NumPy

### Build Instructions

1. **Clone the repository:**
```bash
git clone https://github.com/willchristophersander/Canvas.git
cd Canvas
```

2. **Set up dependencies:**
```bash
./setup.sh
```

Note: The external dependencies (ImGui, stb) are already included in the repository. If you need to regenerate them, the setup script will guide you.

3. **Build the project:**
```bash
mkdir build && cd build
cmake ..
make
```

4. **Run the application:**
```bash
cd ..
./run_canvas.sh
```

Or directly:
```bash
./build/canvas
```

## Usage

### Basic Drawing
1. Select a tool from the tool panel
2. Choose a color using the color picker
3. Adjust brush size with the slider
4. Click and drag on the canvas to draw

### Layer Management
- **Add Layer**: Use the layer panel to create new layers
- **Delete Layer**: Remove layers you no longer need
- **Reorder Layers**: Drag layers to change their stacking order
- **Opacity**: Adjust layer transparency with the opacity slider
- **Visibility**: Toggle layer visibility with the eye icon

### Keyboard Shortcuts
- `Ctrl+Z` - Undo last action
- `Ctrl+Y` - Redo last undone action
- `Shift` (while drawing) - Constrain shapes (squares, circles)

### Applying Filters
1. Select a layer
2. Open the filter menu
3. Choose a filter (Blur, Edge Detection, or Sharpen)
4. The filter is applied to the active layer

## Architecture

### Core Components

**Canvas & Rendering**
- `PixelBuffer` - RGBA pixel data management
- `Renderer` - OpenGL context and texture rendering
- `Texture` - OpenGL texture wrapper
- `LayerCompositor` - Multi-layer alpha blending

**Tool System**
- Abstract `Tool` base class
- Tool-specific implementations (Pencil, Line, Rectangle, etc.)
- `ToolManager` - Tool registration and selection
- Real-time preview rendering for shape tools

**Layer System**
- `Layer` - Individual drawing surface with pixel buffer
- `LayerManager` - Layer stack management and compositing

**Command Pattern**
- `Command` - Base command interface
- `DrawCommand` - Drawing operation commands
- `LayerCommand` - Layer modification commands
- `CommandHistory` - Undo/redo stack management

**File I/O**
- `ImageHandler` - PNG/JPEG loading and saving (using stb_image)
- `FileManager` - File system operations

**Python Integration**
- `FilterManager` - C++/Python bridge
- NumPy array conversion for image data
- Filter script execution

### Project Structure

```
Canvas/
├── CMakeLists.txt          # Build configuration
├── setup.sh                # Dependency setup script
├── run_canvas.sh           # Application launcher
├── external/               # Third-party libraries
│   ├── imgui/             # Immediate mode GUI
│   ├── stb/               # Image I/O libraries
│   └── glad/              # OpenGL loader
├── filters/                # Python filter scripts
│   ├── blur.py
│   ├── edge_detection.py
│   └── sharpen.py
├── include/                # Header files
│   ├── commands/          # Command pattern
│   ├── core/              # Core components
│   ├── filters/           # Filter manager
│   ├── io/                # File I/O
│   ├── layers/            # Layer system
│   ├── rendering/         # OpenGL rendering
│   ├── tools/             # Drawing tools
│   └── ui/                # UI components
└── src/                   # Implementation files
    └── [mirrors include/ structure]
```

## Dependencies

### System Dependencies
- **CMake** 3.16+
- **C++17** compatible compiler
- **OpenGL** 3.3+
- **GLFW** 3.3+
- **Python 3** with NumPy

### Third-Party Libraries

- **[ImGui](https://github.com/ocornut/imgui)** (MIT License) - Immediate mode GUI
- **[stb](https://github.com/nothings/stb)** (Public Domain) - Image loading/writing
- **[GLAD](https://github.com/Dav1dde/glad)** (MIT License) - OpenGL loader
- **[NumPy](https://numpy.org/)** (BSD License) - Python numerical computing

## Technical Highlights

- **Object-Oriented Design** - Clean separation of concerns with polymorphic tool system
- **Command Pattern** - Robust undo/redo implementation
- **Multi-Language Integration** - Seamless C++/Python interop for extensibility
- **Real-Time Rendering** - Efficient OpenGL texture-based rendering
- **Event-Driven Architecture** - Responsive user interaction handling
- **Memory Management** - Smart pointers and RAII principles

## Development

This project was developed as a final project for CS 2300, demonstrating:
- Advanced C++ programming techniques
- Graphics programming with OpenGL
- Software architecture and design patterns
- Multi-language system integration
- Debugging and optimization

## License

This project uses third-party libraries with their respective licenses:
- ImGui: MIT License
- stb: Public Domain / MIT License
- GLAD: MIT License
- NumPy: BSD License

## Author

**William Sander**

CS 2300 Final Project - Large Project (200 points)

## Acknowledgments

- ImGui by Omar Cornut and contributors
- stb libraries by Sean Barrett and contributors
- GLAD by David Herberth
- NumPy by the NumPy development team

---

**Note**: This is an educational project demonstrating advanced C++ graphics programming and software architecture principles.

