#ifndef CANVAS_LAYER_PANEL_H
#define CANVAS_LAYER_PANEL_H

namespace Canvas {

class LayerManager;
class CommandHistory;

/**
 * UI panel for managing layers.
 * Shows layer list with visibility toggles, opacity sliders, and reordering.
 */
class LayerPanel {
public:
    LayerPanel(LayerManager* layerManager, CommandHistory* history);
    ~LayerPanel() = default;

    // Render the layer panel
    void render();

    // Panel visibility
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

private:
    void renderLayerItem(size_t index);
    void renderLayerControls();

private:
    LayerManager* m_layerManager;
    CommandHistory* m_commandHistory;
    bool m_visible = true;
};

} // namespace Canvas

#endif // CANVAS_LAYER_PANEL_H

