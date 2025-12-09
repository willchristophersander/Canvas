#ifndef CANVAS_SPRAY_TOOL_H
#define CANVAS_SPRAY_TOOL_H

#include "tools/Tool.h"
#include <random>

namespace Canvas {

/**
 * Spray paint tool that randomly distributes pixels in a brush area.
 * Creates an airbrush-like effect.
 */
class SprayTool : public Tool {
public:
    SprayTool();
    ~SprayTool() override = default;

    std::string getName() const override { return "Spray"; }

    bool onMouseDown(Layer& layer, const ToolInput& input) override;
    bool onMouseMove(Layer& layer, const ToolInput& input) override;
    bool onMouseUp(Layer& layer, const ToolInput& input) override;

    // Spray-specific settings
    int getDensity() const { return m_density; }
    void setDensity(int density) { m_density = density > 0 ? density : 1; }

private:
    void spray(Layer& layer, int x, int y);

    bool m_isDrawing = false;
    int m_density = 20;  // Pixels per spray burst
    
    std::mt19937 m_rng;
    std::uniform_real_distribution<float> m_dist;
};

} // namespace Canvas

#endif // CANVAS_SPRAY_TOOL_H



