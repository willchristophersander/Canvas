/**
 * DrawCommand.cpp
 * Implementation of drawing commands using snapshot pattern.
 */

#include "commands/DrawCommand.h"
#include "commands/Command.h"
#include "layers/Layer.h"
#include <sstream>

namespace Canvas {

DrawCommand::DrawCommand(Layer* layer, const std::string& toolName)
    : SnapshotCommand(layer, ""), m_toolName(toolName) {
    // Before state is captured automatically by SnapshotCommand constructor
}

void DrawCommand::execute() {
    if (m_afterState && m_layer) {
        restoreSnapshot(*m_afterState);
    }
}

void DrawCommand::undo() {
    if (m_beforeState && m_layer) {
        restoreSnapshot(*m_beforeState);
    }
}

void DrawCommand::redo() {
    execute();
}

std::string DrawCommand::getDescription() const {
    std::ostringstream oss;
    oss << m_toolName << " drawing";
    return oss.str();
}

bool DrawCommand::canMergeWith(const Command& other) const {
    // Can merge consecutive draw commands from the same tool on the same layer
    const DrawCommand* otherDraw = dynamic_cast<const DrawCommand*>(&other);
    if (!otherDraw) {
        return false;
    }
    
    return otherDraw->m_layer == m_layer && 
           otherDraw->m_toolName == m_toolName;
}

void DrawCommand::mergeWith(Command& other) {
    DrawCommand* otherDraw = dynamic_cast<DrawCommand*>(&other);
    if (!otherDraw || !canMergeWith(*otherDraw)) {
        return;
    }

    // Merge by replacing our "after" state with the other command's "after" state
    // This effectively combines both drawing operations
    if (otherDraw->m_afterState) {
        m_afterState = std::move(otherDraw->m_afterState);
    }
}

} // namespace Canvas

