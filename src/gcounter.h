#ifndef gcounter_h
#define gcounter_h

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>

class GCounter {
public:
    std::string nodeID;
    std::unordered_map<std::string, int> state;

    GCounter(const std::string& node_id) : nodeID(node_id) {}

    void increment() {
        state[nodeID]++;
    }

    void increment(int n) {
        if (n > 0) {
            state[nodeID] += n;
        }
    }

    int query() const {
        int total = 0;

        for (auto it = state.begin(); it != state.end(); ++it) {
            total += it->second;
        }

        return total;
    }

    void merge(const GCounter& other) {
        for (auto it = other.state.begin(); it != other.state.end(); ++it) {
            std::string id = it->first;
            int count = it->second;

            if (state.find(id) == state.end()) {
                state[id] = count;
            }
            else {
                if (state[id] < count) {
                    state[id] = count;
                }
            }
        }
    }

    std::string serialize() const {
        std::ostringstream oss;
        oss << nodeID << "|";

        bool first = true;

        for (auto it = state.begin(); it != state.end(); ++it) {
            if (!first) {
                oss << ",";
            }

            oss << it->first << ":" << it->second;
            first = false;
        }

        return oss.str();
    }

    static GCounter deserialize(const std::string& data) {
        size_t pipe = data.find('|');

        if (pipe == std::string::npos) {
            return GCounter(data);
        }

        std::string nodeId = data.substr(0, pipe);
        GCounter res(nodeId);

        std::string stateData = data.substr(pipe + 1);

        if (stateData.empty()) {
            return res;
        }

        std::stringstream ss(stateData);
        std::string entry;

        while (std::getline(ss, entry, ',')) {
            size_t colon = entry.find(':');

            if (colon == std::string::npos) {
                continue;
            }

            std::string id = entry.substr(0, colon);
            int count = std::stoi(entry.substr(colon + 1));

            res.state[id] = count;
        }

        return res;
    }

    void inspect() const {
        std::cout << "Node: " << nodeID << " State: { ";

        for (auto it = state.begin(); it != state.end(); ++it) {
            std::cout << it->first << ":" << it->second << " ";
        }

        std::cout << "} Value: " << query() << "\n";
    }
};

class PNCounter {
public:
    GCounter P;
    GCounter N;
    std::string nodeID;

    PNCounter(const std::string& nodeId) : P(nodeId), N(nodeId), nodeID(nodeId) {}

    void increment() {
        P.increment();
    }

    void increment(int n) {
        if (n >= 0) {
            P.increment(n);
        }
        else {
            N.increment(-n);
        }
    }

    void decrement() {
        N.increment();
    }

    void decrement(int n) {
        if (n > 0) {
            N.increment(n);
        }
    }

    int query() const {
        return P.query() - N.query();
    }

    void merge(const PNCounter& other) {
        P.merge(other.P);
        N.merge(other.N);
    }

    std::string serialize() const {
        return P.serialize() + "\n" + N.serialize();
    }

    static PNCounter deserialize(const std::string& data) {
        std::stringstream ss(data);

        std::string p_line;
        std::string n_line;

        std::getline(ss, p_line);
        std::getline(ss, n_line);

        GCounter p_gc = GCounter::deserialize(p_line);
        GCounter n_gc = GCounter::deserialize(n_line);

        PNCounter res(p_gc.nodeID);
        res.P = p_gc;
        res.N = n_gc;

        return res;
    }

    void inspect() const {
        std::cout << "PNCounter Node: " << nodeID << " Value: " << query() << "\n";

        std::cout << "  P: ";
        P.inspect();

        std::cout << "  N: ";
        N.inspect();
    }
};

#endif