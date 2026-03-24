#ifndef ASTAR_H
#define ASTAR_H

#include "cube.h"
#include "bfs.h"          // for SolveResult
#include "heuristics.h"   // for HeuristicFn

// ─── A* Solver ──────────────────────────────────────────────────
/// A* search from `start` to `goal` using heuristic `h`.
///
/// Uses f(n) = g(n) + h(n) with a min-heap priority queue.
/// Guarantees optimal solution when h is admissible.
///
/// Returns SolveResult with:
///   - moves: optimal move sequence (empty if no solution)
///   - nodes: total states expanded (popped and processed)
SolveResult astar_solve(const State& start, const State& goal, HeuristicFn h);

#endif // ASTAR_H
