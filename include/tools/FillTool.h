#ifndef CANVAS_FILL_TOOL_H
#define CANVAS_FILL_TOOL_H

#include "tools/Tool.h"

namespace Canvas {

/**
 * Flood fill (paint bucket) tool.
 * Fills contiguous regions of similar color.
 */
class FillTool : public Tool {
public:
    FillTool() = default;
    ~FillTool() override = default;

    std::string getName() const override { return "Fill"; }

    bool onMouseDown(Layer& layer, const ToolInput& input) override;
    bool onMouseMove(Layer& layer, const ToolInput& input) override;
    bool onMouseUp(Layer& layer, const ToolInput& input) override;

    // Tolerance for color matching (0 = exact, 255 = match all)
    int getTolerance() const { return m_tolerance; }
    void setTolerance(int tolerance);

private:
    void floodFill(Layer& layer, int startX, int startY, const Color& targetColor);
    bool colorsMatch(const Color& a, const Color& b) const;

    int m_tolerance = 0;
};

} // namespace Canvas

#endif // CANVAS_FILL_TOOL_H



