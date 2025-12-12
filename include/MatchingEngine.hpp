#pragma once
#include "Driver.hpp"
#include "Rider.hpp"
#include <unordered_map>
#include <vector>
#include <mutex>

struct MatchResult {
    int driver_id;
    // Driver's location at moment of match
    double driver_start_x;
    double driver_start_y;
    // Trip info
    double pickup_x;
    double pickup_y;
    double dropoff_x;
    double dropoff_y;
};

class MatchingEngine {
public:
    MatchingEngine(double cell_size=1.0, double max_radius=10.0);
    
    void add_driver(const Driver &d);
    void update_driver_location(Driver::id_t id, double x, double y);
    void driver_available(Driver::id_t id);
    void driver_unavailable(Driver::id_t id);
    void new_rider_request(const Rider &r);
    void cancel_rider(Rider::id_t id);

    MatchResult attempt_match_for_rider(Rider::id_t id);

private:
    struct Cell { std::vector<Driver::id_t> drivers; };
    std::unordered_map<long long, Cell> grid;
    std::unordered_map<Driver::id_t, Driver> drivers;
    std::unordered_map<Rider::id_t, Rider> riders;
    std::mutex mtx; 
    double cell_size;
    double max_radius;
    
    long long cell_key(double x, double y) const;
    std::vector<Driver::id_t> search_nearby(double x, double y, double radius);
};