/**
 * LayerCommand.cpp
 * Implementation of layer-related commands.
 * Uses PropertyChangeCommand base class to minimize boilerplate.
 */

#include "commands/LayerCommand.h"
#include "layers/Layer.h"
#include "layers/LayerManager.h"
#include <sstream>
#include <algorithm>

namespace Canvas {

// ============================================================================
// LayerCommand base class
// ============================================================================

LayerCommand::LayerCommand(LayerManager* manager)
    : m_manager(manager) {
}

std::string LayerCommand::formatLayerName(const std::string& name, size_t index) const {
    if (!name.empty()) {
        return name;
    }
    std::ostringstream oss;
    oss << "Layer " << (index + 1);
    return oss.str();
}

// ============================================================================
// AddLayerCommand
// ============================================================================

AddLayerCommand::AddLayerCommand(LayerManager* manager, const std::string& name)
    : LayerCommand(manager), m_layerName(name) {
}

void AddLayerCommand::execute() {
    if (!m_manager) return;
    
    Layer* layer = m_manager->insertLayer(m_manager->getLayerCount(), m_layerName);
    m_layerIndex = m_manager->getLayerIndex(layer);
    m_manager->setActiveLayer(m_layerIndex);
}

void AddLayerCommand::undo() {
    if (!m_manager) return;
    
    if (m_layerIndex < m_manager->getLayerCount()) {
        m_manager->removeLayer(m_layerIndex);
    }
}

std::string AddLayerCommand::getDescription() const {
    std::ostringstream oss;
    oss << "Add " << formatLayerName(m_layerName, m_layerIndex);
    return oss.str();
}

// ============================================================================
// RemoveLayerCommand
// ============================================================================

RemoveLayerCommand::RemoveLayerCommand(LayerManager* manager, size_t index)
    : LayerCommand(manager), m_layerIndex(index) {
    // Capture the layer to be removed for undo
    if (m_manager) {
        Layer* layer = m_manager->getLayer(m_layerIndex);
        if (layer) {
            m_removedLayer = std::make_unique<Layer>(
                layer->getWidth(), 
                layer->getHeight(), 
                layer->getName()
            );
            m_removedLayer->getPixelBuffer().copyFrom(layer->getPixelBuffer());
            m_removedLayer->setOpacity(layer->getOpacity());
            m_removedLayer->setVisible(layer->isVisible());
        }
    }
}

void RemoveLayerCommand::execute() {
    if (!m_manager) return;
    
    if (m_layerIndex < m_manager->getLayerCount()) {
        m_manager->removeLayer(m_layerIndex);
    }
}

void RemoveLayerCommand::undo() {
    if (!m_manager || !m_removedLayer) return;
    
    Layer* restored = m_manager->insertLayer(m_layerIndex, m_removedLayer->getName());
    if (restored) {
        restored->getPixelBuffer().copyFrom(m_removedLayer->getPixelBuffer());
        restored->setOpacity(m_removedLayer->getOpacity());
        restored->setVisible(m_removedLayer->isVisible());
        restored->updateTexture();
        m_manager->setActiveLayer(m_layerIndex);
    }
}

std::string RemoveLayerCommand::getDescription() const {
    if (!m_removedLayer) {
        return "Remove layer";
    }
    std::ostringstream oss;
    oss << "Remove " << formatLayerName(m_removedLayer->getName(), m_layerIndex);
    return oss.str();
}

// ============================================================================
// LayerOpacityCommand (uses PropertyChangeCommand template)
// ============================================================================

LayerOpacityCommand::LayerOpacityCommand(Layer* layer, float newOpacity)
    : PropertyChangeCommand<float>(
        layer ? layer->getOpacity() : 1.0f,
        std::clamp(newOpacity, 0.0f, 1.0f),
        "Change layer opacity"
    ),
    m_layer(layer) {
}

void LayerOpacityCommand::applyValue(float value) {
    if (m_layer) {
        m_layer->setOpacity(value);
    }
}

// ============================================================================
// LayerVisibilityCommand (uses PropertyChangeCommand template)
// ============================================================================

LayerVisibilityCommand::LayerVisibilityCommand(Layer* layer, bool visible)
    : PropertyChangeCommand<bool>(
        layer ? layer->isVisible() : true,
        visible,
        visible ? "Show layer" : "Hide layer"
    ),
    m_layer(layer) {
}

void LayerVisibilityCommand::applyValue(bool value) {
    if (m_layer) {
        m_layer->setVisible(value);
    }
}

// ============================================================================
// MoveLayerCommand
// ============================================================================

MoveLayerCommand::MoveLayerCommand(LayerManager* manager, size_t fromIndex, size_t toIndex)
    : LayerCommand(manager), m_fromIndex(fromIndex), m_toIndex(toIndex) {
}

void MoveLayerCommand::execute() {
    if (!m_manager) return;
    
    if (m_fromIndex < m_manager->getLayerCount() && 
        m_toIndex < m_manager->getLayerCount()) {
        m_manager->moveLayer(m_fromIndex, m_toIndex);
    }
}

void MoveLayerCommand::undo() {
    if (!m_manager) return;
    
    // Undo by moving back
    if (m_fromIndex < m_manager->getLayerCount() && 
        m_toIndex < m_manager->getLayerCount()) {
        m_manager->moveLayer(m_toIndex, m_fromIndex);
    }
}

std::string MoveLayerCommand::getDescription() const {
    std::ostringstream oss;
    oss << "Move layer from " << (m_fromIndex + 1) << " to " << (m_toIndex + 1);
    return oss.str();
}

} // namespace Canvas

