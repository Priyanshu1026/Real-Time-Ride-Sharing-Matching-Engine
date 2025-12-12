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
git clone [https://github.com/yourusername/cpp-ride-matching-engine.git](https://github.com/yourusername/cpp-ride-matching-engine.git)
cd cpp-ride-matching-engine

# 2. Create build directory
mkdir build && cd build

# 3. Compile
cmake ..
cmake --build .

# 4. Run Simulation
# On Windows
./Debug/ride_matching_simulator.exe ../samples/events.csv

# On Linux/Mac
./ride_matching_simulator ../samples/events.csv