#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>
#include <cmath>
#include <iomanip>
#include "MatchingEngine.hpp"

// CONSTANTS
const double VEHICLE_SPEED = 0.01; // Slower speed (10ms to travel 1 unit)
const int MAX_RETRIES = 3;         // How many times to retry a failed request

// Helper: Write to both Console and File
void log(std::ofstream &file, const std::string &msg) {
    std::cout << msg; // Show on screen
    if (file.is_open()) {
        file << msg;  // Save to file
        file.flush(); // Ensure it saves immediately
    }
}

// Event Types
enum EventType { RIDER_REQUEST, DRIVER_AVAILABLE_AGAIN };

struct Event {
    long long time;
    EventType type;
    int entity_id; // Rider ID or Driver ID
    double x, y; // Destination for drivers
    int retry_count; // Tracks how many times this request failed
    
    // Priority Queue needs this to sort by Time (Ascending)
    bool operator>(const Event& other) const {
        return time > other.time;
    }
};

int main(int argc, char* argv[]) {
    //SETUP LOGGING
    std::ofstream log_file("matches.log");

    // STATS COUNTERS
    int total_riders = 0;
    int total_matches = 0;
    int total_failed = 0; // New stat for final report

    MatchingEngine engine(1.0, 500.0); // Increased radius for testing
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> event_queue;
    
    // 1. LOAD EVENTS FROM FILE
    std::string input_path = (argc > 1) ? argv[1] : "../samples/events.csv";

    std::ifstream file(input_path); 

    if (!file.is_open()) file.open("samples/events.csv"); // Fallback
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string seg;
        std::vector<std::string> parts;

        while(std::getline(ss, seg, ',')) parts.push_back(seg);

        if (parts.empty()) continue; // Safety check for empty parts

        if (parts[0] == "driver") {
            // Safety Check: Drivers need 5 columns (type, id, x, y, rating)
            if (parts.size() < 5) {
                std::cerr << "Warning: Skipping malformed driver line: " << line << "\n";
                continue;
            }

            // Drivers init instantly
            engine.add_driver(Driver(std::stoi(parts[1]), std::stod(parts[2]), std::stod(parts[3]), std::stod(parts[4])));

        } else if (parts[0] == "rider") {
            // Safety Check: Riders need 7 columns (type, id, px, py, dx, dy, time)
            if (parts.size() < 7) {
                std::cerr << "Warning: Skipping malformed rider line: " << line << "\n";
                continue;
            }

            // Create Rider Object
            Rider r(std::stoi(parts[1]), std::stod(parts[2]), std::stod(parts[3]), 
                    std::stod(parts[4]), std::stod(parts[5]), std::stoll(parts[6]));
            
            engine.new_rider_request(r);
            // Push with retry_count = 0
            event_queue.push({r.ts, RIDER_REQUEST, r.id, 0, 0, 0});
        }
    }

    // 2. RUN SIMULATION LOOP
    long long current_time = 0;
    log(log_file, "--- Simulation Started ---\n");

    while (!event_queue.empty()) {
        Event e = event_queue.top();
        event_queue.pop();

        if (e.time > current_time) {
            std::this_thread::sleep_for(std::chrono::milliseconds(e.time - current_time));
            current_time = e.time;
        }

        if (e.type == RIDER_REQUEST) {
            // Only count as "Total Rider" if it's the first attempt (retry_count == 0)
            if (e.retry_count == 0) total_riders++;
            
            // Build string for log
            std::stringstream ss;
            ss << "[Time " << current_time << "] Processing Rider " << e.entity_id;
            if (e.retry_count > 0) ss << " (Retry #" << e.retry_count << ")";
            ss << " ... ";
            log(log_file, ss.str());

            MatchResult result = engine.attempt_match_for_rider(e.entity_id);

            // --- Schedule driver to appear at REAL Dropoff (dx, dy) ---
            if (result.driver_id != -1) {
                total_matches++;
                // --- 1. Calculate Pickup Physics (Manhattan) ---
                double pickup_dist = std::abs(result.driver_start_x - result.pickup_x) + 
                                     std::abs(result.driver_start_y - result.pickup_y);
                long long pickup_time = static_cast<long long>(pickup_dist / VEHICLE_SPEED);
                // --- 2. Calculate Trip Physics (Manhattan) ---
                double trip_dist = std::abs(result.dropoff_x - result.pickup_x) + 
                                   std::abs(result.dropoff_y - result.pickup_y);
                long long trip_time = static_cast<long long>(trip_dist / VEHICLE_SPEED);
                // Total Busy Time
                long long total_busy_time = pickup_time + trip_time;
                if (total_busy_time < 1000) total_busy_time = 1000; // Min threshold
                // Schedule Driver Free Event
                event_queue.push({
                    current_time + total_busy_time, 
                    DRIVER_AVAILABLE_AGAIN, 
                    result.driver_id, 
                    result.dropoff_x, // Use the x returned by engine
                    result.dropoff_y,  // Use the y returned by engine
                    0 // Retry count irrelevant for drivers
                });
                
                // IMPORTANT: Log the MATCH line so Python can find it
                // We construct the string manually since we are outside the engine's print logic now
                // (Note: The engine still prints to cout inside attempt_match, 
                // but we need it in the FILE too. Let's rely on Main to log the success).
                
                std::stringstream match_msg;
                match_msg << "MATCHED: Rider " << e.entity_id << " with Driver " << result.driver_id << "\n";
                log(log_file, match_msg.str());
                
                // --- LOGGING THE PICKUP/TRIP BREAKDOWN ---
                std::stringstream trip_msg;
                trip_msg << "   -> Pickup: " << std::fixed << std::setprecision(1) << pickup_dist << " units (" << pickup_time << "ms). "
                         << "Trip: " << trip_dist << " units (" << trip_time << "ms).\n";
                log(log_file, trip_msg.str());
            } else {
                // --- RETRY LOGIC ---
                if (e.retry_count < MAX_RETRIES) {
                    log(log_file, "NO MATCH. Retrying in 5000ms...\n");
                    // Push event back into queue
                    event_queue.push({
                        current_time + 5000, 
                        RIDER_REQUEST, 
                        e.entity_id, 
                        0, 0, 
                        e.retry_count + 1
                    });
                } else {
                    total_failed++;
                    log(log_file, "NO MATCH. Max retries reached. Request Cancelled.\n");
                    engine.cancel_rider(e.entity_id); // Clean up from engine memory
                }
            }

        } else if (e.type == DRIVER_AVAILABLE_AGAIN) {
            std::stringstream ss;
            ss << "[Time " << current_time << "] Driver " << e.entity_id << " is free again at (" << e.x << ", " << e.y << ")\n";
            log(log_file, ss.str());
            
            engine.driver_available(e.entity_id);
            engine.update_driver_location(e.entity_id, e.x, e.y); 
        }
    }

    // 3. FINAL REPORT CARD
    std::stringstream report;
    report << "\n========================================\n";
    report << "       SIMULATION REPORT CARD           \n";
    report << "========================================\n";
    report << " Total Requests  : " << total_riders << "\n";
    report << " Total Matches   : " << total_matches << "\n";
    report << " Failed/Expired  : " << total_failed << "\n";
    
    double match_rate = (total_riders > 0) ? (double(total_matches) / total_riders) * 100.0 : 0.0;
    report << " Match Rate      : " << std::fixed << std::setprecision(1) << match_rate << "%\n";
    report << "========================================\n";
    // Now 'report' actually contains text, so this will write to BOTH console and file
    log(log_file, report.str());
    return 0;
}