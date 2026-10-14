#!/bin/bash

# Roblox UI Connector Installer

echo "Installing Roblox UI Connector..."

# Compile the application
make clean
make

if [ $? -ne 0 ]; then
    echo "Error: Compilation failed."
    exit 1
fi

# Install the application
sudo make install

if [ $? -ne 0 ]; then
    echo "Error: Installation failed."
    exit 1
fi

echo "Installation completed successfully!"
echo ""
echo "To run the application, type:"
echo "  roblox-ui-connector"
