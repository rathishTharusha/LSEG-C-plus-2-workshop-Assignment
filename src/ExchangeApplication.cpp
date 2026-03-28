#include "ExchangeApplication.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

ExchangeApplication::ExchangeApplication() {
    orderBooks.emplace(Instrument::Rose, OrderBook(Instrument::Rose));
    orderBooks.emplace(Instrument::Lavender, OrderBook(Instrument::Lavender));
    orderBooks.emplace(Instrument::Lotus, OrderBook(Instrument::Lotus));
    orderBooks.emplace(Instrument::Tulip, OrderBook(Instrument::Tulip));
    orderBooks.emplace(Instrument::Orchid, OrderBook(Instrument::Orchid));
}

std::string ExchangeApplication::statusToString(Status status) {
    switch (status) {
        case Status::New: return "New";
        case Status::Rejected: return "Rejected";
        case Status::Fill: return "Fill";
        case Status::PFill: return "PFill";
        default: return "Unknown";
    }
}

std::string ExchangeApplication::getCurrentTransactionTime() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y%m%d-%H%M%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

void ExchangeApplication::writeReport(std::ofstream& outFile, const ExecutionReport& rep) {
    std::string instStr;
    if (rep.instrument == Instrument::Rose) instStr = "Rose";
    else if (rep.instrument == Instrument::Lavender) instStr = "Lavender";
    else if (rep.instrument == Instrument::Lotus) instStr = "Lotus";
    else if (rep.instrument == Instrument::Tulip) instStr = "Tulip";
    else if (rep.instrument == Instrument::Orchid) instStr = "Orchid";

    outFile << rep.orderId << "," << rep.clientOrderId << "," << instStr << ","
            << static_cast<int>(rep.side) << "," << statusToString(rep.status) << ","
            << rep.quantity << "," << std::fixed << std::setprecision(2) << rep.price << ","
            << rep.reason << "," << rep.transactionTime << "\n";
}

std::string ExchangeApplication::generateOrderId() {
    return "ord" + std::to_string(nextOrderId++);
}

// Extracted the core logic into a single-line processor
void ExchangeApplication::processLine(const std::string& line, std::ofstream& outFile) {
    if (line.empty()) return;

    std::stringstream ss(line);
    std::string clientOrderId, instrumentStr, sideStr, qtyStr, priceStr;

    std::getline(ss, clientOrderId, ',');
    std::getline(ss, instrumentStr, ',');
    std::getline(ss, sideStr, ',');
    std::getline(ss, qtyStr, ',');
    std::getline(ss, priceStr, ',');

    int side = std::stoi(sideStr);
    int quantity = std::stoi(qtyStr);
    double price = std::stod(priceStr);

    std::string rejectReason;
    std::string currentOrderId = generateOrderId();

    if (!Validator::isValidOrder(clientOrderId, instrumentStr, side, price, quantity, rejectReason)) {
        ExecutionReport rep;
        rep.orderId = currentOrderId;
        rep.clientOrderId = clientOrderId;
        rep.instrument = Validator::parseInstrument(instrumentStr);
        rep.side = static_cast<Side>(side);
        rep.status = Status::Rejected;
        rep.quantity = quantity;
        rep.price = price;
        rep.reason = rejectReason;
        rep.transactionTime = getCurrentTransactionTime();
        writeReport(outFile, rep);
        return;
    }

    Instrument inst = Validator::parseInstrument(instrumentStr);
    Order order(currentOrderId, clientOrderId, inst, static_cast<Side>(side), quantity, price);

    std::vector<ExecutionReport> reports = orderBooks.at(inst).processOrder(order);

    for (auto& rep : reports) {
        rep.transactionTime = getCurrentTransactionTime();
        writeReport(outFile, rep);
    }
}

// The new Network Server Loop
void ExchangeApplication::startServer(short port, const std::string& outputFilePath) {
    std::ofstream outFile(outputFilePath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return;
    }
    outFile << "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time\n";

    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        std::cout << "Matching Engine Listening on Port " << port << "..." << std::endl;

        // Block and wait for the Python script to connect
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        
        std::cout << "Client Connected! Ingesting orders over TCP..." << std::endl;

        boost::asio::streambuf buffer;
        std::istream is(&buffer);
        std::string line;

        while (true) {
            boost::system::error_code ec;
            // Read exactly one line from the network buffer
            boost::asio::read_until(socket, buffer, '\n', ec);

            if (ec == boost::asio::error::eof) break; // Connection closed gracefully
            else if (ec) throw boost::system::system_error(ec);

            std::getline(is, line);
            
            // Clean up Windows carriage returns if present
            if (!line.empty() && line.back() == '\r') line.pop_back();

            // Check for our poison pill
            if (line.rfind("EOF", 0) == 0) { 
                std::cout << "Poison Pill Received. Shutting down." << std::endl;
                break;
            }

            processLine(line, outFile);
        }
    } catch (std::exception& e) {
        std::cerr << "Network Error: " << e.what() << std::endl;
    }
}