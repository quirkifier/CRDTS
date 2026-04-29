#include "../src/gcounter.h"
#include "../src/lww.h"
#include "../src/orset.h"
#include <cassert>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

using namespace std;

// --- Helper for random strings ---
string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  string str(length, 0);
  generate_n(str.begin(), length, randchar);
  return str;
}

// --- Generic Property Tests ---

template <typename T>
void test_commutativity(function<T(const string &)> generator) {
  for (int i = 0; i < 100; ++i) {
    T a = generator("nodeA");
    T b = generator("nodeB");

    T a_copy = a;
    T b_copy = b;

    a.merge(b);
    b_copy.merge(a_copy);

    assert(a.query() == b_copy.query());
  }
}

template <typename T>
void test_associativity(function<T(const string &)> generator) {
  for (int i = 0; i < 100; ++i) {
    T a = generator("nodeA");
    T b = generator("nodeB");
    T c = generator("nodeC");

    T a_bc = a;
    T b_c = b;
    b_c.merge(c);
    a_bc.merge(b_c);

    T ab_c = a;
    ab_c.merge(b);
    ab_c.merge(c);

    assert(a_bc.query() == ab_c.query());
  }
}

template <typename T>
void test_idempotence(function<T(const string &)> generator) {
  for (int i = 0; i < 100; ++i) {
    T a = generator("nodeA");
    T b = generator("nodeB");

    a.merge(b);
    T once = a;
    a.merge(b);

    assert(once.query() == a.query());
  }
}

template <typename T>
void test_serialization_roundtrip(function<T(const string &)> generator,
                                  function<void(T &)> update) {
  for (int i = 0; i < 100; ++i) {
    T a = generator("nodeA");
    string serialized = a.serialize();
    T b = T::deserialize(serialized);

    assert(a.query() == b.query());

    // Further operations
    update(a);
    update(b);
    assert(a.query() == b.query());
  }
}

// --- Generators ---

GCounter generate_random_gcounter(const string &id) {
  GCounter g(id);
  int updates = rand() % 50;
  for (int i = 0; i < updates; ++i)
    g.increment();
  return g;
}

PNCounter generate_random_pncounter(const string &id) {
  PNCounter p(id);
  int updates = rand() % 50;
  for (int i = 0; i < updates; ++i) {
    if (rand() % 2 == 0)
      p.increment();
    else
      p.decrement();
  }
  return p;
}

LWWRegister generate_random_lww(const string &id) {
  LWWRegister r(id);
  int updates = rand() % 10 + 1;
  for (int i = 0; i < updates; ++i) {
    r.set(random_string(5));
  }
  return r;
}

ORSet generate_random_orset(const string &id) {
  ORSet s(id);
  int updates = rand() % 50;
  vector<string> added_elements;
  for (int i = 0; i < updates; ++i) {
    if (rand() % 2 == 0 || added_elements.empty()) {
      string e = random_string(3);
      s.add(e);
      added_elements.push_back(e);
    } else {
      int idx = rand() % added_elements.size();
      s.remove(added_elements[idx]);
    }
  }
  return s;
}

// --- Main Test Suite ---

int main() {
  srand(time(NULL));

  cout << "Running Comprehensive CRDT Property Tests..." << endl;

  cout << "Testing GCounter..." << endl;
  test_commutativity<GCounter>(generate_random_gcounter);
  test_associativity<GCounter>(generate_random_gcounter);
  test_idempotence<GCounter>(generate_random_gcounter);
  test_serialization_roundtrip<GCounter>(generate_random_gcounter,
                                         [](GCounter &g) { g.increment(); });

  cout << "Testing PNCounter..." << endl;
  test_commutativity<PNCounter>(generate_random_pncounter);
  test_associativity<PNCounter>(generate_random_pncounter);
  test_idempotence<PNCounter>(generate_random_pncounter);
  test_serialization_roundtrip<PNCounter>(generate_random_pncounter,
                                          [](PNCounter &p) { p.decrement(); });

  cout << "Testing LWWRegister..." << endl;
  test_commutativity<LWWRegister>(generate_random_lww);
  test_associativity<LWWRegister>(generate_random_lww);
  test_idempotence<LWWRegister>(generate_random_lww);
  test_serialization_roundtrip<LWWRegister>(
      generate_random_lww, [](LWWRegister &r) { r.set("fixed"); });

  cout << "Testing ORSet..." << endl;
  test_commutativity<ORSet>(generate_random_orset);
  test_associativity<ORSet>(generate_random_orset);
  test_idempotence<ORSet>(generate_random_orset);
  test_serialization_roundtrip<ORSet>(generate_random_orset,
                                      [](ORSet &s) { s.add("final"); });

  cout << "\nAll CRDT Phase 1 tests passed successfully!" << endl;

  return 0;
}
