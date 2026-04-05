#include "cube.h"
#include "heuristics.h"
#include <iostream>

int main() {
    State s = GOAL_STATE;
    s = apply_move(s, MOVE_R);
    s = apply_move(s, MOVE_L);
    int raw = misplaced_stickers_raw(s, GOAL_STATE);
    int h = misplaced_cubies(s, GOAL_STATE);
    std::cout << "2 moves (R L): raw=" << raw << ", h=" << h << std::endl;
    return 0;
}
