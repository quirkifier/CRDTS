#ifndef orset_h
#define orset_h

#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <utility>

class ORSet {
private:
    std::string nodeId;
    int counter;

    std::set<std::pair<std::string, std::string>> added;
    std::set<std::pair<std::string, std::string>> removed;

    void processIncomingTag(const std::string& tag) {
        size_t colonPos = tag.find(":");

        if (colonPos == std::string::npos) {
            return;
        }

        std::string id = tag.substr(0, colonPos);
        std::string numberPart = tag.substr(colonPos + 1);

        try {
            int num = std::stoi(numberPart);

            if (id == nodeId) {
                if (counter < num) {
                    counter = num;
                }
            }
        }
        catch (...) {
            return;
        }
    }

public:
    ORSet(const std::string& id = "") {
        nodeId = id;
        counter = 0;
    }

    void add(const std::string& element) {
        if (element.empty()) {
            return;
        }

        counter++;

        std::string tag = nodeId + ":" + std::to_string(counter);

        added.insert(std::make_pair(element, tag));
    }

    void remove(const std::string& element) {
        if (element.empty()) {
            return;
        }

        for (std::set<std::pair<std::string, std::string>>::const_iterator it = added.begin(); it != added.end(); ++it) {
            if (it->first == element) {
                removed.insert(*it);
            }
        }
    }

    std::set<std::string> query() const {
        std::set<std::string> result;

        for (std::set<std::pair<std::string, std::string>>::const_iterator it = added.begin(); it != added.end(); ++it) {
            if (removed.find(*it) == removed.end()) {
                result.insert(it->first);
            }
        }

        return result;
    }

    void merge(const ORSet& other) {
        for (std::set<std::pair<std::string, std::string>>::const_iterator it = other.added.begin(); it != other.added.end(); ++it) {
            added.insert(*it);
            processIncomingTag(it->second);
        }

        for (std::set<std::pair<std::string, std::string>>::const_iterator it = other.removed.begin(); it != other.removed.end(); ++it) {
            removed.insert(*it);
        }
    }

    std::string serialize() const {
        std::stringstream ss;

        ss << nodeId << "\n";
        ss << counter << "\n";

        ss << added.size() << "\n";

        for (std::set<std::pair<std::string, std::string>>::const_iterator it = added.begin(); it != added.end(); ++it) {
            ss << it->first << "|" << it->second << "\n";
        }

        ss << removed.size() << "\n";

        for (std::set<std::pair<std::string, std::string>>::const_iterator it = removed.begin(); it != removed.end(); ++it) {
            ss << it->first << "|" << it->second << "\n";
        }

        return ss.str();
    }

    static ORSet deserialize(const std::string& data) {
        std::stringstream ss(data);

        std::string nodeIdLine;
        std::string counterLine;
        std::string sizeLine;

        if (!std::getline(ss, nodeIdLine)) {
            return ORSet("");
        }

        ORSet obj(nodeIdLine);

        if (std::getline(ss, counterLine)) {
            try {
                obj.counter = std::stoi(counterLine);
            }
            catch (...) {
                obj.counter = 0;
            }
        }

        if (std::getline(ss, sizeLine)) {
            int addedSize = 0;

            try {
                addedSize = std::stoi(sizeLine);
            }
            catch (...) {
                addedSize = 0;
            }

            for (int i = 0; i < addedSize; i++) {
                std::string line;

                if (!std::getline(ss, line)) {
                    break;
                }

                size_t sep = line.find("|");

                if (sep == std::string::npos) {
                    continue;
                }

                std::string element = line.substr(0, sep);
                std::string tag = line.substr(sep + 1);

                obj.added.insert(std::make_pair(element, tag));
                obj.processIncomingTag(tag);
            }
        }

        if (std::getline(ss, sizeLine)) {
            int removedSize = 0;

            try {
                removedSize = std::stoi(sizeLine);
            }
            catch (...) {
                removedSize = 0;
            }

            for (int i = 0; i < removedSize; i++) {
                std::string line;

                if (!std::getline(ss, line)) {
                    break;
                }

                size_t sep = line.find("|");

                if (sep == std::string::npos) {
                    continue;
                }

                std::string element = line.substr(0, sep);
                std::string tag = line.substr(sep + 1);

                obj.removed.insert(std::make_pair(element, tag));
            }
        }

        return obj;
    }

    void inspect() const {
        std::set<std::string> result = query();

        std::cout << "ORSet Node: " << nodeId << " { ";

        for (std::set<std::string>::const_iterator it = result.begin(); it != result.end(); ++it) {
            std::cout << *it << " ";
        }

        std::cout << "}\n";
    }
};

#endif