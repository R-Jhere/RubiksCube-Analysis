#include "heuristics.h"
#include <stdexcept>

// ─── Misplaced Cubies ───────────────────────────────────────────
// Count sticker positions where state differs from goal.
// Simple, fast, admissible. O(54) per call.
int misplaced_cubies(const State& state, const State& goal) {
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
