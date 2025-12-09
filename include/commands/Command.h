#ifndef CANVAS_COMMAND_H
#define CANVAS_COMMAND_H

#include <string>
#include <memory>

namespace Canvas {

/**
 * Abstract base class for undoable commands (Command Pattern).
 * All operations that modify the canvas should be wrapped in a Command.
 * 
 * The command system uses composition and inheritance hierarchies:
 * - Command: Base interface
 * - PropertyChangeCommand<T>: For commands that swap two values
 * - SnapshotCommand: For commands that use before/after state snapshots
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

    // Command validation (can be overridden to check preconditions)
    virtual bool isValid() const { return true; }
    
    // Called after execution to check if command had any effect
    virtual bool hasEffect() const { return true; }
};

/**
 * Base class for commands that change a property by swapping old and new values.
 * This is useful for commands like opacity changes, visibility toggles, etc.
 * 
 * Template parameter T should be a simple type (float, bool, int, etc.)
 */
template<typename T>
class PropertyChangeCommand : public Command {
public:
    PropertyChangeCommand(T oldValue, T newValue, const std::string& description)
        : m_oldValue(oldValue), m_newValue(newValue), m_description(description) {
    }

    virtual ~PropertyChangeCommand() = default;

    // Get current value (for querying)
    T getCurrentValue() const { return m_newValue; }
    T getOldValue() const { return m_oldValue; }

protected:
    // Subclasses must implement these to actually change the property
    virtual void applyValue(T value) = 0;

    void execute() override {
        applyValue(m_newValue);
    }

    void undo() override {
        applyValue(m_oldValue);
    }

    std::string getDescription() const override {
        return m_description;
    }

protected:
    T m_oldValue;
    T m_newValue;
    std::string m_description;
};

} // namespace Canvas

#endif // CANVAS_COMMAND_H

