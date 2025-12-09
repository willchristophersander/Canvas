#ifndef CANVAS_DRAW_COMMAND_H
#define CANVAS_DRAW_COMMAND_H

#include "commands/SnapshotCommand.h"
#include <memory>

namespace Canvas {

class Layer;

/**
 * Command that captures a drawing operation on a layer.
 * Uses the SnapshotCommand pattern for undo/redo.
 * 
 * Usage:
 *   1. Create command (automatically captures "before" state)
 *   2. Perform drawing operations on the layer
 *   3. Call captureAfter()
 *   4. Add to command history
 */
class DrawCommand : public SnapshotCommand {
public:
    DrawCommand(Layer* layer, const std::string& toolName);
    ~DrawCommand() override = default;

    void execute() override;
    void undo() override;
    void redo() override;
    std::string getDescription() const override;

    // Merge with another draw command if possible (optimization)
    bool canMergeWith(const Command& other) const override;
    void mergeWith(Command& other) override;

private:
    std::string m_toolName;
};

} // namespace Canvas

#endif // CANVAS_DRAW_COMMAND_H

