#ifndef CANVAS_BRUSH_SIZE_SLIDER_H
#define CANVAS_BRUSH_SIZE_SLIDER_H

#include <functional>

namespace Canvas {

/**
 * UI component for adjusting brush size.
 */
class BrushSizeSlider {
public:
    using SizeChangedCallback = std::function<void(int)>;

    BrushSizeSlider();
    ~BrushSizeSlider() = default;

    // Render the slider
    void render();

    // Current size
    int getSize() const { return m_currentSize; }
    void setSize(int size);

    // Size limits
    void setMinSize(int min) { m_minSize = min; }
    void setMaxSize(int max) { m_maxSize = max; }
    int getMinSize() const { return m_minSize; }
    int getMaxSize() const { return m_maxSize; }

    // Callback when size changes
    void setSizeChangedCallback(SizeChangedCallback callback);

    // Visibility
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

private:
    int m_currentSize = 5;
    int m_minSize = 1;
    int m_maxSize = 100;
    SizeChangedCallback m_callback;
    bool m_visible = true;
};

} // namespace Canvas

#endif // CANVAS_BRUSH_SIZE_SLIDER_H



