#include "ExchangeApplication.h"
#include <iostream>
#include <chrono>
#include <string>

int main(int argc, char* argv[]) {
    // Default file names as expected by the specification
    std::string inputFile = "orders.csv";
    std::string outputFile = "execution_rep.csv";

    // Supported modes:
    //   0 args: default input + default output
    //   1 arg : custom input + default output
    if (argc == 2) {
        inputFile = argv[1];
    //   2 args: custom input + custom output
    } else if (argc >= 3) {
        inputFile = argv[1];
        outputFile = argv[2];
    }

    std::cout << "--- Flower Exchange Matching Engine ---" << std::endl;
    std::cout << "Initializing..." << std::endl;
    std::cout << "Input file: " << inputFile << std::endl;

    // Start the high-resolution timer
    auto start = std::chrono::high_resolution_clock::now();

    // Instantiate the orchestrator and process the file
    ExchangeApplication app;
    app.processFiles(inputFile, outputFile);

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Processing complete." << std::endl;
    std::cout << "Output written to: " << outputFile << std::endl;
    
    // Display the critical performance metric
    std::cout << "Total execution time: " << elapsed.count() << " ms\n";
    std::cout << "---------------------------------------" << std::endl;

    return 0;
}