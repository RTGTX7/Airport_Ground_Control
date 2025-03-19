#!/bin/bash

# Set environment variable for the input file path
export AIRPLANE_FILE_PATH="$(pwd)/airplane.txt"

# Clean up old build and binary directories
if [ -d "build" ]; then rm -Rf build; fi
if [ -d "bin" ]; then rm -Rf bin; fi
rm -f *.csv

# Create and navigate to the build directory
mkdir -p build
cd build || exit

# Remove old files, configure, and compile
rm -rf *
cmake ..
make
cd ..

echo "Compilation done. Executable in the bin folder."
echo "AIRPLANE_FILE_PATH set to: $AIRPLANE_FILE_PATH"
