#ifndef CANVAS_IMAGE_HANDLER_H
#define CANVAS_IMAGE_HANDLER_H

#include <string>
#include <memory>
#include <cstdint>

namespace Canvas {

class PixelBuffer;

/**
 * PNG image handler using stb_image/stb_image_write.
 */
class PNGHandler {
public:
    // Save pixel buffer to PNG file
    static bool save(const PixelBuffer& buffer, const std::string& filepath);
    
    // Load PNG file to pixel buffer
    static std::unique_ptr<PixelBuffer> load(const std::string& filepath);
    
    // Get last error message
    static const std::string& getLastError();

private:
    static std::string s_lastError;
};

/**
 * JPEG image handler using stb_image.
 * Note: JPEG is import-only (lossy format, not ideal for saving layer data)
 */
class JPEGHandler {
public:
    // Load JPEG file to pixel buffer (converts to RGBA)
    static std::unique_ptr<PixelBuffer> load(const std::string& filepath);
    
    // Get last error message
    static const std::string& getLastError();

private:
    static std::string s_lastError;
};

} // namespace Canvas

#endif // CANVAS_IMAGE_HANDLER_H



