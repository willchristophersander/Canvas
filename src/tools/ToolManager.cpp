/**
 * ToolManager.cpp
 * Tool management and registration.
 */

#include "tools/ToolManager.h"
#include "tools/Tool.h"
#include "tools/PencilTool.h"
#include "tools/LineTool.h"
#include "tools/RectangleTool.h"
#include "tools/CircleTool.h"
#include "tools/SprayTool.h"
#include "tools/FillTool.h"
#include "tools/EraserTool.h"
#include "core/Canvas.h"  // Need complete PixelBuffer type for tool destructors

namespace Canvas {

ToolManager::ToolManager() {
    initializeDefaultTools();
}

ToolManager::~ToolManager() {
    // Explicitly destroy tools here where PixelBuffer is complete
    // This ensures all tool destructors (which destroy unique_ptr<PixelBuffer>)
    // are instantiated with complete types
    m_tools.clear();
}

void ToolManager::registerTool(std::unique_ptr<Tool> tool) {
    if (tool) {
        tool->setColor(m_currentColor);
        tool->setBrushSize(m_currentBrushSize);
        m_tools.push_back(std::move(tool));
    }
}

Tool* ToolManager::getCurrentTool() {
    return getTool(m_currentToolIndex);
}

const Tool* ToolManager::getCurrentTool() const {
    return getTool(m_currentToolIndex);
}

void ToolManager::setCurrentTool(size_t index) {
    if (index < m_tools.size()) {
        if (m_currentToolIndex < m_tools.size()) {
            m_tools[m_currentToolIndex]->onDeselect();
        }
        m_currentToolIndex = index;
        if (m_currentToolIndex < m_tools.size()) {
            m_tools[m_currentToolIndex]->onSelect();
        }
    }
}

void ToolManager::setCurrentTool(const std::string& name) {
    for (size_t i = 0; i < m_tools.size(); ++i) {
        if (m_tools[i]->getName() == name) {
            setCurrentTool(i);
            return;
        }
    }
}

size_t ToolManager::getCurrentToolIndex() const {
    return m_currentToolIndex;
}

size_t ToolManager::getToolCount() const {
    return m_tools.size();
}

Tool* ToolManager::getTool(size_t index) {
    return index < m_tools.size() ? m_tools[index].get() : nullptr;
}

const Tool* ToolManager::getTool(size_t index) const {
    return index < m_tools.size() ? m_tools[index].get() : nullptr;
}

std::vector<std::string> ToolManager::getToolNames() const {
    std::vector<std::string> names;
    names.reserve(m_tools.size());
    for (const auto& tool : m_tools) {
        names.push_back(tool->getName());
    }
    return names;
}

void ToolManager::setColor(const Color& color) {
    m_currentColor = color;
    for (auto& tool : m_tools) {
        tool->setColor(color);
    }
}

const Color& ToolManager::getColor() const {
    return m_currentColor;
}

void ToolManager::setBrushSize(int size) {
    m_currentBrushSize = size > 0 ? size : 1;
    for (auto& tool : m_tools) {
        tool->setBrushSize(m_currentBrushSize);
    }
}

int ToolManager::getBrushSize() const {
    return m_currentBrushSize;
}

void ToolManager::initializeDefaultTools() {
    registerTool(std::make_unique<PencilTool>());
    registerTool(std::make_unique<LineTool>());
    registerTool(std::make_unique<RectangleTool>());
    registerTool(std::make_unique<CircleTool>());
    registerTool(std::make_unique<SprayTool>());
    registerTool(std::make_unique<FillTool>());
    registerTool(std::make_unique<EraserTool>());
    
    // Set initial tool
    if (!m_tools.empty()) {
        m_tools[0]->onSelect();
    }
}

} // namespace Canvas

