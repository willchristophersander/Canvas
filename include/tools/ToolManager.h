#ifndef CANVAS_TOOL_MANAGER_H
#define CANVAS_TOOL_MANAGER_H

#include "core/Color.h"
#include <vector>
#include <memory>
#include <string>

namespace Canvas {

class Tool;

/**
 * Manages the collection of available tools and the current tool selection.
 */
class ToolManager {
public:
    ToolManager();
    ~ToolManager();

    // Tool registration
    void registerTool(std::unique_ptr<Tool> tool);
    
    // Tool selection
    Tool* getCurrentTool();
    const Tool* getCurrentTool() const;
    void setCurrentTool(size_t index);
    void setCurrentTool(const std::string& name);
    size_t getCurrentToolIndex() const;

    // Tool enumeration
    size_t getToolCount() const;
    Tool* getTool(size_t index);
    const Tool* getTool(size_t index) const;
    std::vector<std::string> getToolNames() const;

    // Global settings (applied to all tools)
    void setColor(const Color& color);
    const Color& getColor() const;
    
    void setBrushSize(int size);
    int getBrushSize() const;

private:
    void initializeDefaultTools();

private:
    std::vector<std::unique_ptr<Tool>> m_tools;
    size_t m_currentToolIndex = 0;
    Color m_currentColor = Color::Black();
    int m_currentBrushSize = 5;
};

} // namespace Canvas

#endif // CANVAS_TOOL_MANAGER_H



