#include "cube.h"
#include <cstdlib>
#include <algorithm>
#include <stdexcept>

// ─── Goal State ─────────────────────────────────────────────────
// Face order: U(0-8) R(9-17) F(18-26) D(27-35) L(36-44) B(45-53)
const State GOAL_STATE = "UUUUUUUUURRRRRRRRR"
                         "FFFFFFFFFDDDDDDDDD"
                         "LLLLLLLLLBBBBBBBBB";

// ─── Move Names ─────────────────────────────────────────────────
const std::array<std::string, NUM_MOVES> MOVE_NAMES = {
    "R", "R'", "R2",
    "U", "U'", "U2",
    "F", "F'", "F2",
    "L", "L'", "L2",
    "D", "D'", "D2",
    "B", "B'", "B2"
};

// ─── Inverse Moves ──────────────────────────────────────────────
// R↔R', R2↔R2, U↔U', etc.
const std::array<int, NUM_MOVES> INVERSE_MOVE = {
    MOVE_R_PRIME, MOVE_R, MOVE_R2,     // R, R', R2
    MOVE_U_PRIME, MOVE_U, MOVE_U2,     // U, U', U2
    MOVE_F_PRIME, MOVE_F, MOVE_F2,     // F, F', F2
    MOVE_L_PRIME, MOVE_L, MOVE_L2,     // L, L', L2
    MOVE_D_PRIME, MOVE_D, MOVE_D2,     // D, D', D2
    MOVE_B_PRIME, MOVE_B, MOVE_B2      // B, B', B2
};

// ─── Helper: Build Permutation Tables ───────────────────────────
//
// For each move we store (from, to) pairs meaning:
//     new_state[to] = old_state[from]
//
// Face CW rotation (viewed from outside):
//     Corners: (b+0→b+2)(b+2→b+8)(b+8→b+6)(b+6→b+0)
//     Edges:   (b+1→b+5)(b+5→b+7)(b+7→b+3)(b+3→b+1)
//
// Face layout (each face, viewed from outside):
//     b+0  b+1  b+2
//     b+3  b+4  b+5
//     b+6  b+7  b+8
//
// Cube net (for adjacency reference):
//              U
//           0  1  2
//           3  4  5
//           6  7  8
//     L        F        R        B
//  36 37 38  18 19 20   9 10 11  45 46 47
//  39 40 41  21 22 23  12 13 14  48 49 50
//  42 43 44  24 25 26  15 16 17  51 52 53
//              D
//          27 28 29
//          30 31 32
//          33 34 35

static MovePerm face_cw(int base) {
    return {
        {base+0, base+2}, {base+2, base+8}, {base+8, base+6}, {base+6, base+0},
        {base+1, base+5}, {base+5, base+7}, {base+7, base+3}, {base+3, base+1}
    };
}

static MovePerm face_ccw(int base) {
    return {
        {base+2, base+0}, {base+8, base+2}, {base+6, base+8}, {base+0, base+6},
        {base+5, base+1}, {base+7, base+5}, {base+3, base+7}, {base+1, base+3}
    };
}

static MovePerm face_180(int base) {
    return {
        {base+0, base+8}, {base+8, base+0}, {base+2, base+6}, {base+6, base+2},
        {base+1, base+7}, {base+7, base+1}, {base+3, base+5}, {base+5, base+3}
    };
}

static MovePerm concat(const MovePerm& a, const MovePerm& b) {
    MovePerm result;
    result.reserve(a.size() + b.size());
    result.insert(result.end(), a.begin(), a.end());
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

// ─── Build All 18 Move Permutations ─────────────────────────────
//
// Adjacent ring cycles derived from physical cube geometry.
// Each 4-cycle is written as 4 (from,to) pairs.
//
// R CW: F→U→B→D→F ring (right column of each adjacent face)
//   Cycle 1: F(20)→U(2)→B(51)→D(29)→F(20)
//   Cycle 2: F(23)→U(5)→B(48)→D(32)→F(23)
//   Cycle 3: F(26)→U(8)→B(45)→D(35)→F(26)

static MovePerm R_ring_cw() {
    return {
        {20, 2},  {2, 51},  {51, 29}, {29, 20},
        {23, 5},  {5, 48},  {48, 32}, {32, 23},
        {26, 8},  {8, 45},  {45, 35}, {35, 26}
    };
}
static MovePerm R_ring_ccw() {
    return {
        {2, 20},  {51, 2},  {29, 51}, {20, 29},
        {5, 23},  {48, 5},  {32, 48}, {23, 32},
        {8, 26},  {45, 8},  {35, 45}, {26, 35}
    };
}
static MovePerm R_ring_180() {
    return {
        {20, 51}, {51, 20}, {2, 29},  {29, 2},
        {23, 48}, {48, 23}, {5, 32},  {32, 5},
        {26, 45}, {45, 26}, {8, 35},  {35, 8}
    };
}

// U CW: F→R→B→L→F ring (top row of each adjacent face)
//   But B is flipped: F(18,19,20)→R(9,10,11)→B(47,46,45)→L(36,37,38)
//   Cycle 1: F(18)→R(9)→B(47)→L(36)→F(18)   (left corners)
//   Cycle 2: F(19)→R(10)→B(46)→L(37)→F(19)   (centers)
//   Cycle 3: F(20)→R(11)→B(45)→L(38)→F(20)   (right corners)

static MovePerm U_ring_cw() {
    return {
        {18, 9},  {9, 47},  {47, 36}, {36, 18},
        {19, 10}, {10, 46}, {46, 37}, {37, 19},
        {20, 11}, {11, 45}, {45, 38}, {38, 20}
    };
}
static MovePerm U_ring_ccw() {
    return {
        {9, 18},  {47, 9},  {36, 47}, {18, 36},
        {10, 19}, {46, 10}, {37, 46}, {19, 37},
        {11, 20}, {45, 11}, {38, 45}, {20, 38}
    };
}
static MovePerm U_ring_180() {
    return {
        {18, 47}, {47, 18}, {9, 36},  {36, 9},
        {19, 46}, {46, 19}, {10, 37}, {37, 10},
        {20, 45}, {45, 20}, {11, 38}, {38, 11}
    };
}

// F CW: U→R→D→L→U ring (bottom row of U, left col of R, top row of D, right col of L)
//   Cycle 1: U(6)→R(9)→D(29)→L(44)→U(6)    -- wait, need to verify
//   Let me trace: F CW from front perspective.
//   U bottom row (6,7,8) → R left col (9,12,15)  but reversed: U(6)→R(12), U(7)→R(9)? NO.
//
//   Correct cycle (verified with physical cube):
//   U(6)→R(9), U(7)→R(12), U(8)→R(15)    -- wrong, not reversed
//   Actually: U(6)→L(44), U(7)→L(41), U(8)→L(38) for CCW... 
//
//   Physical tracing for F CW:
//   - UFL corner: U(6), F(18), L(44) → UFR corner position: U(8), F(20), R(9)
//     U(6)→R(9)? No — U sticker (facing up) stays facing... let me think about orientation.
//     F CW rotates around the F-B axis. Up→Right, Right→Down, Down→Left, Left→Up.
//     So U(6) (facing up) → now faces right = R(9)? 
//     L(44) (facing left) → now faces up = U(8)?
//     But we also need R→D and D→L.
//
//   Complete F CW ring:
//     U(6)→R(9),  R(9)→D(29),  D(29)→L(44), L(44)→U(6)  -- corners UFL
//     U(7)→R(12), R(12)→D(28), D(28)→L(41), L(41)→U(7)  -- edges UF
//     U(8)→R(15), R(15)→D(27), D(27)→L(38), L(38)→U(8)  -- corners UFR
//
//   Wait, let me reverify. D's top row viewed from front:
//   D(27,28,29) where D(27) is below F(24) (left side) and D(29) is below F(26) (right side).
//   After F CW: R left col top (R(9)) → D right side. R(9) is at top of R's left col = DRF? No, URF.
//   
//   Let me just carefully trace each corner/edge:
//
//   F CW (looking at F from front): pieces cycle UFL→UFR→DFR→DFL→UFL
//
//   UFL corner: U(6), F(18), L(44)
//   UFR corner: U(8), F(20), R(9)
//   DFR corner: D(29), F(26), R(15)
//   DFL corner: D(27), F(24), L(42)
//
//   UFL → UFR: 
//     U(6) [up] → R(9) [right]   (up→right for F CW)
//     L(44) [left] → U(8) [up]   (left→up for F CW)
//     F(18) → F(20) [face rotation — already covered]
//
//   UFR → DFR:
//     U(8) [up] → R(15) [right]  (up→right for F CW)
//     R(9) [right] → D(29) [down] (right→down for F CW)
//     F(20) → F(26) [face rotation]
//
//   DFR → DFL:
//     R(15) [right] → D(27) [down]  (right→down for F CW)
//     D(29) [down] → L(42) [left]   (down→left for F CW)
//     F(26) → F(24) [face rotation]
//
//   DFL → UFL:
//     D(27) [down] → L(44) [left]   (down→left for F CW)
//     L(42) [left] → U(6) [up]      (left→up for F CW)
//     F(24) → F(18) [face rotation]
//
//   So corners:
//     U(6)→R(9), R(9)→D(29), D(29)→L(42), L(42)→U(6)     -- WRONG earlier, L(42) not L(44)
//     L(44)→U(8), U(8)→R(15), R(15)→D(27), D(27)→L(44)
//
//   Wait, that gives me two separate 4-cycles for the corners. Let me recheck.
//   UFL → UFR means:
//     The piece at UFL goes to UFR position.
//     At UFL: stickers U(6), F(18), L(44)
//     At UFR: stickers need to fill U(8), F(20), R(9)
//     U(6) [was up, now right] → goes to R(9)
//     L(44) [was left, now up] → goes to U(8)
//     F(18) [was front, stays front] → goes to F(20) [face CW rotation]
//
//   UFR → DFR:
//     At UFR: U(8), F(20), R(9)
//     At DFR: D(29), F(26), R(15)
//     U(8) [was up, but this was FILLED by L(44) from prev step...] 
//
//   No wait, I'm confusing "where sticker goes" with "what fills a position".
//   Let me track ORIGINAL stickers through the chain.
//
//   Original sticker at U(6) [UFL corner, up-facing]:
//     UFL → UFR: U(6) goes to R(9)  [up→right]
//   Original sticker at R(9) [UFR corner, right-facing]:
//     UFR → DFR: R(9) goes to D(29)  [right→down]
//   Original sticker at D(29) [DFR corner, down-facing]:
//     DFR → DFL: D(29) goes to L(42)  [down→left]
//   Original sticker at L(42) [DFL corner, left-facing]:
//     DFL → UFL: L(42) goes to U(6)  [left→up]
//   
//   4-cycle: U(6)→R(9)→D(29)→L(42)→U(6) ✓
//
//   Original sticker at L(44) [UFL corner, left-facing]:
//     UFL → UFR: L(44) goes to U(8)  [left→up]
//   Original sticker at U(8) [UFR corner, up-facing]:
//     UFR → DFR: U(8) goes to R(15)  [up→right]
//   Original sticker at R(15) [DFR corner, right-facing]:
//     DFR → DFL: R(15) goes to D(27)  [right→down]
//   Original sticker at D(27) [DFL corner, down-facing]:
//     DFL → UFL: D(27) goes to L(44)  [down→left]
//
//   4-cycle: L(44)→U(8)→R(15)→D(27)→L(44) ✓
//
//   Edges:
//   UF edge: U(7), F(19) → RF edge position: R(12), F(23)
//   RF edge: R(12), F(23) → DF edge position: D(28), F(25)
//   DF edge: D(28), F(25) → LF edge position: L(41), F(22) [wait, F(21)?]
//   LF edge: L(41), F(21) → UF edge position: U(7), F(19)
//
//   Wait, F face edge positions:
//     F(19) = top-center (UF edge)
//     F(23) = middle-right (RF edge) 
//     F(25) = bottom-center (DF edge)
//     F(21) = middle-left (LF edge)
//
//   U(7) → R(12): [up→right]
//   R(12) → D(28): [right→down]
//   D(28) → L(41): [down→left]
//   L(41) → U(7):  [left→up]
//
//   4-cycle: U(7)→R(12)→D(28)→L(41)→U(7) ✓

static MovePerm F_ring_cw() {
    return {
        {6, 9},   {9, 29},  {29, 42}, {42, 6},
        {44, 8},  {8, 15},  {15, 27}, {27, 44},
        {7, 12},  {12, 28}, {28, 41}, {41, 7}
    };
}
static MovePerm F_ring_ccw() {
    return {
        {9, 6},   {29, 9},  {42, 29}, {6, 42},
        {8, 44},  {15, 8},  {27, 15}, {44, 27},
        {12, 7},  {28, 12}, {41, 28}, {7, 41}
    };
}
static MovePerm F_ring_180() {
    return {
        {6, 29},  {29, 6},  {9, 42},  {42, 9},
        {44, 15}, {15, 44}, {8, 27},  {27, 8},
        {7, 28},  {28, 7},  {12, 41}, {41, 12}
    };
}

// L CW: U→F→D→B ring (left column of U/F/D, right column of B)
//   L CW (looking at L from outside/left):
//   Pieces cycle: UBL → UFL → DFL → DBL → UBL
//
//   For L CW, the axis rotation gives: up→front, front→down, down→back, back→up
//
//   UBL corner: U(0), L(36), B(53)
//   UFL corner: U(6), L(44), F(18)
//   DFL corner: D(27), L(42), F(24)
//   DBL corner: D(33), L(36→wait..
//
//   Wait, let me re-identify corner stickers for L face:
//   L face (viewed from outside/left):
//     36 37 38    ← top row (adjacent to U)
//     39 40 41
//     42 43 44    ← bottom row (adjacent to D)
//   L(36) = top-left of L from outside = UBL corner on L face
//   L(38) = top-right of L from outside = UFL corner on L face
//   L(42) = bottom-left of L from outside = DBL corner on L face
//   L(44) = bottom-right of L from outside = DFL corner on L face
//
//   UBL corner: U(0), L(36), B(53)
//   UFL corner: U(6), L(38), F(18)
//   DFL corner: D(27), L(44), F(24)
//   DBL corner: D(33), L(42), B(47)
//
//   L CW cycle: UBL → UFL → DFL → DBL → UBL
//
//   U(0) [up→front]: U(0) → F(18)
//   F(18) [front→down]: F(18) → D(27)
//   D(27) [down→back]: D(27) → B(53)
//   B(53) [back→up]: B(53) → U(0)
//   4-cycle: U(0)→F(18)→D(27)→B(53)→U(0) ✓
//
//   B(53) traces through: B(53)→U(0) [back→up]
//   L(36) → L(38): [face CW rotation, already covered]
//   
//   Second corner cycle:
//   U(6) → F(24): [up→front, UFL→DFL]
//   F(24) → D(33): [front→down, DFL→DBL]
//   D(33) → B(47): [down→back, DBL→UBL]
//   B(47) → U(6):  [back→up, UBL→UFL]
//   
//   Wait: UFL→DFL means the piece goes DOWN, and:
//     L(38) [left] → L(44) [left — face rotation]
//     U(6) [up] → F(24) [front — up→front] Hmm...
//   
//   Actually for UBL → UFL → DFL → DBL, tracing individual stickers:
//     U(0) at UBL [up-facing]: UBL→UFL, up stays up? No... 
//     For L CW: up→front, front→down, down→back, back→up
//     U(0) [up] → F(18) [front]. But UBL goes to UFL, and at UFL the front-facing sticker is F(18). ✓
//
//   Let me redo more carefully:
//   UBL → UFL:
//     U(0) [up → front] → F(18)
//     B(53) [back → up] → U(6)
//   UFL → DFL:
//     U(6) [up → front] → F(24)
//     F(18) [front → down] → D(27)
//   DFL → DBL:
//     F(24) [front → down] → D(33)
//     D(27) [down → back] → B(53)
//   DBL → UBL:
//     D(33) [down → back] → B(47)
//     B(53) [back → up] → U(0)  — wait, B(53) is at DBL, but after DBL→UBL it should go to U(0)
//     Hmm, but B(53) is at the UBL corner initially, not DBL.
//
//   Let me restart L corners more carefully.
//   
//   L face corners (L viewed from left/outside):
//     L(36)=TL, L(38)=TR, L(42)=BL, L(44)=BR
//   
//   Adjacencies:
//   L's top connects to U's left column: U(0), U(3), U(6)
//   L's right connects to F's left column: F(18), F(21), F(24)
//   L's bottom connects to D's left column: D(27), D(30), D(33)
//   L's left connects to B's right column: B(47), B(50), B(53)
//
//   Corner pieces adjacent to L:
//   L(36) TL → adjacent to U(0) and B(53):    UBL corner {U(0), L(36), B(53)}
//   L(38) TR → adjacent to U(6) and F(18):    UFL corner {U(6), L(38), F(18)}
//   L(44) BR → adjacent to D(27) and F(24):   DFL corner {D(27), L(44), F(24)}
//   L(42) BL → adjacent to D(33) and B(47):   DBL corner {D(33), L(42), B(47)}
//
//   L CW rotation moves pieces: 
//   Looking at L from outside: TL→TR→BR→BL→TL
//   So: UBL→UFL→DFL→DBL→UBL ✓
//
//   Tracking non-L stickers through UBL→UFL→DFL→DBL→UBL:
//   
//   For the up/front/down/back stickers:
//   U(0) at UBL → goes to UFL, was up, L CW makes up→front: → F(18)
//   U(6) at UFL → goes to DFL, was up, up→front: → F(24)
//   So these form separate chains... Let me just track cycles:
//
//   Cycle A: U(0)→F(18)→D(27)→B(53)→U(0)
//     U(0) [UBL, up] → UFL, front = F(18) [up→front]
//     F(18) [UFL, front] → DFL, down = D(27) [front→down]
//     D(27) [DFL, down] → DBL, back = B(47)? or B(53)?
//       DFL→DBL: the down-facing sticker at DFL goes to back-facing at DBL.
//       At DBL, B sticker is B(47).
//       So D(27) → B(47)
//     B(47) [DBL, back] → UBL, up = U(0) [back→up]
//
//   Hmm, that gives: U(0)→F(18)→D(27)→B(47)→U(0). Not B(53).
//
//   Cycle B: B(53)→U(6)→F(24)→D(33)→B(53)
//     B(53) [UBL, back] → UFL, up = U(6)? 
//       Wait: B(53) is at UBL corner. UBL→UFL. B(53) was back-facing. back→up for L CW: → U(6)
//     U(6) [UFL, up] → DFL, front = F(24)
//       up→front: U(6) → F(24) ✓
//     F(24) [DFL, front] → DBL, down = D(33)
//       front→down: F(24) → D(33) ✓
//     D(33) [DBL, down] → UBL, back = B(53)
//       down→back: D(33) → B(53) ✓
//
//   So two corner cycles:
//     U(0)→F(18)→D(27)→B(47)→U(0)
//     B(53)→U(6)→F(24)→D(33)→B(53)
//
//   Edge cycle:
//     UL edge: U(3), L(37) (L top-center)
//     FL edge: F(21), L(41) (L middle-right)
//     DL edge: D(30), L(43) (L bottom-center)
//     BL edge: B(50), L(39) (L middle-left)
//
//     U(3)→F(21)→D(30)→B(50)→U(3)
//       U(3) [up→front] → F(21)
//       F(21) [front→down] → D(30)
//       D(30) [down→back] → B(50)
//       B(50) [back→up] → U(3)

static MovePerm L_ring_cw() {
    return {
        {0, 18},  {18, 27}, {27, 47}, {47, 0},
        {53, 6},  {6, 24},  {24, 33}, {33, 53},
        {3, 21},  {21, 30}, {30, 50}, {50, 3}
    };
}
static MovePerm L_ring_ccw() {
    return {
        {18, 0},  {27, 18}, {47, 27}, {0, 47},
        {6, 53},  {24, 6},  {33, 24}, {53, 33},
        {21, 3},  {30, 21}, {50, 30}, {3, 50}
    };
}
static MovePerm L_ring_180() {
    return {
        {0, 27},  {27, 0},  {18, 47}, {47, 18},
        {53, 24}, {24, 53}, {6, 33},  {33, 6},
        {3, 30},  {30, 3},  {21, 50}, {50, 21}
    };
}

// D CW: F→L→B→R→F ring (bottom row of F, right col of L... etc.)
//   D CW (looking at D from outside/bottom):
//   The rotation around D's axis (up-down) from below: front→left, left→back, back→right, right→front
//
//   Adjacent rows:
//   F bottom row: F(24), F(25), F(26)
//   R bottom row: R(15), R(16), R(17)
//   B bottom row: B(51), B(52), B(53)
//   L bottom row: L(42), L(43), L(44)
//
//   D CW from below: F→L→B→R→F
//   But B row is reversed relative to others (like U ring with B).
//
//   DFL corner: D(27), F(24), L(44)
//   DFR corner: D(29), F(26), R(15)
//   DBR corner: D(35), B(51), R(17)
//   DBL corner: D(33), B(53), L(42)
//
//   D CW from below: DFL→DFR→DBR→DBL→DFL? No...
//   Looking at D from below (outside):
//      27 28 29     ← nearest to F (front)
//      30 31 32
//      33 34 35     ← nearest to B (back)
//   
//   CW from below: the front row sweeps RIGHT (from below perspective).
//   But from below, right=cube's LEFT and left=cube's RIGHT!
//   
//   Actually this is getting confusing. Let me just think:
//   D CW means rotating the bottom layer clockwise when viewed from below.
//   Equivalently, from above it looks CCW.
//   
//   From above (CCW): front→left, left→back, back→right, right→front
//   Wait no — from BELOW CW looks like CW from below.
//   Looking UP at D from below:
//     The face I see has front-of-cube at top of my view.
//     CW from my view (below): front→right(from below)=left(from above)
//   
//   OK this is getting circular. Let me just use the convention that
//   D CW (viewed from outside/below) = F→L→B→R→F for the bottom ring.
//   This is the standard convention matching U CW = F→R→B→L→F.
//
//   F(24)→L(44), F(25)→L(43), F(26)→L(42)   -- F bottom → L bottom (reversed)
//   Hmm, is L's bottom row reversed relative to F?
//
//   Actually, let me use symmetry. U CW: F(top)→R(top)→B(top,rev)→L(top)→F
//   D CW: F(bot)→L(bot)→B(bot,rev)→R(bot)→F
//   (D is opposite to U, so the cycle direction reverses)
//
//   F bottom row L→R: 24, 25, 26
//   L bottom row L→R: 42, 43, 44
//   B bottom row L→R: 51, 52, 53  (but reversed = 53, 52, 51)
//   R bottom row L→R: 15, 16, 17
//
//   D CW: F→L→B(rev)→R→F
//     F(24)→L(42), F(25)→L(43), F(26)→L(44)
//     L(42)→B(53), L(43)→B(52), L(44)→B(51)
//     B(53)→R(17), B(52)→R(16), B(51)→R(15)
//     R(17)→F(26), R(16)→F(25), R(15)→F(24)
//
//   Wait, that doesn't look right. For U CW, the cycle was:
//     F(18)→R(9), F(19)→R(10), F(20)→R(11)
//     R(9)→B(47), R(10)→B(46), R(11)→B(45)
//     B(47)→L(36), B(46)→L(37), B(45)→L(38)
//     L(36)→F(18), L(37)→F(19), L(38)→F(20)
//   Cycle: F→R→B(rev)→L→F, where B is accessed in reverse (47,46,45 vs 45,46,47)
//
//   By symmetry, D CW should be: F→L→B(rev)→R→F
//   But let me verify by checking a corner.
//   D CW from below: DFL→DBL→DBR→DFR→DFL (same TL→TR→BR→BL pattern as face CW)
//
//   DFL: D(27), F(24), L(44)
//   DBL: D(33), B(53), L(42) 
//   DBR: D(35), B(51), R(17)
//   DFR: D(29), F(26), R(15)
//
//   DFL→DBL: F(24)[front→left]→L(42)? 
//   For D CW from below: front→right_from_below = left_from_above
//   Hmm, let me just think physically. Looking at the bottom of cube from below:
//   CW from below: front edge goes to the right (as seen from below) = LEFT of cube from above.
//   So: front→left(above), left(above)→back, back→right(above), right(above)→front
//   
//   DFL→DBL: sticker F(24) [front-facing] → now faces left(from above) = L(42)?
//     At DBL, the L sticker is L(42). ✓
//   DFL→DBL: sticker L(44) [left-facing] → now faces front = at DBL???
//     Wait, DBL doesn't have a front-facing sticker. 
//     For D CW: left→back (from above perspective).
//     L(44) [left] → back. At DBL, back-facing = B(53). ✓
//
//   So: F(24)→L(42), L(44)→B(53)
//
//   DBL→DBR: B(53) [back] → right(above). At DBR, R sticker = R(17). ✓
//             L(42) [left] → back? No: left→back only for the first step.
//             Actually, for each step in the cycle (D CW from below):
//             front→left(above)→back→right(above)→front
//   
//   Sticker at B(53) [DBL, back-facing]:
//     DBL→DBR: back→right_from_above. At DBR, right = R(17). ✓ B(53)→R(17)
//   Sticker at L(42) [DBL, left-facing]: 
//     DBL→DBR: left→front? No... 
//     For D CW from below: front→left, left→back, back→right, right→front
//     L(42) [left] → back-facing at DBR. At DBR, back = B(51). ✓ L(42)→B(51)
//
//   DBR→DFR: R(17) [right] → front. At DFR, front = F(26). ✓ R(17)→F(26)
//            B(51) [back] → right. At DFR, right = R(15). ✓ B(51)→R(15)
//
//   DFR→DFL: F(26) [front] → left. At DFL, left = L(44). ✓ F(26)→L(44)
//            R(15) [right] → front. At DFL, front = F(24). ✓ R(15)→F(24)
//
//   Corner cycles:
//     F(24)→L(42)→B(51)→R(15)→F(24)   -- wait, that's not right
//     Let me collect: F(24)→L(42), L(42)→B(51), B(51)→R(15), R(15)→F(24) ✓
//     And: L(44)→B(53), B(53)→R(17), R(17)→F(26), F(26)→L(44) ✓
//
//   Edge cycle:
//     DF edge: F(25), D(28)
//     DL edge: L(43), D(30)
//     DB edge: B(52), D(34)
//     DR edge: R(16), D(32)
//     
//     F(25)→L(43)→B(52)→R(16)→F(25)

static MovePerm D_ring_cw() {
    return {
        {24, 42}, {42, 51}, {51, 15}, {15, 24},
        {44, 53}, {53, 17}, {17, 26}, {26, 44},
        {25, 43}, {43, 52}, {52, 16}, {16, 25}
    };
}
static MovePerm D_ring_ccw() {
    return {
        {42, 24}, {51, 42}, {15, 51}, {24, 15},
        {53, 44}, {17, 53}, {26, 17}, {44, 26},
        {43, 25}, {52, 43}, {16, 52}, {25, 16}
    };
}
static MovePerm D_ring_180() {
    return {
        {24, 51}, {51, 24}, {42, 15}, {15, 42},
        {44, 17}, {17, 44}, {53, 26}, {26, 53},
        {25, 52}, {52, 25}, {43, 16}, {16, 43}
    };
}

// B CW: U→L→D→R→U ring (top row of U, left col of L, bottom row of D, right col of R)
//   B CW (looking at B from outside/behind):
//   For B CW: the axis goes front-back. From B's perspective looking inward:
//   up→left_from_behind=right_from_front, etc.
//   Actually: B CW from behind means up→right(from behind)=left(from front), etc.
//   
//   Equivalently, B CW = inverse direction of F CW for the ring:
//   F CW ring: up→right, right→down, down→left, left→up
//   B CW ring: up→left(from front), left→down, down→right(from front), right→up
//
//   B face corners (viewed from behind/outside):
//     45 46 47     ← top (adjacent to U)
//     48 49 50
//     51 52 53     ← bottom (adjacent to D)
//   
//   UBR corner: U(2), B(45), R(11)
//   UBL corner: U(0), B(47), L(36)
//   DBL corner: D(33), B(53), L(42)
//   DBR corner: D(35), B(51), R(17)
//
//   B CW from behind: UBR→UBL→DBL→DBR→UBR (TL→TR→BR→BL but from behind view)
//   Wait: B face from outside (behind), CW:
//     B(45)=TL → B(47)=TR → B(53)=BR → B(51)=BL → B(45)
//     So corners: UBR→UBL→DBL→DBR→UBR? Let me check.
//     B(45) is at TL when viewed from behind = UBR from front. 
//     B(47) is at TR from behind = UBL from front. ✓
//     So: UBR→UBL→DBL→DBR→UBR... that seems like CCW from front view. ✓ (B CW from behind = CCW from front)
//
//   Tracking stickers for B CW:
//   For B rotation, axis is front-back. B CW (from behind):
//   up→right(behind)=left(front), left(front)→down, down→left(behind)=right(front), right(front)→up
//   So: up→left(front), left→down, down→right(front), right→up
//
//   UBR→UBL:
//     U(2) [up → left_from_front]: → L(36)
//     R(11) [right_from_front → up]: → U(0)
//   UBL→DBL:
//     U(0) [up → left]: → L(42)    wait, that's wrong because U(0) came from R(11)
//     Let me track ORIGINAL stickers.
//   
//   Original U(2) [UBR, up-facing]:
//     UBR→UBL: up→left(front): U(2) → L(36)
//   Original L(36) [UBL, left-facing]:
//     UBL→DBL: left→down: L(36) → D(33)
//   Original D(33) [DBL, down-facing]:
//     DBL→DBR: down→right(front): D(33) → R(17)
//   Original R(17) [DBR, right-facing]:
//     DBR→UBR: right→up: R(17) → U(2)
//   
//   Cycle: U(2)→L(36)→D(33)→R(17)→U(2) ✓
//
//   Original R(11) [UBR, right-facing]:
//     UBR→UBL: right→up: R(11) → U(0)
//   Original U(0) [UBL, up-facing]:
//     UBL→DBL: up→left: U(0) → L(42)
//   Original L(42) [DBL, left-facing]:
//     DBL→DBR: left→down: L(42) → D(35)
//   Original D(35) [DBR, down-facing]:
//     DBR→UBR: down→right: D(35) → R(11)
//
//   Cycle: R(11)→U(0)→L(42)→D(35)→R(11) ✓
//
//   Edge cycle:
//     UB edge: U(1), B(46)
//     LB edge: L(39), B(50)  [wait: L middle-left when viewed from outside, but from L's perspective...]
//     Actually: B's left col connects to R, B's right col connects to L.
//     No wait: from the net, B is to the right of R. But when folded...
//     
//     From the net: B(47,50,53) right col connects to... nothing in the net directly,
//     but when folded, B's right col (47,50,53) connects to L's left col (36,39,42).
//     And B's left col (45,48,51) connects to R's right col (11,14,17).
//
//   UB edge: U(1), B(46)
//   BL(from front) edge: L(39), B(50)
//   DB edge: D(34), B(52)
//   BR(from front) edge: R(14), B(48)
//
//   From the corner cycle direction (up→left→down→right→up):
//   U(1)→L(39)→D(34)→R(14)→U(1) ✓

static MovePerm B_ring_cw() {
    return {
        {2, 36},  {36, 33}, {33, 17}, {17, 2},
        {11, 0},  {0, 42},  {42, 35}, {35, 11},
        {1, 39},  {39, 34}, {34, 14}, {14, 1}
    };
}
static MovePerm B_ring_ccw() {
    return {
        {36, 2},  {33, 36}, {17, 33}, {2, 17},
        {0, 11},  {42, 0},  {35, 42}, {11, 35},
        {39, 1},  {34, 39}, {14, 34}, {1, 14}
    };
}
static MovePerm B_ring_180() {
    return {
        {2, 33},  {33, 2},  {36, 17}, {17, 36},
        {11, 42}, {42, 11}, {0, 35},  {35, 0},
        {1, 34},  {34, 1},  {39, 14}, {14, 39}
    };
}

// ─── Assemble the MOVE_TABLE ────────────────────────────────────

static std::array<MovePerm, NUM_MOVES> build_move_table() {
    std::array<MovePerm, NUM_MOVES> table;

    // R, R', R2
    table[MOVE_R]       = concat(face_cw(R_BASE),  R_ring_cw());
    table[MOVE_R_PRIME] = concat(face_ccw(R_BASE), R_ring_ccw());
    table[MOVE_R2]      = concat(face_180(R_BASE), R_ring_180());

    // U, U', U2
    table[MOVE_U]       = concat(face_cw(U_BASE),  U_ring_cw());
    table[MOVE_U_PRIME] = concat(face_ccw(U_BASE), U_ring_ccw());
    table[MOVE_U2]      = concat(face_180(U_BASE), U_ring_180());

    // F, F', F2
    table[MOVE_F]       = concat(face_cw(F_BASE),  F_ring_cw());
    table[MOVE_F_PRIME] = concat(face_ccw(F_BASE), F_ring_ccw());
    table[MOVE_F2]      = concat(face_180(F_BASE), F_ring_180());

    // L, L', L2
    table[MOVE_L]       = concat(face_cw(L_BASE),  L_ring_cw());
    table[MOVE_L_PRIME] = concat(face_ccw(L_BASE), L_ring_ccw());
    table[MOVE_L2]      = concat(face_180(L_BASE), L_ring_180());

    // D, D', D2
    table[MOVE_D]       = concat(face_cw(D_BASE),  D_ring_cw());
    table[MOVE_D_PRIME] = concat(face_ccw(D_BASE), D_ring_ccw());
    table[MOVE_D2]      = concat(face_180(D_BASE), D_ring_180());

    // B, B', B2
    table[MOVE_B]       = concat(face_cw(B_BASE),  B_ring_cw());
    table[MOVE_B_PRIME] = concat(face_ccw(B_BASE), B_ring_ccw());
    table[MOVE_B2]      = concat(face_180(B_BASE), B_ring_180());

    return table;
}

const std::array<MovePerm, NUM_MOVES> MOVE_TABLE = build_move_table();

// ─── Core Functions ─────────────────────────────────────────────

State apply_move(const State& s, int move_id) {
    if (move_id < 0 || move_id >= NUM_MOVES) {
        throw std::out_of_range("Invalid move_id: " + std::to_string(move_id));
    }
    State next = s;
    for (const auto& [from, to] : MOVE_TABLE[move_id]) {
        next[to] = s[from];
    }
    return next;
}

State generate_scramble(int depth, int seed) {
    std::srand(seed);
    State state = GOAL_STATE;
    int last_move_base = -1;  // avoid consecutive same-face moves

    for (int i = 0; i < depth; i++) {
        int move_id;
        do {
            move_id = std::rand() % NUM_MOVES;
        } while (move_id / 3 == last_move_base);  // same face group = /3
        state = apply_move(state, move_id);
        last_move_base = move_id / 3;
    }
    return state;
}

std::string move_to_string(int move_id) {
    if (move_id < 0 || move_id >= NUM_MOVES) return "?";
    return MOVE_NAMES[move_id];
}

int get_inverse_move(int move_id) {
    if (move_id < 0 || move_id >= NUM_MOVES) return -1;
    return INVERSE_MOVE[move_id];
}

bool is_solved(const State& s) {
    return s == GOAL_STATE;
}

bool is_valid_state(const State& s) {
    if (static_cast<int>(s.size()) != NUM_STICKERS) return false;
    int counts[256] = {};
    for (char c : s) counts[(unsigned char)c]++;
    // Each face char should appear exactly 9 times
    return counts['U'] == 9 && counts['R'] == 9 && counts['F'] == 9 &&
           counts['D'] == 9 && counts['L'] == 9 && counts['B'] == 9;
}
