#include "cube.h"
#include "bfs.h"
#include "astar.h"
#include "heuristics.h"
#include <iostream>

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
    SolveResult r = astar_solve(GOAL_STATE, GOAL_STATE, misplaced_cubies);
    check(r.moves.empty(), "No moves needed");
    check(r.nodes == 0, "Zero nodes explored");
}

void test_single_moves() {
    std::cout << "\n[1-Move Scrambles]" << std::endl;
    for (int m = 0; m < NUM_MOVES; m++) {
        State s = apply_move(GOAL_STATE, m);
        SolveResult r = astar_solve(s, GOAL_STATE, misplaced_cubies);
        check(verify_solution(s, GOAL_STATE, r.moves),
              move_to_string(m) + " → correct (" +
              std::to_string(r.moves.size()) + " moves, " +
              std::to_string(r.nodes) + " nodes)");
    }
}

void test_optimality_vs_bfs() {
    std::cout << "\n[Optimality: A* moves == BFS moves (depth 3-4)]" << std::endl;

    for (int depth : {3, 4}) {
        for (int seed : {42, 99, 7}) {
            State s = generate_scramble(depth, seed);

            SolveResult bfs_r   = bfs_solve(s, GOAL_STATE);
            SolveResult astar_r = astar_solve(s, GOAL_STATE, misplaced_cubies);

            bool same_length = astar_r.moves.size() == bfs_r.moves.size();
            bool valid = verify_solution(s, GOAL_STATE, astar_r.moves);

            check(same_length && valid,
                  "depth=" + std::to_string(depth) + " seed=" + std::to_string(seed) +
                  ": BFS=" + std::to_string(bfs_r.moves.size()) +
                  " A*=" + std::to_string(astar_r.moves.size()) + " moves");
        }
    }
}

void test_fewer_nodes_than_bfs() {
    std::cout << "\n[Efficiency: A* nodes < BFS nodes]" << std::endl;

    for (int depth : {3, 4}) {
        State s = generate_scramble(depth, 42);

        SolveResult bfs_r   = bfs_solve(s, GOAL_STATE);
        SolveResult astar_r = astar_solve(s, GOAL_STATE, misplaced_cubies);

        bool fewer = astar_r.nodes <= bfs_r.nodes;
        double ratio = bfs_r.nodes > 0
            ? (double)astar_r.nodes / bfs_r.nodes * 100.0 : 0;

        check(fewer,
              "depth=" + std::to_string(depth) +
              ": BFS=" + std::to_string(bfs_r.nodes) +
              " A*=" + std::to_string(astar_r.nodes) +
              " (" + std::to_string((int)ratio) + "%)");
    }
}

void test_known_2move_scramble() {
    // Manually apply R then U — known 2-move optimal solution
    std::cout << "\n[Known 2-Move Scramble: R → U]" << std::endl;
    State s = apply_move(apply_move(GOAL_STATE, MOVE_R), MOVE_U);

    SolveResult r = astar_solve(s, GOAL_STATE, misplaced_cubies);
    check(!r.moves.empty(), "Found solution");
    check(verify_solution(s, GOAL_STATE, r.moves), "Solution is correct");
    check(r.moves.size() == 2, "Optimal 2 moves (got " +
          std::to_string(r.moves.size()) + ")");

    std::cout << "  Nodes: " << r.nodes << ", Solution: ";
    for (int m : r.moves) std::cout << move_to_string(m) << " ";
    std::cout << std::endl;
}

int main() {
    std::cout << "═══════════════════════════════════════════" << std::endl;
    std::cout << " A* Solver — Test Suite" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;

    test_already_solved();
    test_single_moves();
    test_optimality_vs_bfs();
    test_fewer_nodes_than_bfs();
    test_known_2move_scramble();

    std::cout << "\n═══════════════════════════════════════════" << std::endl;
    std::cout << " Results: " << passed << " passed, "
              << failed << " failed" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;
    return failed > 0 ? 1 : 0;
}
