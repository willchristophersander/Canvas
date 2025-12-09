/**
 * ImageHandler.cpp
 * Image I/O using stb_image and stb_image_write.
 * Modular handlers for PNG and JPEG formats.
 */

#include "io/ImageHandler.h"
#include "core/Canvas.h"
#include "core/Color.h"

// stb_image headers (expected in external/stb, included via CMake)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <fstream>
#include <cstring>

namespace Canvas {

// ============================================================================
// PNGHandler
// ============================================================================

std::string PNGHandler::s_lastError;

bool PNGHandler::save(const PixelBuffer& buffer, const std::string& filepath) {
    s_lastError.clear();

    if (buffer.getWidth() <= 0 || buffer.getHeight() <= 0) {
        s_lastError = "Invalid buffer dimensions";
        return false;
    }

    // stb_image_write expects data in top-to-bottom order
    // Our buffer is likely bottom-to-top, so we need to flip it
    std::vector<uint8_t> flippedData;
    flippedData.resize(buffer.getDataSize());
    
    int width = buffer.getWidth();
    int height = buffer.getHeight();
    const uint8_t* sourceData = buffer.getData();

    // Flip vertically for PNG (stb writes from top)
    for (int y = 0; y < height; ++y) {
        int srcY = height - 1 - y;  // Flip Y coordinate
        std::memcpy(
            flippedData.data() + y * width * 4,
            sourceData + srcY * width * 4,
            width * 4
        );
    }

    int result = stbi_write_png(
        filepath.c_str(),
        width,
        height,
        4,  // RGBA channels
        flippedData.data(),
        width * 4  // Stride
    );

    if (result == 0) {
        s_lastError = "Failed to write PNG file: " + filepath;
        return false;
    }

    return true;
}

std::unique_ptr<PixelBuffer> PNGHandler::load(const std::string& filepath) {
    s_lastError.clear();

    int width, height, channels;
    unsigned char* data = stbi_load(
        filepath.c_str(),
        &width,
        &height,
        &channels,
        4  // Force RGBA
    );

    if (!data) {
        s_lastError = "Failed to load PNG: " + std::string(stbi_failure_reason());
        return nullptr;
    }

    if (width <= 0 || height <= 0) {
        stbi_image_free(data);
        s_lastError = "Invalid image dimensions";
        return nullptr;
    }

    auto buffer = std::make_unique<PixelBuffer>(width, height);

    // stb_image loads from top to bottom, we need to flip for our coordinate system
    for (int y = 0; y < height; ++y) {
        int dstY = height - 1 - y;  // Flip Y coordinate
        for (int x = 0; x < width; ++x) {
            int srcIdx = (y * width + x) * 4;
            uint8_t r = data[srcIdx];
            uint8_t g = data[srcIdx + 1];
            uint8_t b = data[srcIdx + 2];
            uint8_t a = data[srcIdx + 3];
            
            buffer->setPixel(x, dstY, Color(r, g, b, a));
        }
    }

    stbi_image_free(data);
    return buffer;
}

const std::string& PNGHandler::getLastError() {
    return s_lastError;
}

// ============================================================================
// JPEGHandler
// ============================================================================

std::string JPEGHandler::s_lastError;

std::unique_ptr<PixelBuffer> JPEGHandler::load(const std::string& filepath) {
    s_lastError.clear();

    int width, height, channels;
    unsigned char* data = stbi_load(
        filepath.c_str(),
        &width,
        &height,
        &channels,
        4  // Force RGBA (JPEG doesn't have alpha, will be 255)
    );

    if (!data) {
        s_lastError = "Failed to load JPEG: " + std::string(stbi_failure_reason());
        return nullptr;
    }

    if (width <= 0 || height <= 0) {
        stbi_image_free(data);
        s_lastError = "Invalid image dimensions";
        return nullptr;
    }

    auto buffer = std::make_unique<PixelBuffer>(width, height);

    // JPEG has no alpha channel, set to opaque
    // stb_image loads from top to bottom, flip for our coordinate system
    for (int y = 0; y < height; ++y) {
        int dstY = height - 1 - y;  // Flip Y coordinate
        for (int x = 0; x < width; ++x) {
            int srcIdx = (y * width + x) * 4;
            uint8_t r = data[srcIdx];
            uint8_t g = data[srcIdx + 1];
            uint8_t b = data[srcIdx + 2];
            uint8_t a = channels == 4 ? data[srcIdx + 3] : 255;
            
            buffer->setPixel(x, dstY, Color(r, g, b, a));
        }
    }

    stbi_image_free(data);
    return buffer;
}

const std::string& JPEGHandler::getLastError() {
    return s_lastError;
}

} // namespace Canvas

