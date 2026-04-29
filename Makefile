CXX = g++
CXXFLAGS = -std=c++17 -Wall -Isrc

TEST_SOURCES = tests/test_gcounter.cpp tests/test_lww.cpp tests/test_orset.cpp tests/comprehensive_tests.cpp
TEST_EXECUTABLES = $(TEST_SOURCES:.cpp=)

all: $(TEST_EXECUTABLES)

tests/test_%: tests/test_%.cpp src/*.h
	$(CXX) $(CXXFLAGS) $< -o $@

tests/comprehensive_tests: tests/comprehensive_tests.cpp src/*.h
	$(CXX) $(CXXFLAGS) tests/comprehensive_tests.cpp -o tests/comprehensive_tests

run_tests: all
	@for test in $(TEST_EXECUTABLES); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done

clean:
	rm -f $(TEST_EXECUTABLES)

.PHONY: all run_tests clean
