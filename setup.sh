#!/bin/bash

# Setup script for Roblox UI Connector

echo "Setting up Roblox UI Connector..."

# Make all scripts executable
chmod +x *.sh 2>/dev/null || true

echo "Setup complete!"
echo ""
echo "To test compilation:"
echo "  ./test_compile.sh"
echo ""
echo "To install the application:"
echo "  ./install.sh"
echo ""
echo "To run locally without installing:"
echo "  make"
echo "  ./roblox-ui-connector"
