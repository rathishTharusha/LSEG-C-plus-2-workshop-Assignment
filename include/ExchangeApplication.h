#pragma once
#include "OrderBook.h"
#include "Validator.h"
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <boost/asio.hpp> // The network library

using boost::asio::ip::tcp;

class ExchangeApplication {
private:
    std::unordered_map<Instrument, OrderBook> orderBooks;
    int nextOrderId = 1;

    std::string generateOrderId();
    std::string getCurrentTransactionTime();
    std::string statusToString(Status status);
    void writeReport(std::ofstream& outFile, const ExecutionReport& report);

    // Helper to process a single string line (extracted for clean network reading)
    void processLine(const std::string& line, std::ofstream& outFile);

public:
    ExchangeApplication();

    // Replaces processFiles. This binds to a TCP port and listens for data.
    void startServer(short port, const std::string& outputFilePath);
};