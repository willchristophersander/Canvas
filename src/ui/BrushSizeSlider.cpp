/**
 * BrushSizeSlider.cpp
 * Brush size adjustment UI using ImGui.
 */

#include "ui/BrushSizeSlider.h"
#include <imgui.h>
#include <string>

namespace Canvas {

BrushSizeSlider::BrushSizeSlider() = default;

void BrushSizeSlider::setSize(int size) {
    m_currentSize = std::clamp(size, m_minSize, m_maxSize);
}

void BrushSizeSlider::setSizeChangedCallback(SizeChangedCallback callback) {
    m_callback = callback;
}

void BrushSizeSlider::render() {
    if (!m_visible) {
        return;
    }

    ImGui::Begin("Brush Size", &m_visible, 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse
    );

    ImGui::Text("Brush Size: %d", m_currentSize);
    ImGui::Spacing();
    
    if (ImGui::SliderInt("##Size", &m_currentSize, m_minSize, m_maxSize, "%d px")) {
        if (m_callback) {
            m_callback(m_currentSize);
        }
    }
    
    // Quick size buttons
    ImGui::Spacing();
    ImGui::Text("Quick Sizes:");
    int quickSizes[] = {1, 5, 10, 20, 50};
    for (int quickSize : quickSizes) {
        if (ImGui::Button(std::to_string(quickSize).c_str(), ImVec2(40, 0))) {
            m_currentSize = quickSize;
            if (m_callback) {
                m_callback(m_currentSize);
            }
        }
        if (quickSize != 50) {
            ImGui::SameLine();
        }
    }
    
    ImGui::Spacing();
    
    // Visual preview
    ImGui::Text("Preview:");
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImVec2(120, 120);
    ImVec2 center = ImVec2(canvasPos.x + canvasSize.x / 2, canvasPos.y + canvasSize.y / 2);
    
    // Draw background
    drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(40, 40, 40, 255));
    
    // Draw brush preview circle
    float radius = static_cast<float>(m_currentSize) / 2.0f;
    if (radius > 0) {
        drawList->AddCircle(center, radius, IM_COL32(255, 255, 255, 255), 32);
        drawList->AddCircle(center, radius, IM_COL32(0, 0, 0, 255), 32, 2.0f);
    }

    ImGui::Dummy(canvasSize);

    ImGui::End();
}

} // namespace Canvas

