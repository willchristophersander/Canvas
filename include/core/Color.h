#ifndef CANVAS_COLOR_H
#define CANVAS_COLOR_H

#include <cstdint>

namespace Canvas {

/**
 * RGBA color representation with 8-bit channels.
 * Used throughout the application for pixel data and color selection.
 */
struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;

    Color() = default;
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);

    // Predefined colors
    static Color Black();
    static Color White();
    static Color Red();
    static Color Green();
    static Color Blue();
    static Color Transparent();

    // Color operations
    Color blend(const Color& other) const;  // Alpha blend with another color
    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;

    // Convert to/from packed 32-bit format
    uint32_t toUint32() const;
    static Color fromUint32(uint32_t packed);
};

} // namespace Canvas

#endif // CANVAS_COLOR_H



