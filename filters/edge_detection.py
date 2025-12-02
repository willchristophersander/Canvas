"""
Edge Detection Filter
Detects edges using the Sobel operator.
"""

import numpy as np
from scipy import ndimage

def apply(image_data: np.ndarray, width: int, height: int, threshold: float = 50.0) -> np.ndarray:
    """
    Apply edge detection using Sobel operator.
    
    Args:
        image_data: RGBA image as numpy array of shape (height, width, 4)
        width: Image width
        height: Image height
        threshold: Edge threshold (0-255)
    
    Returns:
        Edge-detected image as numpy array
    """
    if image_data.ndim == 1:
        image_data = image_data.reshape((height, width, 4))
    
    # Convert to grayscale for edge detection
    gray = 0.299 * image_data[:, :, 0] + 0.587 * image_data[:, :, 1] + 0.114 * image_data[:, :, 2]
    
    # Apply Sobel operator
    sobel_x = ndimage.sobel(gray, axis=1)
    sobel_y = ndimage.sobel(gray, axis=0)
    
    # Compute magnitude
    magnitude = np.sqrt(sobel_x**2 + sobel_y**2)
    
    # Apply threshold
    edges = np.where(magnitude > threshold, 255, 0).astype(np.uint8)
    
    # Create output image (white edges on black background)
    result = np.zeros_like(image_data)
    result[:, :, 0] = edges  # R
    result[:, :, 1] = edges  # G
    result[:, :, 2] = edges  # B
    result[:, :, 3] = image_data[:, :, 3]  # Preserve alpha
    
    return result.astype(np.uint8)


def get_parameters() -> list:
    """
    Return filter parameter definitions for UI.
    """
    return [
        {"name": "threshold", "type": "float", "min": 0.0, "max": 255.0, "default": 50.0}
    ]

