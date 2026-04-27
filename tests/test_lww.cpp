#include "../src/LWW.cpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

void test_lww() {
    std::cout << "Testing LWW_Register..." << std::endl;
    LWW_Register r1;
    LWW_Register r2;

    r1.set("val1", "A");
    assert(r1.get() == "val1");

    // Sleep a bit to ensure a different timestamp
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    r2.set("val2", "B");
    assert(r2.get() == "val2");

    r1.merge(r2);
    assert(r1.get() == "val2");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    r1.set("val3", "A");
    assert(r1.get() == "val3");

    r1.merge(r2); // r2 has older timestamp
    assert(r1.get() == "val3");

    std::cout << "LWW_Register tests passed!" << std::endl;
}

int main() {
    test_lww();
    return 0;
}
