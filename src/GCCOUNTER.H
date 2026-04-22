#ifndef gc_h
#define gc_h
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
class GC_Counter {
private:
  std::string nodeID;
  std::unordered_map<std::string, int> state;

public:
  GC_Counter(const std::string &node_id) : nodeID(node_id) {}
  void increment() { state[nodeID]++; }
  int sum_all() const {
    int total = 0;
    for (const auto &[id, count] : state) {
      total += count;
    }
    return total;
  }
  void merge(const GC_Counter &other) {
    for (auto &[id, count] : other.state) {
      state[id] = std::max(state[id], count);
    }
  }
  const std::unordered_map<std::string, int> &get_state() const {
    return state;
  }
  void inspect() const {
    std::cout << "Node: " << nodeID << "\n";
    std::cout << "State: { ";
    for (const auto &[id, count] : state) {
      std::cout << id << ": " << count << " ";
    }
    std::cout << "}\n";
    std::cout << "sum of all: " << sum_all() << "\n";
  }
};

#endif
