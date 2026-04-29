#include "../src/gcounter.h"
#include <cassert>
#include <iostream>

void test_gcounter() {
    std::cout << "Testing GCounter..." << std::endl;
    GCounter c1("A");
    GCounter c2("B");

    c1.increment();
    c1.increment();
    assert(c1.query() == 2);

    c2.increment();
    assert(c2.query() == 1);

    c1.merge(c2);
    assert(c1.query() == 3);
    assert(c1.state.at("A") == 2);
    assert(c1.state.at("B") == 1);

    c2.increment();
    c2.increment();
    c1.merge(c2);
    assert(c1.query() == 5);
    assert(c1.state.at("B") == 3);
    
    std::cout << "GCounter tests passed!" << std::endl;
}

void test_pncounter() {
    std::cout << "Testing PNCounter..." << std::endl;
    PNCounter p1("A");
    PNCounter p2("B");

    p1.increment();
    p1.increment();
    p1.decrement();
    assert(p1.query() == 1);

    p2.increment();
    p2.decrement();
    p2.decrement();
    assert(p2.query() == -1);

    p1.merge(p2);
    // p1: P={A:2, B:1}, N={A:1, B:2} -> 3 - 3 = 0
    assert(p1.query() == 0);

    p1.increment();
    assert(p1.query() == 1);

    std::cout << "PNCounter tests passed!" << std::endl;
}

int main() {
    test_gcounter();
    test_pncounter();
    return 0;
}
