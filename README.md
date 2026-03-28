# LSEG Flower Exchange: High-Performance Matching Engine

A high-performance order matching engine developed in C++ for the LSEG UoM Workshop.

This branch demonstrates an evolution of the core system from a file-I/O bound application into a distributed client-server architecture. By implementing real-time TCP socket ingestion and containerizing the application, this version isolates the matching logic from disk bottlenecks to evaluate the engine's true throughput capabilities.

## System Architecture

The project operates on a client-server microservice model, decoupling the user interface from the core matching logic. The breakdown below explains the repository structure and each component's responsibility.

### 1. Core C++ Engine (/src and /include)

- main.cpp: Entry point of the C++ backend. Boots ExchangeApplication and binds it to the designated TCP port.
- ExchangeApplication.h / ExchangeApplication.cpp: Central orchestrator. Operates as the TCP server using Boost.Asio, parses comma-separated network payloads into objects, and routes them to the correct instrument order book in $O(1)$ time.
- OrderBook.h / OrderBook.cpp: Core matching logic. Maintains independent buy and sell books using std::map (Red-Black Trees) combined with std::queue to guarantee strict price-time priority.
- Validator.h / Validator.cpp: Stateless validation gateway. Enforces business rules (for example, quantity multiples of 10, strictly positive prices, and valid instrument names) before an order enters the matching engine.
- Entities.h: Lightweight, cache-efficient data structures (Order, ExecutionReport) and strongly typed enumerations (Side, Status, Instrument) used throughout the system.

### 2. Web Interface and Networking

- app.py: Python Flask web server. Handles HTTP requests, serves the UI, accepts CSV uploads, manages the C++ engine as a background process, and acts as the TCP client to stream data into engine memory.
- templates/index.html: Tailwind CSS-powered frontend where users upload orders, view processing metrics, and download execution reports.
- tcp_client.py: High-performance load testing script. Preloads large CSV files into RAM and transmits them over TCP to benchmark absolute network throughput.

### 3. Build and Deployment

- build.sh: Compiles the C++ codebase with aggressive -O3 optimizations and links required POSIX and Boost networking libraries.
- Dockerfile and .dockerignore: Containerize the full stack (Ubuntu runtime, C++ compiler, Boost, Python, Flask) into a reproducible, isolated deployment.
- /test_data: Sample order datasets and stress-test CSV payloads used to evaluate system integrity.

## Deployment Instructions

### Option A: Docker Execution (Recommended)

To ensure environment consistency and prevent dependency conflicts, the complete application stack (C++ compiler, Boost libraries, Python, and Flask) is containerized.

Prerequisite: Docker must be installed and running on the host machine.

Build the container image:

```bash
docker build -t lseg-flower-exchange .
```

Run the container:

```bash
docker run -p 5000:5000 lseg-flower-exchange
```

Access the application at http://localhost:5000.

### Option B: Manual Build (Local Environment)

For local development or testing without Docker, run the C++ engine and Python server natively.

Prerequisites:
- g++ with C++17 support
- libboost-all-dev
- python3
- python3-flask (or Flask installed via pip in a virtual environment)

Compile the C++ matching engine:

```bash
chmod +x build.sh
./build.sh
```

Start the Flask web gateway:

```bash
python3 app.py
```

Access the application at http://127.0.0.1:5000.

## Stress Testing and Benchmarking

To measure the engine's theoretical throughput limit, use the dedicated TCP load tester. This script bypasses the web interface, preloads order data into RAM, and streams it over the TCP socket.

Execution steps:

1. Start the C++ matching engine in one terminal:

```bash
./flower_exchange
```

The engine will print that it is listening on port 8080.

2. In a separate terminal, run the load tester:

```bash
python3 tcp_client.py
```

On completion, the script prints total execution time and calculated messages per second.

## Engineering and Optimization Highlights

The matching engine was designed with strict hardware and algorithmic constraints in mind.

- Memory management: Strict RAII (Resource Acquisition Is Initialization). Standard library containers manage their own lifecycles. Valgrind profiling confirms zero memory leaks during full execution cycles.
- Small String Optimization (SSO): Using std::string for clientOrderId (capped at 7 characters) enables stack-local storage in many implementations, avoiding expensive heap allocations.
- Algorithmic complexity:
	- Instrument routing uses std::unordered_map (hash tables) with strongly typed enum class values, guaranteeing $O(1)$ routing time without string-comparison overhead.
	- Order book insertions use std::map (Red-Black Trees) to keep sorted price levels in $O(\log N)$ time. Empirical testing showed this outperformed flat contiguous arrays (std::vector), where high-churn randomized insertion causes $O(N)$ memory shifting.