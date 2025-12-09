/**
 * SnapshotCommand.h
 * Base class for commands that use before/after state snapshots.
 * Useful for drawing operations and any command that needs to capture full state.
 */

#ifndef CANVAS_SNAPSHOT_COMMAND_H
#define CANVAS_SNAPSHOT_COMMAND_H

#include "commands/Command.h"
#include "core/Canvas.h"
#include <memory>
#include <string>

namespace Canvas {

class Layer;

/**
 * Base class for commands that capture state before and after execution.
 * This pattern is used by DrawCommand and could be extended for other
 * operations that need full state restoration.
 */
class SnapshotCommand : public Command {
public:
    SnapshotCommand(Layer* layer, const std::string& description);
    virtual ~SnapshotCommand() = default;

    // Capture the current state as "before" snapshot
    void captureBefore();

    // Capture the current state as "after" snapshot
    void captureAfter();

    // Check if both snapshots are available
    bool hasSnapshots() const { return m_beforeState != nullptr && m_afterState != nullptr; }

protected:
    Layer* m_layer;
    std::string m_description;
    std::unique_ptr<PixelBuffer> m_beforeState;
    std::unique_ptr<PixelBuffer> m_afterState;

    // Helper to restore a snapshot
    void restoreSnapshot(const PixelBuffer& snapshot);
};

} // namespace Canvas

#endif // CANVAS_SNAPSHOT_COMMAND_H

