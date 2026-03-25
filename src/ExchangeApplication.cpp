#include "ExchangeApplication.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
using namespace std;

ExchangeApplication::ExchangeApplication() {
    // Initialize the 5 order books
    orderBooks.emplace(Instrument::Rose, OrderBook(Instrument::Rose));
    orderBooks.emplace(Instrument::Lavender, OrderBook(Instrument::Lavender));
    orderBooks.emplace(Instrument::Lotus, OrderBook(Instrument::Lotus));
    orderBooks.emplace(Instrument::Tulip, OrderBook(Instrument::Tulip));
    orderBooks.emplace(Instrument::Orchid, OrderBook(Instrument::Orchid));
}

string ExchangeApplication::statusToString(Status status) {
    // Map our Status enum back to the string required for the CSV
    switch (status) {
        case Status::New: return "New";
        case Status::Rejected: return "Rejected";
        case Status::Fill: return "Fill";
        case Status::PFill: return "PFill";
        default: return "Unknown";
    }
}

string ExchangeApplication::getCurrentTransactionTime() {
    // Generates the required YYYYMMDD-HHMMSS.sss format
    auto now = chrono::system_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = chrono::system_clock::to_time_t(now);
    tm bt = *localtime(&timer);

    ostringstream oss;
    oss << put_time(&bt, "%Y%m%d-%H%M%S") << '.' << setfill('0') << setw(3) << ms.count();
    return oss.str();
}

void ExchangeApplication::writeReport(ofstream& outFile, const ExecutionReport& rep) {
    // Convert Instrument enum back to string for output
    string instStr;
    if (rep.instrument == Instrument::Rose) instStr = "Rose";
    else if (rep.instrument == Instrument::Lavender) instStr = "Lavender";
    else if (rep.instrument == Instrument::Lotus) instStr = "Lotus";
    else if (rep.instrument == Instrument::Tulip) instStr = "Tulip";
    else if (rep.instrument == Instrument::Orchid) instStr = "Orchid";

    outFile << rep.orderId << ","
            << rep.clientOrderId << ","
            << instStr << ","
            << static_cast<int>(rep.side) << ","
            << statusToString(rep.status) << ","
            << rep.quantity << ","
            << fixed << setprecision(2) << rep.price << ","
            << rep.reason << ","
            << rep.transactionTime << "\n";
}

void ExchangeApplication::processFiles(const string& inputFilePath, const string& outputFilePath) {
    ifstream inFile(inputFilePath);
    ofstream outFile(outputFilePath);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error: Could not open files." << endl;
        return;
    }

    // Write the CSV Header required for the output
    outFile << "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time\n";

    string line;
    // Skip the header line in the input file
    getline(inFile, line);

    while (getline(inFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string clientOrderId, instrumentStr, sideStr, qtyStr, priceStr;

        // Parse fields separated by commas [cite: 16]
        getline(ss, clientOrderId, ',');
        getline(ss, instrumentStr, ',');
        getline(ss, sideStr, ',');
        getline(ss, qtyStr, ',');
        getline(ss, priceStr, ',');

        int side = stoi(sideStr);
        int quantity = stoi(qtyStr);
        double price = stod(priceStr);

        string rejectReason;
        string currentOrderId = generateOrderId(); // Generate ID globally here
        
        // 1. Validate the Order
        if (!Validator::isValidOrder(clientOrderId, instrumentStr, side, price, quantity, rejectReason)) {
            // Immediately write a Rejected report
            ExecutionReport rep(currentOrderId, 
                clientOrderId, 
                Validator::parseInstrument(instrumentStr), 
                static_cast<Side>(side), 
                Status::Rejected, 
                quantity, 
                price, 
                rejectReason, 
                getCurrentTransactionTime());
            writeReport(outFile, rep);
            continue;
        }

        // 2. Build the Valid Order
        Instrument inst = Validator::parseInstrument(instrumentStr);
        Order order(currentOrderId, 
            clientOrderId, 
            inst, 
            static_cast<Side>(side), 
            quantity, 
            price);
        
        // 3. Process through the correct Order Book
        vector<ExecutionReport> reports = orderBooks.at(inst).processOrder(order);

        // 4. Write all resulting reports to the output file
        for (auto& rep : reports) {
            rep.transactionTime = getCurrentTransactionTime();
            writeReport(outFile, rep);
        }
    }
}

string ExchangeApplication::generateOrderId() {
    return "ord" + to_string(nextOrderId++);
}