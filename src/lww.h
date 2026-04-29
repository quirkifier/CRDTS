#ifndef lww_h
#define lww_h

#include <chrono>
#include <iostream>
#include <string>
#include <sstream>

class LWWRegister {
private:
    std::string value;
    long long timestamp; // wall_clock_ms
    std::string node_id;

public:
    LWWRegister(const std::string& id = "") : value(""), timestamp(0), node_id(id) {}

    static long long get_current_time_ms() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    void set(const std::string& new_value) {
        long long new_time = get_current_time_ms();
        // Locally, we always update the value and timestamp. 
        // We ensure the timestamp is monotonic.
        if (new_time <= timestamp) {
            new_time = timestamp + 1;
        }
        value = new_value;
        timestamp = new_time;
    }

    // This is the version for merging where we have the other's node_id
    void merge(const LWWRegister& other) {
        if (other.timestamp > timestamp ||
            (other.timestamp == timestamp && other.node_id > node_id)) {
            value = other.value;
            timestamp = other.timestamp;
            node_id = other.node_id;
        }
    }

    std::string query() const { return value; }

    std::string serialize() const {
        std::ostringstream oss;
        oss << node_id << "|" << timestamp << "|" << value;
        return oss.str();
    }

    static LWWRegister deserialize(const std::string& data) {
        std::stringstream ss(data);
        std::string node_id, ts_str, value;
        
        std::getline(ss, node_id, '|');
        std::getline(ss, ts_str, '|');
        std::getline(ss, value);

        LWWRegister res(node_id);
        res.timestamp = ts_str.empty() ? 0 : std::stoll(ts_str);
        res.value = value;
        return res;
    }

    void inspect() const {
        std::cout << "LWWRegister Node: " << node_id 
                  << " Value: " << value 
                  << " Timestamp: " << timestamp << std::endl;
    }
};

#endif
