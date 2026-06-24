/**
 * LineTool.cpp
 * Straight line drawing tool with preview.
 */

#include "tools/LineTool.h"
#include "core/DrawingUtils.h"
#include "core/Color.h"
#include "layers/Layer.h"
#include "core/Canvas.h"
#include <memory>

namespace Canvas {

LineTool::~LineTool() = default;

void LineTool::onDeselect() {
    // Clear preview state when tool is deselected
    // If a preview is active, restore the base state on the tracked layer
    if (m_previewSnapshot && m_previewLayer) {
        m_previewLayer->restoreFromSnapshot(*m_previewSnapshot);
    }
    m_previewSnapshot.reset();
    m_previewLayer = nullptr;
    m_isDrawing = false;
}

bool LineTool::onMouseDown(Layer& layer, const ToolInput& input) {
    if (!input.leftButton) {
        return false;
    }

    m_isDrawing = true;
    m_startX = input.x;
    m_startY = input.y;
    m_endX = input.x;
    m_endY = input.y;
    // Capture base state for preview (before any drawing)
    m_previewLayer = &layer;
    m_previewSnapshot = layer.createSnapshot();
    return true; // Return true so command system tracks this operation
}

bool LineTool::onMouseMove(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing) {
        return false;
    }

    m_endX = input.x;
    m_endY = input.y;
    return false; // Preview only, no modification yet
}

bool LineTool::onMouseUp(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing) {
        return false;
    }

    m_endX = input.x;
    m_endY = input.y;
    m_isDrawing = false;

    // Restore base state (remove any preview)
    if (m_previewSnapshot && m_previewLayer == &layer) {
        layer.restoreFromSnapshot(*m_previewSnapshot);
    }
    m_previewSnapshot.reset();
    m_previewLayer = nullptr;

    // Actually draw the line
    drawLine(layer, m_startX, m_startY, m_endX, m_endY);
    return true;
}

void LineTool::renderPreview(Layer& layer) {
    if (!m_isDrawing || !m_previewSnapshot) {
        return;
    }

    // Restore base state (remove previous preview frame)
    layer.restoreFromSnapshot(*m_previewSnapshot);

    // Draw new preview
    drawLine(layer, m_startX, m_startY, m_endX, m_endY);
}

bool LineTool::isPreviewActive() const {
    return m_isDrawing && static_cast<bool>(m_previewSnapshot);
}

void LineTool::drawLine(Layer& layer, int x0, int y0, int x1, int y1) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    
    // Use thicker line for better visibility
    int lineWidth = std::max(1, m_brushSize);
    
    DrawingUtils::drawLine(x0, y0, x1, y1, m_color,
        [&buffer, lineWidth, this](int x, int y, const Color&) -> bool {
            DrawingUtils::drawBrushStamp(x, y, lineWidth, m_color,
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
    
    layer.updateTexture();
}

} // namespace Canvas

