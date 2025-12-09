/**
 * Texture.cpp
 * OpenGL texture implementation.
 */

#include <glad/glad.h>
#include "rendering/Texture.h"
#include "core/Canvas.h"
#include <algorithm>

namespace Canvas {

Texture::Texture() : m_textureID(0), m_width(0), m_height(0) {
}

Texture::~Texture() {
    destroy();
}

Texture::Texture(Texture&& other) noexcept
    : m_textureID(other.m_textureID), m_width(other.m_width), m_height(other.m_height) {
    other.m_textureID = 0;
    other.m_width = 0;
    other.m_height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        destroy();
        m_textureID = other.m_textureID;
        m_width = other.m_width;
        m_height = other.m_height;
        
        other.m_textureID = 0;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

void Texture::create(int width, int height) {
    if (m_textureID != 0) {
        destroy();
    }
    
    m_width = width;
    m_height = height;
    
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    // Allocate texture storage
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::update(const PixelBuffer& buffer) {
    if (m_textureID == 0) {
        create(buffer.getWidth(), buffer.getHeight());
    } else if (buffer.getWidth() != m_width || buffer.getHeight() != m_height) {
        // Resize texture if dimensions changed
        destroy();
        create(buffer.getWidth(), buffer.getHeight());
    }
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer.getWidth(), buffer.getHeight(),
                    GL_RGBA, GL_UNSIGNED_BYTE, buffer.getData());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::updateRegion(const PixelBuffer& buffer, int x, int y, int width, int height) {
    if (m_textureID == 0) {
        return;
    }
    
    // Extract region from buffer
    PixelBuffer region(width, height);
    for (int sy = 0; sy < height && (y + sy) < buffer.getHeight(); ++sy) {
        for (int sx = 0; sx < width && (x + sx) < buffer.getWidth(); ++sx) {
            region.setPixel(sx, sy, buffer.getPixel(x + sx, y + sy));
        }
    }
    
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height,
                    GL_RGBA, GL_UNSIGNED_BYTE, region.getData());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int slot) const {
    if (m_textureID != 0) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::getID() const {
    return m_textureID;
}

int Texture::getWidth() const {
    return m_width;
}

int Texture::getHeight() const {
    return m_height;
}

bool Texture::isValid() const {
    return m_textureID != 0;
}

void Texture::destroy() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_width = 0;
    m_height = 0;
}

} // namespace Canvas
