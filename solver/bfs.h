#ifndef BFS_H
#define BFS_H

#include "cube.h"
#include <vector>

// ─── Solve Result ───────────────────────────────────────────────
struct SolveResult {
    std::vector<int> moves;   // Sequence of MoveID values
    int nodes;                // Total nodes explored (popped from queue)
};

// ─── BFS Configuration ─────────────────────────────────────────
constexpr int BFS_MAX_DEPTH = 7;  // Hard cap — BFS OOMs beyond this

// ─── BFS Solver ─────────────────────────────────────────────────
/// Breadth-first search from `start` to `goal`.
/// Explores all states layer by layer. Guarantees optimal (shortest) solution.
/// Hard depth cap at BFS_MAX_DEPTH to prevent memory exhaustion.
///
/// Returns SolveResult with:
///   - moves: sequence of MoveIDs (empty if no solution within depth cap)
///   - nodes: total states popped from queue
SolveResult bfs_solve(const State& start, const State& goal);

#endif // BFS_H
