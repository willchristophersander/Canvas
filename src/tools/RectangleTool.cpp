/**
 * RectangleTool.cpp
 * Rectangle drawing tool with filled/outline options.
 */

#include "tools/RectangleTool.h"
#include "core/DrawingUtils.h"
#include "core/Canvas.h"
#include "layers/Layer.h"
#include <algorithm>
#include <memory>

namespace Canvas {

RectangleTool::~RectangleTool() = default;

void RectangleTool::onDeselect() {
    // No preview state to clean up; just mark not drawing
    m_isDrawing = false;
}

bool RectangleTool::onMouseDown(Layer& layer, const ToolInput& input) {
    if (!input.leftButton) {
        return false;
    }

    m_isDrawing = true;
    m_startX = input.x;
    m_startY = input.y;
    m_endX = input.x;
    m_endY = input.y;
    return true; // Return true so command system tracks this operation
}

bool RectangleTool::onMouseMove(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing) {
        return false;
    }

    m_endX = input.x;
    m_endY = input.y;
    m_shiftHeld = input.shiftHeld; // Track shift state for preview

    // Constrain to square if shift held
    if (input.shiftHeld) {
        int dx = m_endX - m_startX;
        int dy = m_endY - m_startY;
        int size = std::max(std::abs(dx), std::abs(dy));
        m_endX = m_startX + (dx >= 0 ? size : -size);
        m_endY = m_startY + (dy >= 0 ? size : -size);
    }

    return false; // Preview only
}

bool RectangleTool::onMouseUp(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing || !input.leftButton) {
        return false;
    }

    m_endX = input.x;
    m_endY = input.y;

    // Constrain to square if shift held
    if (input.shiftHeld) {
        int dx = m_endX - m_startX;
        int dy = m_endY - m_startY;
        int size = std::max(std::abs(dx), std::abs(dy));
        m_endX = m_startX + (dx >= 0 ? size : -size);
        m_endY = m_startY + (dy >= 0 ? size : -size);
    }

    m_isDrawing = false;

    // Actually draw the rectangle
    drawRectangle(layer, m_startX, m_startY, m_endX, m_endY, m_filled);
    return true;
}

void RectangleTool::drawRectangle(Layer& layer, int x1, int y1, int x2, int y2, bool filled) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    
    auto setter = [&buffer](int px, int py, const Color& c) -> bool {
        if (buffer.isInBounds(px, py)) {
            Color existing = buffer.getPixel(px, py);
            Color blended = existing.blend(c);
            buffer.setPixel(px, py, blended);
        }
        return true;
    };

    if (filled) {
        DrawingUtils::drawFilledRectangle(x1, y1, x2, y2, m_color, setter);
    } else {
        // For outline, use brush size for line thickness
        int minX = std::min(x1, x2);
        int maxX = std::max(x1, x2);
        int minY = std::min(y1, y2);
        int maxY = std::max(y1, y2);

        // Draw four sides with brush thickness
        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                bool onEdge = (x == minX || x == maxX || y == minY || y == maxY);
                if (onEdge) {
                    DrawingUtils::drawBrushStamp(x, y, m_brushSize, m_color, setter);
                }
            }
        }
    }
    
    layer.updateTexture();
}

} // namespace Canvas

