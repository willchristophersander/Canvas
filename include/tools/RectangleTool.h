#ifndef CANVAS_RECTANGLE_TOOL_H
#define CANVAS_RECTANGLE_TOOL_H

#include "tools/Tool.h"

namespace Canvas {

/**
 * Draws rectangles from corner to corner.
 * Hold Shift for square, Alt for center-based drawing.
 */
class RectangleTool : public Tool {
public:
    RectangleTool() = default;
    ~RectangleTool() override = default;

    std::string getName() const override { return "Rectangle"; }

    bool onMouseDown(Layer& layer, const ToolInput& input) override;
    bool onMouseMove(Layer& layer, const ToolInput& input) override;
    bool onMouseUp(Layer& layer, const ToolInput& input) override;

    bool hasPreview() const override { return true; }
    void renderPreview(Layer& layer) override;

    // Options
    bool isFilled() const { return m_filled; }
    void setFilled(bool filled) { m_filled = filled; }

private:
    void drawRectangle(Layer& layer, int x1, int y1, int x2, int y2, bool filled);

    bool m_isDrawing = false;
    bool m_filled = false;
    int m_startX = 0;
    int m_startY = 0;
    int m_endX = 0;
    int m_endY = 0;
};

} // namespace Canvas

#endif // CANVAS_RECTANGLE_TOOL_H

