#include "astar.h"
#include <queue>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <functional>

// ─── A* Solver Implementation ───────────────────────────────────
//
// Algorithm:
//   1. Push start with f = h(start), g = 0 onto min-heap
//   2. Pop node with smallest f value
//   3. Skip if we've already found a better g for this state
//   4. If state == goal → return solution
//   5. Record best_g[state] = g
//   6. Expand all 18 successors with new_g = g+1, new_f = new_g + h
//   7. Only push if no better g recorded for successor
//
// Node tuple: (f, g, state, moves)
//   - f first for min-heap ordering
//   - g second for tie-breaking (prefer deeper = closer to goal)

SolveResult astar_solve(const State& start, const State& goal, HeuristicFn h) {
    // Early exit
    if (start == goal) return {{}, 0};

    // Node: (f, g, state, move_sequence)
    using Node = std::tuple<int, int, State, std::vector<int>>;

    // Min-heap: smallest f on top (std::greater for min-heap)
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    // Best g-cost found for each state
    std::unordered_map<State, int> best_g;

    int nodes = 0;

    // Push start node
    pq.push({h(start, goal), 0, start, {}});

    while (!pq.empty()) {
        auto [f, g, state, moves] = pq.top();
        pq.pop();

        // Skip stale entries: we already found a cheaper path to this state
        if (best_g.count(state) && best_g[state] <= g) {
            continue;
        }

        // Record this as the best path to this state
        best_g[state] = g;
        nodes++;

        // Goal check
        if (state == goal) {
            return {moves, nodes};
        }

        // Determine last move's face group for pruning
        int last_face = moves.empty() ? -1 : (moves.back() / 3);

        // Expand successors
        for (int m = 0; m < NUM_MOVES; m++) {
            // Same-face pruning
            if (m / 3 == last_face) continue;

            State next = apply_move(state, m);
            int new_g = g + 1;

            // Only push if we haven't found a better path to this state
            if (!best_g.count(next) || best_g[next] > new_g) {
                int new_f = new_g + h(next, goal);
                std::vector<int> new_moves = moves;
                new_moves.push_back(m);
                pq.push({new_f, new_g, std::move(next), std::move(new_moves)});
            }
        }
    }

    // No solution found
    return {{}, nodes};
}
