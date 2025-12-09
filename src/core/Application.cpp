/**
 * Application.cpp
 * Main application loop and event handling.
 * Coordinates all subsystems and manages the main loop.
 */

#include "core/Application.h"
#include "rendering/Renderer.h"
#include "layers/LayerManager.h"
#include "layers/Layer.h"
#include "tools/ToolManager.h"
#include "commands/CommandHistory.h"
#include "ui/UIManager.h"
#include "filters/FilterManager.h"
#include "io/FileManager.h"
#include "commands/DrawCommand.h"
#include "tools/Tool.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <mach/mach.h>

namespace Canvas {

// Static callbacks for GLFW
static Application* s_applicationInstance = nullptr;

static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    
    if (s_applicationInstance) {
        s_applicationInstance->onMouseButton(button, action, mods);
    }
}

static void glfwMouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    if (s_applicationInstance) {
        s_applicationInstance->onMouseMove(xpos, ypos);
    }
}

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (s_applicationInstance) {
        s_applicationInstance->onKey(key, scancode, action, mods);
    }
}

static void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    if (s_applicationInstance) {
        s_applicationInstance->onScroll(xoffset, yoffset);
    }
}

static void glfwResizeCallback(GLFWwindow* window, int width, int height) {
    if (s_applicationInstance) {
        s_applicationInstance->onResize(width, height);
    }
}

static void glfwCharCallback(GLFWwindow* window, unsigned int c) {
    ImGui_ImplGlfw_CharCallback(window, c);
}

Application::Application() {
    s_applicationInstance = this;
}

Application::~Application() {
    shutdown();
    s_applicationInstance = nullptr;
}

bool Application::initialize(int width, int height, const std::string& title) {
    m_windowWidth = width;
    m_windowHeight = height;

    // Initialize subsystems in order
    if (!initializeGLFW()) {
        return false;
    }

    if (!initializeOpenGL()) {
        return false;
    }

    // Create subsystems
    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    m_layerManager = std::make_unique<LayerManager>(m_canvasWidth, m_canvasHeight);
    m_toolManager = std::make_unique<ToolManager>();
    m_commandHistory = std::make_unique<CommandHistory>(30);
    m_fileManager = std::make_unique<FileManager>();

    m_filterManager = std::make_unique<FilterManager>();
    m_filterManager->setFilterDirectory("filters");
    if (!m_filterManager->initialize()) {
        std::cerr << "Warning: FilterManager initialization failed" << std::endl;
    }

    m_uiManager = std::make_unique<UIManager>(this);
    if (!m_uiManager->initialize(m_window)) {
        std::cerr << "Failed to initialize UI manager" << std::endl;
        return false;
    }

    setupCallbacks();

    m_running = true;
    return true;
}

void Application::run() {
    while (m_running && !glfwWindowShouldClose(m_window)) {
        processInput();
        update();
        render();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Application::shutdown() {
    if (m_uiManager) {
        m_uiManager->shutdown();
    }

    if (m_filterManager) {
        m_filterManager->shutdown();
    }

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
}

Renderer& Application::getRenderer() {
    return *m_renderer;
}

LayerManager& Application::getLayerManager() {
    return *m_layerManager;
}

ToolManager& Application::getToolManager() {
    return *m_toolManager;
}

CommandHistory& Application::getCommandHistory() {
    return *m_commandHistory;
}

UIManager& Application::getUIManager() {
    return *m_uiManager;
}

FilterManager& Application::getFilterManager() {
    return *m_filterManager;
}

FileManager& Application::getFileManager() {
    return *m_fileManager;
}

int Application::getWindowWidth() const {
    return m_windowWidth;
}

int Application::getWindowHeight() const {
    return m_windowHeight;
}

GLFWwindow* Application::getWindow() const {
    return m_window;
}

int Application::getCanvasWidth() const {
    return m_canvasWidth;
}

int Application::getCanvasHeight() const {
    return m_canvasHeight;
}

void Application::onMouseButton(int button, int action, int mods) {
    // Ignore if UI is capturing input
    if (m_uiManager->wantsMouseInput()) {
        return;
    }

    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        Layer* activeLayer = m_layerManager->getActiveLayer();
        Tool* currentTool = m_toolManager->getCurrentTool();

        if (!activeLayer || !currentTool) {
            return;
        }

        // Convert screen coordinates to canvas coordinates (account for viewport scaling)
        double mx, my;
        glfwGetCursorPos(m_window, &mx, &my);

        // Map screen space to canvas space
        // Canvas is rendered as full screen quad, so it's simple scaling
        float scaleX = (float)m_canvasWidth / m_windowWidth;
        float scaleY = (float)m_canvasHeight / m_windowHeight;

        int canvasX = static_cast<int>(mx * scaleX);
        // Invert Y axis because OpenGL texture origin (0,0) is bottom-left
        // but window origin (0,0) is top-left
        int canvasY = m_canvasHeight - 1 - static_cast<int>(my * scaleY);
        
        // Clamp to valid range
        if (canvasX < 0) canvasX = 0;
        if (canvasX >= m_canvasWidth) canvasX = m_canvasWidth - 1;
        if (canvasY < 0) canvasY = 0;
        if (canvasY >= m_canvasHeight) canvasY = m_canvasHeight - 1;

        // Create tool input
        ToolInput input;
        input.x = canvasX;
        input.y = canvasY;
        input.prevX = canvasX;
        input.prevY = canvasY;
        input.leftButton = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
        input.rightButton = (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
        input.shiftHeld = (mods & GLFW_MOD_SHIFT) != 0;
        input.ctrlHeld = (mods & GLFW_MOD_CONTROL) != 0;
        input.altHeld = (mods & GLFW_MOD_ALT) != 0;
        
        static int s_prevCanvasX = canvasX;
        static int s_prevCanvasY = canvasY;
        
        if (action == GLFW_PRESS) {
             s_prevCanvasX = canvasX;
             s_prevCanvasY = canvasY;
        }
        input.prevX = s_prevCanvasX;
        input.prevY = s_prevCanvasY;

        // Handle drawing with command system
        static DrawCommand* currentDrawCommand = nullptr;

        if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
            // Start drawing command
            currentDrawCommand = new DrawCommand(activeLayer, currentTool->getName());
            bool modified = currentTool->onMouseDown(*activeLayer, input);
            // Texture updated lazily (lines 258-261)
        } else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT && currentDrawCommand) {
            // Finish drawing command
            ToolInput releaseInput = input;
            releaseInput.leftButton = false;
            bool modified = currentTool->onMouseUp(*activeLayer, releaseInput);
            
            if (modified) {
                currentDrawCommand->captureAfter();
                // Texture updated lazily
                m_commandHistory->addCommand(std::unique_ptr<DrawCommand>(currentDrawCommand));
            } else {
                delete currentDrawCommand;
            }
            currentDrawCommand = nullptr;
        }
    }
}

// Static variables to track previous position for mouse move events
static int s_lastCanvasX = 0;
static int s_lastCanvasY = 0;

void Application::onMouseMove(double xpos, double ypos) {
    // Ignore if UI is capturing input
    if (m_uiManager->wantsMouseInput()) {
        return;
    }

    Layer* activeLayer = m_layerManager->getActiveLayer();
    Tool* currentTool = m_toolManager->getCurrentTool();

    if (!activeLayer || !currentTool) {
        // If no active layer or tool, reset static previous positions
        // so that the next valid move starts fresh.
        s_lastCanvasX = static_cast<int>(xpos); // Using screen coords as a fallback
        s_lastCanvasY = static_cast<int>(ypos); // Using screen coords as a fallback
        return;
    }
    
    // Map screen space to canvas space
    float scaleX = (float)m_canvasWidth / m_windowWidth;
    float scaleY = (float)m_canvasHeight / m_windowHeight;

    int canvasX = static_cast<int>(xpos * scaleX);
    int canvasY = m_canvasHeight - 1 - static_cast<int>(ypos * scaleY);
    
    // Clamp
    if (canvasX < 0) canvasX = 0;
    if (canvasX >= m_canvasWidth) canvasX = m_canvasWidth - 1;
    if (canvasY < 0) canvasY = 0;
    if (canvasY >= m_canvasHeight) canvasY = m_canvasHeight - 1;

    ToolInput input;
    input.x = canvasX;
    input.y = canvasY;
    input.prevX = s_lastCanvasX;
    input.prevY = s_lastCanvasY;
    
    // Update static prevs for the next mouse move event
    s_lastCanvasX = canvasX;
    s_lastCanvasY = canvasY;

    // Get current modifiers (this is approximate as GLFW callbacks don't provide mods for mouse move)
    // We can cache them from key/mouse callbacks if needed, or query keys
    int mods = 0;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) mods |= GLFW_MOD_SHIFT;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) mods |= GLFW_MOD_CONTROL;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) mods |= GLFW_MOD_ALT;

    input.leftButton = (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    input.rightButton = (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    input.shiftHeld = (mods & GLFW_MOD_SHIFT) != 0;
    input.ctrlHeld = (mods & GLFW_MOD_CONTROL) != 0;
    input.altHeld = (mods & GLFW_MOD_ALT) != 0;

    // Update tool
    if (input.leftButton || input.rightButton) {
        bool modified = currentTool->onMouseMove(*activeLayer, input);
        // Texture update will happen lazily during render if modified
    }
}

void Application::onKey(int key, int scancode, int action, int mods) {
    // Ignore if UI is capturing input
    if (m_uiManager->wantsKeyboardInput()) {
        return;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // Handle keyboard shortcuts
        bool ctrl = (mods & GLFW_MOD_CONTROL) != 0;

        if (ctrl && key == GLFW_KEY_Z) {
            // Undo
            if (mods & GLFW_MOD_SHIFT) {
                m_commandHistory->redo();
            } else {
                m_commandHistory->undo();
            }
        } else if (ctrl && key == GLFW_KEY_Y) {
            // Redo
            m_commandHistory->redo();
        } else if (key == GLFW_KEY_ESCAPE) {
            m_running = false;
        }
    }
}

void Application::onScroll(double xoffset, double yoffset) {
    // Ignore if UI is capturing input
    if (m_uiManager->wantsMouseInput()) {
        return;
    }

    // Zoom canvas (Ctrl+Scroll)
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(m_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        float currentScale = m_renderer->getCanvasScale();
        float newScale = currentScale + static_cast<float>(yoffset) * 0.1f;
        newScale = std::max(0.1f, std::min(5.0f, newScale));
        m_renderer->setCanvasScale(newScale);
    }
}

void Application::onResize(int width, int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    glViewport(0, 0, width, height);
    m_renderer->setViewport(0, 0, width, height);
}

void Application::processInput() {
    // Additional input processing can go here
}

void Application::update() {
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount % 60 == 0) {
        struct task_basic_info t_info;
        mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
        
        if (KERN_SUCCESS == task_info(mach_task_self(),
                                      TASK_BASIC_INFO, (task_info_t)&t_info, 
                                      &t_info_count))
        {
            std::cout << "[DEBUG] Frame " << frameCount << " | RAM: " << t_info.resident_size / 1024 / 1024 << " MB" << std::endl;
        }
    }
}

void Application::render() {
    // Clear screen
    m_renderer->clear(0.2f, 0.2f, 0.2f, 1.0f);

    // Render tool preview BEFORE compositing layers to screen
    // This ensures preview is drawn on the layer texture first
    Tool* currentTool = m_toolManager->getCurrentTool();
    Layer* activeLayer = m_layerManager->getActiveLayer();
    if (currentTool && activeLayer && currentTool->hasPreview() && currentTool->isPreviewActive()) {
        currentTool->renderPreview(*activeLayer);
    }

    // Render canvas (all layers composited)
    m_renderer->compositeToScreen(*m_layerManager);

    // Render UI
    m_uiManager->render();
}

bool Application::initializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Canvas - Drawing Application", nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);  // V-Sync

    return true;
}

bool Application::initializeOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Application::setupCallbacks() {
    glfwSetMouseButtonCallback(m_window, glfwMouseButtonCallback);
    glfwSetCursorPosCallback(m_window, glfwMouseMoveCallback);
    glfwSetKeyCallback(m_window, glfwKeyCallback);
    glfwSetScrollCallback(m_window, glfwScrollCallback);
    glfwSetWindowSizeCallback(m_window, glfwResizeCallback);
    glfwSetCharCallback(m_window, glfwCharCallback);
}

} // namespace Canvas

