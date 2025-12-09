#ifndef CANVAS_ERASER_TOOL_H
#define CANVAS_ERASER_TOOL_H

#include "tools/Tool.h"

namespace Canvas {

/**
 * Eraser tool that sets pixels to transparent.
 * Works like the pencil tool but erases instead of drawing.
 */
class EraserTool : public Tool {
public:
    EraserTool() = default;
    ~EraserTool() override = default;

    std::string getName() const override { return "Eraser"; }

    bool onMouseDown(Layer& layer, const ToolInput& input) override;
    bool onMouseMove(Layer& layer, const ToolInput& input) override;
    bool onMouseUp(Layer& layer, const ToolInput& input) override;

private:
    void eraseLine(Layer& layer, int x0, int y0, int x1, int y1);
    void eraseBrush(Layer& layer, int x, int y);

    bool m_isDrawing = false;
};

} // namespace Canvas

#endif // CANVAS_ERASER_TOOL_H



