#include "ExchangeApplication.h"
#include <iostream>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " Flower Exchange Engine (Network Node)  " << std::endl;
    std::cout << "========================================" << std::endl;
    
    ExchangeApplication app;
    
    // Start listening on port 8080, write results to network_rep.csv
    app.startServer(8080, "network_rep.csv"); 
    
    std::cout << "Server shutdown gracefully." << std::endl;
    return 0;
}