#pragma once
#include <cstdint>

struct Driver {
    using id_t = int;

    id_t id;
    double x; 
    double y; 
    double rating;
    bool available;

    // This SINGLE constructor handles everything:
    // - If you provide arguments: uses them.
    // - If you provide NOTHING: uses defaults (act as a default constructor).
    Driver(id_t id = 0, double x = 0.0, double y = 0.0, double rating = 5.0, bool available = true)
        : id(id), x(x), y(y), rating(rating), available(available) {}
    
    // REMOVED: Driver() = default;  <-- This caused the ambiguity!
};