#ifndef CANVAS_CANVAS_H
#define CANVAS_CANVAS_H

#include "core/Color.h"
#include <vector>
#include <cstdint>

namespace Canvas {

/**
 * The main pixel buffer that stores image data.
 * Represents a single layer's pixel content as an RGBA buffer.
 */
class PixelBuffer {
public:
    PixelBuffer(int width, int height);
    ~PixelBuffer() = default;

    // Dimensions
    int getWidth() const;
    int getHeight() const;

    // Pixel access
    Color getPixel(int x, int y) const;
    void setPixel(int x, int y, const Color& color);
    
    // Bounds checking
    bool isInBounds(int x, int y) const;

    // Buffer operations
    void clear(const Color& color = Color::Transparent());
    void fill(const Color& color);
    
    // Raw data access (for OpenGL textures and file I/O)
    const uint8_t* getData() const;
    uint8_t* getData();
    size_t getDataSize() const;

    // Copy operations
    void copyFrom(const PixelBuffer& other);
    PixelBuffer clone() const;

private:
    int m_width;
    int m_height;
    std::vector<uint8_t> m_data;  // RGBA format, 4 bytes per pixel
};

} // namespace Canvas

#endif // CANVAS_CANVAS_H



