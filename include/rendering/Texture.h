#ifndef CANVAS_TEXTURE_H
#define CANVAS_TEXTURE_H

#include <cstdint>

namespace Canvas {

class PixelBuffer;

/**
 * OpenGL texture wrapper for rendering pixel buffers to the screen.
 */
class Texture {
public:
    Texture();
    ~Texture();

    // Prevent copying (OpenGL resource)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Move semantics
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // Create texture from dimensions
    void create(int width, int height);
    
    // Update texture data from a pixel buffer
    void update(const PixelBuffer& buffer);
    void updateRegion(const PixelBuffer& buffer, int x, int y, int width, int height);

    // Bind/unbind for rendering
    void bind(unsigned int slot = 0) const;
    void unbind() const;

    // Properties
    unsigned int getID() const;
    int getWidth() const;
    int getHeight() const;
    bool isValid() const;

    // Cleanup
    void destroy();

private:
    unsigned int m_textureID = 0;
    int m_width = 0;
    int m_height = 0;
};

} // namespace Canvas

#endif // CANVAS_TEXTURE_H



