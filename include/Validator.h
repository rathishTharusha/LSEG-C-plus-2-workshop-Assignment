#pragma once
#include "Entities.h"
#include <string>
#include <string_view>

class Validator {
public:
    // Takes the raw inputs and an out-parameter for the rejection reason.
    // Returns true if valid, false if it should be rejected.
    static bool isValidOrder(std::string_view clientOrderId, 
                             std::string_view instrumentStr, 
                             int side, 
                             double price, 
                             int quantity, 
                             std::string& rejectReason);

    // Utility to safely convert the string to our fast Enum
    static Instrument parseInstrument(std::string_view instrumentStr);
};