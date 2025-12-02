#ifndef CANVAS_RENDERER_H
#define CANVAS_RENDERER_H

#include <memory>
#include <vector>

namespace Canvas {

class Layer;
class LayerManager;
class Texture;

/**
 * Handles all OpenGL rendering operations.
 * Responsible for compositing layers and drawing to the screen.
 */
class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialization
    bool initialize();
    void shutdown();

    // Viewport management
    void setViewport(int x, int y, int width, int height);
    void clear(float r = 0.2f, float g = 0.2f, float b = 0.2f, float a = 1.0f);

    // Layer rendering
    void renderLayer(const Layer& layer, float opacity = 1.0f);
    void renderAllLayers(const LayerManager& layerManager);
    
    // Composite all visible layers into a final image
    void compositeToScreen(const LayerManager& layerManager);

    // Texture rendering (for UI elements, etc.)
    void renderTexture(const Texture& texture, float x, float y, float width, float height);

    // Canvas area (where drawing happens)
    void setCanvasOffset(float x, float y);
    void setCanvasScale(float scale);
    float getCanvasOffsetX() const;
    float getCanvasOffsetY() const;
    float getCanvasScale() const;

private:
    bool initializeShaders();
    bool initializeQuad();

private:
    unsigned int m_shaderProgram = 0;
    unsigned int m_quadVAO = 0;
    unsigned int m_quadVBO = 0;
    
    float m_canvasOffsetX = 0.0f;
    float m_canvasOffsetY = 0.0f;
    float m_canvasScale = 1.0f;
};

} // namespace Canvas

#endif // CANVAS_RENDERER_H

