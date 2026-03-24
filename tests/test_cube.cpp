#include "cube.h"
#include <iostream>
#include <cassert>

// ─── Test Helpers ───────────────────────────────────────────────

static int tests_passed = 0;
static int tests_failed = 0;

void check(bool condition, const std::string& name) {
    if (condition) {
        std::cout << "  ✓ " << name << std::endl;
        tests_passed++;
    } else {
        std::cout << "  ✗ FAIL: " << name << std::endl;
        tests_failed++;
    }
}

// ─── Tests ──────────────────────────────────────────────────────

void test_goal_state() {
    std::cout << "\n[Goal State]" << std::endl;
    check(GOAL_STATE.size() == 54, "GOAL_STATE has 54 chars");
    check(is_solved(GOAL_STATE), "GOAL_STATE is_solved");
    check(is_valid_state(GOAL_STATE), "GOAL_STATE is_valid_state");
}

void test_apply_move_inverse() {
    std::cout << "\n[Inverse Property: M then M' == identity]" << std::endl;

    // Test with GOAL state
    for (int base = 0; base < 6; base++) {
        int cw   = base * 3;       // R, U, F, L, D, B
        int ccw  = base * 3 + 1;   // R', U', F', L', D', B'
        int half = base * 3 + 2;   // R2, U2, F2, L2, D2, B2

        std::string face = MOVE_NAMES[cw];

        // CW then CCW = identity
        State after_cw  = apply_move(GOAL_STATE, cw);
        State restored  = apply_move(after_cw, ccw);
        check(restored == GOAL_STATE,
              face + " then " + face + "' on GOAL == GOAL");

        // CCW then CW = identity
        State after_ccw = apply_move(GOAL_STATE, ccw);
        State restored2 = apply_move(after_ccw, cw);
        check(restored2 == GOAL_STATE,
              face + "' then " + face + " on GOAL == GOAL");

        // Half turn twice = identity
        State after_h1  = apply_move(GOAL_STATE, half);
        State after_h2  = apply_move(after_h1, half);
        check(after_h2 == GOAL_STATE,
              face + "2 twice on GOAL == GOAL");
    }

    // Test with a SCRAMBLED state (more rigorous — catches index collisions)
    std::cout << "\n[Inverse Property on SCRAMBLED state]" << std::endl;
    State scrambled = generate_scramble(10, 42);
    check(scrambled != GOAL_STATE, "scramble(10) != GOAL");
    check(is_valid_state(scrambled), "scrambled state is valid");

    for (int base = 0; base < 6; base++) {
        int cw   = base * 3;
        int ccw  = base * 3 + 1;
        int half = base * 3 + 2;
        std::string face = MOVE_NAMES[cw];

        State after_cw  = apply_move(scrambled, cw);
        State restored  = apply_move(after_cw, ccw);
        check(restored == scrambled,
              face + " then " + face + "' on scrambled == scrambled");

        State after_h1  = apply_move(scrambled, half);
        State after_h2  = apply_move(after_h1, half);
        check(after_h2 == scrambled,
              face + "2 twice on scrambled == scrambled");
    }
}

void test_cw_four_times_is_identity() {
    std::cout << "\n[4× CW == identity]" << std::endl;
    State scrambled = generate_scramble(8, 99);

    for (int base = 0; base < 6; base++) {
        int cw = base * 3;
        std::string face = MOVE_NAMES[cw];
        State s = scrambled;
        for (int i = 0; i < 4; i++) s = apply_move(s, cw);
        check(s == scrambled, face + " ×4 == identity");
    }
}

void test_move_changes_state() {
    std::cout << "\n[Each move changes the state]" << std::endl;
    for (int m = 0; m < NUM_MOVES; m++) {
        State after = apply_move(GOAL_STATE, m);
        check(after != GOAL_STATE,
              MOVE_NAMES[m] + " changes GOAL state");
    }
}

void test_generate_scramble() {
    std::cout << "\n[Scramble generation]" << std::endl;
    State s1 = generate_scramble(5, 42);
    State s2 = generate_scramble(5, 42);
    State s3 = generate_scramble(5, 99);

    check(s1 == s2, "same seed → same scramble");
    check(s1 != s3, "different seed → different scramble");
    check(is_valid_state(s1), "scramble has valid sticker counts");
    check(!is_solved(s1), "scramble is not solved");
}

void test_move_to_string() {
    std::cout << "\n[move_to_string]" << std::endl;
    check(move_to_string(MOVE_R) == "R", "MOVE_R → \"R\"");
    check(move_to_string(MOVE_U_PRIME) == "U'", "MOVE_U_PRIME → \"U'\"");
    check(move_to_string(MOVE_F2) == "F2", "MOVE_F2 → \"F2\"");
}

void test_r2_equals_r_r() {
    std::cout << "\n[R2 == R then R]" << std::endl;
    State scrambled = generate_scramble(7, 55);
    for (int base = 0; base < 6; base++) {
        int cw   = base * 3;
        int half = base * 3 + 2;
        std::string face = MOVE_NAMES[cw];

        State via_half = apply_move(scrambled, half);
        State via_two  = apply_move(apply_move(scrambled, cw), cw);
        check(via_half == via_two,
              face + "2 == " + face + " then " + face);
    }
}

// ─── Main ───────────────────────────────────────────────────────

int main() {
    std::cout << "═══════════════════════════════════════════" << std::endl;
    std::cout << " Rubik's Cube State Model — Test Suite" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;

    test_goal_state();
    test_apply_move_inverse();
    test_cw_four_times_is_identity();
    test_move_changes_state();
    test_generate_scramble();
    test_move_to_string();
    test_r2_equals_r_r();

    std::cout << "\n═══════════════════════════════════════════" << std::endl;
    std::cout << " Results: " << tests_passed << " passed, "
              << tests_failed << " failed" << std::endl;
    std::cout << "═══════════════════════════════════════════" << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
