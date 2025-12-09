/**
 * UIManager.cpp
 * Main UI manager coordinating all ImGui components.
 */

#include "ui/UIManager.h"
#include "core/Application.h"
#include "ui/ToolPanel.h"
#include "ui/ColorPicker.h"
#include "ui/BrushSizeSlider.h"
#include "ui/LayerPanel.h"
#include "ui/FilterMenu.h"
#include "tools/ToolManager.h"
#include "layers/LayerManager.h"
#include "commands/CommandHistory.h"
#include "io/FileManager.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

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

UIManager::UIManager(Application* app)
    : m_app(app) {
    // Create UI components
    m_toolPanel = std::make_unique<ToolPanel>(&app->getToolManager());
    m_colorPicker = std::make_unique<ColorPicker>();
    m_brushSizeSlider = std::make_unique<BrushSizeSlider>();
    m_layerPanel = std::make_unique<LayerPanel>(&app->getLayerManager(), &app->getCommandHistory());
    m_filterMenu = std::make_unique<FilterMenu>(&app->getFilterManager(), &app->getLayerManager(), &app->getCommandHistory());
    
    // Hide filters by default - can be shown from View menu
    m_filterMenu->setVisible(false);

    // Connect color picker to tool manager
    m_colorPicker->setColorChangedCallback([this](const Color& color) {
        if (m_app) {
            m_app->getToolManager().setColor(color);
        }
    });

    // Connect brush size slider to tool manager
    m_brushSizeSlider->setSizeChangedCallback([this](int size) {
        m_app->getToolManager().setBrushSize(size);
    });
}

UIManager::~UIManager() {
    shutdown();
}

bool UIManager::initialize(GLFWwindow* window) {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    // NUCLEAR OPTION: Disable ini file completely - no caching of positions!
    io.IniFilename = nullptr;

    // Setup ImGui style
    ImGui::StyleColorsDark();
    setupCustomStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void UIManager::setupCustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Rounding
    style.WindowRounding = 6.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    
    // Spacing
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    
    // Colors - improve contrast and readability
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.45f, 0.70f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.45f, 0.65f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.55f, 0.75f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.60f, 0.85f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.30f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.65f, 0.95f, 1.00f);
}

void UIManager::shutdown() {
    if (m_isShutdown) {
        return;  // Already shutdown, prevent double-shutdown
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    m_isShutdown = true;
}

void UIManager::render() {
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderMenuBar();
    
    // Render all UI components with HARDCODED POSITIONS
    const float MENU_BAR_HEIGHT = 20.0f;
    const float PADDING = 10.0f;
    const float LEFT_X = PADDING;
    const float LEFT_Y = MENU_BAR_HEIGHT + PADDING;
    const float RIGHT_X = 1280.0f - 300.0f - PADDING;
    
    // 1. Tool Panel
    if (m_toolPanel->isVisible()) {
        ImGui::SetNextWindowPos(ImVec2(LEFT_X, LEFT_Y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(180.0f, 300.0f), ImGuiCond_Always);
        m_toolPanel->render();
    }
    
    // 2. Color Picker
    if (m_colorPicker->isVisible()) {
        ImGui::SetNextWindowPos(ImVec2(RIGHT_X, LEFT_Y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300.0f, 280.0f), ImGuiCond_Always);
        m_colorPicker->render();
    }
    
    // 3. Brush Size
    if (m_brushSizeSlider->isVisible()) {
        ImGui::SetNextWindowPos(ImVec2(RIGHT_X, LEFT_Y + 290.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300.0f, 200.0f), ImGuiCond_Always);
        m_brushSizeSlider->render();
    }
    
    // 4. Layers
    if (m_layerPanel->isVisible()) {
        ImGui::SetNextWindowPos(ImVec2(RIGHT_X, LEFT_Y + 500.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300.0f, 180.0f), ImGuiCond_Always);
        m_layerPanel->render();
    }
    
    // 5. Filters
    if (m_filterMenu->isVisible()) {
        ImGui::SetNextWindowPos(ImVec2(400.0f, 100.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(280.0f, 200.0f), ImGuiCond_FirstUseEver);
        m_filterMenu->render();
    }

    // Render Modal Dialogs (if active)
    renderSaveDialog();
    renderOpenDialog();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool UIManager::wantsMouseInput() const {
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool UIManager::wantsKeyboardInput() const {
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

ToolPanel& UIManager::getToolPanel() {
    return *m_toolPanel;
}

ColorPicker& UIManager::getColorPicker() {
    return *m_colorPicker;
}

BrushSizeSlider& UIManager::getBrushSizeSlider() {
    return *m_brushSizeSlider;
}

LayerPanel& UIManager::getLayerPanel() {
    return *m_layerPanel;
}

FilterMenu& UIManager::getFilterMenu() {
    return *m_filterMenu;
}

void UIManager::renderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                m_app->getLayerManager().clear();
                m_app->getCommandHistory().clear();
                m_app->getLayerManager().addLayer(); // Ensure at least one layer
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                m_showOpenDialog = true;
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                // Pre-fill buffer if useful, but empty is safe
                m_showSaveDialog = true;
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                m_showSaveDialog = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                m_app->shutdown();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, m_app->getCommandHistory().canUndo())) {
                m_app->getCommandHistory().undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, m_app->getCommandHistory().canRedo())) {
                m_app->getCommandHistory().redo();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            bool toolVisible = m_toolPanel->isVisible();
            bool colorVisible = m_colorPicker->isVisible();
            bool brushVisible = m_brushSizeSlider->isVisible();
            bool layerVisible = m_layerPanel->isVisible();
            bool filterVisible = m_filterMenu->isVisible();

            if (ImGui::MenuItem("Tools", nullptr, &toolVisible)) {
                m_toolPanel->setVisible(toolVisible);
            }
            if (ImGui::MenuItem("Color Picker", nullptr, &colorVisible)) {
                m_colorPicker->setVisible(colorVisible);
            }
            if (ImGui::MenuItem("Brush Size", nullptr, &brushVisible)) {
                m_brushSizeSlider->setVisible(brushVisible);
            }
            if (ImGui::MenuItem("Layers", nullptr, &layerVisible)) {
                m_layerPanel->setVisible(layerVisible);
            }
            if (ImGui::MenuItem("Filters", nullptr, &filterVisible)) {
                m_filterMenu->setVisible(filterVisible);
            }
            ImGui::EndMenu();
        }

        m_filterMenu->renderMenuBarItem();

        ImGui::EndMainMenuBar();
    }
}

void UIManager::renderMainLayout() {
    // HARDCODED LAYOUT FOR 1280x720
    // Simple and clean: Left sidebar (tools), Right sidebar (everything else stacked)
    
    const float MENU_BAR_HEIGHT = 20.0f;
    const float PADDING = 10.0f;
    
    // LEFT SIDEBAR - Tools only
    const float LEFT_SIDEBAR_X = PADDING;
    const float LEFT_SIDEBAR_Y = MENU_BAR_HEIGHT + PADDING;
    const float LEFT_SIDEBAR_WIDTH = 180.0f;
    const float LEFT_SIDEBAR_HEIGHT = 300.0f;
    
    // RIGHT SIDEBAR - All other panels stacked
    const float RIGHT_SIDEBAR_X = 1280.0f - 300.0f - PADDING;
    const float RIGHT_SIDEBAR_WIDTH = 300.0f;
    
    static bool debugPrinted = false;
    if (!debugPrinted) {
        std::cout << "=== UI LAYOUT DEBUG ===" << std::endl;
        std::cout << "Tools: pos(" << LEFT_SIDEBAR_X << ", " << LEFT_SIDEBAR_Y << ") size(" << LEFT_SIDEBAR_WIDTH << ", " << LEFT_SIDEBAR_HEIGHT << ")" << std::endl;
        std::cout << "Right sidebar X: " << RIGHT_SIDEBAR_X << std::endl;
        debugPrinted = true;
    }
    
    // Tools Panel - Left sidebar
    if (m_toolPanel->isVisible()) {
        ImGui::SetNextWindowPos(
            ImVec2(LEFT_SIDEBAR_X, LEFT_SIDEBAR_Y), 
            ImGuiCond_Always
        );
        ImGui::SetNextWindowSize(
            ImVec2(LEFT_SIDEBAR_WIDTH, LEFT_SIDEBAR_HEIGHT), 
            ImGuiCond_Always
        );
    }
    
    // Right sidebar panels - stacked vertically
    float rightY = MENU_BAR_HEIGHT + PADDING;
    
    // Color Picker - Top right
    if (m_colorPicker->isVisible()) {
        ImGui::SetNextWindowPos(
            ImVec2(RIGHT_SIDEBAR_X, rightY), 
            ImGuiCond_Always
        );
        ImGui::SetNextWindowSize(
            ImVec2(RIGHT_SIDEBAR_WIDTH, 280.0f), 
            ImGuiCond_Always
        );
        rightY += 290.0f;  // height + spacing
    }
    
    // Brush Size - Below color picker
    if (m_brushSizeSlider->isVisible()) {
        ImGui::SetNextWindowPos(
            ImVec2(RIGHT_SIDEBAR_X, rightY), 
            ImGuiCond_Always
        );
        ImGui::SetNextWindowSize(
            ImVec2(RIGHT_SIDEBAR_WIDTH, 200.0f), 
            ImGuiCond_Always
        );
        rightY += 210.0f;  // height + spacing
    }
    
    // Layers - Below brush size, fills remaining space
    if (m_layerPanel->isVisible()) {
        float remainingHeight = 720.0f - rightY - PADDING - 10.0f;
        ImGui::SetNextWindowPos(
            ImVec2(RIGHT_SIDEBAR_X, rightY), 
            ImGuiCond_Always
        );
        ImGui::SetNextWindowSize(
            ImVec2(RIGHT_SIDEBAR_WIDTH, remainingHeight), 
            ImGuiCond_Always
        );
    }
    
    // Filters - Hidden by default, can be shown from menu
    if (m_filterMenu->isVisible()) {
        ImGui::SetNextWindowPos(
            ImVec2(400.0f, 100.0f),  // Center-ish
            ImGuiCond_FirstUseEver   // Allow dragging
        );
        ImGui::SetNextWindowSize(
            ImVec2(280.0f, 200.0f), 
            ImGuiCond_FirstUseEver
        );
    }
    }

    // Render Modal Dialogs (if active)



void UIManager::refreshFileList() {
    m_fileList.clear();
    try {
        for (const auto& entry : fs::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                    m_fileList.push_back(entry.path().filename().string());
                }
            }
        }
    } catch (...) {
        // Ignore errors
    }
    std::sort(m_fileList.begin(), m_fileList.end());
}

void UIManager::renderSaveDialog() {
    if (m_showSaveDialog) {
        ImGui::OpenPopup("Save File");
    }

    if (ImGui::BeginPopupModal("Save File", &m_showSaveDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter filename:");
        
        bool enterPressed = ImGui::InputText("##filename", m_filenameBuffer, sizeof(m_filenameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
        
        std::string filename = m_filenameBuffer;
        if (!filename.empty() && filename.find('.') == std::string::npos) {
            ImGui::TextDisabled("(will append .png)");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Save", ImVec2(120, 0)) || enterPressed) {
            if (!filename.empty()) {
                // Formatting
                if (filename.find('.') == std::string::npos) {
                    filename += ".png";
                }
                
                // Save logic
                m_app->getFileManager().exportFlattenedToPNG(m_app->getLayerManager(), filename);
                
                m_showSaveDialog = false;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showSaveDialog = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void UIManager::renderOpenDialog() {
    if (m_showOpenDialog) {
        ImGui::OpenPopup("Open File");
        refreshFileList();
        m_showOpenDialog = false; // OpenPopup sets internal state
    }

    if (ImGui::BeginPopupModal("Open File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Select file to import as layer:");
        ImGui::Spacing();

        if (ImGui::BeginListBox("##files", ImVec2(300, 200))) {
            for (int i = 0; i < m_fileList.size(); i++) {
                const auto& filename = m_fileList[i];
                bool isSelected = (m_selectedFileIndex == i);
                if (ImGui::Selectable(filename.c_str(), isSelected)) {
                    m_selectedFileIndex = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canOpen = (m_selectedFileIndex >= 0 && m_selectedFileIndex < m_fileList.size());
        if (!canOpen) ImGui::BeginDisabled();
        if (ImGui::Button("Open", ImVec2(120, 0))) {
            std::string filename = m_fileList[m_selectedFileIndex];
            m_app->getFileManager().importAsLayer(m_app->getLayerManager(), filename);
            ImGui::CloseCurrentPopup();
        }
        if (!canOpen) ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

} // namespace Canvas

