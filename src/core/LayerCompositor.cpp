/**
 * LayerCompositor.cpp
 * Implementation of layer compositing utilities.
 */

#include "core/LayerCompositor.h"
#include "layers/Layer.h"
#include "core/Canvas.h"
#include <algorithm>

namespace Canvas {
namespace LayerCompositor {

Color blendOver(const Color& source, const Color& destination) {
    if (source.a == 0) {
        return destination;
    }
    if (source.a == 255) {
        return source;
    }
    if (destination.a == 0) {
        return source;
    }

    // Standard alpha compositing: result = src * src_alpha + dst * (1 - src_alpha)
    float srcAlpha = source.a / 255.0f;
    float dstAlpha = destination.a / 255.0f;
    float resultAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);

    if (resultAlpha == 0.0f) {
        return Color::Transparent();
    }

    float invResultAlpha = 1.0f / resultAlpha;
    uint8_t r = static_cast<uint8_t>(
        (source.r * srcAlpha + destination.r * dstAlpha * (1.0f - srcAlpha)) * invResultAlpha
    );
    uint8_t g = static_cast<uint8_t>(
        (source.g * srcAlpha + destination.g * dstAlpha * (1.0f - srcAlpha)) * invResultAlpha
    );
    uint8_t b = static_cast<uint8_t>(
        (source.b * srcAlpha + destination.b * dstAlpha * (1.0f - srcAlpha)) * invResultAlpha
    );

    return Color(r, g, b, static_cast<uint8_t>(resultAlpha * 255.0f));
}

void compositeOver(const PixelBuffer& source, PixelBuffer& destination, float opacity) {
    if (source.getWidth() != destination.getWidth() || 
        source.getHeight() != destination.getHeight()) {
        return; // Dimensions must match
    }

    opacity = std::clamp(opacity, 0.0f, 1.0f);

    int width = source.getWidth();
    int height = source.getHeight();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color srcColor = source.getPixel(x, y);
            
            // Apply opacity multiplier
            if (opacity < 1.0f) {
                srcColor.a = static_cast<uint8_t>(srcColor.a * opacity);
            }

            Color dstColor = destination.getPixel(x, y);
            Color blended = blendOver(srcColor, dstColor);
            destination.setPixel(x, y, blended);
        }
    }
}

void compositeLayer(const Layer& layer, PixelBuffer& destination) {
    if (!layer.isVisible() || layer.getOpacity() == 0.0f) {
        return; // Skip invisible or fully transparent layers
    }

    compositeOver(layer.getPixelBuffer(), destination, layer.getOpacity());
}

std::unique_ptr<PixelBuffer> flattenLayers(
    const std::vector<const Layer*>& layers,
    int width,
    int height
) {
    auto result = std::make_unique<PixelBuffer>(width, height);
    result->clear(Color::Transparent());

    for (const Layer* layer : layers) {
        if (layer && layer->isVisible()) {
            compositeLayer(*layer, *result);
        }
    }

    return result;
}

void mergeLayers(const Layer& source, Layer& destination) {
    if (source.getWidth() != destination.getWidth() ||
        source.getHeight() != destination.getHeight()) {
        return;
    }

    // Composite source over destination with full opacity
    compositeOver(source.getPixelBuffer(), destination.getPixelBuffer(), 1.0f);
    destination.updateTexture();
}

void compositeWithBlendMode(
    const PixelBuffer& source,
    PixelBuffer& destination,
    BlendMode mode,
    float opacity
) {
    switch (mode) {
        case BlendMode::Normal:
            compositeOver(source, destination, opacity);
            break;
        // Future blend modes can be implemented here
        default:
            compositeOver(source, destination, opacity);
            break;
    }
}

} // namespace LayerCompositor
} // namespace Canvas

