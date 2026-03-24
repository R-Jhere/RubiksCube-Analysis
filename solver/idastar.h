#ifndef IDASTAR_H
#define IDASTAR_H

#include "cube.h"
#include "bfs.h"          // for SolveResult
#include "heuristics.h"   // for HeuristicFn

// ─── IDA* Solver ────────────────────────────────────────────────
/// Iterative Deepening A* from `start` to `goal` using heuristic `h`.
///
/// Combines depth-first search with heuristic pruning:
///   - Each iteration uses a threshold on f = g + h
///   - Paths with f > threshold are pruned (backtrack)
///   - Next threshold = minimum f among all pruned paths
///   - Repeats until solution found or time cap exceeded
///
/// Memory: O(d) — only stores the current path, not the frontier.
/// This is why IDA* can solve depth 7+ where BFS/A* run out of RAM.
///
/// Time cap: 60 seconds (checked between iterations).
///
/// Returns SolveResult with:
///   - moves: optimal move sequence (empty if no solution / timeout)
///   - nodes: total states expanded across all iterations
SolveResult idastar_solve(const State& start, const State& goal, HeuristicFn h);

#endif // IDASTAR_H
