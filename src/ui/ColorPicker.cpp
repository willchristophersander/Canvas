/**
 * ColorPicker.cpp
 * Color selection UI using ImGui.
 */

#include "ui/ColorPicker.h"
#include "core/Color.h"
#include <imgui.h>
#include <algorithm>

namespace Canvas {

ColorPicker::ColorPicker() {
    updateFloatColor();
}

void ColorPicker::setColor(const Color& color) {
    m_currentColor = color;
    updateFloatColor();
}

void ColorPicker::setColorChangedCallback(ColorChangedCallback callback) {
    m_callback = callback;
}

void ColorPicker::render() {
    if (!m_visible) {
        return;
    }

    ImGui::Begin("Color Picker", &m_visible, 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse
    );

    // Compact color picker
    bool colorChanged = ImGui::ColorPicker3("Color", m_colorFloat,
                                            ImGuiColorEditFlags_PickerHueWheel |
                                            ImGuiColorEditFlags_DisplayRGB |
                                            ImGuiColorEditFlags_DisplayHSV |
                                            ImGuiColorEditFlags_DisplayHex);

    ImGui::Separator();
    
    // Alpha channel separately
    ImGui::Text("Opacity:");
    if (ImGui::SliderFloat("##Alpha", &m_colorFloat[3], 0.0f, 1.0f)) {
        colorChanged = true;
    }

    if (colorChanged) {
        // Update color from float values
        m_currentColor.r = static_cast<uint8_t>(m_colorFloat[0] * 255.0f);
        m_currentColor.g = static_cast<uint8_t>(m_colorFloat[1] * 255.0f);
        m_currentColor.b = static_cast<uint8_t>(m_colorFloat[2] * 255.0f);
        m_currentColor.a = static_cast<uint8_t>(m_colorFloat[3] * 255.0f);

        if (m_callback) {
            m_callback(m_currentColor);
        }
    }

    // Quick color presets in a grid
    ImGui::Separator();
    ImGui::Text("Quick Colors:");
    ImGui::Spacing();

    struct PresetColor {
        const char* name;
        Color color;
    };

    PresetColor presets[] = {
        {"Black", Color::Black()},
        {"White", Color::White()},
        {"Red", Color::Red()},
        {"Green", Color::Green()},
        {"Blue", Color::Blue()},
    };
    
    constexpr size_t presetCount = 5;
    int columns = 3;
    for (size_t i = 0; i < presetCount; ++i) {
        const auto& preset = presets[i];
        
        // Color button showing the actual color
        ImVec4 presetColor = ImVec4(
            preset.color.r / 255.0f,
            preset.color.g / 255.0f,
            preset.color.b / 255.0f,
            preset.color.a / 255.0f
        );
        
        ImGui::PushStyleColor(ImGuiCol_Button, presetColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(
            std::min(presetColor.x * 1.2f, 1.0f),
            std::min(presetColor.y * 1.2f, 1.0f),
            std::min(presetColor.z * 1.2f, 1.0f),
            1.0f
        ));
        
        if (ImGui::Button(preset.name, ImVec2(70, 0))) {
            setColor(preset.color);
            if (m_callback) {
                m_callback(m_currentColor);
            }
        }
        
        ImGui::PopStyleColor(2);
        
        if ((i + 1) % columns != 0 && i + 1 < presetCount) {
            ImGui::SameLine();
        }
    }

    ImGui::End();
}

void ColorPicker::updateFloatColor() {
    m_colorFloat[0] = m_currentColor.r / 255.0f;
    m_colorFloat[1] = m_currentColor.g / 255.0f;
    m_colorFloat[2] = m_currentColor.b / 255.0f;
    m_colorFloat[3] = m_currentColor.a / 255.0f;
}

} // namespace Canvas

