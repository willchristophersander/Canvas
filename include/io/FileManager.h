#ifndef CANVAS_FILE_MANAGER_H
#define CANVAS_FILE_MANAGER_H

#include <string>
#include <memory>

namespace Canvas {

class PixelBuffer;
class Layer;
class LayerManager;

/**
 * Handles file import/export operations.
 * Coordinates between image handlers and the layer system.
 */
class FileManager {
public:
    FileManager();
    ~FileManager();

    // Export operations
    bool exportToPNG(const PixelBuffer& buffer, const std::string& filepath);
    bool exportLayerToPNG(const Layer& layer, const std::string& filepath);
    bool exportFlattenedToPNG(const LayerManager& layers, const std::string& filepath);

    // Import operations
    std::unique_ptr<PixelBuffer> importFromPNG(const std::string& filepath);
    std::unique_ptr<PixelBuffer> importFromJPEG(const std::string& filepath);
    
    // Import as new layer
    Layer* importAsLayer(LayerManager& manager, const std::string& filepath);

    // File dialogs (returns empty string if cancelled)
    std::string showSaveDialog(const std::string& defaultName = "untitled.png");
    std::string showOpenDialog();

    // Error handling
    const std::string& getLastError() const { return m_lastError; }

private:
    bool detectImageFormat(const std::string& filepath);

private:
    std::string m_lastError;
};

} // namespace Canvas

#endif // CANVAS_FILE_MANAGER_H



