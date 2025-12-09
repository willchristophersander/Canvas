#ifndef CANVAS_PENCIL_TOOL_H
#define CANVAS_PENCIL_TOOL_H

#include "tools/Tool.h"

namespace Canvas {

/**
 * Freehand drawing tool that draws continuous strokes.
 * Uses Bresenham's line algorithm for smooth lines between mouse positions.
 */
class PencilTool : public Tool {
public:
    PencilTool() = default;
    ~PencilTool() override = default;

    std::string getName() const override { return "Pencil"; }

    bool onMouseDown(Layer& layer, const ToolInput& input) override;
    bool onMouseMove(Layer& layer, const ToolInput& input) override;
    bool onMouseUp(Layer& layer, const ToolInput& input) override;

private:
    // Draw a line from (x0, y0) to (x1, y1) with the current brush
    void drawLine(Layer& layer, int x0, int y0, int x1, int y1);
    
    // Draw a single brush stamp at the given position
    void drawBrush(Layer& layer, int x, int y);

    bool m_isDrawing = false;
};

} // namespace Canvas

#endif // CANVAS_PENCIL_TOOL_H



