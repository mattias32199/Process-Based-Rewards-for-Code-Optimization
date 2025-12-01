#!/bin/bash
# Setup script for SimdBench environment

echo "Setting up SimdBench environment..."

# Install Python dependencies
echo "Installing Python packages..."
pip3 install tqdm fire numpy psutil futures tabulate

# Install transformers and torch for Qwen model
echo "Installing LLM dependencies..."
pip3 install transformers torch accelerate

# Set PYTHONPATH
export PYTHONPATH=$(pwd)
echo "Set PYTHONPATH=$PYTHONPATH"

# Check if benchmark directory exists
if [ ! -d "benchmark" ]; then
    echo "Google Benchmark not found. Setting up..."
    git submodule update --init --recursive
    cd benchmark
    cmake -E make_directory "build"
    cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release ../
    cmake --build "build" --config Release
    cd ..
else
    echo "Google Benchmark already exists"
fi

# Verify installation
echo ""
echo "Verifying installation..."
python3 -c "import tqdm, fire, numpy, psutil, tabulate; print('All required packages installed')"

echo ""
echo "Setup complete!"
echo ""
echo "To use SimdBench in new terminals, run:"
echo "  export PYTHONPATH=$(pwd)"
