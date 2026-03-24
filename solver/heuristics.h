#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "cube.h"
#include <functional>

// ─── Heuristic Function Type ────────────────────────────────────
using HeuristicFn = std::function<int(const State&, const State&)>;

// ─── Misplaced Cubies Heuristic (Phase 2) ───────────────────────
/// Counts sticker positions where state[i] != goal[i].
///
/// Admissibility proof:
///   Each move affects at most 20 stickers (8 face + 12 ring).
///   Each misplaced cubie requires >= 1 move to fix.
///   Therefore h(n) <= h*(n) for all n, guaranteeing optimal
///   solutions when used with A* or IDA*.
///
/// Verification:
///   misplaced_cubies(GOAL, GOAL) == 0
///   misplaced_cubies(apply_move(GOAL, R), GOAL) > 0
///   misplaced_cubies(scramble_n, GOAL) <= n * 20  (each move changes ≤ 20)
int misplaced_cubies(const State& state, const State& goal);

// ─── Manhattan Distance Heuristic (Phase 3 — stub) ─────────────
/// For each cubie, compute minimum moves to reach goal position.
/// Stronger than misplaced_cubies. NOT YET IMPLEMENTED.
int manhattan_distance(const State& state, const State& goal);

// ─── Heuristic Selector ─────────────────────────────────────────
/// Returns the heuristic function for the given name.
/// Supported: "misplaced", "manhattan" (Phase 3)
HeuristicFn get_heuristic(const std::string& name);

#endif // HEURISTICS_H
