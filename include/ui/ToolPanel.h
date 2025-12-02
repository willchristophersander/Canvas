#ifndef CANVAS_TOOL_PANEL_H
#define CANVAS_TOOL_PANEL_H

namespace Canvas {

class ToolManager;

/**
 * UI panel for selecting drawing tools.
 * Displays tool icons/buttons and handles tool selection.
 */
class ToolPanel {
public:
    ToolPanel(ToolManager* toolManager);
    ~ToolPanel() = default;

    // Render the tool panel
    void render();

    // Panel visibility
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

private:
    ToolManager* m_toolManager;
    bool m_visible = true;
};

} // namespace Canvas

#endif // CANVAS_TOOL_PANEL_H

