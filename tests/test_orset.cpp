#include "../src/orset.h"
#include <cassert>
#include <iostream>

void test_orset() {
    std::cout << "Testing ORSet..." << std::endl;
    ORSet s1("A");
    ORSet s2("B");

    s1.add("x");
    s1.add("y");
    assert(s1.query().size() == 2);
    assert(s1.query().count("x") == 1);
    assert(s1.query().count("y") == 1);

    s2.add("y");
    s2.add("z");
    assert(s2.query().size() == 2);

    s1.merge(s2);
    // {x, y, z}
    assert(s1.query().size() == 3);
    assert(s1.query().count("x") == 1);
    assert(s1.query().count("y") == 1);
    assert(s1.query().count("z") == 1);

    s1.remove("y");
    assert(s1.query().size() == 2);
    assert(s1.query().count("y") == 0);

    s1.merge(s2); 
    assert(s1.query().count("y") == 0);

    s2.add("y"); 
    s1.merge(s2);
    assert(s1.query().count("y") == 1);

    std::cout << "ORSet tests passed!" << std::endl;
}

int main() {
    test_orset();
    return 0;
}
