#ifndef gcounter_h
#define gcounter_h

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

class GCounter {
public:
    std::string nodeID;
    std::unordered_map<std::string, int> state;

    GCounter(const std::string &node_id) : nodeID(node_id) {}

    void increment() { state[nodeID]++; }

    int query() const {
        int total = 0;
        for (const auto &[id, count] : state) {
            total += count;
        }
        return total;
    }

    void merge(const GCounter &other) {
        for (const auto &[id, count] : other.state) {
            state[id] = std::max(state[id], count);
        }
    }

    std::string serialize() const {
        std::ostringstream oss;
        oss << nodeID << "|";
        bool first = true;
        for (const auto &[id, count] : state) {
            if (!first)
                oss << ",";
            oss << id << ":" << count;
            first = false;
        }
        return oss.str();
    }

    static GCounter deserialize(const std::string &data) {
        size_t pipe = data.find('|');
        if (pipe == std::string::npos) {
            // Fallback for old format or empty state
            return GCounter(data); 
        }

        std::string nodeId = data.substr(0, pipe);
        GCounter res(nodeId);
        std::string stateData = data.substr(pipe + 1);
        if (stateData.empty()) return res;

        std::stringstream ss(stateData);
        std::string entry;
        while (std::getline(ss, entry, ',')) {
            size_t colon = entry.find(':');
            if (colon == std::string::npos)
                continue;

            std::string id = entry.substr(0, colon);
            int count = std::stoi(entry.substr(colon + 1));
            res.state[id] = count;
        }
        return res;
    }

    void inspect() const {
        std::cout << "Node: " << nodeID << " State: { ";
        for (const auto &[id, count] : state) {
            std::cout << id << ":" << count << " ";
        }
        std::cout << "} Value: " << query() << "\n";
    }
};

class PNCounter {
public:
    GCounter P;
    GCounter N;
    std::string nodeID;

    PNCounter(const std::string &nodeId) : P(nodeId), N(nodeId), nodeID(nodeId) {}

    void increment() { P.increment(); }
    void decrement() { N.increment(); }

    int query() const { return P.query() - N.query(); }

    void merge(const PNCounter &other) {
        P.merge(other.P);
        N.merge(other.N);
    }

    std::string serialize() const {
        return P.serialize() + "\n" + N.serialize();
    }

    static PNCounter deserialize(const std::string &data) {
        std::stringstream ss(data);
        std::string p_line, n_line;
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
        std::cout << "  P: "; P.inspect();
        std::cout << "  N: "; N.inspect();
    }
};

#endif
