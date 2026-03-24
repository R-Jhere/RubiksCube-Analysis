#include "cube.h"
#include "bfs.h"
#include "idastar.h"
#include "heuristics.h"
#include <iostream>
#include <chrono>

static int passed = 0, failed = 0;

void check(bool cond, const std::string& name) {
    if (cond) { std::cout << "  ✓ " << name << std::endl; passed++; }
    else      { std::cout << "  ✗ FAIL: " << name << std::endl; failed++; }
}

bool verify_solution(const State& start, const State& goal,
                     const std::vector<int>& moves) {
    State s = start;
    for (int m : moves) s = apply_move(s, m);
    return s == goal;
}

void test_already_solved() {
    std::cout << "\n[Already Solved]" << std::endl;
    SolveResult r = idastar_solve(GOAL_STATE, GOAL_STATE, misplaced_cubies);
    check(r.moves.empty(), "No moves needed");
    check(r.nodes == 0, "Zero nodes explored");
}

void test_single_moves() {
    std::cout << "\n[1-Move Scrambles]" << std::endl;
    for (int m = 0; m < NUM_MOVES; m++) {
        State s = apply_move(GOAL_STATE, m);
        SolveResult r = idastar_solve(s, GOAL_STATE, misplaced_cubies);
        check(verify_solution(s, GOAL_STATE, r.moves),
              move_to_string(m) + " → correct (" +
              std::to_string(r.moves.size()) + " moves, " +
              std::to_string(r.nodes) + " nodes)");
    }
}

void test_optimality_vs_bfs() {
    std::cout << "\n[Optimality: IDA* moves == BFS moves (depth 3-4)]" << std::endl;
    for (int depth : {3, 4}) {
        for (int seed : {42, 99, 7}) {
            State s = generate_scramble(depth, seed);
            SolveResult bfs_r = bfs_solve(s, GOAL_STATE);
            SolveResult ida_r = idastar_solve(s, GOAL_STATE, misplaced_cubies);

            bool same = ida_r.moves.size() == bfs_r.moves.size();
            bool valid = verify_solution(s, GOAL_STATE, ida_r.moves);

            check(same && valid,
                  "depth=" + std::to_string(depth) + " seed=" + std::to_string(seed) +
                  ": BFS=" + std::to_string(bfs_r.moves.size()) +
                  " IDA*=" + std::to_string(ida_r.moves.size()) +
                  " (BFS nodes=" + std::to_string(bfs_r.nodes) +
                  " IDA* nodes=" + std::to_string(ida_r.nodes) + ")");
        }
    }
}

void test_known_scrambles() {
    std::cout << "\n[Known Scrambles]" << std::endl;

    // 2-move: R then U
    State s2 = apply_move(apply_move(GOAL_STATE, MOVE_R), MOVE_U);
    SolveResult r2 = idastar_solve(s2, GOAL_STATE, misplaced_cubies);
    check(r2.moves.size() == 2 && verify_solution(s2, GOAL_STATE, r2.moves),
          "R→U: solved in 2 moves, " + std::to_string(r2.nodes) + " nodes");

    // 3-move: R U F
    State s3 = apply_move(apply_move(apply_move(GOAL_STATE, MOVE_R), MOVE_U), MOVE_F);
    SolveResult r3 = idastar_solve(s3, GOAL_STATE, misplaced_cubies);
    check(r3.moves.size() == 3 && verify_solution(s3, GOAL_STATE, r3.moves),
          "R→U→F: solved in 3 moves, " + std::to_string(r3.nodes) + " nodes");
}

void test_depth_5_timed() {
    std::cout << "\n[Depth-5 Scramble with timing]" << std::endl;
    State s = generate_scramble(5, 42);

    auto t0 = std::chrono::steady_clock::now();
    SolveResult r = idastar_solve(s, GOAL_STATE, misplaced_cubies);
    auto t1 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    check(!r.moves.empty(), "Found solution");
    check(verify_solution(s, GOAL_STATE, r.moves), "Solution is correct");
    check(r.moves.size() <= 5, "≤ 5 moves (got " + std::to_string(r.moves.size()) + ")");

    std::cout << "  Time: " << ms << " ms" << std::endl;
    std::cout << "  Nodes: " << r.nodes << std::endl;
    std::cout << "  Solution: ";
    for (int m : r.moves) std::cout << move_to_string(m) << " ";
    std::cout << std::endl;
}

int main() {
    std::cout << "═══════════════════════════════════════════" << std::endl;
    std::cout << " IDA* Solver — Test Suite" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;

    test_already_solved();
    test_single_moves();
    test_optimality_vs_bfs();
    test_known_scrambles();
    test_depth_5_timed();

    std::cout << "\n═══════════════════════════════════════════" << std::endl;
    std::cout << " Results: " << passed << " passed, "
              << failed << " failed" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;
    return failed > 0 ? 1 : 0;
}
