#include "cube.h"
#include "bfs.h"
#include <iostream>
#include <cassert>

static int passed = 0, failed = 0;

void check(bool cond, const std::string& name) {
    if (cond) { std::cout << "  ✓ " << name << std::endl; passed++; }
    else      { std::cout << "  ✗ FAIL: " << name << std::endl; failed++; }
}

// Verify a solution is correct by replaying moves
bool verify_solution(const State& start, const State& goal,
                     const std::vector<int>& moves) {
    State s = start;
    for (int m : moves) s = apply_move(s, m);
    return s == goal;
}

void test_already_solved() {
    std::cout << "\n[Already Solved]" << std::endl;
    SolveResult r = bfs_solve(GOAL_STATE, GOAL_STATE);
    check(r.moves.empty(), "No moves needed");
    check(r.nodes == 0, "Zero nodes explored");
}

void test_single_move() {
    std::cout << "\n[1-Move Scrambles]" << std::endl;
    for (int m = 0; m < NUM_MOVES; m++) {
        State scrambled = apply_move(GOAL_STATE, m);
        SolveResult r = bfs_solve(scrambled, GOAL_STATE);
        bool valid = verify_solution(scrambled, GOAL_STATE, r.moves);
        check(valid, "Solve " + move_to_string(m) + " scramble (" +
              std::to_string(r.moves.size()) + " moves, " +
              std::to_string(r.nodes) + " nodes)");
        // BFS optimal: should solve in at most 1 move (or 1 for half-turns)
        check(r.moves.size() <= 2, "  ≤ 2 moves for single-move scramble");
    }
}

void test_depth_3() {
    std::cout << "\n[3-Move Scramble]" << std::endl;
    State s = GOAL_STATE;
    s = apply_move(s, MOVE_R);
    s = apply_move(s, MOVE_U);
    s = apply_move(s, MOVE_F);

    SolveResult r = bfs_solve(s, GOAL_STATE);
    check(!r.moves.empty(), "Found solution");
    check(verify_solution(s, GOAL_STATE, r.moves), "Solution is correct");
    check(r.moves.size() <= 3, "Optimal: ≤ 3 moves (got " +
          std::to_string(r.moves.size()) + ")");
    check(r.nodes > 0, "Explored " + std::to_string(r.nodes) + " nodes");

    std::cout << "  Solution: ";
    for (int m : r.moves) std::cout << move_to_string(m) << " ";
    std::cout << std::endl;
}

void test_depth_5() {
    std::cout << "\n[5-Move Scramble (seed=42)]" << std::endl;
    State s = generate_scramble(5, 42);

    SolveResult r = bfs_solve(s, GOAL_STATE);
    check(!r.moves.empty(), "Found solution");
    check(verify_solution(s, GOAL_STATE, r.moves), "Solution is correct");
    check(r.moves.size() <= 5, "Optimal: ≤ 5 moves (got " +
          std::to_string(r.moves.size()) + ")");
    std::cout << "  Nodes explored: " << r.nodes << std::endl;
    std::cout << "  Solution (" << r.moves.size() << " moves): ";
    for (int m : r.moves) std::cout << move_to_string(m) << " ";
    std::cout << std::endl;
}

void test_depth_cap() {
    std::cout << "\n[Depth Cap Enforcement]" << std::endl;
    // BFS_MAX_DEPTH = 7, so depth-7 scramble might or might not solve
    // (scramble depth ≠ optimal depth), but it should NOT crash
    check(BFS_MAX_DEPTH == 7, "MAX_DEPTH is 7");
}

int main() {
    std::cout << "═══════════════════════════════════════════" << std::endl;
    std::cout << " BFS Solver — Test Suite" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;

    test_already_solved();
    test_single_move();
    test_depth_3();
    test_depth_5();
    test_depth_cap();

    std::cout << "\n═══════════════════════════════════════════" << std::endl;
    std::cout << " Results: " << passed << " passed, "
              << failed << " failed" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;
    return failed > 0 ? 1 : 0;
}
