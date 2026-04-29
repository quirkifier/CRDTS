#include "../src/lww.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

void test_lww() {
    std::cout << "Testing LWWRegister..." << std::endl;
    LWWRegister r1("A");
    LWWRegister r2("B");

    r1.set("val1");
    assert(r1.query() == "val1");

    // Sleep a bit to ensure a different timestamp
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    r2.set("val2");
    assert(r2.query() == "val2");

    r1.merge(r2);
    assert(r1.query() == "val2");

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    r1.set("val3");
    assert(r1.query() == "val3");

    r1.merge(r2); // r2 has older timestamp
    assert(r1.query() == "val3");

    std::cout << "LWWRegister tests passed!" << std::endl;
}

int main() {
    test_lww();
    return 0;
}
