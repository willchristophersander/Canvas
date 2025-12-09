cmake_minimum_required(VERSION 3.16)
project(Canvas VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =============================================================================
# Dependencies
# =============================================================================

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW (window management)
find_package(glfw3 3.3 REQUIRED)

# Python (for filter integration)
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

# =============================================================================
# Third-party libraries (bundled or fetched)
# =============================================================================

# ImGui (UI library) - expects imgui to be in external/imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

# stb_image (header-only) - expects stb in external/stb
set(STB_DIR ${CMAKE_SOURCE_DIR}/external/stb)

# glad (OpenGL loader) - expects glad in external/glad
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/external/glad)

# =============================================================================
# Source files
# =============================================================================

set(CORE_SOURCES
    src/core/Color.cpp
    src/core/Canvas.cpp
    src/core/DrawingUtils.cpp
    src/core/LayerCompositor.cpp
    src/core/Application.cpp
)

set(RENDERING_SOURCES
    src/rendering/Texture.cpp
    src/rendering/Renderer.cpp
)

set(LAYERS_SOURCES
    src/layers/Layer.cpp
    src/layers/LayerManager.cpp
)

set(TOOLS_SOURCES
    src/tools/PencilTool.cpp
    src/tools/LineTool.cpp
    src/tools/RectangleTool.cpp
    src/tools/CircleTool.cpp
    src/tools/SprayTool.cpp
    src/tools/FillTool.cpp
    src/tools/EraserTool.cpp
    src/tools/ToolManager.cpp
)

set(COMMANDS_SOURCES
    src/commands/SnapshotCommand.cpp
    src/commands/DrawCommand.cpp
    src/commands/LayerCommand.cpp
    src/commands/CommandHistory.cpp
)

set(UI_SOURCES
    src/ui/UIManager.cpp
    src/ui/ToolPanel.cpp
    src/ui/ColorPicker.cpp
    src/ui/BrushSizeSlider.cpp
    src/ui/LayerPanel.cpp
    src/ui/FilterMenu.cpp
)

set(IO_SOURCES
    src/io/FileManager.cpp
    src/io/ImageHandler.cpp
)

set(FILTERS_SOURCES
    src/filters/FilterManager.cpp
)

# ImGui sources
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# glad source (C++ implementation)
set(GLAD_SOURCES
    ${GLAD_DIR}/src/glad.cpp
)

# =============================================================================
# Main executable
# =============================================================================

add_executable(canvas
    src/main.cpp
    ${CORE_SOURCES}
    ${RENDERING_SOURCES}
    ${LAYERS_SOURCES}
    ${TOOLS_SOURCES}
    ${COMMANDS_SOURCES}
    ${UI_SOURCES}
    ${IO_SOURCES}
    ${FILTERS_SOURCES}
    ${IMGUI_SOURCES}
    ${GLAD_SOURCES}
)

target_include_directories(canvas PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${STB_DIR}
    ${GLAD_DIR}/include
    ${Python3_INCLUDE_DIRS}
)

target_link_libraries(canvas PRIVATE
    OpenGL::GL
    glfw
    Python3::Python
    Python3::NumPy
)

# =============================================================================
# Platform-specific settings
# =============================================================================

if(APPLE)
    target_link_libraries(canvas PRIVATE "-framework Cocoa" "-framework IOKit")
endif()

if(WIN32)
    target_link_libraries(canvas PRIVATE opengl32)
endif()

# =============================================================================
# Copy filters to build directory
# =============================================================================

add_custom_command(TARGET canvas POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/filters
    $<TARGET_FILE_DIR:canvas>/filters
)

# =============================================================================
# Compiler warnings
# =============================================================================

if(MSVC)
    target_compile_options(canvas PRIVATE /W4)
else()
    target_compile_options(canvas PRIVATE -Wall -Wextra -Wpedantic)
endif()

