# ─── Rubik's Cube Solver — Makefile ──────────────────────────────
# Build: make solver
# Run:   make run
# Test:  make test
# Clean: make clean

CXX      = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra
SRC_DIR  = solver
BUILD_DIR = build
TEST_DIR = tests

# Source files
SRCS = $(SRC_DIR)/cube.cpp \
       $(SRC_DIR)/bfs.cpp \
       $(SRC_DIR)/astar.cpp \
       $(SRC_DIR)/idastar.cpp \
       $(SRC_DIR)/heuristics.cpp \
       $(SRC_DIR)/main.cpp

BINARY = $(BUILD_DIR)/rubiks_solver

# ─── Targets ────────────────────────────────────────────────────

.PHONY: solver run test clean all

all: solver

solver: $(BINARY)

$(BINARY): $(SRCS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -I $(SRC_DIR)

# Run with a solved cube using BFS (no-op solve — validation test)
run: $(BINARY)
	./$(BINARY) --state UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB --algo bfs

# Run all test suites
test:
	@echo "=== Cube Model Tests ==="
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_cube \
		$(TEST_DIR)/test_cube.cpp $(SRC_DIR)/cube.cpp -I $(SRC_DIR)
	./$(TEST_DIR)/test_cube
	@echo ""
	@echo "=== BFS Tests ==="
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_bfs \
		$(TEST_DIR)/test_bfs.cpp $(SRC_DIR)/cube.cpp $(SRC_DIR)/bfs.cpp -I $(SRC_DIR)
	./$(TEST_DIR)/test_bfs
	@echo ""
	@echo "=== Heuristics Tests ==="
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_heuristics \
		$(TEST_DIR)/test_heuristics.cpp $(SRC_DIR)/cube.cpp $(SRC_DIR)/heuristics.cpp -I $(SRC_DIR)
	./$(TEST_DIR)/test_heuristics
	@echo ""
	@echo "=== A* Tests ==="
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_astar \
		$(TEST_DIR)/test_astar.cpp $(SRC_DIR)/cube.cpp $(SRC_DIR)/bfs.cpp \
		$(SRC_DIR)/astar.cpp $(SRC_DIR)/heuristics.cpp -I $(SRC_DIR)
	./$(TEST_DIR)/test_astar
	@echo ""
	@echo "=== IDA* Tests ==="
	$(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_idastar \
		$(TEST_DIR)/test_idastar.cpp $(SRC_DIR)/cube.cpp $(SRC_DIR)/bfs.cpp \
		$(SRC_DIR)/idastar.cpp $(SRC_DIR)/heuristics.cpp -I $(SRC_DIR)
	./$(TEST_DIR)/test_idastar

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TEST_DIR)/test_cube $(TEST_DIR)/test_bfs \
	      $(TEST_DIR)/test_heuristics $(TEST_DIR)/test_astar \
	      $(TEST_DIR)/test_idastar
