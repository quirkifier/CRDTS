#include "../src/command_handler.h"
#include <cassert>
#include <iostream>
#include <string>

void test_command_handler() {
    std::cout << "Testing CommandHandler..." << std::endl;

    KeyValueStore store("A");
    CommandHandler handler(store);
    bool shouldQuit = false;

    assert(handler.handleCommand("CREATE likes GCOUNTER", shouldQuit) == "OK");
    assert(handler.handleCommand("INC likes 2", shouldQuit) == "OK (likes = 2)");
    assert(handler.handleCommand("GET likes", shouldQuit) == "2");

    assert(handler.handleCommand("CREATE score PNCOUNTER", shouldQuit) == "OK");
    assert(handler.handleCommand("INC score -3", shouldQuit) == "OK (score = -3)");
    assert(handler.handleCommand("GET score", shouldQuit) == "-3");

    assert(handler.handleCommand("CREATE name LWWREG", shouldQuit) == "OK");
    assert(handler.handleCommand("SET name Alice", shouldQuit) == "OK");
    assert(handler.handleCommand("GET name", shouldQuit) == "Alice");

    assert(handler.handleCommand("CREATE tags ORSET", shouldQuit) == "OK");
    assert(handler.handleCommand("ADD tags red", shouldQuit) == "OK");
    assert(handler.handleCommand("ADD tags blue", shouldQuit) == "OK");
    assert(handler.handleCommand("GET tags", shouldQuit) == "blue\nred");

    std::string listed = handler.handleCommand("\\list", shouldQuit);
    assert(listed.find("likes GCOUNTER 2") != std::string::npos);
    assert(listed.find("score PNCOUNTER -3") != std::string::npos);
    assert(listed.find("name LWWREG Alice") != std::string::npos);
    assert(listed.find("tags ORSET blue red") != std::string::npos);

    assert(handler.handleCommand("\\peers", shouldQuit) == "no peers");
    assert(handler.handleCommand("\\sync", shouldQuit) == "OK no-op");

    assert(handler.handleCommand("QUIT", shouldQuit) == "bye");
    assert(shouldQuit);

    std::cout << "CommandHandler tests passed!" << std::endl;
}

int main() {
    test_command_handler();
    return 0;
}
