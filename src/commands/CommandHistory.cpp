/**
 * CommandHistory.cpp
 * Command history management with undo/redo functionality.
 * Designed to be efficient and extensible.
 */

#include "commands/CommandHistory.h"
#include "commands/Command.h"
#include <algorithm>

namespace Canvas {

// Helper functions to reduce boilerplate
namespace {
    inline void trimStack(std::vector<std::unique_ptr<Command>>& stack, size_t maxSize) {
        if (stack.size() > maxSize) {
            stack.erase(stack.begin(), stack.begin() + (stack.size() - maxSize));
        }
    }
}

CommandHistory::CommandHistory(size_t maxHistorySize)
    : m_maxHistorySize(maxHistorySize) {
}

CommandHistory::~CommandHistory() = default;

void CommandHistory::executeCommand(std::unique_ptr<Command> command) {
    if (!command || !command->isValid()) {
        return;
    }

    // Clear redo stack when new command is executed
    m_redoStack.clear();

    // Try to merge with last command if possible (optimization)
    if (!m_undoStack.empty() && m_undoStack.back()->canMergeWith(*command)) {
        m_undoStack.back()->mergeWith(*command);
        return;
    }

    // Execute and add to undo stack
    command->execute();
    
    if (command->hasEffect()) {
        m_undoStack.push_back(std::move(command));
        trimStack(m_undoStack, m_maxHistorySize);
    }
}

void CommandHistory::addCommand(std::unique_ptr<Command> command) {
    if (!command || !command->isValid()) {
        return;
    }

    // Clear redo stack
    m_redoStack.clear();

    // Try to merge if possible
    if (!m_undoStack.empty() && m_undoStack.back()->canMergeWith(*command)) {
        m_undoStack.back()->mergeWith(*command);
        return;
    }

    if (command->hasEffect()) {
        m_undoStack.push_back(std::move(command));
        trimStack(m_undoStack, m_maxHistorySize);
    }
}

bool CommandHistory::canUndo() const {
    return !m_undoStack.empty();
}

bool CommandHistory::canRedo() const {
    return !m_redoStack.empty();
}

void CommandHistory::undo() {
    if (m_undoStack.empty()) {
        return;
    }

    auto command = std::move(m_undoStack.back());
    m_undoStack.pop_back();

    command->undo();
    m_redoStack.push_back(std::move(command));
    trimStack(m_redoStack, m_maxHistorySize);
}

void CommandHistory::redo() {
    if (m_redoStack.empty()) {
        return;
    }

    auto command = std::move(m_redoStack.back());
    m_redoStack.pop_back();

    command->redo();
    m_undoStack.push_back(std::move(command));
    trimStack(m_undoStack, m_maxHistorySize);
}

void CommandHistory::undo(size_t steps) {
    steps = std::min(steps, m_undoStack.size());
    for (size_t i = 0; i < steps; ++i) {
        undo();
    }
}

void CommandHistory::redo(size_t steps) {
    steps = std::min(steps, m_redoStack.size());
    for (size_t i = 0; i < steps; ++i) {
        redo();
    }
}

size_t CommandHistory::getUndoCount() const {
    return m_undoStack.size();
}

size_t CommandHistory::getRedoCount() const {
    return m_redoStack.size();
}

std::string CommandHistory::getUndoDescription() const {
    if (m_undoStack.empty()) {
        return "";
    }
    return m_undoStack.back()->getDescription();
}

std::string CommandHistory::getRedoDescription() const {
    if (m_redoStack.empty()) {
        return "";
    }
    return m_redoStack.back()->getDescription();
}

void CommandHistory::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
}

void CommandHistory::setMaxHistorySize(size_t size) {
    m_maxHistorySize = size;
    trimStack(m_undoStack, m_maxHistorySize);
    trimStack(m_redoStack, m_maxHistorySize);
}

size_t CommandHistory::getMaxHistorySize() const {
    return m_maxHistorySize;
}

} // namespace Canvas

