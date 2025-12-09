/**
 * FileManager.cpp
 * High-level file operations coordinating image handlers and layer system.
 * Modular design for easy extension to other formats.
 */

#include "io/FileManager.h"
#include "io/ImageHandler.h"
#include "core/Canvas.h"
#include "layers/Layer.h"
#include "layers/LayerManager.h"
#include "core/LayerCompositor.h"
#include <algorithm>
#include <cctype>
#include <fstream>

#ifdef _WIN32
    #include <windows.h>
    #include <commdlg.h>
#elif defined(__APPLE__)
    // macOS - file dialogs would need Objective-C++ bridge
    // For now, return empty string (UI can handle file selection)
#else
    // Linux - use native file dialog library or command-line fallback
    // For now, return empty string (UI can handle file selection)
#endif

#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "No filesystem support available"
#endif

namespace Canvas {

// Helper functions for file operations
namespace {
    // Extract file extension
    std::string getExtension(const std::string& filepath) {
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos == std::string::npos) {
            return "";
        }
        
        std::string ext = filepath.substr(dotPos + 1);
        // Convert to lowercase
        std::transform(ext.begin(), ext.end(), ext.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return ext;
    }

    // Check if file exists
    bool fileExists(const std::string& filepath) {
        std::ifstream file(filepath);
        return file.good();
    }
}

FileManager::FileManager() = default;

FileManager::~FileManager() = default;

bool FileManager::exportToPNG(const PixelBuffer& buffer, const std::string& filepath) {
    m_lastError.clear();
    
    if (buffer.getWidth() <= 0 || buffer.getHeight() <= 0) {
        m_lastError = "Cannot export empty buffer";
        return false;
    }

    bool success = PNGHandler::save(buffer, filepath);
    if (!success) {
        m_lastError = PNGHandler::getLastError();
    }
    return success;
}

bool FileManager::exportLayerToPNG(const Layer& layer, const std::string& filepath) {
    m_lastError.clear();
    return exportToPNG(layer.getPixelBuffer(), filepath);
}

bool FileManager::exportFlattenedToPNG(const LayerManager& layers, const std::string& filepath) {
    m_lastError.clear();
    
    auto flattenedLayers = layers.getLayersBottomToTop();
    if (flattenedLayers.empty()) {
        m_lastError = "No layers to export";
        return false;
    }

    auto flattened = LayerCompositor::flattenLayers(
        flattenedLayers,
        layers.getCanvasWidth(),
        layers.getCanvasHeight()
    );

    if (!flattened) {
        m_lastError = "Failed to flatten layers";
        return false;
    }

    return exportToPNG(*flattened, filepath);
}

std::unique_ptr<PixelBuffer> FileManager::importFromPNG(const std::string& filepath) {
    m_lastError.clear();
    
    auto buffer = PNGHandler::load(filepath);
    if (!buffer) {
        m_lastError = PNGHandler::getLastError();
    }
    return buffer;
}

std::unique_ptr<PixelBuffer> FileManager::importFromJPEG(const std::string& filepath) {
    m_lastError.clear();
    
    auto buffer = JPEGHandler::load(filepath);
    if (!buffer) {
        m_lastError = JPEGHandler::getLastError();
    }
    return buffer;
}

Layer* FileManager::importAsLayer(LayerManager& manager, const std::string& filepath) {
    m_lastError.clear();
    
    std::string ext = getExtension(filepath);
    std::unique_ptr<PixelBuffer> buffer;

    if (ext == "png") {
        buffer = importFromPNG(filepath);
    } else if (ext == "jpg" || ext == "jpeg") {
        buffer = importFromJPEG(filepath);
    } else {
        // Try to auto-detect
        buffer = importFromPNG(filepath);
        if (!buffer) {
            buffer = importFromJPEG(filepath);
        }
    }

    if (!buffer) {
        return nullptr;
    }

    // Create layer with imported data
    Layer* newLayer = manager.addLayer();
    if (!newLayer) {
        m_lastError = "Failed to create layer";
        return nullptr;
    }

    // Resize layer if dimensions don't match
    if (newLayer->getWidth() != buffer->getWidth() || 
        newLayer->getHeight() != buffer->getHeight()) {
        // For simplicity, we'll copy what fits
        // In a full implementation, we might resize the layer
    }

    newLayer->getPixelBuffer().copyFrom(*buffer);
    newLayer->updateTexture();

    return newLayer;
}

bool FileManager::detectImageFormat(const std::string& filepath) {
    std::string ext = getExtension(filepath);
    return ext == "png" || ext == "jpg" || ext == "jpeg";
}

std::string FileManager::showSaveDialog(const std::string& defaultName) {
#ifdef _WIN32
    // Windows file dialog
    char filename[MAX_PATH] = {0};
    strncpy_s(filename, defaultName.c_str(), MAX_PATH - 1);
    
    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "PNG Files\0*.png\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = "png";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetSaveFileNameA(&ofn)) {
        return std::string(filename);
    }
    return "";
#elif defined(__APPLE__)
    // macOS - file dialogs require Objective-C++ bridge
    // For now, return default name (UI should handle file dialogs via ImGui or native)
    return defaultName;
#else
    // Linux fallback - return default name
    // In a full implementation, use native file dialog library (e.g., GTK, Qt)
    return defaultName;
#endif
}

std::string FileManager::showOpenDialog() {
#ifdef _WIN32
    // Windows file dialog
    char filename[MAX_PATH] = {0};
    
    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg\0PNG Files\0*.png\0JPEG Files\0*.jpg;*.jpeg\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(filename);
    }
    return "";
#elif defined(__APPLE__)
    // macOS - file dialogs require Objective-C++ bridge
    // Return empty string (UI should handle file dialogs via ImGui or native)
    return "";
#else
    // Linux fallback - return empty (user must provide path)
    // In a full implementation, use native file dialog library
    return "";
#endif
}

} // namespace Canvas

