#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

#include "key_value_store.h"

class CommandHandler {
private:
    KeyValueStore& store;

    std::string trim(const std::string& text) const {
        size_t start = text.find_first_not_of(" \t\r\n\0");

        if (start == std::string::npos) {
            return "";
        }

        size_t end = text.find_last_not_of(" \t\r\n\0");

        return text.substr(start, end - start + 1);
    }

    std::string toUpper(std::string text) const {
        for (size_t i = 0; i < text.size(); i++) {
            text[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(text[i])));
        }

        return text;
    }

public:
    CommandHandler(KeyValueStore& kvStore) : store(kvStore) {}

    std::string handleCommand(const std::string& inputLine, bool& shouldQuit) {
        shouldQuit = false;

        std::string line = trim(inputLine);

        std::stringstream ss(line);
        std::string command;

        ss >> command;
        command = toUpper(trim(command));

        if (command.empty()) {
            return "ERROR empty command";
        }

        if (command == "CREATE") {
            std::string key;
            std::string type;

            ss >> key >> type;

            key = trim(key);
            type = toUpper(trim(type));

            if (key.empty() || type.empty()) {
                return "ERROR usage: CREATE key type";
            }

            return store.createKey(key, type);
        }

        if (command == "INC") {
            std::string key;
            int amount;

            ss >> key >> amount;

            key = trim(key);

            if (key.empty() || ss.fail()) {
                return "ERROR usage: INC key n";
            }

            return store.incrementKey(key, amount);
        }

        if (command == "SET") {
            std::string key;
            std::string value;

            ss >> key;
            std::getline(ss, value);

            key = trim(key);
            value = trim(value);

            if (key.empty() || value.empty()) {
                return "ERROR usage: SET key value";
            }

            return store.setKey(key, value);
        }

        if (command == "ADD") {
            std::string key;
            std::string element;

            ss >> key;
            std::getline(ss, element);

            key = trim(key);
            element = trim(element);

            if (key.empty() || element.empty()) {
                return "ERROR usage: ADD key elem";
            }

            return store.addElement(key, element);
        }

        if (command == "REMOVE") {
            std::string key;
            std::string element;

            ss >> key;
            std::getline(ss, element);

            key = trim(key);
            element = trim(element);

            if (key.empty() || element.empty()) {
                return "ERROR usage: REMOVE key elem";
            }

            return store.removeElement(key, element);
        }

        if (command == "GET") {
            std::string key;

            ss >> key;
            key = trim(key);

            if (key.empty()) {
                return "ERROR usage: GET key";
            }

            return store.getKey(key);
        }

        if (command == "\\LIST") {
            return store.listKeys();
        }

        if (command == "\\PEERS") {
            return store.peers();
        }

        if (command == "\\SYNC") {
            return store.sync();
        }

        if (command == "QUIT") {
            shouldQuit = true;
            return "bye";
        }

        return "ERROR unknown command";
    }
};

#endif