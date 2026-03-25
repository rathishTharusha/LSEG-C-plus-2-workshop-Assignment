# Flower Exchange Matching Engine

A high-performance order matching engine built in C++ for the LSEG UoM Workshop. This system processes CSV-based trading orders, executes matches based on Price-Time priority, and outputs standard execution reports.

## Architecture Overview

The application is built with a focus on speed, modularity, and SOLID design principles. It processes data using a streaming architecture to ensure a low memory footprint, even when handling massive input files.

## Core Components

- **ExchangeApplication** (`ExchangeApplication.h` / `.cpp`): The central orchestrator. It manages fast file I/O operations and routes incoming orders to the correct order book in $O(1)$ time using a `std::unordered_map`.
- **Validator** (`Validator.h` / `.cpp`): A static utility that intercepts parsed strings and enforces business rules (e.g., positive prices, valid instruments, quantity limits in multiples of 10) before order creation.
- **OrderBook** (`OrderBook.h` / `.cpp`): The matching engine core. Maintains independent Buy and Sell books using `std::map` (Red-Black Trees) combined with `std::queue`.
- **Buy Side**: Sorted descending (`std::greater`) to ensure the highest bid is evaluated first.
- **Sell Side**: Sorted ascending (`std::less`) to ensure the lowest ask is evaluated first.
- **Time Priority**: FIFO is strictly maintained for resting orders at the same price level.
- **Entities** (`Entities.h`): Lightweight, statically sized data structures (`Order`, `ExecutionReport`) optimized for cache efficiency, utilizing fast enumerations for execution statuses and instrument types.

## Prerequisites

To compile and run this application, you will need:

- A Linux/WSL environment
- GCC / G++ compiler with C++17 support

## Build Instructions

The project includes a bash script to compile the application with aggressive optimizations (`-O3`) for maximum execution speed.

Give the script execution permissions:

```bash
chmod +x build.sh
```

Run the build script:

```bash
./build.sh
```

This will compile the source files in the `src/` directory, link the headers from the `include/` directory, and generate the `flower_exchange` executable.

## Usage

You can run the application in three ways:

### 1. Default Mode

If you run the executable without arguments, it will look for `orders.csv` in the current directory and output to `execution_rep.csv`.

```bash
./flower_exchange
```

### 2. Custom File Mode

You can specify only the input file path as a command-line argument. In this mode, the output remains the default `execution_rep.csv`.

```bash
./flower_exchange test_data/SampleOrder1.csv
```

### 3. Custom Input + Output Mode

You can specify the input and output file paths as command-line arguments. This is useful for running the provided sample test suites.

```bash
./flower_exchange test_data/SampleOrder1.csv test_data/execReports1.csv
```

## Performance Measurement

To assist with the evaluation phase, the `main.cpp` entry point includes a high-resolution timer. Upon completion, the console will output the total execution time (in milliseconds) required to read, parse, process, and write the entire batch of orders.