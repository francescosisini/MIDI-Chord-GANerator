#!/bin/bash

# Install dependencies
echo "Installing dependencies..."
sudo apt-get update
sudo apt-get install -y libsmf-dev libglib2.0-dev

# Compile the project
echo "Compiling the project..."
make

echo "Setup complete. You can now run the program with 'make run'."
