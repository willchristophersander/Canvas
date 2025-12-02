#ifndef CANVAS_UI_MANAGER_H
#define CANVAS_UI_MANAGER_H

#include <memory>

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

private:
    Application* m_app;
    
    std::unique_ptr<ToolPanel> m_toolPanel;
    std::unique_ptr<ColorPicker> m_colorPicker;
    std::unique_ptr<BrushSizeSlider> m_brushSizeSlider;
    std::unique_ptr<LayerPanel> m_layerPanel;
    std::unique_ptr<FilterMenu> m_filterMenu;
};

} // namespace Canvas

#endif // CANVAS_UI_MANAGER_H

