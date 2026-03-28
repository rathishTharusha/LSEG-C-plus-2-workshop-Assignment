# Flower Exchange Matching Engine (Network Sockets Branch)

A high-performance order matching engine built in C++ for the LSEG UoM Workshop.

## Branch Highlight

This experimental branch upgrades the matching engine from a file-based I/O system to a real-time, event-driven TCP server using Boost.Asio. It demonstrates production-grade network ingestion, bypassing hard-drive bottlenecks to process over 500,000 orders per second.

## Architecture Overview

The application is built with a focus on speed, modularity, and SOLID design principles. It operates on a strict client-server model.

## Core Components

- ExchangeApplication (`ExchangeApplication.h` / `.cpp`): The central orchestrator. It acts as a synchronous TCP server, binding to port 8080 to listen for incoming data streams, parsing them instantly, and routing them to the correct order book in $O(1)$ time.
- Python Load Tester (`tcp_client.py`): A custom client script that pre-loads 100,000+ CSV orders into RAM and blasts them across the network socket to stress-test the C++ engine.
- Validator (`Validator.h` / `.cpp`): A static utility that intercepts parsed strings and enforces business rules (for example, positive prices and valid instruments) before order creation.
- OrderBook (`OrderBook.h` / `.cpp`): The matching engine core. Maintains independent buy and sell books using `std::map` (red-black trees) combined with `std::queue` to guarantee strict price-time priority.
- Entities (`Entities.h`): Lightweight, statically sized data structures (`Order`, `ExecutionReport`) optimized for cache efficiency.

## Prerequisites

To compile and run this networked application, you will need:

- A Linux/WSL environment
- GCC/G++ compiler with C++17 support
- Boost C++ Libraries (for Boost.Asio)
- Python 3 (to run the load tester)

On Debian/Ubuntu/WSL, install Boost via:

```bash
sudo apt update
sudo apt install libboost-all-dev -y
```

## Build Instructions

The project includes a bash script to compile the application with aggressive optimizations (`-O3`) and link the required Boost and POSIX thread libraries.

Give the script execution permissions:

```bash
chmod +x build.sh
```

Run the build script:

```bash
./build.sh
```

## Usage: Running the Distributed Load Test

Because this is a server-client architecture, you must run the application using two separate terminal windows.

### 1. Start the Server (Terminal 1)

Boot up the C++ matching engine. It will bind to localhost and wait for incoming network connections. Output will be written to `network_rep.csv`.

```bash
./flower_exchange
```

Expected output:

```text
Matching Engine Listening on Port 8080...
```

### 2. Fire the Payload (Terminal 2)

In a second terminal, execute the Python load tester. This will connect to the engine and fire the `stress_orders.csv` payload across the socket.

```bash
python3 tcp_client.py
```

## Performance Measurement

Because the system is no longer constrained by file I/O bottlenecks, performance is measured in network throughput. Upon completing the payload delivery, the `tcp_client.py` script will output the exact time taken and the total messages processed per second.