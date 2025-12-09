/**
 * CircleTool.cpp
 * Circle/ellipse drawing tool.
 */

#include "tools/CircleTool.h"
#include "core/DrawingUtils.h"
#include "core/Canvas.h"
#include "layers/Layer.h"
#include <algorithm>
#include <memory>
#include <cmath>

namespace Canvas {

CircleTool::~CircleTool() = default;

void CircleTool::onDeselect() {
    // Clear preview state when tool is deselected
    // If a preview is active, restore the base state on the tracked layer
    if (m_previewSnapshot && m_previewLayer) {
        m_previewLayer->restoreFromSnapshot(*m_previewSnapshot);
    }
    m_previewSnapshot.reset();
    m_previewLayer = nullptr;
    m_isDrawing = false;
}

bool CircleTool::onMouseDown(Layer& layer, const ToolInput& input) {
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

bool CircleTool::onMouseMove(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing) {
        return false;
    }

    m_endX = input.x;
    m_endY = input.y;
    m_shiftHeld = input.shiftHeld; // Track shift state for preview
    return false; // Preview only
}

bool CircleTool::onMouseUp(Layer& layer, const ToolInput& input) {
    if (!m_isDrawing || !input.leftButton) {
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

    int cx = (m_startX + m_endX) / 2;
    int cy = (m_startY + m_endY) / 2;
    int rx = std::abs(m_endX - m_startX) / 2;
    int ry = std::abs(m_endY - m_startY) / 2;

    if (input.shiftHeld) {
        // Make it a perfect circle
        int radius = std::max(rx, ry);
        drawCircle(layer, cx, cy, radius, m_filled);
    } else {
        drawEllipse(layer, cx, cy, rx, ry, m_filled);
    }

    return true;
}

void CircleTool::renderPreview(Layer& layer) {
    if (!m_isDrawing || !m_previewSnapshot) {
        return;
    }

    // Restore base state (remove previous preview frame)
    layer.restoreFromSnapshot(*m_previewSnapshot);
    
    int cx = (m_startX + m_endX) / 2;
    int cy = (m_startY + m_endY) / 2;
    int rx = std::abs(m_endX - m_startX) / 2;
    int ry = std::abs(m_endY - m_startY) / 2;

    // Draw new preview
    if (m_shiftHeld && rx > 0 && ry > 0) {
        // Make it a perfect circle for preview too
        int radius = std::max(rx, ry);
        drawCircle(layer, cx, cy, radius, m_filled);
    } else if (rx > 0 && ry > 0) {
        drawEllipse(layer, cx, cy, rx, ry, m_filled);
    }
}

bool CircleTool::isPreviewActive() const {
    return m_isDrawing && static_cast<bool>(m_previewSnapshot);
}

void CircleTool::drawEllipse(Layer& layer, int cx, int cy, int rx, int ry, bool filled) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    
    auto setter = [&buffer, this](int px, int py, const Color& c) -> bool {
        if (buffer.isInBounds(px, py)) {
            Color existing = buffer.getPixel(px, py);
            Color blended = existing.blend(c);
            buffer.setPixel(px, py, blended);
        }
        return true;
    };

    if (filled) {
        DrawingUtils::drawFilledEllipse(cx, cy, rx, ry, m_color, setter);
    } else {
        DrawingUtils::drawEllipse(cx, cy, rx, ry, m_color, setter);
        // For outline, we could add brush thickness here if needed
    }
    
    layer.updateTexture();
}

void CircleTool::drawCircle(Layer& layer, int cx, int cy, int radius, bool filled) {
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
        DrawingUtils::drawFilledCircle(cx, cy, radius, m_color, setter);
    } else {
        DrawingUtils::drawCircle(cx, cy, radius, m_color, setter);
    }
    
    layer.updateTexture();
}

} // namespace Canvas
