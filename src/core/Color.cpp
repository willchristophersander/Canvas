/**
 * Color.cpp
 * Implementation of the Color struct.
 */

#include "core/Color.h"

namespace Canvas {

Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    : r(red), g(green), b(blue), a(alpha) {
}

Color Color::Black() {
    return Color(0, 0, 0, 255);
}

Color Color::White() {
    return Color(255, 255, 255, 255);
}

Color Color::Red() {
    return Color(255, 0, 0, 255);
}

Color Color::Green() {
    return Color(0, 255, 0, 255);
}

Color Color::Blue() {
    return Color(0, 0, 255, 255);
}

Color Color::Transparent() {
    return Color(0, 0, 0, 0);
}

Color Color::blend(const Color& other) const {
    if (other.a == 0) {
        return *this;
    }
    if (other.a == 255) {
        return other;
    }
    
    // Alpha blending: result = src * src_alpha + dst * (1 - src_alpha)
    float srcAlpha = other.a / 255.0f;
    float dstAlpha = a / 255.0f;
    float resultAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);
    
    if (resultAlpha == 0.0f) {
        return Transparent();
    }
    
    float invResultAlpha = 1.0f / resultAlpha;
    uint8_t r_blend = static_cast<uint8_t>(
        (other.r * srcAlpha + r * dstAlpha * (1.0f - srcAlpha)) * invResultAlpha
    );
    uint8_t g_blend = static_cast<uint8_t>(
        (other.g * srcAlpha + g * dstAlpha * (1.0f - srcAlpha)) * invResultAlpha
    );
    uint8_t b_blend = static_cast<uint8_t>(
        (other.b * srcAlpha + b * dstAlpha * (1.0f - srcAlpha)) * invResultAlpha
    );
    
    return Color(r_blend, g_blend, b_blend, static_cast<uint8_t>(resultAlpha * 255.0f));
}

bool Color::operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

bool Color::operator!=(const Color& other) const {
    return !(*this == other);
}

uint32_t Color::toUint32() const {
    return (static_cast<uint32_t>(a) << 24) |
           (static_cast<uint32_t>(r) << 16) |
           (static_cast<uint32_t>(g) << 8) |
           static_cast<uint32_t>(b);
}

Color Color::fromUint32(uint32_t packed) {
    return Color(
        static_cast<uint8_t>((packed >> 16) & 0xFF),  // R
        static_cast<uint8_t>((packed >> 8) & 0xFF),   // G
        static_cast<uint8_t>(packed & 0xFF),          // B
        static_cast<uint8_t>((packed >> 24) & 0xFF)   // A
    );
}

} // namespace Canvas

