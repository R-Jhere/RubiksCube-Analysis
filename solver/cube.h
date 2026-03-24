#ifndef CUBE_H
#define CUBE_H

#include <string>
#include <vector>
#include <array>
#include <utility>

// ─── Type Aliases ───────────────────────────────────────────────
using State = std::string;  // 54-char string representing all 6 faces
using MovePerm = std::vector<std::pair<int, int>>;  // (from, to) pairs

// ─── Constants ──────────────────────────────────────────────────
// Face order: U(0-8) R(9-17) F(18-26) D(27-35) L(36-44) B(45-53)
// Each face sticker layout (viewed from outside):
//   0 1 2
//   3 4 5
//   6 7 8

constexpr int NUM_FACES    = 6;
constexpr int FACE_SIZE    = 9;
constexpr int NUM_STICKERS = 54;
constexpr int NUM_MOVES    = 18;

// Face base indices
constexpr int U_BASE = 0;
constexpr int R_BASE = 9;
constexpr int F_BASE = 18;
constexpr int D_BASE = 27;
constexpr int L_BASE = 36;
constexpr int B_BASE = 45;

// Move IDs — ordered as: R R' R2 U U' U2 F F' F2 L L' L2 D D' D2 B B' B2
enum MoveID {
    MOVE_R = 0,  MOVE_R_PRIME,  MOVE_R2,
    MOVE_U,      MOVE_U_PRIME,  MOVE_U2,
    MOVE_F,      MOVE_F_PRIME,  MOVE_F2,
    MOVE_L,      MOVE_L_PRIME,  MOVE_L2,
    MOVE_D,      MOVE_D_PRIME,  MOVE_D2,
    MOVE_B,      MOVE_B_PRIME,  MOVE_B2
};

extern const State GOAL_STATE;
extern const std::array<MovePerm, NUM_MOVES> MOVE_TABLE;
extern const std::array<std::string, NUM_MOVES> MOVE_NAMES;
extern const std::array<int, NUM_MOVES> INVERSE_MOVE;

// ─── Functions ──────────────────────────────────────────────────

/// Apply a move to a cube state, returning the new state.
State apply_move(const State& s, int move_id);

/// Generate a scrambled cube state from GOAL by applying `depth` random moves.
/// Uses the given seed for reproducibility.
State generate_scramble(int depth, int seed = 42);

/// Convert a move ID to its human-readable name (e.g. "R", "U'", "F2").
std::string move_to_string(int move_id);

/// Get the inverse move ID for a given move.
int get_inverse_move(int move_id);

/// Check if a state is the solved (goal) state.
bool is_solved(const State& s);

/// Validate that a state string has exactly 54 chars with valid face chars.
bool is_valid_state(const State& s);

#endif // CUBE_H
