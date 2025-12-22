# Canvas

A C++ drawing application built with OpenGL that provides a digital canvas with multiple drawing tools, layer management, and Python-based image filters.

## Project Overview

Canvas is a C++ drawing and graphics application using OpenGL that provides a digital canvas with multiple tools, layers, and real-time interaction. The application includes pencil, shape, fill, eraser, and spray tools, along with a color picker, adjustable brush sizes, and graphical UI controls. Canvas supports multiple layers with opacity and visibility controls, file import/export for PNG and JPEG formats, and a full undo/redo system using a command-based architecture. The project incorporates a Python-based filter pipeline, enabling advanced image processing such as blur, edge detection, and sharpening.

The project demonstrates event-driven programming, object-oriented design, C++ graphics programming, debugging techniques, and multi-language integration between C++ and Python.

## File Structure

```
Canvas/
├── CMakeLists.txt              # Build configuration
├── setup.sh                    # Dependency setup script
├── run_canvas.sh              # Application launcher
├── build/                      # Build output directory
│   ├── canvas                  # Executable
│   └── filters/               # Copied Python filters
├── external/                   # Third-party dependencies
│   ├── imgui/                 # Immediate mode GUI library
│   ├── stb/                   # Single-file image libraries
│   └── glad/                  # OpenGL loader
├── filters/                    # Python filter scripts
│   ├── blur.py
│   ├── edge_detection.py
│   └── sharpen.py
├── include/                    # Header files
│   ├── commands/              # Command pattern implementation
│   ├── core/                  # Core application components
│   ├── filters/               # Filter manager interface
│   ├── io/                    # File I/O interfaces
│   ├── layers/                # Layer system interfaces
│   ├── rendering/             # OpenGL rendering interfaces
│   ├── tools/                 # Drawing tool interfaces
│   └── ui/                    # UI component interfaces
└── src/                        # Implementation files
    ├── commands/              # Command implementations
    ├── core/                 # Core implementations
    ├── filters/              # Filter manager implementation
    ├── io/                   # File I/O implementations
    ├── layers/               # Layer system implementations
    ├── rendering/            # OpenGL rendering implementations
    ├── tools/                # Drawing tool implementations
    ├── ui/                   # UI component implementations
    └── main.cpp              # Application entry point
```

## Directory Descriptions

### include/ and src/

The project follows a header/implementation separation pattern. Each module has corresponding header and source files.

**commands/**: Implements the command pattern for undo/redo operations. Commands capture layer state before and after operations, enabling reversible actions across all tools and layer modifications.

**core/**: Contains fundamental application components. Application manages the main loop and subsystem coordination. Canvas defines the PixelBuffer class for RGBA pixel data. Color provides color representation and blending. DrawingUtils implements rasterization algorithms. LayerCompositor handles multi-layer alpha blending.

**filters/**: FilterManager provides the C++ interface for Python filter integration. It handles data conversion between C++ PixelBuffer and NumPy arrays, manages Python interpreter state, and executes filter scripts.

**io/**: FileManager handles file system operations. ImageHandler uses stb_image for loading and stb_image_write for saving PNG/JPEG files.

**layers/**: Layer represents a single drawing surface with pixel data, texture, opacity, and visibility. LayerManager maintains the layer stack, handles layer ordering, and provides active layer selection.

**rendering/**: Renderer manages OpenGL context, shaders, and texture rendering. Texture wraps OpenGL texture objects and handles pixel buffer to texture conversion.

**tools/**: Each tool implements the Tool interface with mouse event handlers. Tools include PencilTool, LineTool, RectangleTool, CircleTool, SprayTool, FillTool, and EraserTool. ToolManager registers tools and manages tool state.

**ui/**: UI components use ImGui for rendering. UIManager coordinates all UI elements. ToolPanel displays tool selection. ColorPicker provides color selection. BrushSizeSlider controls brush size. LayerPanel manages layer operations. FilterMenu lists and executes Python filters.

### filters/

Python filter scripts that process image data. Each filter exports an `apply` function that accepts a NumPy array and returns a processed array. Filters work with or without SciPy, falling back to NumPy-only implementations when SciPy is unavailable.

### external/

Third-party libraries bundled with the project. These are not modified and are included for build reproducibility.

## Project Philosophy

The architecture emphasizes modularity and separation of concerns. Each subsystem operates independently with well-defined interfaces. The command pattern ensures all operations are reversible. The tool system uses polymorphism to allow easy extension of drawing capabilities.

Rendering uses OpenGL textures for performance, with lazy texture updates to minimize GPU transfers. Layer compositing performs alpha blending in software for precise control over transparency effects.

Python integration enables extensible image processing without recompiling the C++ application. The filter system converts pixel data to NumPy arrays, executes Python code, and converts results back to the native format.

The UI uses immediate mode rendering through ImGui, simplifying state management and allowing rapid UI iteration. All user interactions route through the Application class, which coordinates between subsystems.

## Dependencies

### System Dependencies

- CMake 3.16 or later
- C++17 compatible compiler
- OpenGL 3.3 or later
- GLFW 3.3 or later
- Python 3 with NumPy

### Third-Party Libraries

**ImGui** (MIT License)
- Immediate mode GUI library
- Source: https://github.com/ocornut/imgui
- Used for all user interface components

**stb** (Public Domain / MIT License)
- Single-file image libraries
- Source: https://github.com/nothings/stb
- Used for PNG/JPEG loading and writing (stb_image.h, stb_image_write.h)

**GLAD** (MIT License)
- OpenGL loader generator
- Source: https://github.com/Dav1dde/glad
- Used for OpenGL function loading

**NumPy** (BSD License)
- Python numerical computing library
- Used for image data manipulation in filters

**SciPy** (BSD License, optional)
- Python scientific computing library
- Used for advanced image processing operations when available
- Filters include fallback implementations when SciPy is not installed

## Building

1. Run `./setup.sh` to download external dependencies
2. Install system dependencies:
   - macOS: `brew install glfw python3`
   - Ubuntu: `sudo apt install libglfw3-dev python3-dev python3-numpy`
3. Build the project:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
4. Run the application:
   ```bash
   ./run_canvas.sh
   ```

## Usage

The application provides a drawing canvas with multiple tools accessible through the UI. Select tools from the tool panel, adjust colors and brush sizes, and work with multiple layers. Apply Python filters through the filter menu. All operations support undo/redo via Ctrl+Z and Ctrl+Y.

## Next Steps

Future development may explore training a neural network to operate the drawing application. The goal would be to create an agent that can interact with the program's interface, select tools, apply colors, and generate drawings autonomously. While such a system would not match the capabilities of current generative AI models, the challenge lies in training a model to operate a custom-designed program through its UI controls rather than generating images directly.

The application's command-based architecture and well-defined tool interfaces provide a structured environment for reinforcement learning or imitation learning approaches. The agent would need to learn tool selection, brush size adjustment, color choice, layer management, and spatial reasoning for drawing operations. Given an interest in abstract art, the training objective could focus on generating non-representational compositions that demonstrate creative use of the available tools and filters.

## Acknowledgments

This project uses the following open-source libraries:

- ImGui by Omar Cornut and contributors
- stb libraries by Sean Barrett and contributors
- GLAD by David Herberth
- NumPy by the NumPy development team
- SciPy by the SciPy development team (optional)

OpenGL and GLFW provide the graphics and windowing infrastructure.

## Project Author

William Sander

CS 2300 Final Project - Large Project (200 points)

