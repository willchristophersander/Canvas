#ifndef CANVAS_COMMAND_H
#define CANVAS_COMMAND_H

#include <string>
#include <memory>

namespace Canvas {

/**
 * Abstract base class for undoable commands (Command Pattern).
 * All operations that modify the canvas should be wrapped in a Command.
 */
class Command {
public:
    virtual ~Command() = default;

    // Execute the command
    virtual void execute() = 0;
    
    // Undo the command (restore previous state)
    virtual void undo() = 0;
    
    // Redo the command (re-apply after undo)
    virtual void redo() { execute(); }

    // Description for UI display
    virtual std::string getDescription() const = 0;

    // Memory optimization: some commands can be merged (e.g., continuous drawing)
    virtual bool canMergeWith(const Command& other) const { return false; }
    virtual void mergeWith(Command& other) {}
};

} // namespace Canvas

#endif // CANVAS_COMMAND_H

