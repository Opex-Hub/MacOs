#!/bin/bash
# Test script to verify the application compiles and runs

echo "Testing Roblox UI Connector..."

# Compile the application
echo "Compiling..."
make clean
make

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Application is ready to use."
    echo "Run './roblox-ui-connector' to start the application."
    chmod +x roblox-ui-connector
else
    echo "Compilation failed!"
    exit 1
fi
