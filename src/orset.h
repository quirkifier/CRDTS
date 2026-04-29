#ifndef orset_h
#define orset_h

#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
#include <utility>

class ORSet {
private:
    std::string nodeId;
    int counter;
    std::set<std::pair<std::string, std::string>> added;
    std::set<std::pair<std::string, std::string>> removed;

    void processIncomingTag(const std::string& tag) {
        size_t colonPos = tag.find(":");
        if (colonPos != std::string::npos) {
            std::string id = tag.substr(0, colonPos);
            try {
                int num = std::stoi(tag.substr(colonPos + 1));
                if (id == nodeId) {
                    counter = std::max(counter, num);
                }
            } catch (...) {}
        }
    }

public:
    ORSet(const std::string& id = "") : nodeId(id), counter(0) {}

    void add(const std::string& element) {
        counter++;
        std::string tag = nodeId + ":" + std::to_string(counter);
        added.insert({element, tag});
    }

    void remove(const std::string& element) {
        for (const auto& item : added) {
            if (item.first == element) {
                removed.insert(item);
            }
        }
    }

    std::set<std::string> query() const {
        std::set<std::string> result;
        for (const auto& item : added) {
            if (removed.find(item) == removed.end()) {
                result.insert(item.first);
            }
        }
        return result;
    }

    void merge(const ORSet& other) {
        for (const auto& item : other.added) {
            added.insert(item);
            processIncomingTag(item.second);
        }
        for (const auto& item : other.removed) {
            removed.insert(item);
        }
    }

    std::string serialize() const {
        std::stringstream ss;
        ss << nodeId << "\n";
        ss << counter << "\n";
        ss << added.size() << "\n";
        for (const auto& item : added) {
            ss << item.first << "|" << item.second << "\n";
        }
        ss << removed.size() << "\n";
        for (const auto& item : removed) {
            ss << item.first << "|" << item.second << "\n";
        }
        return ss.str();
    }

    static ORSet deserialize(const std::string& data) {
        std::stringstream ss(data);
        std::string nodeId_line, counter_line, size_line;
        
        if (!std::getline(ss, nodeId_line)) return ORSet("");
        if (!std::getline(ss, counter_line)) return ORSet(nodeId_line);
        
        ORSet obj(nodeId_line);
        try {
            obj.counter = std::stoi(counter_line);
        } catch (...) { obj.counter = 0; }

        if (std::getline(ss, size_line)) {
            int addedSize = 0;
            try { addedSize = std::stoi(size_line); } catch (...) {}
            for (int i = 0; i < addedSize; i++) {
                std::string line;
                if (!std::getline(ss, line)) break;
                size_t sep = line.find("|");
                if (sep != std::string::npos) {
                    std::string element = line.substr(0, sep);
                    std::string tag = line.substr(sep + 1);
                    obj.added.insert({element, tag});
                    obj.processIncomingTag(tag);
                }
            }
        }

        if (std::getline(ss, size_line)) {
            int removedSize = 0;
            try { removedSize = std::stoi(size_line); } catch (...) {}
            for (int i = 0; i < removedSize; i++) {
                std::string line;
                if (!std::getline(ss, line)) break;
                size_t sep = line.find("|");
                if (sep != std::string::npos) {
                    std::string element = line.substr(0, sep);
                    std::string tag = line.substr(sep + 1);
                    obj.removed.insert({element, tag});
                }
            }
        }
        return obj;
    }

    void inspect() const {
        std::set<std::string> res = query();
        std::cout << "ORSet Node: " << nodeId << " { ";
        for (const auto& e : res) std::cout << e << " ";
        std::cout << "}\n";
    }
};

#endif
