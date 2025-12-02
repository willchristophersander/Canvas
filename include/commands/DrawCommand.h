#ifndef CANVAS_DRAW_COMMAND_H
#define CANVAS_DRAW_COMMAND_H

#include "commands/Command.h"
#include "core/Canvas.h"
#include <memory>

namespace Canvas {

class Layer;

/**
 * Command that captures a drawing operation on a layer.
 * Stores the before/after state for undo/redo.
 */
class DrawCommand : public Command {
public:
    // Create command by capturing the current layer state as "before"
    // Call captureAfter() when the drawing operation is complete
    DrawCommand(Layer* layer, const std::string& toolName);
    ~DrawCommand() override = default;

    // Call this after the drawing operation completes
    void captureAfter();

    void execute() override;
    void undo() override;
    void redo() override;
    std::string getDescription() const override;

private:
    Layer* m_layer;
    std::string m_toolName;
    std::unique_ptr<PixelBuffer> m_beforeState;
    std::unique_ptr<PixelBuffer> m_afterState;
};

} // namespace Canvas

#endif // CANVAS_DRAW_COMMAND_H

