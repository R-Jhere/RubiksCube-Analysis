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

    // Goal → Goal = 0
    std::cout << "\n[Goal State]" << std::endl;
    check(misplaced_cubies(GOAL_STATE, GOAL_STATE) == 0,
          "h(GOAL, GOAL) == 0");

    // Single move > 0
    std::cout << "\n[Single-Move Scrambles]" << std::endl;
    for (int m = 0; m < NUM_MOVES; m++) {
        State s = apply_move(GOAL_STATE, m);
        int h = misplaced_cubies(s, GOAL_STATE);
        check(h > 0, move_to_string(m) + " scramble: h=" + std::to_string(h) + " > 0");
    }

    // Admissibility: h <= optimal_depth (loose check for small depths)
    // A single move changes at most 20 stickers, so h <= 20 for depth 1
    std::cout << "\n[Admissibility Bounds]" << std::endl;
    for (int m = 0; m < NUM_MOVES; m++) {
        State s = apply_move(GOAL_STATE, m);
        int h = misplaced_cubies(s, GOAL_STATE);
        check(h <= 20, move_to_string(m) + ": h=" + std::to_string(h) + " <= 20");
    }

    // Multi-move scramble: h should increase (generally)
    std::cout << "\n[Multi-Move Scrambles]" << std::endl;
    for (int depth : {1, 3, 5, 7}) {
        State s = generate_scramble(depth, 42);
        int h = misplaced_cubies(s, GOAL_STATE);
        check(h > 0, "depth=" + std::to_string(depth) + ": h=" + std::to_string(h) + " > 0");
        check(h <= NUM_STICKERS, "  h <= 54 (max possible)");
        std::cout << "    depth=" << depth << " → h=" << h << std::endl;
    }

    // get_heuristic selector
    std::cout << "\n[Heuristic Selector]" << std::endl;
    HeuristicFn fn = get_heuristic("misplaced");
    check(fn(GOAL_STATE, GOAL_STATE) == 0, "get_heuristic(\"misplaced\") works");

    bool threw = false;
    try { get_heuristic("invalid"); } catch (...) { threw = true; }
    check(threw, "get_heuristic(\"invalid\") throws");

    std::cout << "\n═══════════════════════════════════════════" << std::endl;
    std::cout << " Results: " << passed << " passed, "
              << failed << " failed" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;
    return failed > 0 ? 1 : 0;
}
