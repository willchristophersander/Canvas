#!/bin/bash

pkill -9 canvas 2>/dev/null

cd "$(dirname "$0")/build"

echo "Starting Canvas with debug output..."
./canvas

echo ""
echo "Exit code: $?"
