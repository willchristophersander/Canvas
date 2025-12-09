#ifndef CANVAS_CIRCLE_TOOL_H
#define CANVAS_CIRCLE_TOOL_H

#include "tools/Tool.h"
#include <memory>

namespace Canvas {

// Forward declaration
class PixelBuffer;

/**
 * Draws circles/ellipses.
 * Hold Shift for perfect circles.
 */
class CircleTool : public Tool {
public:
    CircleTool() = default;
    ~CircleTool() override; // Defined in .cpp where PixelBuffer is complete

    std::string getName() const override { return "Circle"; }

    bool onMouseDown(Layer& layer, const ToolInput& input) override;
    bool onMouseMove(Layer& layer, const ToolInput& input) override;
    bool onMouseUp(Layer& layer, const ToolInput& input) override;

    bool hasPreview() const override { return true; }
    void renderPreview(Layer& layer) override;
    bool isPreviewActive() const override;
    void onDeselect() override;

    // Options
    bool isFilled() const { return m_filled; }
    void setFilled(bool filled) { m_filled = filled; }

private:
    void drawEllipse(Layer& layer, int cx, int cy, int rx, int ry, bool filled);
    void drawCircle(Layer& layer, int cx, int cy, int radius, bool filled);

    bool m_isDrawing = false;
    bool m_filled = false;
    bool m_shiftHeld = false;
    int m_startX = 0;
    int m_startY = 0;
    int m_endX = 0;
    int m_endY = 0;
    Layer* m_previewLayer = nullptr;               // Layer associated with the snapshot
    std::unique_ptr<PixelBuffer> m_previewSnapshot; // Base state before preview
};

} // namespace Canvas

#endif // CANVAS_CIRCLE_TOOL_H



