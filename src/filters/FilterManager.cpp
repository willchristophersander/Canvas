/**
 * FilterManager.cpp
 * Python filter integration using Python C API.
 * Handles C++ → Python → C++ round trip for image processing.
 */

#include "filters/FilterManager.h"
#include "core/Canvas.h"
#include <Python.h>
#include <numpy/arrayobject.h>
#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "No filesystem support available"
#endif
#include <fstream>
#include <algorithm>

namespace Canvas {

FilterManager::FilterManager() {
    // Filter directory will be set relative to executable
}

FilterManager::~FilterManager() {
    shutdown();
}

bool FilterManager::initialize() {
    if (m_initialized) {
        return true;
    }

    // Initialize Python interpreter if not already done
    if (!Py_IsInitialized()) {
        Py_Initialize();
        if (!Py_IsInitialized()) {
            m_lastError = "Failed to initialize Python interpreter";
            return false;
        }
    }

    // Import NumPy (required for array handling)
    import_array();
    if (PyErr_Occurred()) {
        PyErr_Print();
        m_lastError = "Failed to import NumPy";
        return false;
    }

    // Add filter directory to Python path
    PyObject* sysPath = PySys_GetObject("path");
    if (sysPath) {
        PyObject* filterDir = PyUnicode_FromString(m_filterDirectory.c_str());
        PyList_Append(sysPath, filterDir);
        Py_DECREF(filterDir);
    }

    m_initialized = true;
    refreshFilterList();
    return true;
}

void FilterManager::shutdown() {
    if (m_initialized) {
        // Don't finalize Python here - might be used elsewhere
        // Py_Finalize();  // Commented out - let application handle this
        m_initialized = false;
    }
}

void FilterManager::setFilterDirectory(const std::string& path) {
    m_filterDirectory = path;
    if (m_initialized) {
        refreshFilterList();
    }
}

std::vector<std::string> FilterManager::getAvailableFilters() const {
    return m_availableFilters;
}

void FilterManager::refreshFilterList() {
    m_availableFilters.clear();
    m_lastError.clear();

    if (m_filterDirectory.empty()) {
        return;
    }

    try {
        // Scan directory for .py files
        fs::path filterPath(m_filterDirectory);
        if (!fs::exists(filterPath) || !fs::is_directory(filterPath)) {
            m_lastError = "Filter directory does not exist: " + m_filterDirectory;
            return;
        }

        for (const auto& entry : fs::directory_iterator(filterPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.size() > 3 && filename.substr(filename.size() - 3) == ".py") {
                    // Extract base name (without .py extension)
                    std::string filterName = filename.substr(0, filename.size() - 3);
                    m_availableFilters.push_back(filterName);
                }
            }
        }

        std::sort(m_availableFilters.begin(), m_availableFilters.end());
    } catch (const std::exception& e) {
        m_lastError = "Error scanning filter directory: " + std::string(e.what());
    }
}

bool FilterManager::applyFilter(const std::string& filterName, PixelBuffer& buffer) {
    std::vector<std::pair<std::string, float>> emptyParams;
    return applyFilter(filterName, buffer, emptyParams);
}

bool FilterManager::applyFilter(const std::string& filterName, PixelBuffer& buffer,
                                const std::vector<std::pair<std::string, float>>& params) {
    m_lastError.clear();

    if (!m_initialized) {
        m_lastError = "FilterManager not initialized";
        return false;
    }

    if (!loadFilter(filterName)) {
        return false;
    }

    // Convert PixelBuffer to NumPy array
    int width = buffer.getWidth();
    int height = buffer.getHeight();
    
    // Create a copy of the buffer data in the format Python expects (height, width, 4)
    // Python filters expect row-major format with Y=0 at top
    // Keep data alive in a unique_ptr that will be managed by NumPy
    auto pythonData = std::make_unique<uint8_t[]>(height * width * 4);
    for (int y = 0; y < height; ++y) {
        int srcY = height - 1 - y;  // Flip Y coordinate
        for (int x = 0; x < width; ++x) {
            Color pixel = buffer.getPixel(x, srcY);
            size_t idx = (y * width + x) * 4;
            pythonData[idx] = pixel.r;
            pythonData[idx + 1] = pixel.g;
            pythonData[idx + 2] = pixel.b;
            pythonData[idx + 3] = pixel.a;
        }
    }

    // Create NumPy array from copied data (height, width, 4)
    // NumPy will own the data
    npy_intp dims[3] = {height, width, 4};
    PyObject* imageArray = PyArray_SimpleNewFromData(3, dims, NPY_UINT8, pythonData.get());
    if (!imageArray) {
        m_lastError = "Failed to create NumPy array";
        PyErr_Print();
        return false;
    }

    // Transfer ownership to NumPy array
    PyArray_ENABLEFLAGS((PyArrayObject*)imageArray, NPY_ARRAY_OWNDATA);
    PyArray_ENABLEFLAGS((PyArrayObject*)imageArray, NPY_ARRAY_WRITEABLE);
    
    // Release our unique_ptr - NumPy now owns the data
    pythonData.release();

    // Import the filter module
    PyObject* moduleName = PyUnicode_FromString(filterName.c_str());
    PyObject* filterModule = PyImport_Import(moduleName);
    Py_DECREF(moduleName);

    if (!filterModule) {
        Py_DECREF(imageArray);
        m_lastError = "Failed to import filter module: " + filterName;
        PyErr_Print();
        return false;
    }

    // Get the apply function
    PyObject* applyFunc = PyObject_GetAttrString(filterModule, "apply");
    if (!applyFunc || !PyCallable_Check(applyFunc)) {
        Py_DECREF(imageArray);
        Py_DECREF(filterModule);
        m_lastError = "Filter module does not have a callable 'apply' function";
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        return false;
    }

    // Build keyword arguments for filter parameters
    PyObject* kwargs = nullptr;
    if (!params.empty()) {
        kwargs = PyDict_New();
        for (const auto& param : params) {
            PyObject* key = PyUnicode_FromString(param.first.c_str());
            PyObject* value = PyFloat_FromDouble(param.second);
            PyDict_SetItem(kwargs, key, value);
            Py_DECREF(key);
            Py_DECREF(value);
        }
    }

    // Build positional arguments: (image_data, width, height)
    PyObject* args = PyTuple_New(3);
    PyTuple_SetItem(args, 0, imageArray);  // Steals reference
    PyTuple_SetItem(args, 1, PyLong_FromLong(width));
    PyTuple_SetItem(args, 2, PyLong_FromLong(height));

    // Call the filter function with positional and keyword arguments
    PyObject* result = PyObject_Call(applyFunc, args, kwargs);
    Py_DECREF(args);
    if (kwargs) {
        Py_DECREF(kwargs);
    }
    Py_DECREF(applyFunc);
    Py_DECREF(filterModule);

    if (!result) {
        m_lastError = "Filter function failed";
        PyErr_Print();
        return false;
    }

    // Check if result is a NumPy array
    if (!PyArray_Check(result)) {
        Py_DECREF(result);
        m_lastError = "Filter did not return a NumPy array";
        return false;
    }

    // Extract result data
    PyArrayObject* resultArray = (PyArrayObject*)result;
    
    // Verify dimensions
    if (PyArray_NDIM(resultArray) != 3 || 
        PyArray_DIM(resultArray, 0) != height ||
        PyArray_DIM(resultArray, 1) != width ||
        PyArray_DIM(resultArray, 2) != 4) {
        Py_DECREF(result);
        m_lastError = "Filter returned array with incorrect dimensions";
        return false;
    }

    // Copy result back to buffer
    uint8_t* resultData = (uint8_t*)PyArray_DATA(resultArray);
    
    // Convert from (height, width, 4) Python format back to our buffer
    // Result is in same format as input (Y=0 at top), flip back
    for (int y = 0; y < height; ++y) {
        int dstY = height - 1 - y;  // Flip Y coordinate back
        for (int x = 0; x < width; ++x) {
            size_t srcIdx = (y * width + x) * 4;
            buffer.setPixel(x, dstY, Color(
                resultData[srcIdx],
                resultData[srcIdx + 1],
                resultData[srcIdx + 2],
                resultData[srcIdx + 3]
            ));
        }
    }

    Py_DECREF(result);
    return true;
}

bool FilterManager::loadFilter(const std::string& filterName) {
    // Filter is loaded on-demand when applyFilter is called
    // This is a placeholder for any pre-loading logic
    return true;
}

bool FilterManager::callFilterFunction(const std::string& filterName, PixelBuffer& buffer) {
    // This is handled directly in applyFilter now
    return applyFilter(filterName, buffer);
}

} // namespace Canvas

