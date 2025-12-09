#ifndef CANVAS_APPLICATION_H
#define CANVAS_APPLICATION_H

#include <memory>
#include <string>

// Forward declarations
struct GLFWwindow;

namespace Canvas {

// Forward declarations
class Renderer;
class LayerManager;
class ToolManager;
class CommandHistory;
class UIManager;
class FilterManager;
class FileManager;

/**
 * Main application class that owns and coordinates all subsystems.
 * Handles window creation, main loop, and event dispatching.
 */
class Application {
public:
    Application();
    ~Application();

    // Prevent copying
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Initialization and main loop
    bool initialize(int width = 1280, int height = 720, const std::string& title = "Canvas");
    void run();
    void shutdown();

    // Accessors for subsystems
    Renderer& getRenderer();
    LayerManager& getLayerManager();
    ToolManager& getToolManager();
    CommandHistory& getCommandHistory();
    UIManager& getUIManager();
    FilterManager& getFilterManager();
    FileManager& getFileManager();

    // Window properties
    int getWindowWidth() const;
    int getWindowHeight() const;
    GLFWwindow* getWindow() const;

    // Canvas properties
    int getCanvasWidth() const;
    int getCanvasHeight() const;

    // Event handlers (called by GLFW callbacks - public for static callbacks)
    void onMouseButton(int button, int action, int mods);
    void onMouseMove(double xpos, double ypos);
    void onKey(int key, int scancode, int action, int mods);
    void onScroll(double xoffset, double yoffset);
    void onResize(int width, int height);

private:

    // Main loop steps
    void processInput();
    void update();
    void render();

    // Setup helpers
    bool initializeGLFW();
    bool initializeOpenGL();
    void setupCallbacks();

private:
    GLFWwindow* m_window = nullptr;
    int m_windowWidth = 1280;
    int m_windowHeight = 720;
    int m_canvasWidth = 800;
    int m_canvasHeight = 600;
    bool m_running = false;

    // Subsystems (order matters for initialization/destruction)
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<LayerManager> m_layerManager;
    std::unique_ptr<ToolManager> m_toolManager;
    std::unique_ptr<CommandHistory> m_commandHistory;
    std::unique_ptr<UIManager> m_uiManager;
    std::unique_ptr<FilterManager> m_filterManager;
    std::unique_ptr<FileManager> m_fileManager;
};

} // namespace Canvas

#endif // CANVAS_APPLICATION_H



