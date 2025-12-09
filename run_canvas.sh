#!/bin/bash

# Kill any running instances
pkill -9 canvas 2>/dev/null

cd "$(dirname "$0")/build"

echo "========================================="
echo "Canvas - Clean Start"
echo "========================================="
echo ""
echo "Starting fresh instance..."
echo ""

./canvas

echo ""
echo "Exit code: $?"
