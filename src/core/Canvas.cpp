/**
 * Canvas.cpp
 * Implementation of the PixelBuffer class.
 */

#include "core/Canvas.h"
#include <cstring>
#include <algorithm>

namespace Canvas {

PixelBuffer::PixelBuffer(int width, int height)
    : m_width(width), m_height(height) {
    if (width > 0 && height > 0) {
        m_data.resize(static_cast<size_t>(width) * height * 4, 0);
    }
}

int PixelBuffer::getWidth() const {
    return m_width;
}

int PixelBuffer::getHeight() const {
    return m_height;
}

Color PixelBuffer::getPixel(int x, int y) const {
    if (!isInBounds(x, y)) {
        return Color::Transparent();
    }
    
    size_t index = static_cast<size_t>(y * m_width + x) * 4;
    return Color(
        m_data[index],     // R
        m_data[index + 1], // G
        m_data[index + 2], // B
        m_data[index + 3]  // A
    );
}

void PixelBuffer::setPixel(int x, int y, const Color& color) {
    if (!isInBounds(x, y)) {
        return;
    }
    
    size_t index = static_cast<size_t>(y * m_width + x) * 4;
    m_data[index] = color.r;
    m_data[index + 1] = color.g;
    m_data[index + 2] = color.b;
    m_data[index + 3] = color.a;
}

bool PixelBuffer::isInBounds(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

void PixelBuffer::clear(const Color& color) {
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            setPixel(x, y, color);
        }
    }
}

void PixelBuffer::fill(const Color& color) {
    clear(color);
}

const uint8_t* PixelBuffer::getData() const {
    return m_data.data();
}

uint8_t* PixelBuffer::getData() {
    return m_data.data();
}

size_t PixelBuffer::getDataSize() const {
    return m_data.size();
}

void PixelBuffer::copyFrom(const PixelBuffer& other) {
    if (other.m_width != m_width || other.m_height != m_height) {
        m_width = other.m_width;
        m_height = other.m_height;
        m_data.resize(other.m_data.size());
    }
    std::memcpy(m_data.data(), other.m_data.data(), m_data.size());
}

PixelBuffer PixelBuffer::clone() const {
    PixelBuffer cloned(m_width, m_height);
    cloned.m_data = m_data;
    return cloned;
}

} // namespace Canvas

