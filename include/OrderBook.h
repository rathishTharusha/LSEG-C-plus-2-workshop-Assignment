#pragma once
#include "Entities.h"
#include <map>
#include <queue>
#include <vector>
#include <string>
using namespace std;

class OrderBook {
private:
    Instrument instrument;

    // Separate order books for buy and sell sides, using maps to maintain sorted order by price
    map<double, queue<Order>, greater<double>> buyOrders; // Buy orders sorted by price descending
    map<double, queue<Order>, less<double>> sellOrders; // Sell orders sorted by price ascending

    // Helper functions for matching orders
    void matchBuyOrder(Order& incomingBuy, vector<ExecutionReport>& reports);
    void matchSellOrder(Order& incomingSell, vector<ExecutionReport>& reports);
public:
    OrderBook(Instrument instrument) : instrument(instrument) {}

    // Function to add a new order to the order book and attempt to match it
    vector<ExecutionReport> processOrder(Order& order);

};