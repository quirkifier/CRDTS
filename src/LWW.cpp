#pragma once
#include <chrono>
#include <iostream>
#include <string>

using namespace std;

class LWW_Register {
private:
  string value;
  long long timestamp;
  string node_id;

public:
  LWW_Register() {
    value = "";
    timestamp = 0;
    node_id = "";
  }

  long long get_time_ms() {
    return chrono::duration_cast<chrono::milliseconds>(
               chrono::system_clock::now().time_since_epoch())
        .count();
  }

  void set(string new_value, string id) {
    long long new_time = get_time_ms();

    if (new_time > timestamp || (new_time == timestamp && id > node_id)) {
      value = new_value;
      timestamp = new_time;
      node_id = id;
    }
  }

  void merge(LWW_Register other) {
    if (other.timestamp > timestamp ||
        (other.timestamp == timestamp && other.node_id > node_id)) {
      value = other.value;
      timestamp = other.timestamp;
      node_id = other.node_id;
    }
  }
  string get() { return value; }

  void print() {
    cout << "Value: " << value << ", Time: " << timestamp
         << ", Node: " << node_id << endl;
  }
};
