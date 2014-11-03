
#include <vector>
#include <iostream>

struct Programme {
    unsigned begin, length;
};

struct Tv {
    std::string name;
    std::vector<Programme> programs;
};

int main() {

    typedef std::vector<Tv> Tvs;

    Tvs tvs;

    std::string name;
    while (std::cin >> name) {
        unsigned n;
        std::cin >> n;

        std::vector<Programme> programs(n);

        for (unsigned i = 0; i < n; ++i) {
            std::cin >> programs[i].begin >> programs[i].length;
        }

        tvs.push_back(Tv{name, programs});
    }
    std::cout << "out" << std::endl;
}
