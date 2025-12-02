#ifndef CANVAS_LINE_TOOL_H
#define CANVAS_LINE_TOOL_H

#include "tools/Tool.h"

namespace Canvas {

/**
 * Draws straight lines from click point to release point.
 * Shows preview while dragging.
 */
class LineTool : public Tool {
public:
    LineTool() = default;
    ~LineTool() override = default;

    std::string getName() const override { return "Line"; }

    bool onMouseDown(Layer& layer, const ToolInput& input) override;
    bool onMouseMove(Layer& layer, const ToolInput& input) override;
    bool onMouseUp(Layer& layer, const ToolInput& input) override;

    bool hasPreview() const override { return true; }
    void renderPreview(Layer& layer) override;

private:
    void drawLine(Layer& layer, int x0, int y0, int x1, int y1);

    bool m_isDrawing = false;
    int m_startX = 0;
    int m_startY = 0;
    int m_endX = 0;
    int m_endY = 0;
};

} // namespace Canvas

#endif // CANVAS_LINE_TOOL_H

