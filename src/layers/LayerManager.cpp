/**
 * LayerManager.cpp
 * Layer management implementation with shared abstractions to minimize boilerplate.
 */

#include "layers/LayerManager.h"
#include "layers/Layer.h"
#include "core/LayerCompositor.h"
#include <algorithm>
#include <sstream>

namespace Canvas {

// Helper functions to reduce boilerplate
namespace {
    // Validate layer index bounds
    inline bool isValidIndex(size_t index, size_t count) {
        return index < count;
    }

    // Validate layer pointer
    inline bool isValidLayer(const Layer* layer) {
        return layer != nullptr;
    }

    // Clamp index to valid range
    inline size_t clampIndex(size_t index, size_t count) {
        return std::min(index, count > 0 ? count - 1 : 0);
    }
}

LayerManager::LayerManager(int canvasWidth, int canvasHeight)
    : m_canvasWidth(canvasWidth), m_canvasHeight(canvasHeight) {
    // Start with one default layer
    addLayer("Background");
}

LayerManager::~LayerManager() = default;

Layer* LayerManager::addLayer(const std::string& name) {
    return insertLayer(m_layers.size(), name);
}

Layer* LayerManager::insertLayer(size_t index, const std::string& name) {
    std::string layerName = name.empty() ? generateLayerName() : name;
    auto layer = std::make_unique<Layer>(m_canvasWidth, m_canvasHeight, layerName);
    
    Layer* layerPtr = layer.get();
    index = clampIndex(index, m_layers.size());
    m_layers.insert(m_layers.begin() + index, std::move(layer));

    // Adjust active layer index if needed
    if (index <= m_activeLayerIndex && m_layers.size() > 1) {
        m_activeLayerIndex++;
    }

    return layerPtr;
}

void LayerManager::removeLayer(size_t index) {
    if (!isValidIndex(index, m_layers.size())) {
        return;
    }

    m_layers.erase(m_layers.begin() + index);

    // Adjust active layer index
    if (m_layers.empty()) {
        m_activeLayerIndex = 0;
    } else if (index < m_activeLayerIndex) {
        m_activeLayerIndex--;
    } else if (m_activeLayerIndex >= m_layers.size()) {
        m_activeLayerIndex = m_layers.size() - 1;
    }
}

void LayerManager::removeLayer(Layer* layer) {
    int index = getLayerIndex(layer);
    if (index >= 0) {
        removeLayer(static_cast<size_t>(index));
    }
}

void LayerManager::clear() {
    m_layers.clear();
    m_activeLayerIndex = 0;
    m_layerCounter = 0;
    // Re-add default layer
    addLayer("Background");
}

Layer* LayerManager::getLayer(size_t index) {
    return isValidIndex(index, m_layers.size()) ? m_layers[index].get() : nullptr;
}

const Layer* LayerManager::getLayer(size_t index) const {
    return isValidIndex(index, m_layers.size()) ? m_layers[index].get() : nullptr;
}

Layer* LayerManager::getActiveLayer() {
    return getLayer(m_activeLayerIndex);
}

const Layer* LayerManager::getActiveLayer() const {
    return getLayer(m_activeLayerIndex);
}

size_t LayerManager::getLayerCount() const {
    return m_layers.size();
}

int LayerManager::getLayerIndex(const Layer* layer) const {
    if (!isValidLayer(layer)) {
        return -1;
    }

    for (size_t i = 0; i < m_layers.size(); ++i) {
        if (m_layers[i].get() == layer) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void LayerManager::setActiveLayer(size_t index) {
    if (isValidIndex(index, m_layers.size())) {
        m_activeLayerIndex = index;
    }
}

void LayerManager::setActiveLayer(Layer* layer) {
    int index = getLayerIndex(layer);
    if (index >= 0) {
        m_activeLayerIndex = static_cast<size_t>(index);
    }
}

size_t LayerManager::getActiveLayerIndex() const {
    return m_activeLayerIndex;
}

void LayerManager::moveLayerUp(size_t index) {
    if (index > 0 && isValidIndex(index, m_layers.size())) {
        moveLayer(index, index - 1);
    }
}

void LayerManager::moveLayerDown(size_t index) {
    if (isValidIndex(index, m_layers.size()) && index < m_layers.size() - 1) {
        moveLayer(index, index + 1);
    }
}

void LayerManager::moveLayer(size_t fromIndex, size_t toIndex) {
    if (!isValidIndex(fromIndex, m_layers.size()) || 
        !isValidIndex(toIndex, m_layers.size()) ||
        fromIndex == toIndex) {
        return;
    }

    // Move the layer
    auto layer = std::move(m_layers[fromIndex]);
    m_layers.erase(m_layers.begin() + fromIndex);

    size_t insertPos = toIndex > fromIndex ? toIndex : toIndex;
    m_layers.insert(m_layers.begin() + insertPos, std::move(layer));

    // Update active layer index
    if (m_activeLayerIndex == fromIndex) {
        m_activeLayerIndex = toIndex;
    } else if (fromIndex < m_activeLayerIndex && toIndex >= m_activeLayerIndex) {
        m_activeLayerIndex--;
    } else if (fromIndex > m_activeLayerIndex && toIndex <= m_activeLayerIndex) {
        m_activeLayerIndex++;
    }
}

std::vector<Layer*> LayerManager::getLayersBottomToTop() {
    std::vector<Layer*> result;
    result.reserve(m_layers.size());
    for (auto& layer : m_layers) {
        if (layer) {
            result.push_back(layer.get());
        }
    }
    return result;
}

std::vector<const Layer*> LayerManager::getLayersBottomToTop() const {
    std::vector<const Layer*> result;
    result.reserve(m_layers.size());
    for (const auto& layer : m_layers) {
        if (layer) {
            result.push_back(layer.get());
        }
    }
    return result;
}

int LayerManager::getCanvasWidth() const {
    return m_canvasWidth;
}

int LayerManager::getCanvasHeight() const {
    return m_canvasHeight;
}

void LayerManager::mergeDown(size_t index) {
    if (index == 0 || !isValidIndex(index, m_layers.size())) {
        return; // Can't merge bottom layer or invalid index
    }

    Layer* upperLayer = m_layers[index].get();
    Layer* lowerLayer = m_layers[index - 1].get();

    if (upperLayer && lowerLayer) {
        LayerCompositor::mergeLayers(*upperLayer, *lowerLayer);
        removeLayer(index);
    }
}

std::unique_ptr<Layer> LayerManager::flattenAll() const {
    auto layers = getLayersBottomToTop();
    auto flattenedBuffer = LayerCompositor::flattenLayers(
        layers, m_canvasWidth, m_canvasHeight
    );

    // Create a new layer with the flattened result
    auto result = std::make_unique<Layer>(m_canvasWidth, m_canvasHeight, "Flattened");
    result->getPixelBuffer().copyFrom(*flattenedBuffer);
    result->updateTexture();
    
    return result;
}

std::string LayerManager::generateLayerName() {
    m_layerCounter++;
    std::ostringstream oss;
    oss << "Layer " << m_layerCounter;
    return oss.str();
}

} // namespace Canvas

