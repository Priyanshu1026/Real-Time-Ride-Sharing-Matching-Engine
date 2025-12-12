#pragma once
#include <queue>
#include <vector>


// example comparator for drivers by distance then score
template<typename T, typename Comp>
class SimplePQ {
public:
void push(const T &t) { pq.push(t); }
bool empty() const { return pq.empty(); }
T top() const { return pq.top(); }
void pop() { pq.pop(); }
size_t size() const { return pq.size(); }
private:
std::priority_queue<T, std::vector<T>, Comp> pq;
};