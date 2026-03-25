#pragma once
#include <string>
#include <vector>
#include <chrono>
using namespace std;

// Define enums for fast comparisons during matching
enum class Side { Buy = 1, Sell = 2, invalid = 3 };
enum class Status { New = 0, Rejected = 1, Fill = 2, PFill = 3 };
enum class Instrument { Rose, Lavender, Lotus, Tulip, Orchid, Invalid };

struct Order {
    // string for <=15 chars utilizes Small String Optimization (SSO) in C++, avoiding heap allocation and improving performance
    string orderId;
    string clientId;
    Instrument instrument;
    Side side;
    int quantity;
    double price;

    // default constructor for creating an empty order, useful for initializing variables or creating placeholder orders
    Order() = default;

    // parameterized constructor for creating an order with specific details, allowing for easy instantiation of orders with all necessary information
    Order(const string& orderId, const string& clientId, Instrument instrument, Side side, int quantity, double price)
        : orderId(orderId), clientId(clientId), instrument(instrument), side(side), quantity(quantity), price(price) {}

};

struct ExecutionReport {
    string orderId;
    string clientId;
    Instrument instrument;
    Side side;
    Status status;
    int quantity;
    double price;
    string reason; // for rejected orders, provides a reason for rejection, optional
    string timestamp; //  YYYYMMDD-HHMMSS.sss format

    // default constructor for creating an empty execution, useful for initializing variables or creating placeholder executions
    ExecutionReport() = default;

};
