#include "solver/cube.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int depth = 5;
    int seed = 42;
    if (argc > 1) depth = std::stoi(argv[1]);
    if (argc > 2) seed = std::stoi(argv[2]);
    
    std::cout << generate_scramble(depth, seed) << std::endl;
    return 0;
}
