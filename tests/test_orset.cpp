#include "../src/ORSET.h"
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

    // Merge s2 back into s1. s2 still has 'y' but it's an older 'y' than the one s1 removed?
    // Actually ORSet remove removes all currently seen tags for that element.
    // s1's remove("y") should have removed both s1's "y" and s2's "y" if they were already merged.
    // Wait, let's trace:
    // s1.add("x") -> (x, A:1)
    // s1.add("y") -> (y, A:2)
    // s2.add("y") -> (y, B:1)
    // s2.add("z") -> (z, B:2)
    // s1.merge(s2) -> added: {(x, A:1), (y, A:2), (y, B:1), (z, B:2)}
    // s1.remove("y") -> removed: {(y, A:2), (y, B:1)}
    // query() -> {x, z}
    
    s1.merge(s2); // merging s2 again shouldn't bring 'y' back because s1 already has the tags for 'y' in its removed set.
    assert(s1.query().count("y") == 0);

    s2.add("y"); // New 'y' with tag (y, B:3)
    s1.merge(s2);
    assert(s1.query().count("y") == 1);

    std::cout << "ORSet tests passed!" << std::endl;
}

int main() {
    test_orset();
    return 0;
}
