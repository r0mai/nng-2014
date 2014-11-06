
#include <ctime>
#include <vector>
#include <cstdlib>
#include <iostream>

#include "common.hpp"

int main() {

    std::srand(std::time(0));

    tiles_t tiles = read_from_cin();

    do_montecarlo(tiles);

}
