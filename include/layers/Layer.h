#ifndef CANVAS_LAYER_H
#define CANVAS_LAYER_H

#include "core/Canvas.h"
#include "rendering/Texture.h"
#include <string>
#include <memory>

namespace Canvas {

/**
 * Represents a single layer in the canvas.
 * Each layer has its own pixel buffer, opacity, visibility, and name.
 */
class Layer {
public:
    Layer(int width, int height, const std::string& name = "Layer");
    ~Layer() = default;

    // Layer properties
    const std::string& getName() const;
    void setName(const std::string& name);

    float getOpacity() const;
    void setOpacity(float opacity);  // 0.0 to 1.0

    bool isVisible() const;
    void setVisible(bool visible);

    // Pixel buffer access
    PixelBuffer& getPixelBuffer();
    const PixelBuffer& getPixelBuffer() const;

    // Texture for rendering (lazy-updated)
    Texture& getTexture();
    const Texture& getTexture() const;
    void updateTexture() const;  // Call after modifying pixel buffer
    bool isTextureDirty() const;

    // Dimensions
    int getWidth() const;
    int getHeight() const;

    // Layer operations
    void clear();
    void fill(const Color& color);
    
    // Create a snapshot for undo/redo
    std::unique_ptr<PixelBuffer> createSnapshot() const;
    void restoreFromSnapshot(const PixelBuffer& snapshot);

private:
    std::string m_name;
    float m_opacity = 1.0f;
    bool m_visible = true;
    mutable bool m_textureDirty = true;

    PixelBuffer m_pixelBuffer;
    mutable Texture m_texture;
};

} // namespace Canvas

#endif // CANVAS_LAYER_H



