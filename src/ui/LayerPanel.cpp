/**
 * LayerPanel.cpp
 * Layer management UI panel using ImGui.
 */

#include "ui/LayerPanel.h"
#include "layers/LayerManager.h"
#include "layers/Layer.h"
#include "commands/CommandHistory.h"
#include "commands/LayerCommand.h"
#include "core/Canvas.h"
#include <imgui.h>
#include <memory>
#include <cstring>
#include <string>

namespace Canvas {

LayerPanel::LayerPanel(LayerManager* layerManager, CommandHistory* history)
    : m_layerManager(layerManager), m_commandHistory(history) {
}

void LayerPanel::render() {
    if (!m_visible || !m_layerManager || !m_commandHistory) {
        return;
    }

    ImGui::Begin("Layers", &m_visible, 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse
    );

    renderLayerControls();

    ImGui::Separator();
    ImGui::Spacing();

    // Render layers from top to bottom (for intuitive UI)
    size_t layerCount = m_layerManager->getLayerCount();
    
    if (layerCount == 0) {
        ImGui::Text("No layers");
    } else {
        for (int i = static_cast<int>(layerCount) - 1; i >= 0; --i) {
            renderLayerItem(static_cast<size_t>(i));
            if (i > 0) {
                ImGui::Spacing();
            }
        }
    }

    ImGui::End();
}

void LayerPanel::renderLayerControls() {
    Layer* activeLayer = m_layerManager->getActiveLayer();
    
    if (ImGui::Button("+ Add", ImVec2(60, 0))) {
        auto cmd = std::make_unique<AddLayerCommand>(m_layerManager, "");
        m_commandHistory->executeCommand(std::move(cmd));
    }

    ImGui::SameLine();
    
    bool canRemove = activeLayer != nullptr && m_layerManager->getLayerCount() > 1;
    if (ImGui::Button("- Remove", ImVec2(70, 0)) && canRemove) {
        size_t activeIndex = m_layerManager->getActiveLayerIndex();
        auto cmd = std::make_unique<RemoveLayerCommand>(m_layerManager, activeIndex);
        m_commandHistory->executeCommand(std::move(cmd));
    }

    ImGui::SameLine();
    
    if (ImGui::Button("↑", ImVec2(25, 0)) && activeLayer) {
        size_t index = m_layerManager->getActiveLayerIndex();
        if (index < m_layerManager->getLayerCount() - 1) {
            auto cmd = std::make_unique<MoveLayerCommand>(m_layerManager, index, index + 1);
            m_commandHistory->executeCommand(std::move(cmd));
        }
    }

    ImGui::SameLine();
    
    if (ImGui::Button("↓", ImVec2(25, 0)) && activeLayer) {
        size_t index = m_layerManager->getActiveLayerIndex();
        if (index > 0) {
            auto cmd = std::make_unique<MoveLayerCommand>(m_layerManager, index, index - 1);
            m_commandHistory->executeCommand(std::move(cmd));
        }
    }
}

void LayerPanel::renderLayerItem(size_t index) {
    Layer* layer = m_layerManager->getLayer(index);
    if (!layer) {
        return;
    }

    bool isActive = (index == m_layerManager->getActiveLayerIndex());

    ImGui::PushID(static_cast<int>(index));
    
    // Visibility toggle
    bool visible = layer->isVisible();
    if (ImGui::Checkbox(("##Visibility" + std::to_string(index)).c_str(), &visible)) {
        auto cmd = std::make_unique<LayerVisibilityCommand>(layer, visible);
        m_commandHistory->executeCommand(std::move(cmd));
    }

    ImGui::SameLine();

    // Layer name (editable)
    char nameBuffer[256];
    strncpy(nameBuffer, layer->getName().c_str(), sizeof(nameBuffer) - 1);
    nameBuffer[sizeof(nameBuffer) - 1] = '\0';
    
    // Highlight active layer name field
    if (isActive) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.25f, 0.45f, 0.65f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.5f, 0.7f, 0.7f));
    }
    
    ImGui::PushItemWidth(120);
    if (ImGui::InputText(("##Name" + std::to_string(index)).c_str(), nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        layer->setName(nameBuffer);
    }
    if (ImGui::IsItemClicked(0)) {
        m_layerManager->setActiveLayer(index);
    }
    ImGui::PopItemWidth();
    
    if (isActive) {
        ImGui::PopStyleColor(2);
    }

    ImGui::SameLine();
    
    // Opacity slider with label
    float opacity = layer->getOpacity();
    ImGui::Text("Opacity:");
    ImGui::SameLine();
    ImGui::PushItemWidth(80);
    // Convert to percentage for display (0-100%)
    float opacityPercent = opacity * 100.0f;
    if (ImGui::SliderFloat(("##Opacity" + std::to_string(index)).c_str(), &opacityPercent, 0.0f, 100.0f, "%.0f%%")) {
        // Convert back to 0.0-1.0 range
        opacity = opacityPercent / 100.0f;
        auto cmd = std::make_unique<LayerOpacityCommand>(layer, opacity);
        m_commandHistory->executeCommand(std::move(cmd));
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::PopID();
}

} // namespace Canvas
