/**
 * PencilTool.cpp
 * Freehand drawing tool using DrawingUtils.
 */

#include "tools/PencilTool.h"
#include "core/DrawingUtils.h"
#include "layers/Layer.h"
#include <algorithm>

namespace Canvas {

bool PencilTool::onMouseDown(Layer& layer, const ToolInput& input) {
    if (!input.leftButton) {
        return false;
    }

    m_isDrawing = true;
    drawBrush(layer, input.x, input.y);
    return true;
}

bool PencilTool::onMouseMove(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing || !input.leftButton) {
        return false;
    }

    drawLine(layer, input.prevX, input.prevY, input.x, input.y);
    return true;
}

bool PencilTool::onMouseUp(Layer& layer, const ToolInput& input) {
    bool wasDrawing = m_isDrawing;
    m_isDrawing = false;
    return wasDrawing;
}

void PencilTool::drawLine(Layer& layer, int x0, int y0, int x1, int y1) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    
    DrawingUtils::drawLine(x0, y0, x1, y1, m_color, 
        [&buffer, this](int x, int y, const Color& color) -> bool {
            DrawingUtils::drawBrushStamp(x, y, m_brushSize, m_color,
                [&buffer](int px, int py, const Color& c) -> bool {
                    if (buffer.isInBounds(px, py)) {
                        Color existing = buffer.getPixel(px, py);
                        Color blended = existing.blend(c);
                        buffer.setPixel(px, py, blended);
                    }
                    return true;
                });
            return true;
        });
    
    // layer.updateTexture() handled by renderer
}

void PencilTool::drawBrush(Layer& layer, int x, int y) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    
    DrawingUtils::drawBrushStamp(x, y, m_brushSize, m_color,
        [&buffer](int px, int py, const Color& c) -> bool {
            if (buffer.isInBounds(px, py)) {
                        Color existing = buffer.getPixel(px, py);
                        Color blended = existing.blend(c);
                        buffer.setPixel(px, py, blended);
            }
            return true;
        });
    
    // layer.updateTexture() handled by renderer
}

} // namespace Canvas

