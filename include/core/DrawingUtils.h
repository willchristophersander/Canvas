/**
 * DrawingUtils.h
 * Shared drawing primitives and utilities for tools.
 * This module provides reusable drawing functions to avoid code duplication.
 */

#ifndef CANVAS_DRAWING_UTILS_H
#define CANVAS_DRAWING_UTILS_H

#include "core/Color.h"
#include <functional>

namespace Canvas {

class PixelBuffer;
class Layer;

/**
 * Utility functions for drawing primitives.
 * All functions work with PixelBuffer or Layer and use callbacks for flexibility.
 */
namespace DrawingUtils {

    /**
     * Callback type for setting pixels.
     * Returns false if drawing should stop (e.g., out of bounds or color match).
     */
    using PixelSetter = std::function<bool(int x, int y, const Color& color)>;

    /**
     * Draw a line using Bresenham's algorithm.
     * Calls the setter function for each pixel on the line.
     */
    void drawLine(int x0, int y0, int x1, int y1, const Color& color, 
                  const PixelSetter& setter);

    /**
     * Draw a circle outline.
     */
    void drawCircle(int cx, int cy, int radius, const Color& color,
                    const PixelSetter& setter);

    /**
     * Draw a filled circle.
     */
    void drawFilledCircle(int cx, int cy, int radius, const Color& color,
                          const PixelSetter& setter);

    /**
     * Draw an ellipse outline.
     */
    void drawEllipse(int cx, int cy, int rx, int ry, const Color& color,
                     const PixelSetter& setter);

    /**
     * Draw a filled ellipse.
     */
    void drawFilledEllipse(int cx, int cy, int rx, int ry, const Color& color,
                           const PixelSetter& setter);

    /**
     * Draw a rectangle outline.
     */
    void drawRectangle(int x1, int y1, int x2, int y2, const Color& color,
                       const PixelSetter& setter);

    /**
     * Draw a filled rectangle.
     */
    void drawFilledRectangle(int x1, int y1, int x2, int y2, const Color& color,
                             const PixelSetter& setter);

    /**
     * Draw a circular brush stamp at the given position.
     * Uses the brush size and applies the color with alpha blending.
     */
    void drawBrushStamp(int centerX, int centerY, int brushSize, const Color& color,
                        const PixelSetter& setter);

    /**
     * Check if two colors match within a tolerance threshold.
     * Tolerance is a value from 0-255 representing maximum difference per channel.
     */
    bool colorsMatch(const Color& a, const Color& b, int tolerance = 0);

    /**
     * Calculate color distance (useful for tolerance matching).
     * Returns a value representing how "far apart" two colors are.
     */
    int colorDistance(const Color& a, const Color& b);

} // namespace DrawingUtils

} // namespace Canvas

#endif // CANVAS_DRAWING_UTILS_H

