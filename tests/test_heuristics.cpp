#include "cube.h"
#include "heuristics.h"
#include <iostream>

static int passed = 0, failed = 0;

void check(bool cond, const std::string& name) {
    if (cond) { std::cout << "  ✓ " << name << std::endl; passed++; }
    else      { std::cout << "  ✗ FAIL: " << name << std::endl; failed++; }
}

int main() {
    std::cout << "═══════════════════════════════════════════" << std::endl;
    std::cout << " Heuristics — Test Suite" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;

    // h(goal) = 0
    std::cout << "\n[Goal State]" << std::endl;
    check(misplaced_cubies(GOAL_STATE, GOAL_STATE) == 0, "h(GOAL) == 0");
    check(misplaced_stickers_raw(GOAL_STATE, GOAL_STATE) == 0, "raw(GOAL) == 0");

    // h(scramble) > 0 for all moves
    std::cout << "\n[Single-Move: h > 0]" << std::endl;
    for (int m = 0; m < NUM_MOVES; m++) {
        State s = apply_move(GOAL_STATE, m);
        int h = misplaced_cubies(s, GOAL_STATE);
        int raw = misplaced_stickers_raw(s, GOAL_STATE);
        check(h > 0, move_to_string(m) + ": h=" + std::to_string(h) +
              " (raw=" + std::to_string(raw) + ")");
    }

    // Admissibility: h(1-move) <= 1 (optimal cost for 1-move scramble)
    std::cout << "\n[Admissibility: h(1-move) <= optimal=1]" << std::endl;
    for (int m = 0; m < NUM_MOVES; m++) {
        State s = apply_move(GOAL_STATE, m);
        int h = misplaced_cubies(s, GOAL_STATE);
        check(h <= 1, move_to_string(m) + ": h=" + std::to_string(h) + " <= 1");
    }

    // Multi-depth admissibility
    std::cout << "\n[Multi-Depth Admissibility]" << std::endl;
    for (int depth : {1, 3, 5}) {
        State s = generate_scramble(depth, 42);
        int h = misplaced_cubies(s, GOAL_STATE);
        int raw = misplaced_stickers_raw(s, GOAL_STATE);
        check(h <= depth, "depth=" + std::to_string(depth) +
              ": h=" + std::to_string(h) + " <= " + std::to_string(depth) +
              " (raw=" + std::to_string(raw) + ")");
    }

    // Selector
    std::cout << "\n[Selector]" << std::endl;
    HeuristicFn fn = get_heuristic("misplaced");
    check(fn(GOAL_STATE, GOAL_STATE) == 0, "get_heuristic works");
    bool threw = false;
    try { get_heuristic("invalid"); } catch (...) { threw = true; }
    check(threw, "invalid name throws");

    std::cout << "\n═══════════════════════════════════════════" << std::endl;
    std::cout << " Results: " << passed << " passed, "
              << failed << " failed" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;
    return failed > 0 ? 1 : 0;
}
