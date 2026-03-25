#pragma once
#include "OrderBook.h"
#include "Validator.h"
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

class ExchangeApplication {
private:
    // Our 5 dedicated order books for each instrument, using an unordered_map for O(1) access by instrument
    std::unordered_map<Instrument, OrderBook> orderBooks;

    int nextOrderId = 1; // Global ID counter

    // Helper method to generate the unique "ordX" ID 
    std::string generateOrderId();

    // Helper to generate the exact time format required: YYYYMMDD-HHMMSS.sss
    std::string getCurrentTransactionTime();

    // Helper to map our Status enum back to the string required for the CSV
    std::string statusToString(Status status);

    // Helper to write a single report to the output file stream
    void writeReport(std::ofstream& outFile, const ExecutionReport& report);

public:
    ExchangeApplication();

    // Main execution function
    void processFiles(const std::string& inputFilePath, const std::string& outputFilePath);
};