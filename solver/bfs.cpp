#include "bfs.h"
#include <queue>
#include <unordered_set>
#include <utility>

// ─── BFS Solver Implementation ──────────────────────────────────
//
// Algorithm:
//   1. Push start state with empty move list onto queue
//   2. Pop front of queue, increment node count
//   3. If state == goal → return moves + node count
//   4. If depth >= MAX_DEPTH → skip (don't expand)
//   5. Generate all 18 successor states
//   6. If successor not visited → mark visited, push to queue
//   7. Repeat until queue empty (no solution) or goal found
//
// Pruning: skip moves on the same face as the previous move
//   (e.g., R then R' is pointless — already visited via identity)
//
// Memory: O(b^d) — stores all visited states. At depth 7:
//   worst case ~18^7 ≈ 600M states (pruned to ~100M in practice)
//   This is why MAX_DEPTH = 7 is a hard cap.

SolveResult bfs_solve(const State& start, const State& goal) {
    // Early exit: already solved
    if (start == goal) {
        return {{}, 0};
    }

    // Queue stores: (state, move_sequence)
    std::queue<std::pair<State, std::vector<int>>> frontier;
    std::unordered_set<State> visited;

    frontier.push({start, {}});
    visited.insert(start);
    int nodes = 0;

    while (!frontier.empty()) {
        auto [state, moves] = std::move(frontier.front());
        frontier.pop();
        nodes++;

        // Depth cap: don't expand nodes at MAX_DEPTH
        if (static_cast<int>(moves.size()) >= BFS_MAX_DEPTH) {
            continue;
        }

        // Determine last move's face group for pruning
        // Moves are grouped in triples: (R,R',R2)=0-2, (U,U',U2)=3-5, etc.
        int last_face = moves.empty() ? -1 : (moves.back() / 3);

        // Try all 18 moves
        for (int m = 0; m < NUM_MOVES; m++) {
            // Prune: skip moves on the same face as the last move
            // R then R' = identity (already visited)
            // R then R2 = R' (will be found via R' directly)
            if (m / 3 == last_face) continue;

            State next = apply_move(state, m);

            // Skip if already visited
            if (visited.count(next)) continue;

            // Build the new move sequence
            std::vector<int> new_moves = moves;
            new_moves.push_back(m);

            // Goal check on generation (not expansion) — faster
            if (next == goal) {
                return {new_moves, nodes};
            }

            visited.insert(next);
            frontier.push({std::move(next), std::move(new_moves)});
        }
    }

    // No solution found within depth cap
    return {{}, nodes};
}
