#ifndef CANVAS_LAYER_COMMAND_H
#define CANVAS_LAYER_COMMAND_H

#include "commands/Command.h"
#include <memory>
#include <string>

namespace Canvas {

class Layer;
class LayerManager;

/**
 * Base class for layer-related commands.
 * Provides common functionality for layer operations.
 */
class LayerCommand : public Command {
public:
    LayerCommand(LayerManager* manager);
    virtual ~LayerCommand() = default;

protected:
    LayerManager* m_manager;
    
    // Helper to generate command descriptions
    std::string formatLayerName(const std::string& name, size_t index) const;
};

/**
 * Command for adding a new layer.
 */
class AddLayerCommand : public LayerCommand {
public:
    AddLayerCommand(LayerManager* manager, const std::string& name = "");
    ~AddLayerCommand() override = default;

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    std::string m_layerName;
    size_t m_layerIndex = 0;
};

/**
 * Command for removing a layer.
 */
class RemoveLayerCommand : public LayerCommand {
public:
    RemoveLayerCommand(LayerManager* manager, size_t index);
    ~RemoveLayerCommand() override = default;

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    size_t m_layerIndex;
    std::unique_ptr<Layer> m_removedLayer;  // Stored for undo
};

/**
 * Command for changing layer opacity.
 * Uses PropertyChangeCommand pattern for clean value swapping.
 */
class LayerOpacityCommand : public PropertyChangeCommand<float> {
public:
    LayerOpacityCommand(Layer* layer, float newOpacity);
    ~LayerOpacityCommand() override = default;

protected:
    void applyValue(float value) override;

private:
    Layer* m_layer;
};

/**
 * Command for changing layer visibility.
 * Uses PropertyChangeCommand pattern for clean value swapping.
 */
class LayerVisibilityCommand : public PropertyChangeCommand<bool> {
public:
    LayerVisibilityCommand(Layer* layer, bool visible);
    ~LayerVisibilityCommand() override = default;

protected:
    void applyValue(bool value) override;

private:
    Layer* m_layer;
};

/**
 * Command for reordering layers.
 */
class MoveLayerCommand : public LayerCommand {
public:
    MoveLayerCommand(LayerManager* manager, size_t fromIndex, size_t toIndex);
    ~MoveLayerCommand() override = default;

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    size_t m_fromIndex;
    size_t m_toIndex;
};

} // namespace Canvas

#endif // CANVAS_LAYER_COMMAND_H

