/**
 * EraserTool.cpp
 * Eraser tool - sets pixels to transparent.
 */

#include "tools/EraserTool.h"
#include "core/DrawingUtils.h"
#include "core/Color.h"
#include "layers/Layer.h"

namespace Canvas {

bool EraserTool::onMouseDown(Layer& layer, const ToolInput& input) {
    if (!input.leftButton) {
        return false;
    }

    m_isDrawing = true;
    eraseBrush(layer, input.x, input.y);
    return true;
}

bool EraserTool::onMouseMove(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing || !input.leftButton) {
        return false;
    }

    eraseLine(layer, input.prevX, input.prevY, input.x, input.y);
    return true;
}

bool EraserTool::onMouseUp(Layer& layer, const ToolInput& input) {
    bool wasDrawing = m_isDrawing;
    m_isDrawing = false;
    return wasDrawing;
}

void EraserTool::eraseLine(Layer& layer, int x0, int y0, int x1, int y1) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    Color transparent = Color::Transparent();
    
    DrawingUtils::drawLine(x0, y0, x1, y1, transparent,
        [&buffer, this, transparent](int x, int y, const Color&) -> bool {
            DrawingUtils::drawBrushStamp(x, y, m_brushSize, transparent,
                [&buffer](int px, int py, const Color& c) -> bool {
                    if (buffer.isInBounds(px, py)) {
                        buffer.setPixel(px, py, c);
                    }
                    return true;
                });
            return true;
        });
    
    // layer.updateTexture() handled by renderer
}

void EraserTool::eraseBrush(Layer& layer, int x, int y) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    Color transparent = Color::Transparent();
    
    DrawingUtils::drawBrushStamp(x, y, m_brushSize, transparent,
        [&buffer](int px, int py, const Color& c) -> bool {
            if (buffer.isInBounds(px, py)) {
                buffer.setPixel(px, py, c);
            }
            return true;
        });
    
    // layer.updateTexture() handled by renderer
}

} // namespace Canvas

