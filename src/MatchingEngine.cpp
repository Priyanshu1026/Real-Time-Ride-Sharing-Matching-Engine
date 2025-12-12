#include "MatchingEngine.hpp"
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>

// Constructor
MatchingEngine::MatchingEngine(double cell_size, double max_radius)
    : cell_size(cell_size), max_radius(max_radius) {
}

// ----------------------
// Driver Management
// ----------------------

void MatchingEngine::add_driver(const Driver &d) {
    std::lock_guard<std::mutex> lock(mtx);
    drivers[d.id] = d;
    long long key = cell_key(d.x, d.y);
    grid[key].drivers.push_back(d.id);
}

void MatchingEngine::update_driver_location(Driver::id_t id, double x, double y) {
    std::lock_guard<std::mutex> lock(mtx);
    if (drivers.find(id) == drivers.end()) return;

    Driver &d = drivers[id];
    long long old_key = cell_key(d.x, d.y);
    long long new_key = cell_key(x, y);

    d.x = x;
    d.y = y;

    if (old_key != new_key) {
        auto &old_vec = grid[old_key].drivers;
        old_vec.erase(std::remove(old_vec.begin(), old_vec.end(), id), old_vec.end());
        grid[new_key].drivers.push_back(id);
    }
}

void MatchingEngine::driver_available(Driver::id_t id) {
    std::lock_guard<std::mutex> lock(mtx);
    if (drivers.find(id) != drivers.end()) {
        drivers[id].available = true;
    }
}

void MatchingEngine::driver_unavailable(Driver::id_t id) {
    std::lock_guard<std::mutex> lock(mtx);
    if (drivers.find(id) != drivers.end()) {
        drivers[id].available = false;
    }
}

// ----------------------
// Rider Management
// ----------------------

void MatchingEngine::new_rider_request(const Rider &r) {
    std::lock_guard<std::mutex> lock(mtx);
    riders[r.id] = r;
}

void MatchingEngine::cancel_rider(Rider::id_t id) {
    std::lock_guard<std::mutex> lock(mtx);
    riders.erase(id);
}

// --- UPDATED: Manhattan Distance & New Struct ---
MatchResult MatchingEngine::attempt_match_for_rider(Rider::id_t rider_id) {
    std::lock_guard<std::mutex> lock(mtx);

    // Initializer list format: {id, d_sx, d_sy, px, py, dx, dy}
    if (riders.find(rider_id) == riders.end()) return {-1, 0, 0, 0, 0, 0, 0};
    Rider &r = riders[rider_id];

    auto nearby_driver_ids = search_nearby(r.px, r.py, max_radius);

    double best_score = std::numeric_limits<double>::max();
    int best_driver_id = -1;
    double matched_dist = 0.0;

    for (auto did : nearby_driver_ids) {
        Driver &d = drivers[did];
        if (!d.available) continue;

        // MANHATTAN DISTANCE: |x1-x2| + |y1-y2|
        double dist = std::abs(d.x - r.px) + std::abs(d.y - r.py);
        
        double score = dist - (d.rating * 2.0); 

        if (score < best_score) {
            best_score = score;
            best_driver_id = did;
            matched_dist = dist;
        }
    }

    if (best_driver_id != -1) {
        drivers[best_driver_id].available = false;
        
        // Capture all coordinates
        double d_sx = drivers[best_driver_id].x;
        double d_sy = drivers[best_driver_id].y;
        double px = r.px, py = r.py;
        double dx = r.dx, dy = r.dy;
        
        riders.erase(rider_id);
        
        std::cout << "MATCHED: Rider " << rider_id << " with Driver " << best_driver_id 
                  << " (Dist: " << matched_dist << ", Rating: " << drivers[best_driver_id].rating << ")\n";
        
        return {best_driver_id, d_sx, d_sy, px, py, dx, dy};
    } else {
        std::cout << "NO MATCH found for Rider " << rider_id << "\n";
        return {-1, 0, 0, 0, 0, 0, 0};
    }
}

long long MatchingEngine::cell_key(double x, double y) const {
    long long cx = static_cast<long long>(std::floor(x / cell_size));
    long long cy = static_cast<long long>(std::floor(y / cell_size));
    return (cx * 1000000) + cy; 
}

std::vector<Driver::id_t> MatchingEngine::search_nearby(double x, double y, double radius) {
    std::vector<Driver::id_t> candidates;
    long long min_cx = static_cast<long long>(std::floor((x - radius) / cell_size));
    long long max_cx = static_cast<long long>(std::floor((x + radius) / cell_size));
    long long min_cy = static_cast<long long>(std::floor((y - radius) / cell_size));
    long long max_cy = static_cast<long long>(std::floor((y + radius) / cell_size));

    for (long long cx = min_cx; cx <= max_cx; ++cx) {
        for (long long cy = min_cy; cy <= max_cy; ++cy) {
            long long key = (cx * 1000000) + cy;
            if (grid.find(key) != grid.end()) {
                const auto &cell_drivers = grid[key].drivers;
                candidates.insert(candidates.end(), cell_drivers.begin(), cell_drivers.end());
            }
        }
    }
    return candidates;
}