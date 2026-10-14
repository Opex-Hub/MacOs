#!/bin/bash

echo "Testing compilation of Roblox UI Connector..."

# Make scripts executable
chmod +x *.sh 2>/dev/null || true

# Clean and compile
make clean
make

if [ $? -eq 0 ]; then
    echo "✓ SUCCESS: Application compiled successfully!"
    echo "✓ Binary created: roblox-ui-connector"
    echo "✓ Ready for installation or local testing"
    echo ""
    echo "To run locally: ./roblox-ui-connector"
    echo "To install system-wide: sudo make install"
else
    echo "✗ ERROR: Compilation failed!"
    echo "Please check the error messages above."
    exit 1
fi
