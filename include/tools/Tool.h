#ifndef CANVAS_TOOL_H
#define CANVAS_TOOL_H

#include "core/Color.h"
#include <string>

namespace Canvas {

class Layer;
class CommandHistory;

/**
 * Input state passed to tools during mouse events.
 */
struct ToolInput {
    int x = 0;              // Canvas X coordinate
    int y = 0;              // Canvas Y coordinate
    int prevX = 0;          // Previous X (for dragging)
    int prevY = 0;          // Previous Y (for dragging)
    bool leftButton = false;
    bool rightButton = false;
    bool shiftHeld = false;
    bool ctrlHeld = false;
    bool altHeld = false;
};

/**
 * Abstract base class for all drawing tools.
 * Defines the interface that all tools must implement.
 */
class Tool {
public:
    virtual ~Tool() = default;

    // Tool identification
    virtual std::string getName() const = 0;
    virtual std::string getIcon() const { return ""; }  // Optional icon path

    // Tool settings
    int getBrushSize() const { return m_brushSize; }
    void setBrushSize(int size) { m_brushSize = size > 0 ? size : 1; }

    const Color& getColor() const { return m_color; }
    void setColor(const Color& color) { m_color = color; }

    // Mouse event handlers
    // Return true if the layer was modified
    virtual bool onMouseDown(Layer& layer, const ToolInput& input) = 0;
    virtual bool onMouseMove(Layer& layer, const ToolInput& input) = 0;
    virtual bool onMouseUp(Layer& layer, const ToolInput& input) = 0;

    // Called when tool is selected/deselected
    virtual void onSelect() {}
    virtual void onDeselect() {}

    // Preview rendering (optional - for shape tools, etc.)
    virtual bool hasPreview() const { return false; }
    virtual void renderPreview(Layer& layer) {}

protected:
    int m_brushSize = 5;
    Color m_color = Color::Black();
};

} // namespace Canvas

#endif // CANVAS_TOOL_H

