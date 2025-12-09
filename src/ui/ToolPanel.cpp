/**
 * ToolPanel.cpp
 * Tool selection UI panel using ImGui.
 */

#include "ui/ToolPanel.h"
#include "tools/ToolManager.h"
#include "tools/Tool.h"
#include <imgui.h>

namespace Canvas {

ToolPanel::ToolPanel(ToolManager* toolManager)
    : m_toolManager(toolManager) {
}

void ToolPanel::render() {
    if (!m_visible || !m_toolManager) {
        return;
    }

    ImGui::Begin("Tools", &m_visible, 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse
    );

    size_t toolCount = m_toolManager->getToolCount();
    size_t currentIndex = m_toolManager->getCurrentToolIndex();

    ImGui::Text("Drawing Tools");
    ImGui::Separator();
    ImGui::Spacing();

    for (size_t i = 0; i < toolCount; ++i) {
        const auto* tool = m_toolManager->getTool(i);
        if (!tool) {
            continue;
        }

        std::string toolName = tool->getName();
        bool isSelected = (i == currentIndex);

        // Style selected button differently
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.45f, 0.75f, 1.0f));
        }

        if (ImGui::Button(toolName.c_str(), ImVec2(-1, 30))) {
            m_toolManager->setCurrentTool(i);
        }

        if (isSelected) {
            ImGui::PopStyleColor(3);
        }
    }

    ImGui::End();
}

} // namespace Canvas

