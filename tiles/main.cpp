
#include <vector>
#include <iostream>

#include "common.hpp"

int main() {

    tiles_t tiles = read_from_cin();

    do_montecarlo(tiles);

}
