#pragma once
struct Rider {
    using id_t = int;
    
    id_t id;
    double px, py; // Pickup
    double dx, dy; // Destination
    long long ts;  // Timestamp

    Rider(id_t id=0, double px=0, double py=0, double dx=0, double dy=0, long long ts=0) 
        : id(id), px(px), py(py), dx(dx), dy(dy), ts(ts) {}
};