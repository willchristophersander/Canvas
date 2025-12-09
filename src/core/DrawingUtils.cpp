/**
 * DrawingUtils.cpp
 * Implementation of shared drawing primitives.
 */

#include "core/DrawingUtils.h"
#include <cmath>
#include <algorithm>

namespace Canvas {
namespace DrawingUtils {

void drawLine(int x0, int y0, int x1, int y1, const Color& color, 
              const PixelSetter& setter) {
    // Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;

    while (true) {
        if (!setter(x, y, color)) {
            break;  // Callback requested to stop
        }

        if (x == x1 && y == y1) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void drawCircle(int cx, int cy, int radius, const Color& color,
                const PixelSetter& setter) {
    if (radius <= 0) {
        setter(cx, cy, color);
        return;
    }

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    auto plotCirclePoints = [&](int x, int y) {
        setter(cx + x, cy + y, color);
        setter(cx - x, cy + y, color);
        setter(cx + x, cy - y, color);
        setter(cx - x, cy - y, color);
        setter(cx + y, cy + x, color);
        setter(cx - y, cy + x, color);
        setter(cx + y, cy - x, color);
        setter(cx - y, cy - x, color);
    };

    plotCirclePoints(x, y);

    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plotCirclePoints(x, y);
    }
}

void drawFilledCircle(int cx, int cy, int radius, const Color& color,
                      const PixelSetter& setter) {
    if (radius <= 0) {
        setter(cx, cy, color);
        return;
    }

    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x * x + y * y <= radius * radius) {
                setter(cx + x, cy + y, color);
            }
        }
    }
}

void drawEllipse(int cx, int cy, int rx, int ry, const Color& color,
                 const PixelSetter& setter) {
    if (rx <= 0 || ry <= 0) {
        return;
    }

    // Midpoint ellipse algorithm
    int x = 0;
    int y = ry;
    
    long long rx2 = static_cast<long long>(rx) * rx;
    long long ry2 = static_cast<long long>(ry) * ry;
    long long twoRx2 = 2 * rx2;
    long long twoRy2 = 2 * ry2;
    
    long long p;
    long long px = 0;
    long long py = twoRx2 * y;

    // Region 1
    p = static_cast<long long>(std::round(ry2 - (rx2 * ry) + (0.25 * rx2)));
    while (px < py) {
        setter(cx + x, cy + y, color);
        setter(cx - x, cy + y, color);
        setter(cx + x, cy - y, color);
        setter(cx - x, cy - y, color);

        x++;
        px += twoRy2;
        if (p < 0) {
            p += ry2 + px;
        } else {
            y--;
            py -= twoRx2;
            p += ry2 + px - py;
        }
    }

    // Region 2
    p = static_cast<long long>(std::round(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2));
    while (y >= 0) {
        setter(cx + x, cy + y, color);
        setter(cx - x, cy + y, color);
        setter(cx + x, cy - y, color);
        setter(cx - x, cy - y, color);

        y--;
        py -= twoRx2;
        if (p > 0) {
            p += rx2 - py;
        } else {
            x++;
            px += twoRy2;
            p += rx2 - py + px;
        }
    }
}

void drawFilledEllipse(int cx, int cy, int rx, int ry, const Color& color,
                       const PixelSetter& setter) {
    if (rx <= 0 || ry <= 0) {
        return;
    }

    long long rx2 = static_cast<long long>(rx) * rx;
    long long ry2 = static_cast<long long>(ry) * ry;

    for (int y = -ry; y <= ry; ++y) {
        for (int x = -rx; x <= rx; ++x) {
            // Check if point is inside ellipse: (x/rx)^2 + (y/ry)^2 <= 1
            long long xTerm = static_cast<long long>(x) * x * ry2;
            long long yTerm = static_cast<long long>(y) * y * rx2;
            if (xTerm + yTerm <= rx2 * ry2) {
                setter(cx + x, cy + y, color);
            }
        }
    }
}

void drawRectangle(int x1, int y1, int x2, int y2, const Color& color,
                   const PixelSetter& setter) {
    int minX = std::min(x1, x2);
    int maxX = std::max(x1, x2);
    int minY = std::min(y1, y2);
    int maxY = std::max(y1, y2);

    // Top and bottom edges
    for (int x = minX; x <= maxX; ++x) {
        setter(x, minY, color);
        setter(x, maxY, color);
    }

    // Left and right edges
    for (int y = minY + 1; y < maxY; ++y) {
        setter(minX, y, color);
        setter(maxX, y, color);
    }
}

void drawFilledRectangle(int x1, int y1, int x2, int y2, const Color& color,
                         const PixelSetter& setter) {
    int minX = std::min(x1, x2);
    int maxX = std::max(x1, x2);
    int minY = std::min(y1, y2);
    int maxY = std::max(y1, y2);

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            setter(x, y, color);
        }
    }
}

void drawBrushStamp(int centerX, int centerY, int brushSize, const Color& color,
                    const PixelSetter& setter) {
    int radius = brushSize / 2;
    if (brushSize % 2 == 0) {
        radius -= 1;  // For even sizes, make slightly smaller
    }

    // Draw a filled circle with the brush color
    // The setter function should handle alpha blending if needed
    drawFilledCircle(centerX, centerY, radius, color, setter);
}

bool colorsMatch(const Color& a, const Color& b, int tolerance) {
    if (tolerance == 0) {
        return a == b;
    }

    return colorDistance(a, b) <= tolerance;
}

int colorDistance(const Color& a, const Color& b) {
    // Calculate Euclidean distance in RGB space
    int dr = static_cast<int>(a.r) - static_cast<int>(b.r);
    int dg = static_cast<int>(a.g) - static_cast<int>(b.g);
    int db = static_cast<int>(a.b) - static_cast<int>(b.b);
    int da = static_cast<int>(a.a) - static_cast<int>(b.a);

    // Use a weighted distance (RGB more important than alpha for matching)
    return static_cast<int>(std::sqrt(dr * dr + dg * dg + db * db + da * da / 4));
}

} // namespace DrawingUtils
} // namespace Canvas

