#include "ExchangeApplication.h"
#include <iostream>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " Flower Exchange Engine (Network Node)  " << std::endl;
    std::cout << "========================================" << std::endl;
    
    ExchangeApplication app;
    
    // Start listening on port 8080, write results to execution_report.csv
    app.startServer(8080, "execution_report.csv"); 
    
    std::cout << "Server shutdown gracefully." << std::endl;
    return 0;
}