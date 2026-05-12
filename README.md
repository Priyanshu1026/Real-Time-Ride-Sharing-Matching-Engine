# Real-Time Ride Sharing Simulation Engine

A high-performance **Discrete Event Simulator (DES)** built in C++ that models a real-time ride-sharing ecosystem. The engine ingests thousands of rider requests and driver statuses, utilizing spatial indexing to efficiently match users based on proximity and driver rating heuristics.

This project demonstrates core backend engineering concepts including **Event-Driven Architecture**, **Spatial Partitioning**, and **Physics-Based Timing**.

## 🚀 Key Features

* **Discrete Event Simulation:** Uses a priority queue to process asynchronous events (Ride Requests, Trip Completions, Retries) in precise chronological order.
* **Spatial Indexing (Grid Partitioning):** Implements a custom Grid-based index to perform `O(k)` nearby driver lookups, avoiding the performance pitfall of `O(N)` linear scans.
* **Smart Matching Algorithm:** Matches riders not just to the closest driver, but optimizes for a weighted score of `Distance vs. Driver Rating`.
* **Physics-Based Movement:** Trip durations are calculated dynamically based on Euclidean distance and vehicle speed, rather than fixed time windows.
* **Retry & Backoff Logic:** Simulates real-world demand; if a rider cannot be matched, the request enters a "Retry Queue" with exponential backoff before eventually expiring.
* **Data-Driven:** Fully configurable via CSV input files for stress testing different scenarios (e.g., high density, long trips).

## 🛠️ Technical Stack

* **Language:** C++17 (STL: `priority_queue`, `unordered_map`, `mutex`)
* **Build System:** CMake
* **Architecture:** Modular separation between the `MatchingEngine` (Logic) and `SimulationLoop` (Event Processing).

## 📊 How It Works

The simulation runs a "virtual clock." Instead of ticking every second, it jumps efficiently to the next scheduled event time (Next-Event Time Advance).

1.  **Rider Request:** A rider appears at `(x, y)` at time `T`.
2.  **Spatial Search:** The engine queries the grid for drivers within a specific radius.
3.  **Heuristic Scoring:** `Score = Distance - (Rating * Weight)`. Lowest score wins.
4.  **Dispatch:**
    * **Success:** Driver is marked unavailable. Arrival time is calculated: `T_arrival = T_current + (Distance / Speed)`. A "Driver Free" event is scheduled for `T_arrival`.
    * **Failure:** The request is pushed back into the queue with a future timestamp (Retry logic).

## 🏃‍♂️ Getting Started

### Prerequisites
* C++ Compiler (GCC, Clang, or MSVC)
* CMake (3.10+)

### Build & Run
```bash
# 1. Clone the repository
git clone [https://github.com/Priyanshu1026/Real-Time-Ride-Sharing-Matching-Engine.git](https://github.com/Priyanshu1026/Real-Time-Ride-Sharing-Matching-Engine.git)
cd cpp-ride-matching-engine

# 2. Create build directory
mkdir build && cd build

# 3. Compile
cmake -S . -B build
cmake --build build --config Debug 
cmake --build build --config Release (for benchmark performance)

# 4. Run Simulation

You can run the simulator in two ways: manually (for a single scenario) or using the automated test runner (for multiple scenarios).

### Option A: Run Single Simulation (Manual)
Run the executable directly and pass the path to any CSV file as an argument.

*From the `build` directory:*

# **Windows:**
.\build\Debug\ride_matching_simulator.exe samples\events_stress.csv
.\build\Release\ride_matching_simulator.exe samples\events_stress.csv (for benchmark performance)

# **Linux/Mac:**
./ride_matching_simulator ../samples/events.csv

### Option B: Run Automated Test Suite (Multiple Scenarios)
We provide a Python script that automatically runs the engine against multiple test cases (Basic Match, Retry Logic, Failure Scenarios) and verifies the Match Rate.
From the project root directory:
#**Windows(bash)**
python run_tests.py
#Expected Output (for current test cases):
========================================
      AUTOMATED INTEGRATION TESTS       
========================================
🔹 Running Test: Basic Match (samples/test_basic.csv)...
✅ PASS. (Expected 100.0%, Got 100.0%)
----------------------------------------
🔹 Running Test: Retry Logic (samples/test_retry.csv)...
✅ PASS. (Expected 100.0%, Got 100.0%)
----------------------------------------
🔹 Running Test: Impossible Match (samples/test_fail.csv)...
✅ PASS. (Expected 0.0%, Got 0.0%)
----------------------------------------
Summary: 3/3 Tests Passed.
🎉 ALL SYSTEMS GO!

for both cases, we generate matches.log which stores output. for Option A, matches.log is in build folder, while in Option B, it is in root folder.

sample report for debug version and cout-ing text for every match or fail or retry

========================================
       SIMULATION REPORT CARD           
========================================
 Total Requests  : 100000
 Total Matches   : 16063
 Failed/Expired  : 83937
 Match Rate      : 16.1%
 Processing Time : 483111.0 ms
 Throughput      : 207.0 req/sec
========================================


sample report for release version(benchmark)
========================================
       SIMULATION REPORT CARD           
========================================
 Total Requests  : 100000
 Total Matches   : 16063
 Failed/Expired  : 83937
 Match Rate      : 16.1%
 Processing Time : 13701.2 ms
 Throughput      : 7298.6 req/sec
========================================