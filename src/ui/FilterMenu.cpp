/**
 * FilterMenu.cpp
 * Filter application UI using ImGui.
 */

#include "ui/FilterMenu.h"
#include "filters/FilterManager.h"
#include "layers/LayerManager.h"
#include "layers/Layer.h"
#include "commands/CommandHistory.h"
#include "commands/DrawCommand.h"
#include "core/Canvas.h"
#include <imgui.h>
#include <algorithm>

namespace Canvas {

FilterMenu::FilterMenu(FilterManager* filterManager, LayerManager* layerManager, CommandHistory* history)
    : m_filterManager(filterManager),
      m_layerManager(layerManager),
      m_commandHistory(history) {
    refreshFilterList();
}

void FilterMenu::render() {
    if (!m_visible || !m_filterManager || !m_layerManager) {
        return;
    }

    ImGui::Begin("Filters", &m_visible, 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse
    );

    if (m_filterNames.empty()) {
        ImGui::TextWrapped("No filters available.\nPlace Python filter files in the filters directory.");
        ImGui::Spacing();
        if (ImGui::Button("Refresh", ImVec2(-1, 0))) {
            refreshFilterList();
        }
        ImGui::End();
        return;
    }

    ImGui::Text("Apply Filter to Active Layer");
    ImGui::Separator();
    ImGui::Spacing();

    // Filter list
    for (const auto& filterName : m_filterNames) {
        if (ImGui::Button(filterName.c_str(), ImVec2(-1, 0))) {
            applyFilter(filterName);
        }
    }

    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Refresh Filters", ImVec2(-1, 0))) {
        refreshFilterList();
    }

    ImGui::End();
}

void FilterMenu::renderMenuBarItem() {
    if (ImGui::BeginMenu("Filters")) {
        if (ImGui::MenuItem("Show Filter Panel", nullptr, &m_visible)) {
        }
        if (ImGui::MenuItem("Refresh Filters")) {
            refreshFilterList();
        }
        ImGui::EndMenu();
    }
}

void FilterMenu::refreshFilterList() {
    if (m_filterManager) {
        m_filterNames = m_filterManager->getAvailableFilters();
    }
}

void FilterMenu::applyFilter(const std::string& filterName) {
    Layer* activeLayer = m_layerManager->getActiveLayer();
    if (!activeLayer || !m_filterManager || !m_commandHistory) {
        return;
    }

    // Create draw command for undo/redo
    auto cmd = std::make_unique<DrawCommand>(activeLayer, "Filter: " + filterName);

    // Apply filter to layer's pixel buffer
    PixelBuffer& buffer = activeLayer->getPixelBuffer();
    bool success = m_filterManager->applyFilter(filterName, buffer);

    if (success) {
        // Capture after state and add to history
        cmd->captureAfter();
        activeLayer->updateTexture();
        m_commandHistory->addCommand(std::move(cmd));
    } else {
        // Filter failed, don't add command
        // Error message available via FilterManager::getLastError()
    }
}

} // namespace Canvas

