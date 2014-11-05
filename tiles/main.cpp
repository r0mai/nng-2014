
#include <vector>
#include <iostream>

int main() {
    unsigned rows, columns;

    std::cin >> rows >> columns;

    std::vector<std::vector<int>> tiles(columns, std::vector<int>(rows));

    for (unsigned y = 0; y < rows; ++y) {
        for (unsigned x = 0; x < columns; ++x) {
            std::cin >> tiles[x][y];
        }
    }

}
