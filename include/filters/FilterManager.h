#ifndef CANVAS_FILTER_MANAGER_H
#define CANVAS_FILTER_MANAGER_H

#include <string>
#include <vector>

namespace Canvas {

class PixelBuffer;

/**
 * Manages Python filter integration.
 * Handles the C++ → Python → C++ round trip for image processing.
 */
class FilterManager {
public:
    FilterManager();
    ~FilterManager();

    // Initialize Python interpreter
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized; }

    // Filter discovery
    void setFilterDirectory(const std::string& path);
    const std::string& getFilterDirectory() const { return m_filterDirectory; }
    std::vector<std::string> getAvailableFilters() const;
    void refreshFilterList();

    // Filter execution
    // Applies the filter to the buffer in-place
    bool applyFilter(const std::string& filterName, PixelBuffer& buffer);
    
    // Apply filter with parameters (passed as key-value pairs)
    bool applyFilter(const std::string& filterName, PixelBuffer& buffer,
                    const std::vector<std::pair<std::string, float>>& params);

    // Error handling
    const std::string& getLastError() const { return m_lastError; }

private:
    bool loadFilter(const std::string& filterName);
    bool callFilterFunction(const std::string& filterName, PixelBuffer& buffer);

private:
    bool m_initialized = false;
    std::string m_filterDirectory = "filters";
    std::vector<std::string> m_availableFilters;
    std::string m_lastError;
};

} // namespace Canvas

#endif // CANVAS_FILTER_MANAGER_H



