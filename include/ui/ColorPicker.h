#ifndef CANVAS_COLOR_PICKER_H
#define CANVAS_COLOR_PICKER_H

#include "core/Color.h"
#include <functional>

namespace Canvas {

/**
 * Color picker UI component.
 * Allows users to select colors using various methods (wheel, sliders, etc.)
 */
class ColorPicker {
public:
    using ColorChangedCallback = std::function<void(const Color&)>;

    ColorPicker();
    ~ColorPicker() = default;

    // Render the color picker
    void render();

    // Current color
    const Color& getColor() const { return m_currentColor; }
    void setColor(const Color& color);

    // Callback when color changes
    void setColorChangedCallback(ColorChangedCallback callback);

    // Panel visibility
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

private:
    Color m_currentColor = Color::Black();
    float m_colorFloat[4] = {0.0f, 0.0f, 0.0f, 1.0f};  // For ImGui
    ColorChangedCallback m_callback;
    bool m_visible = true;
};

} // namespace Canvas

#endif // CANVAS_COLOR_PICKER_H

