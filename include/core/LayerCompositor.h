/**
 * LayerCompositor.h
 * Utilities for compositing and blending layers.
 * Abstracted to be reusable across different compositing needs.
 */

#ifndef CANVAS_LAYER_COMPOSITOR_H
#define CANVAS_LAYER_COMPOSITOR_H

#include "core/Color.h"
#include <functional>

namespace Canvas {

class PixelBuffer;
class Layer;

/**
 * Utilities for layer compositing and blending operations.
 */
namespace LayerCompositor {

    /**
     * Blend two colors using standard alpha compositing (over operator).
     * Result = source * source_alpha + destination * (1 - source_alpha)
     */
    Color blendOver(const Color& source, const Color& destination);

    /**
     * Composite a source pixel buffer over a destination buffer.
     * Applies standard alpha blending with optional opacity multiplier.
     * 
     * @param source The layer to composite on top
     * @param destination The buffer to composite onto (modified in-place)
     * @param opacity Optional opacity multiplier (0.0 to 1.0)
     */
    void compositeOver(const PixelBuffer& source, PixelBuffer& destination, float opacity = 1.0f);

    /**
     * Composite a layer over a destination buffer.
     * Respects layer's visibility and opacity.
     */
    void compositeLayer(const Layer& layer, PixelBuffer& destination);

    /**
     * Composite multiple layers (from bottom to top) into a single buffer.
     * Creates a new buffer with the composited result.
     * 
     * @param layers Vector of layers (bottom to top order)
     * @param width Output buffer width
     * @param height Output buffer height
     * @return Completely flattened image
     */
    std::unique_ptr<PixelBuffer> flattenLayers(
        const std::vector<const Layer*>& layers, 
        int width, 
        int height
    );

    /**
     * Merge two layers, combining their pixels.
     * The result goes into the destination layer.
     */
    void mergeLayers(const Layer& source, Layer& destination);

    /**
     * Blend mode enumeration for future extensibility.
     */
    enum class BlendMode {
        Normal,      // Standard alpha over
        Multiply,
        Screen,
        Overlay,
        // Can add more blend modes as needed
    };

    /**
     * Generic compositing with blend mode selection.
     */
    void compositeWithBlendMode(
        const PixelBuffer& source,
        PixelBuffer& destination,
        BlendMode mode,
        float opacity = 1.0f
    );

} // namespace LayerCompositor

} // namespace Canvas

#endif // CANVAS_LAYER_COMPOSITOR_H

