#include "Validator.h"

Instrument Validator::parseInstrument(std::string_view instStr) {
    if (instStr == "Rose") return Instrument::Rose;
    if (instStr == "Lavender") return Instrument::Lavender;
    if (instStr == "Lotus") return Instrument::Lotus;
    if (instStr == "Tulip") return Instrument::Tulip;
    if (instStr == "Orchid") return Instrument::Orchid;
    return Instrument::Invalid;
}

bool Validator::isValidOrder(std::string_view clientOrderId, 
                             std::string_view instrumentStr, 
                             int side, 
                             double price, 
                             int quantity, 
                             std::string& rejectReason) {
    
    // 1. Validate Client Order ID (max 7 chars)
    // The spec says alpha-numeric, max 7 chars.
    if (clientOrderId.empty() || clientOrderId.length() > 7) {
        rejectReason = "Invalid client order ID";
        return false;
    }

    // 2. Validate Instrument
    Instrument inst = parseInstrument(instrumentStr);
    if (inst == Instrument::Invalid) {
        rejectReason = "Invalid instrument"; // Exact string from specs
        return false;
    }

    // 3. Validate Side (1 = Buy, 2 = Sell)
    if (side != 1 && side != 2) {
        rejectReason = "Invalid side"; // Exact string from specs
        return false;
    }

    // 4. Validate Price (Must be > 0.0)
    if (price <= 0.0) {
        rejectReason = "Invalid price"; // Exact string from specs
        return false;
    }

    // 5. Validate Quantity (Min 10, Max 1000, multiple of 10)
    if (quantity < 10 || quantity > 1000 || (quantity % 10 != 0)) {
        rejectReason = "Invalid size"; // Exact string from specs
        return false;
    }

    // If it passes all checks, it's good to go!
    return true;
}