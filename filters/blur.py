"""
Blur Filter
Applies a Gaussian blur to the image.
"""
import numpy as np
from scipy import ndimage

def apply(image_data: np.ndarray, width: int, height: int, radius: float = 2.0) -> np.ndarray:
    """
    Apply Gaussian blur to the image.
    
    Args:
        image_data: RGBA image as numpy array of shape (height, width, 4)
        width: Image width
        height: Image height
        radius: Blur radius (sigma for Gaussian)
    
    Returns:
        Blurred image as numpy array
    """
    # Reshape flat array to image if needed
    if image_data.ndim == 1:
        image_data = image_data.reshape((height, width, 4))
    
    result = np.zeros_like(image_data)
    
    # Apply blur to RGB channels (preserve alpha)
    for i in range(3):  # R, G, B
        result[:, :, i] = ndimage.gaussian_filter(image_data[:, :, i].astype(float), sigma=radius)
    
    # Keep alpha channel unchanged
    result[:, :, 3] = image_data[:, :, 3]
    
    return result.astype(np.uint8)


def get_parameters() -> list:
    """
    Return filter parameter definitions for UI.
    """
    return [
        {"name": "radius", "type": "float", "min": 0.1, "max": 20.0, "default": 2.0}
    ]

