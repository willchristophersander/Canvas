/**
 * Layer.cpp
 * Layer implementation with minimal boilerplate.
 */

#include "layers/Layer.h"
#include "core/Color.h"
#include <algorithm>

namespace Canvas {

Layer::Layer(int width, int height, const std::string& name)
    : m_name(name), m_pixelBuffer(width, height) {
    m_texture.create(width, height);
    m_textureDirty = true;
}

const std::string& Layer::getName() const {
    return m_name;
}

void Layer::setName(const std::string& name) {
    m_name = name;
}

float Layer::getOpacity() const {
    return m_opacity;
}

void Layer::setOpacity(float opacity) {
    m_opacity = std::clamp(opacity, 0.0f, 1.0f);
}

bool Layer::isVisible() const {
    return m_visible;
}

void Layer::setVisible(bool visible) {
    m_visible = visible;
}

PixelBuffer& Layer::getPixelBuffer() {
    m_textureDirty = true; // Mark dirty when accessed for modification
    return m_pixelBuffer;
}

const PixelBuffer& Layer::getPixelBuffer() const {
    return m_pixelBuffer;
}

Texture& Layer::getTexture() {
    if (m_textureDirty) {
        updateTexture();
    }
    return m_texture;
}

const Texture& Layer::getTexture() const {
    if (m_textureDirty) {
        updateTexture();
    }
    return m_texture;
}

void Layer::updateTexture() const {
    m_texture.update(m_pixelBuffer);
    m_textureDirty = false;
}

bool Layer::isTextureDirty() const {
    return m_textureDirty;
}

int Layer::getWidth() const {
    return m_pixelBuffer.getWidth();
}

int Layer::getHeight() const {
    return m_pixelBuffer.getHeight();
}

void Layer::clear() {
    m_pixelBuffer.clear(Color::Transparent());
    m_textureDirty = true;
}

void Layer::fill(const Color& color) {
    m_pixelBuffer.fill(color);
    m_textureDirty = true;
}

std::unique_ptr<PixelBuffer> Layer::createSnapshot() const {
    return std::make_unique<PixelBuffer>(m_pixelBuffer.clone());
}

void Layer::restoreFromSnapshot(const PixelBuffer& snapshot) {
    m_pixelBuffer.copyFrom(snapshot);
    m_textureDirty = true;
}

} // namespace Canvas

