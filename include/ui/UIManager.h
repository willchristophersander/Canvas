#ifndef CANVAS_UI_MANAGER_H
#define CANVAS_UI_MANAGER_H

#include <memory>
#include <vector>
#include <string>

// Forward declaration for GLFW
struct GLFWwindow;

namespace Canvas {

// Forward declarations
class Application;
class ToolPanel;
class ColorPicker;
class BrushSizeSlider;
class LayerPanel;
class FilterMenu;

/**
 * Manages all UI components and ImGui rendering.
 * Coordinates between UI elements and the application state.
 */
class UIManager {
public:
    UIManager(Application* app);
    ~UIManager();

    // Initialization
    bool initialize(GLFWwindow* window);
    void shutdown();

    // Main render call (call each frame)
    void render();

    // Input handling (returns true if UI captured the input)
    bool wantsMouseInput() const;
    bool wantsKeyboardInput() const;

    // Access to UI components
    ToolPanel& getToolPanel();
    ColorPicker& getColorPicker();
    BrushSizeSlider& getBrushSizeSlider();
    LayerPanel& getLayerPanel();
    FilterMenu& getFilterMenu();

private:
    void renderMenuBar();
    void renderMainLayout();
    void setupCustomStyle();

private:
    Application* m_app;
    bool m_isShutdown = false;  // Track shutdown state
    
    std::unique_ptr<ToolPanel> m_toolPanel;
    std::unique_ptr<ColorPicker> m_colorPicker;
    std::unique_ptr<BrushSizeSlider> m_brushSizeSlider;
    std::unique_ptr<LayerPanel> m_layerPanel;
    std::unique_ptr<FilterMenu> m_filterMenu;

    // File Dialog State
    bool m_showSaveDialog = false;
    bool m_showOpenDialog = false;
    char m_filenameBuffer[256] = "";  // For save dialog
    std::vector<std::string> m_fileList; // For open dialog
    int m_selectedFileIndex = -1;
    
    void renderSaveDialog();
    void renderOpenDialog();
    void refreshFileList();
};

} // namespace Canvas

#endif // CANVAS_UI_MANAGER_H



