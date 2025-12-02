#ifndef CANVAS_LAYER_MANAGER_H
#define CANVAS_LAYER_MANAGER_H

#include <vector>
#include <memory>
#include <string>

namespace Canvas {

class Layer;

/**
 * Manages the collection of layers in the canvas.
 * Handles layer creation, deletion, reordering, and selection.
 */
class LayerManager {
public:
    LayerManager(int canvasWidth, int canvasHeight);
    ~LayerManager();

    // Layer creation/deletion
    Layer* addLayer(const std::string& name = "");
    Layer* insertLayer(size_t index, const std::string& name = "");
    void removeLayer(size_t index);
    void removeLayer(Layer* layer);
    void clear();

    // Layer access
    Layer* getLayer(size_t index);
    const Layer* getLayer(size_t index) const;
    Layer* getActiveLayer();
    const Layer* getActiveLayer() const;
    size_t getLayerCount() const;
    
    // Find layer index
    int getLayerIndex(const Layer* layer) const;  // Returns -1 if not found

    // Active layer selection
    void setActiveLayer(size_t index);
    void setActiveLayer(Layer* layer);
    size_t getActiveLayerIndex() const;

    // Layer reordering
    void moveLayerUp(size_t index);
    void moveLayerDown(size_t index);
    void moveLayer(size_t fromIndex, size_t toIndex);

    // Iteration (bottom to top for rendering)
    std::vector<Layer*> getLayersBottomToTop();
    std::vector<const Layer*> getLayersBottomToTop() const;

    // Canvas dimensions
    int getCanvasWidth() const;
    int getCanvasHeight() const;

    // Merge operations
    void mergeDown(size_t index);  // Merge layer with the one below it
    std::unique_ptr<Layer> flattenAll() const;  // Create a flattened copy

private:
    std::string generateLayerName() const;

private:
    std::vector<std::unique_ptr<Layer>> m_layers;
    size_t m_activeLayerIndex = 0;
    int m_canvasWidth;
    int m_canvasHeight;
    int m_layerCounter = 0;  // For auto-naming
};

} // namespace Canvas

#endif // CANVAS_LAYER_MANAGER_H

