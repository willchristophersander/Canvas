/**
 * FillTool.cpp
 * Flood fill tool using DrawingUtils color matching.
 */

#include "tools/FillTool.h"
#include "core/DrawingUtils.h"
#include "layers/Layer.h"
#include <queue>
#include <algorithm>
#include <vector>

namespace Canvas {

bool FillTool::onMouseDown(Layer& layer, const ToolInput& input) {
    if (!input.leftButton) {
        return false;
    }

    floodFill(layer, input.x, input.y, m_color);
    return true;
}

bool FillTool::onMouseMove(Layer& layer, const ToolInput& input) {
    // Fill tool doesn't respond to mouse move
    return false;
}

bool FillTool::onMouseUp(Layer& layer, const ToolInput& input) {
    // Fill happens on mouse down
    return false;
}

void FillTool::setTolerance(int tolerance) {
    m_tolerance = std::clamp(tolerance, 0, 255);
}

void FillTool::floodFill(Layer& layer, int startX, int startY, const Color& fillColor) {
    PixelBuffer& buffer = layer.getPixelBuffer();
    
    if (!buffer.isInBounds(startX, startY)) {
        return;
    }

    Color targetColor = buffer.getPixel(startX, startY);
    
    // If target and fill colors match (within tolerance), nothing to do
    if (DrawingUtils::colorsMatch(targetColor, fillColor, m_tolerance)) {
        return;
    }

    int width = buffer.getWidth();
    int height = buffer.getHeight();
    std::vector<bool> visited(width * height, false);

    // Flood fill using queue-based algorithm
    std::queue<std::pair<int, int>> pixels;
    pixels.push(std::make_pair(startX, startY));
    visited[startY * width + startX] = true;

    while (!pixels.empty()) {
        std::pair<int, int> pixel = pixels.front();
        int x = pixel.first;
        int y = pixel.second;
        pixels.pop();

        if (!buffer.isInBounds(x, y)) {
            continue;
        }

        Color current = buffer.getPixel(x, y);
        if (!DrawingUtils::colorsMatch(current, targetColor, m_tolerance)) {
            continue;
        }

        buffer.setPixel(x, y, fillColor);

        // Check neighbors
        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};

        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int idx = ny * width + nx;
                if (!visited[idx]) {
                    visited[idx] = true;
                    pixels.push(std::make_pair(nx, ny));
                }
            }
        }
    }

    // Lazy update handled by renderer
}

bool FillTool::colorsMatch(const Color& a, const Color& b) const {
    return DrawingUtils::colorsMatch(a, b, m_tolerance);
}

} // namespace Canvas

