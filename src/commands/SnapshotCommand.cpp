/**
 * SnapshotCommand.cpp
 * Base implementation for snapshot-based commands.
 */

#include "commands/SnapshotCommand.h"
#include "layers/Layer.h"

namespace Canvas {

SnapshotCommand::SnapshotCommand(Layer* layer, const std::string& description)
    : m_layer(layer), m_description(description) {
    if (m_layer) {
        captureBefore();
    }
}

void SnapshotCommand::captureBefore() {
    if (m_layer) {
        m_beforeState = m_layer->createSnapshot();
    }
}

void SnapshotCommand::captureAfter() {
    if (m_layer) {
        m_afterState = m_layer->createSnapshot();
    }
}

void SnapshotCommand::restoreSnapshot(const PixelBuffer& snapshot) {
    if (m_layer) {
        m_layer->restoreFromSnapshot(snapshot);
        m_layer->updateTexture();
    }
}

} // namespace Canvas

