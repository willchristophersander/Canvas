#!/bin/bash
# Test the simplified UI layout

cd "$(dirname "$0")/build"

# CRITICAL: Delete imgui.ini to reset positions
rm -f imgui.ini

echo "========================================="
echo "Canvas - Simplified UI Test"
echo "========================================="
echo ""
echo "Resolution: 1280x720 (hardcoded)"
echo ""
echo "Layout:"
echo "  LEFT: Tools panel"
echo "  RIGHT: Color Picker -> Brush Size -> Layers"
echo ""
echo "Controls:"
echo "  ESC or close window to exit"
echo "  View menu to toggle panels"
echo ""
echo "========================================="
echo ""

./canvas

EXIT_CODE=$?
echo ""
echo "Exit code: $EXIT_CODE"

if [ $EXIT_CODE -eq 0 ]; then
    echo "✓ Application exited cleanly"
else
    echo "✗ Application crashed (code $EXIT_CODE)"
fi
