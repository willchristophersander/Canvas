#ifndef CANVAS_FILTER_MENU_H
#define CANVAS_FILTER_MENU_H

#include <vector>
#include <string>

namespace Canvas {

class FilterManager;
class LayerManager;
class CommandHistory;

/**
 * UI menu for applying filters to layers.
 * Lists available Python filters and handles filter execution.
 */
class FilterMenu {
public:
    FilterMenu(FilterManager* filterManager, LayerManager* layerManager, CommandHistory* history);
    ~FilterMenu() = default;

    // Render the filter menu
    void render();
    
    // Render filter menu bar item
    void renderMenuBarItem();

    // Refresh available filters list
    void refreshFilterList();

    // Panel visibility
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

private:
    void applyFilter(const std::string& filterName);

private:
    FilterManager* m_filterManager;
    LayerManager* m_layerManager;
    CommandHistory* m_commandHistory;
    std::vector<std::string> m_filterNames;
    bool m_visible = true;
};

} // namespace Canvas

#endif // CANVAS_FILTER_MENU_H



