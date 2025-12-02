#ifndef CANVAS_COMMAND_HISTORY_H
#define CANVAS_COMMAND_HISTORY_H

#include <vector>
#include <memory>

namespace Canvas {

class Command;

/**
 * Manages the undo/redo history using a stack-based approach.
 * Supports multi-step undo/redo operations.
 */
class CommandHistory {
public:
    CommandHistory(size_t maxHistorySize = 100);
    ~CommandHistory();

    // Execute a command and add it to history
    void executeCommand(std::unique_ptr<Command> command);
    
    // Add a command to history without executing it
    // (for commands that have already been executed)
    void addCommand(std::unique_ptr<Command> command);

    // Undo/Redo operations
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();

    // Multi-step undo/redo
    void undo(size_t steps);
    void redo(size_t steps);

    // History info
    size_t getUndoCount() const;
    size_t getRedoCount() const;
    std::string getUndoDescription() const;
    std::string getRedoDescription() const;

    // Clear history
    void clear();

    // Set maximum history size (older commands are discarded)
    void setMaxHistorySize(size_t size);
    size_t getMaxHistorySize() const;

private:
    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
    size_t m_maxHistorySize;
};

} // namespace Canvas

#endif // CANVAS_COMMAND_HISTORY_H

