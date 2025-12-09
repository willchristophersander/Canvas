/**
 * SprayTool.cpp
 * Spray paint tool with random pixel distribution.
 */

#include "tools/SprayTool.h"
#include "core/DrawingUtils.h"
#include "layers/Layer.h"
#include <random>
#include <cmath>

namespace Canvas {

SprayTool::SprayTool() 
    : m_rng(std::random_device{}()), m_dist(-1.0f, 1.0f) {
}

bool SprayTool::onMouseDown(Layer& layer, const ToolInput& input) {
    if (!input.leftButton) {
        return false;
    }

    m_isDrawing = true;
    spray(layer, input.x, input.y);
    return true;
}

bool SprayTool::onMouseMove(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing || !input.leftButton) {
        return false;
    }

    // Spray at multiple points along the line for smooth strokes
    int dx = input.x - input.prevX;
    int dy = input.y - input.prevY;
    int steps = std::max(std::abs(dx), std::abs(dy));
    
    if (steps > 0) {
        for (int i = 0; i <= steps; ++i) {
            float t = static_cast<float>(i) / steps;
            int px = input.prevX + static_cast<int>(dx * t);
            int py = input.prevY + static_cast<int>(dy * t);
            spray(layer, px, py);
        }
    } else {
        spray(layer, input.x, input.y);
    }

    return true;
}

bool SprayTool::onMouseUp(Layer& layer, const ToolInput& input) {
    bool wasDrawing = m_isDrawing;
    m_isDrawing = false;
    return wasDrawing;
}

void SprayTool::spray(Layer& layer, int x, int y) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    int radius = m_brushSize / 2;
    
    // Generate random pixels within the brush area
    for (int i = 0; i < m_density; ++i) {
        float angle = m_dist(m_rng) * 3.14159f * 2.0f; // Random angle
        float distance = m_dist(m_rng) * radius; // Random distance
        if (distance < 0) distance = -distance; // Make positive
        
        int px = x + static_cast<int>(std::cos(angle) * distance);
        int py = y + static_cast<int>(std::sin(angle) * distance);
        
        if (buffer.isInBounds(px, py)) {
            // Apply spray with some transparency variation
            Color sprayColor = m_color;
            // Slightly randomize alpha for spray effect
            float alphaFactor = 0.5f + m_dist(m_rng) * 0.5f;
            sprayColor.a = static_cast<uint8_t>(sprayColor.a * alphaFactor);
            
            Color existing = buffer.getPixel(px, py);
            Color blended = existing.blend(sprayColor);
            buffer.setPixel(px, py, blended);
        }
    }
    
    // layer.updateTexture() handled by renderer
}

} // namespace Canvas

