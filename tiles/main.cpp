
#include <ctime>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "common.hpp"

int main(int argc, char **argv) {

    std::srand(std::time(0));

    if (argc < 2) {
        std::cerr << "Usage : ./tiles naive|graph|mc|bit|cell|fromto" << std::endl;
        return 1;
    }


    if (argv[1] == std::string("naive")) {
        tiles_t tiles = read_from();
        do_naive(tiles);
    } else if (argv[1] == std::string("graph")) {
        tiles_t tiles = read_from();
        do_graph(tiles);
    } else if (argv[1] == std::string("mc")) {
        tiles_t tiles = read_from();
        do_montecarlo(tiles);
    } else if (argv[1] == std::string("bit")) {
        tiles_t tiles = read_from();
        do_bit_magic(tiles);
    } else if (argv[1] == std::string("cell")) {
        tiles_t tiles = read_from();
        do_cellular(tiles);
    } else if (argv[1] == std::string("fromto")) {
        if (argc != 4) {
            std::cerr << "Usage ./tiles fromto from_file to_file" << std::endl;
            return 1;
        }
        std::ifstream fromf(argv[2]);
        std::ifstream tof(argv[3]);

        print_swaps(do_from_to(read_from(fromf), read_from(tof)));
    } else {
        std::cerr << "Unknown method" << std::endl;
        return 1;
    }
}
