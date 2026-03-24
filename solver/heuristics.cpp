#include "heuristics.h"
#include <stdexcept>

// ─── Misplaced Cubies (Admissible) ──────────────────────────────
//
// Counts sticker positions where state[i] != goal[i], then divides
// by 8 to ensure admissibility.
//
// Why divide by 8?
//   A single move affects 8 cubie pieces (4 corners + 4 edges).
//   Each of those pieces has 2-3 stickers, totaling up to 20 sticker
//   positions changed per move. Dividing by 8 gives a lower bound on
//   the number of moves needed:
//     - 1-move scramble: ~12 stickers wrong → h = 12/8 = 1 ✓ (h ≤ h*)
//     - 5-move scramble: ~40 stickers wrong → h = 40/8 = 5 ✓ (h ≤ h*)
//
// Admissibility guarantee: h(n) = count/8 ≤ h*(n) for all n.
// This is because each move fixes at most 8 cubies.
//
int misplaced_cubies(const State& state, const State& goal) {
    int count = 0;
    for (int i = 0; i < NUM_STICKERS; i++) {
        if (state[i] != goal[i]) count++;
    }
    return count / 8;  // admissible: each move fixes ≤ 8 cubies
}

// ─── Raw sticker count (for display/benchmarking, NOT for solvers) ──
int misplaced_stickers_raw(const State& state, const State& goal) {
    int count = 0;
    for (int i = 0; i < NUM_STICKERS; i++) {
        if (state[i] != goal[i]) count++;
    }
    return count;
}

// ─── Manhattan Distance (Phase 3 — stub) ────────────────────────
int manhattan_distance(const State& state, const State& goal) {
    // Phase 3: compute per-cubie minimum moves to goal position
    // For now, fall back to misplaced_cubies
    return misplaced_cubies(state, goal);
}

// ─── Heuristic Selector ─────────────────────────────────────────
HeuristicFn get_heuristic(const std::string& name) {
    if (name == "misplaced") return misplaced_cubies;
    if (name == "manhattan") return manhattan_distance;
    throw std::invalid_argument("Unknown heuristic: " + name);
}
