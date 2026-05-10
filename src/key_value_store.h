#ifndef KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_H

#include <algorithm>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

#include "crdt_value.h"

class KeyValueStore {
private:
    std::string nodeId;
    std::unordered_map<std::string, CRDTValue> keyspace;
    mutable std::mutex keyspaceMutex;

    std::string setToString(const std::set<std::string>& values) const {
        if (values.empty()) {
            return "(empty)";
        }

        std::ostringstream oss;
        bool first = true;

        for (const auto& value : values) {
            if (!first) {
                oss << " ";
            }

            oss << value;
            first = false;
        }

        return oss.str();
    }

public:
    KeyValueStore(const std::string& id) : nodeId(id) {}

    std::string createKey(const std::string& key, const std::string& typeText) {
        std::lock_guard<std::mutex> lock(keyspaceMutex);

        if (key.empty()) {
            return "ERROR key cannot be empty";
        }

        if (keyspace.find(key) != keyspace.end()) {
            return "ERROR key already exists";
        }

        CRDTType type;

        if (!stringToCRDTType(typeText, type)) {
            return "ERROR invalid CRDT type";
        }

        keyspace.emplace(key, CRDTValue(nodeId, type));
        return "OK";
    }

    std::string incrementKey(const std::string& key, int amount) {
        std::lock_guard<std::mutex> lock(keyspaceMutex);

        auto it = keyspace.find(key);

        if (it == keyspace.end()) {
            return "ERROR key not found";
        }

        CRDTValue& value = it->second;

        if (value.type == CRDTType::GCOUNTER) {
            if (amount < 0) {
                return "ERROR GCOUNTER cannot be incremented by negative value";
            }

            value.gcounter.increment(amount);

            return "OK (" + key + " = " + std::to_string(value.gcounter.query()) + ")";
        }

        if (value.type == CRDTType::PNCOUNTER) {
            value.pncounter.increment(amount);

            return "OK (" + key + " = " + std::to_string(value.pncounter.query()) + ")";
        }

        return "ERROR INC only works on GCOUNTER or PNCOUNTER";
    }

    std::string setKey(const std::string& key, const std::string& newValue) {
        std::lock_guard<std::mutex> lock(keyspaceMutex);

        auto it = keyspace.find(key);

        if (it == keyspace.end()) {
            return "ERROR key not found";
        }

        CRDTValue& value = it->second;

        if (value.type != CRDTType::LWWREG) {
            return "ERROR SET only works on LWWREG";
        }

        value.lwwreg.set(newValue);
        return "OK";
    }

    std::string addElement(const std::string& key, const std::string& element) {
        std::lock_guard<std::mutex> lock(keyspaceMutex);

        auto it = keyspace.find(key);

        if (it == keyspace.end()) {
            return "ERROR key not found";
        }

        CRDTValue& value = it->second;

        if (value.type != CRDTType::ORSET) {
            return "ERROR ADD only works on ORSET";
        }

        value.orset.add(element);
        return "OK";
    }

    std::string removeElement(const std::string& key, const std::string& element) {
        std::lock_guard<std::mutex> lock(keyspaceMutex);

        auto it = keyspace.find(key);

        if (it == keyspace.end()) {
            return "ERROR key not found";
        }

        CRDTValue& value = it->second;

        if (value.type != CRDTType::ORSET) {
            return "ERROR REMOVE only works on ORSET";
        }

        value.orset.remove(element);
        return "OK";
    }

    std::string getKey(const std::string& key) const {
        std::lock_guard<std::mutex> lock(keyspaceMutex);

        auto it = keyspace.find(key);

        if (it == keyspace.end()) {
            return "ERROR key not found";
        }

        const CRDTValue& value = it->second;

        if (value.type == CRDTType::GCOUNTER) {
            return std::to_string(value.gcounter.query());
        }

        if (value.type == CRDTType::PNCOUNTER) {
            return std::to_string(value.pncounter.query());
        }

        if (value.type == CRDTType::LWWREG) {
            return value.lwwreg.query();
        }

        if (value.type == CRDTType::ORSET) {
            return setToString(value.orset.query());
        }

        return "ERROR unknown CRDT type";
    }

    std::string listKeys() const {
        std::lock_guard<std::mutex> lock(keyspaceMutex);

        if (keyspace.empty()) {
            return "(empty)";
        }

        std::ostringstream oss;

        for (const auto& item : keyspace) {
            const std::string& key = item.first;
            const CRDTValue& value = item.second;

            oss << key << " " << crdtTypeToString(value.type) << " ";

            if (value.type == CRDTType::GCOUNTER) {
                oss << value.gcounter.query();
            }
            else if (value.type == CRDTType::PNCOUNTER) {
                oss << value.pncounter.query();
            }
            else if (value.type == CRDTType::LWWREG) {
                oss << value.lwwreg.query();
            }
            else if (value.type == CRDTType::ORSET) {
                oss << setToString(value.orset.query());
            }
            else {
                oss << "UNKNOWN";
            }

            oss << "\n";
        }

        std::string result = oss.str();

        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }

        return result;
    }

    std::string peers() const {
        return "no peers";
    }

    std::string sync() const {
        return "OK no-op";
    }
};

#endif