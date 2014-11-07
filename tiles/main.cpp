
#include <ctime>
#include <vector>
#include <cstdlib>
#include <iostream>

#include "common.hpp"

int main(int argc, char **argv) {

    std::srand(std::time(0));

    if (argc != 2) {
        std::cerr << "Usage : ./tiles naive|graph|mc|bit" << std::endl;
        return 1;
    }

    tiles_t tiles = read_from_cin();

    if (argv[1] == std::string("naive")) {
        do_naive(tiles);
    } else if (argv[1] == std::string("graph")) {
        do_graph(tiles);
    } else if (argv[1] == std::string("mc")) {
        do_montecarlo(tiles);
    } else if (argv[1] == std::string("bit")) {
        do_bit_magic(tiles);
    } else {
        std::cerr << "Unknown method" << std::endl;
        return 1;
    }
}
