#include "idastar.h"
#include <algorithm>
#include <climits>
#include <chrono>

// ─── Sentinel Values ────────────────────────────────────────────
static constexpr int FOUND = -1;

// ─── Time Cap ───────────────────────────────────────────────────
static constexpr double TIME_CAP_SECONDS = 60.0;

// ─── Move Pruning: opposite faces ───────────────────────────────
// Two moves on opposite faces commute (R then L = L then R).
// To avoid exploring both orderings, if last move was on the
// "higher" face of an opposite pair, skip moves on the "lower" face.
//   R(0) <-> L(3), U(1) <-> D(4), F(2) <-> B(5)
// Face groups: R=0, U=1, F=2, L=3, D=4, B=5
static bool should_skip_move(int move_id, int last_move) {
    if (last_move < 0) return false;
    int face = move_id / 3;     // 0=R, 1=U, 2=F, 3=L, 4=D, 5=B
    int last_face = last_move / 3;

    // Same face: always skip (R then R' = identity)
    if (face == last_face) return true;

    // Opposite faces: enforce ordering (lower face number first)
    // R(0) & L(3), U(1) & D(4), F(2) & B(5)
    if ((face == 0 && last_face == 3) || (face == 3 && last_face == 0)) {
        return face > last_face;
    }
    if ((face == 1 && last_face == 4) || (face == 4 && last_face == 1)) {
        return face > last_face;
    }
    if ((face == 2 && last_face == 5) || (face == 5 && last_face == 2)) {
        return face > last_face;
    }

    return false;
}

// ─── IDA* Recursive Search ──────────────────────────────────────
//
//  Memory analysis:
//    - `path` vector: only current DFS path (max depth d states)
//    - `moves` vector: only current move sequence (max d ints)
//    - No visited set, no priority queue, no hash map
//    - Total memory: O(d) where d = solution depth
//    - For Rubik's Cube: ~20 states × 54 bytes ≈ 1 KB
//
static bool timed_out = false;

static int ida_search(
    std::vector<State>& path,
    std::vector<int>& moves,
    int g,
    int threshold,
    const State& goal,
    HeuristicFn& h,
    int& nodes,
    const std::chrono::steady_clock::time_point& start_time)
{
    const State node = path.back();  // copy — survives push_back reallocation
    int f = g + h(node, goal);

    // ── PRUNE: f exceeds current threshold ──
    if (f > threshold) return f;

    // ── GOAL CHECK ──
    if (node == goal) return FOUND;

    nodes++;

    // Check time cap periodically
    if (nodes % 50000 == 0) {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start_time).count();
        if (elapsed > TIME_CAP_SECONDS) {
            timed_out = true;
            return INT_MAX;
        }
    }

    int minimum = INT_MAX;

    // Last move for pruning
    int last_move = moves.empty() ? -1 : moves.back();

    for (int m = 0; m < NUM_MOVES; m++) {
        // Skip redundant moves (same face + opposite face ordering)
        if (should_skip_move(m, last_move)) continue;

        State child = apply_move(node, m);

        // ── CYCLE CHECK: is child already on the current path? ──
        // Path is short (≤ ~20), so linear scan is acceptable
        bool on_path = false;
        for (const auto& p : path) {
            if (p == child) { on_path = true; break; }
        }
        if (on_path) continue;

        // ── RECURSE ──
        path.push_back(child);
        moves.push_back(m);

        int result = ida_search(path, moves, g + 1, threshold,
                                goal, h, nodes, start_time);

        if (result == FOUND) return FOUND;
        if (timed_out) return INT_MAX;
        minimum = std::min(minimum, result);

        // ── BACKTRACK — this is why memory stays O(d) ──
        path.pop_back();
        moves.pop_back();
    }

    return minimum;
}

// ─── IDA* Outer Loop ────────────────────────────────────────────
SolveResult idastar_solve(const State& start, const State& goal, HeuristicFn h) {
    if (start == goal) return {{}, 0};

    timed_out = false;
    auto start_time = std::chrono::steady_clock::now();

    int threshold = h(start, goal);
    std::vector<State> path = {start};
    std::vector<int> moves;
    int nodes = 0;

    while (true) {
        int result = ida_search(path, moves, 0, threshold,
                                goal, h, nodes, start_time);

        if (result == FOUND) {
            return {moves, nodes};
        }

        if (result == INT_MAX || timed_out) {
            return {{}, nodes};  // no solution or timeout
        }

        // ── RAISE THRESHOLD ──
        threshold = result;

        // Check time between iterations
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start_time).count();
        if (elapsed > TIME_CAP_SECONDS) {
            return {{}, nodes};
        }
    }
}
