#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "========================================"
echo " Building Flower Exchange Engine"
echo "========================================"

# The compiler command with aggressive optimizations (-O3) 
# and the include path (-I./include)
g++ -O3 -std=c++17 -I./include \
    src/main.cpp \
    src/ExchangeApplication.cpp \
    src/OrderBook.cpp \
    src/Validator.cpp \
    -o flower_exchange

echo " Compilation successful!"
echo " Executable 'flower_exchange' created."
echo "========================================"
echo ""
echo "To run the engine with default files, use:"
echo "  ./flower_exchange"
echo ""
echo "To run with a custom order file and default report file, use:"
echo "  ./flower_exchange test_data/SampleOrder1.csv"
echo ""
echo "To run with specific LSEG sample files, use:"
echo "  ./flower_exchange test_data/SampleOrder1.csv test_data/execReports1.csv"
echo "========================================"