#include "cube.h"
#include "bfs.h"
#include "astar.h"
#include "idastar.h"
#include "heuristics.h"

#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <cstdlib>

// getrusage for peak memory measurement
#include <sys/resource.h>

// ─── JSON Helpers ───────────────────────────────────────────────

static std::string moves_to_json(const std::vector<int>& moves) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < moves.size(); i++) {
        if (i > 0) oss << ",";
        oss << "\"" << move_to_string(moves[i]) << "\"";
    }
    oss << "]";
    return oss.str();
}

// ─── Usage ──────────────────────────────────────────────────────

static void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog
              << " --state <54-char-string>"
              << " --algo <bfs|astar|idastar>"
              << " [--heuristic <misplaced|manhattan>]"
              << " [--scramble <depth>]"
              << " [--seed <int>]"
              << std::endl;
}

// ─── Main ───────────────────────────────────────────────────────
//
// FastAPI reads ONLY stdout. All errors go to stderr.
// Stdout must be a single line of valid JSON:
//   {"moves":["R","U'"],"nodes":123,"time_ms":4.5,"mem_kb":380}

int main(int argc, char* argv[]) {
    std::string state;
    std::string algo;
    std::string heuristic_name = "misplaced";
    int scramble_depth = -1;
    int seed = 42;

    // ── Parse Arguments ──
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--state" && i + 1 < argc) {
            state = argv[++i];
        } else if (arg == "--algo" && i + 1 < argc) {
            algo = argv[++i];
        } else if (arg == "--heuristic" && i + 1 < argc) {
            heuristic_name = argv[++i];
        } else if (arg == "--scramble" && i + 1 < argc) {
            scramble_depth = std::atoi(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = std::atoi(argv[++i]);
        } else if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        }
    }

    // ── Generate scramble if requested ──
    if (scramble_depth > 0) {
        state = generate_scramble(scramble_depth, seed);
    }

    // ── Validate inputs ──
    if (state.empty()) {
        std::cerr << "Error: --state or --scramble required" << std::endl;
        print_usage(argv[0]);
        return 1;
    }
    if (!is_valid_state(state)) {
        std::cerr << "Error: invalid state (need 54 chars of URFDLB)" << std::endl;
        return 1;
    }
    if (algo.empty()) {
        std::cerr << "Error: --algo required (bfs, astar, idastar)" << std::endl;
        return 1;
    }

    // ── Solve ──
    SolveResult result;

    auto start_time = std::chrono::high_resolution_clock::now();

    if (algo == "bfs") {
        result = bfs_solve(state, GOAL_STATE);
    } else if (algo == "astar") {
        HeuristicFn h = get_heuristic(heuristic_name);
        result = astar_solve(state, GOAL_STATE, h);
    } else if (algo == "idastar") {
        HeuristicFn h = get_heuristic(heuristic_name);
        result = idastar_solve(state, GOAL_STATE, h);
    } else {
        std::cerr << "Error: unknown algo '" << algo
                  << "' (use bfs, astar, idastar)" << std::endl;
        return 1;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    // ── Measure time ──
    double time_ms = std::chrono::duration<double, std::milli>(
        end_time - start_time).count();

    // ── Measure peak memory (KB) ──
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    long mem_kb = usage.ru_maxrss;  // KB on Linux, bytes on macOS

    // ── Output JSON to stdout ──
    // FastAPI reads this via subprocess.stdout
    std::cout << "{"
              << "\"moves\":" << moves_to_json(result.moves) << ","
              << "\"nodes\":" << result.nodes << ","
              << "\"time_ms\":" << time_ms << ","
              << "\"mem_kb\":" << mem_kb
              << "}" << std::endl;

    return 0;
}
