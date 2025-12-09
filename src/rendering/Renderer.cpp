/**
 * Renderer.cpp
 * OpenGL rendering implementation.
 * Handles layer compositing and rendering to screen.
 */

#include <glad/glad.h>
#include "rendering/Renderer.h"
#include "layers/Layer.h"
#include "layers/LayerManager.h"
#include "rendering/Texture.h"
#include "core/Color.h"
#include <vector>
#include <algorithm>

namespace Canvas {

// Simple vertex shader for texture rendering
static const char* VERTEX_SHADER_SOURCE = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform vec2 uOffset;
uniform float uScale;

void main() {
    vec2 scaledPos = aPos * uScale + uOffset;
    gl_Position = vec4(scaledPos.x, scaledPos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

// Simple fragment shader for texture rendering with opacity
static const char* FRAGMENT_SHADER_SOURCE = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D uTexture;
uniform float uOpacity;

void main() {
    vec4 texColor = texture(uTexture, TexCoord);
    FragColor = vec4(texColor.rgb, texColor.a * uOpacity);
}
)";

Renderer::Renderer() {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize() {
    if (!initializeShaders()) {
        return false;
    }
    if (!initializeQuad()) {
        return false;
    }
    return true;
}

void Renderer::shutdown() {
    if (m_quadVBO != 0) {
        glDeleteBuffers(1, &m_quadVBO);
        m_quadVBO = 0;
    }
    if (m_quadVAO != 0) {
        glDeleteVertexArrays(1, &m_quadVAO);
        m_quadVAO = 0;
    }
    if (m_shaderProgram != 0) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }
}

void Renderer::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void Renderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::renderLayer(const Layer& layer, float opacity) {
    if (!layer.isVisible()) {
        return;
    }

    layer.getTexture().bind(0);

    glUseProgram(m_shaderProgram);

    // Set uniforms
    int offsetLoc = glGetUniformLocation(m_shaderProgram, "uOffset");
    int scaleLoc = glGetUniformLocation(m_shaderProgram, "uScale");
    int opacityLoc = glGetUniformLocation(m_shaderProgram, "uOpacity");

    // Convert canvas coordinates to normalized device coordinates
    // Assuming viewport maps -1 to 1 for both axes
    float offsetX = m_canvasOffsetX;
    float offsetY = m_canvasOffsetY;
    
    glUniform2f(offsetLoc, offsetX, offsetY);
    glUniform1f(scaleLoc, m_canvasScale);
    glUniform1f(opacityLoc, opacity * layer.getOpacity());

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glUseProgram(0);
    layer.getTexture().unbind();
}

void Renderer::renderAllLayers(const LayerManager& layerManager) {
    auto layers = layerManager.getLayersBottomToTop();
    
    for (const Layer* layer : layers) {
        if (layer && layer->isVisible()) {
            renderLayer(*layer, 1.0f);
        }
    }
}

void Renderer::compositeToScreen(const LayerManager& layerManager) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    renderAllLayers(layerManager);
    
    glDisable(GL_BLEND);
}

void Renderer::renderTexture(const Texture& texture, float x, float y, float width, float height) {
    texture.bind(0);
    
    glUseProgram(m_shaderProgram);
    
    int offsetLoc = glGetUniformLocation(m_shaderProgram, "uOffset");
    int scaleLoc = glGetUniformLocation(m_shaderProgram, "uScale");
    int opacityLoc = glGetUniformLocation(m_shaderProgram, "uOpacity");
    
    // For UI elements, use direct screen coordinates
    glUniform2f(offsetLoc, x, y);
    glUniform1f(scaleLoc, 1.0f);
    glUniform1f(opacityLoc, 1.0f);
    
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    glUseProgram(0);
    texture.unbind();
}

void Renderer::setCanvasOffset(float x, float y) {
    m_canvasOffsetX = x;
    m_canvasOffsetY = y;
}

void Renderer::setCanvasScale(float scale) {
    m_canvasScale = scale > 0.0f ? scale : 1.0f;
}

float Renderer::getCanvasOffsetX() const {
    return m_canvasOffsetX;
}

float Renderer::getCanvasOffsetY() const {
    return m_canvasOffsetY;
}

float Renderer::getCanvasScale() const {
    return m_canvasScale;
}

bool Renderer::initializeShaders() {
    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        // Log error (would use proper logging in production)
        glDeleteShader(vertexShader);
        return false;
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Link shader program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
        return false;
    }

    // Clean up shaders (they're linked now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

bool Renderer::initializeQuad() {
    // Full screen quad for rendering textures
    float vertices[] = {
        // positions   // tex coords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return true;
}

} // namespace Canvas

