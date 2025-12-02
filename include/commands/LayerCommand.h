#ifndef CANVAS_LAYER_COMMAND_H
#define CANVAS_LAYER_COMMAND_H

#include "commands/Command.h"
#include <memory>
#include <string>

namespace Canvas {

class Layer;
class LayerManager;

/**
 * Command for adding a new layer.
 */
class AddLayerCommand : public Command {
public:
    AddLayerCommand(LayerManager* manager, const std::string& name = "");
    ~AddLayerCommand() override = default;

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    LayerManager* m_manager;
    std::string m_layerName;
    size_t m_layerIndex = 0;
};

/**
 * Command for removing a layer.
 */
class RemoveLayerCommand : public Command {
public:
    RemoveLayerCommand(LayerManager* manager, size_t index);
    ~RemoveLayerCommand() override = default;

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    LayerManager* m_manager;
    size_t m_layerIndex;
    std::unique_ptr<Layer> m_removedLayer;  // Stored for undo
};

/**
 * Command for changing layer opacity.
 */
class LayerOpacityCommand : public Command {
public:
    LayerOpacityCommand(Layer* layer, float newOpacity);
    ~LayerOpacityCommand() override = default;

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Layer* m_layer;
    float m_oldOpacity;
    float m_newOpacity;
};

/**
 * Command for changing layer visibility.
 */
class LayerVisibilityCommand : public Command {
public:
    LayerVisibilityCommand(Layer* layer, bool visible);
    ~LayerVisibilityCommand() override = default;

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Layer* m_layer;
    bool m_oldVisible;
    bool m_newVisible;
};

/**
 * Command for reordering layers.
 */
class MoveLayerCommand : public Command {
public:
    MoveLayerCommand(LayerManager* manager, size_t fromIndex, size_t toIndex);
    ~MoveLayerCommand() override = default;

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    LayerManager* m_manager;
    size_t m_fromIndex;
    size_t m_toIndex;
};

} // namespace Canvas

#endif // CANVAS_LAYER_COMMAND_H

