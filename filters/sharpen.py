"""
Sharpen Filter
Applies unsharp masking to enhance image details.
"""

import numpy as np
from scipy import ndimage

def apply(image_data: np.ndarray, width: int, height: int, amount: float = 1.5, radius: float = 1.0) -> np.ndarray:
    """
    Apply sharpening using unsharp mask technique.
    
    Args:
        image_data: RGBA image as numpy array of shape (height, width, 4)
        width: Image width
        height: Image height
        amount: Sharpening strength (1.0 = normal, 2.0 = stronger)
        radius: Blur radius for unsharp mask
    
    Returns:
        Sharpened image as numpy array
    """
    if image_data.ndim == 1:
        image_data = image_data.reshape((height, width, 4))
    
    result = np.zeros_like(image_data, dtype=float)
    
    for i in range(3):  # R, G, B
        channel = image_data[:, :, i].astype(float)
        blurred = ndimage.gaussian_filter(channel, sigma=radius)
        # Unsharp mask: original + amount * (original - blurred)
        sharpened = channel + amount * (channel - blurred)
        result[:, :, i] = np.clip(sharpened, 0, 255)
    
    # Keep alpha unchanged
    result[:, :, 3] = image_data[:, :, 3]
    
    return result.astype(np.uint8)


def get_parameters() -> list:
    """
    Return filter parameter definitions for UI.
    """
    return [
        {"name": "amount", "type": "float", "min": 0.1, "max": 5.0, "default": 1.5},
        {"name": "radius", "type": "float", "min": 0.1, "max": 10.0, "default": 1.0}
    ]

