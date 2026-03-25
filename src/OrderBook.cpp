#include "OrderBook.h"
#include <algorithm> // For min
using namespace std;

vector<ExecutionReport> OrderBook::processOrder(Order& order) {
    vector<ExecutionReport> reports;
    
    // Track the starting quantity to know if we crossed the spread
    int initialQty = order.quantity;

    if (order.side == Side::Buy) {
        matchBuyOrder(order, reports);
        
        if (order.quantity > 0) {
            buyOrders[order.price].push(order);
            
            // ONLY generate a 'New' report if it didn't match anything
            if (order.quantity == initialQty) {
                ExecutionReport rep;
                rep.orderId = order.orderId;
                rep.clientOrderId = order.clientOrderId;
                rep.instrument = order.instrument;
                rep.side = order.side;
                rep.status = Status::New; 
                rep.quantity = order.quantity;
                rep.price = order.price;
                reports.push_back(rep);
            }
        }
    } else if (order.side == Side::Sell) {
        matchSellOrder(order, reports);
        
        if (order.quantity > 0) {
            sellOrders[order.price].push(order);
            
            if (order.quantity == initialQty) {
                ExecutionReport rep;
                rep.orderId = order.orderId;
                rep.clientOrderId = order.clientOrderId;
                rep.instrument = order.instrument;
                rep.side = order.side;
                rep.status = Status::New; 
                rep.quantity = order.quantity;
                rep.price = order.price;
                reports.push_back(rep);
            }
        }
    }

    return reports;
}

void OrderBook::matchBuyOrder(Order& incomingBuy, vector<ExecutionReport>& reports) {
    // Keep matching as long as the incoming order has quantity and there are sellers
    while (incomingBuy.quantity > 0 && !sellOrders.empty()) {
        
        // Get the best available sell price (lowest price due to less)
        auto bestSellIt = sellOrders.begin();
        double bestSellPrice = bestSellIt->first;
        
        // If our incoming buy price is strictly less than the best sell price, 
        // the spread is not crossed. Stop matching.
        if (incomingBuy.price < bestSellPrice) {
            break; 
        }

        // Get the queue of orders resting at this price level
        queue<Order>& restingSells = bestSellIt->second;
        Order& restingSell = restingSells.front();

        // Determine trade quantity
        int tradeQty = min(incomingBuy.quantity, restingSell.quantity);
        
        // The execution price is the price of the resting order 
        double execPrice = restingSell.price;

        // Update quantities
        incomingBuy.quantity -= tradeQty;
        restingSell.quantity -= tradeQty;

        // 1. Generate Report for the Incoming Aggressive Buy
        ExecutionReport aggressiveRep;
        aggressiveRep.orderId = incomingBuy.orderId;
        aggressiveRep.clientOrderId = incomingBuy.clientOrderId;
        aggressiveRep.instrument = incomingBuy.instrument;
        aggressiveRep.side = incomingBuy.side;
        aggressiveRep.price = execPrice; // Note: Uses execution price, not original order price!
        aggressiveRep.quantity = tradeQty; // Report the executed quantity
        aggressiveRep.status = (incomingBuy.quantity == 0) ? Status::Fill : Status::PFill;
        reports.push_back(aggressiveRep);

        // 2. Generate Report for the Resting Passive Sell
        ExecutionReport passiveRep;
        passiveRep.orderId = restingSell.orderId;
        passiveRep.clientOrderId = restingSell.clientOrderId;
        passiveRep.instrument = restingSell.instrument;
        passiveRep.side = restingSell.side;
        passiveRep.price = execPrice;
        passiveRep.quantity = tradeQty;
        passiveRep.status = (restingSell.quantity == 0) ? Status::Fill : Status::PFill;
        reports.push_back(passiveRep);

        // If the resting order is fully filled, remove it from the queue
        if (restingSell.quantity == 0) {
            restingSells.pop();
            // If the queue for this price level is empty, remove the price level entirely
            if (restingSells.empty()) {
                sellOrders.erase(bestSellIt);
            }
        }
    }
}

void OrderBook::matchSellOrder(Order& incomingSell, vector<ExecutionReport>& reports) {
    // Keep matching as long as the incoming order has quantity and there are buyers
    while (incomingSell.quantity > 0 && !buyOrders.empty()) {
        
        // Get the best available buy price (highest price due to greater)
        auto bestBuyIt = buyOrders.begin();
        double bestBuyPrice = bestBuyIt->first;
        
        // If our incoming sell price is strictly greater than the best buy price, 
        // the spread is not crossed. Stop matching.
        if (incomingSell.price > bestBuyPrice) {
            break; 
        }

        // Get the queue of orders resting at this price level
        queue<Order>& restingBuys = bestBuyIt->second;
        Order& restingBuy = restingBuys.front();

        // Determine trade quantity
        int tradeQty = min(incomingSell.quantity, restingBuy.quantity);
        
        // The execution price is the price of the resting order 
        double execPrice = restingBuy.price;

        // Update quantities for both the incoming and resting orders
        incomingSell.quantity -= tradeQty;
        restingBuy.quantity -= tradeQty;

        // 1. Generate Report for the Incoming Aggressive Sell
        ExecutionReport aggressiveRep;
        aggressiveRep.orderId = incomingSell.orderId;
        aggressiveRep.clientOrderId = incomingSell.clientOrderId;
        aggressiveRep.instrument = incomingSell.instrument;
        aggressiveRep.side = incomingSell.side;
        aggressiveRep.price = execPrice;   // Uses execution price 
        aggressiveRep.quantity = tradeQty; // Report the executed quantity
        aggressiveRep.status = (incomingSell.quantity == 0) ? Status::Fill : Status::PFill;
        reports.push_back(aggressiveRep);

        // 2. Generate Report for the Resting Passive Buy
        ExecutionReport passiveRep;
        passiveRep.orderId = restingBuy.orderId;
        passiveRep.clientOrderId = restingBuy.clientOrderId;
        passiveRep.instrument = restingBuy.instrument;
        passiveRep.side = restingBuy.side;
        passiveRep.price = execPrice;
        passiveRep.quantity = tradeQty;
        passiveRep.status = (restingBuy.quantity == 0) ? Status::Fill : Status::PFill;
        reports.push_back(passiveRep);

        // If the resting order is fully filled, pop it from the queue
        if (restingBuy.quantity == 0) {
            restingBuys.pop();
            // If the queue for this price level is now empty, erase the price level
            if (restingBuys.empty()) {
                buyOrders.erase(bestBuyIt);
            }
        }
    }
}