
#include <map>
#include <tuple>
#include <vector>
#include <cassert>
#include <iostream>

#include <boost/optional.hpp>

struct Programme {
    std::string tv;
    unsigned begin, length;
};

bool operator<(const Programme& lhs, const Programme& rhs) {
    return lhs.begin < rhs.begin;
}

struct Mem {
    std::vector<bool> hasIt;
    unsigned coverage = 0;
};

typedef std::vector<Programme> Programs;

Programs programs;

Mem createMem() { Mem mem; mem.hasIt.resize(programs.size()); return mem; }

unsigned getCoverage(const std::vector<bool>& hasIt) {
    assert(hasIt.size() == programs.size());

    unsigned coverage = 0;

    for (unsigned i = 0; i < hasIt.size(); ++i) {
        if (hasIt[i]) {
            coverage += programs[i].length;
        }
    }

    return coverage;
}

bool overlap(unsigned i, unsigned j) {
    unsigned b1 = programs[i].begin;
    unsigned e1 = programs[i].begin + programs[i].length;
    unsigned b2 = programs[j].begin;
    unsigned e2 = programs[j].begin + programs[j].length;
    return e1 > b2 && e2 > b1;
}

Mem combine(const Mem& mem, unsigned n) {
    assert(!mem.hasIt[n]);

    Mem result = mem;

    for (unsigned i = 0; i < programs.size(); ++i) {
        if (!mem.hasIt[i]) {
            continue;
        }
        if (overlap(i, n)) {
            return result;
        }
    }

    result.hasIt[n] = true;
    result.coverage += programs[n].length;

    assert(getCoverage(result.hasIt) == result.coverage);

    return result;
}

std::vector<std::vector<boost::optional<Mem>>> memoize;

Mem coverage(unsigned i, unsigned j) {
    if (i == j) {
        return createMem();
    }
    if (i+1 == j) {
        Mem result = createMem();
        result.hasIt[i] = true;
        result.coverage = programs[i].length;
        return result;
    }

    Mem max = createMem();
    for (unsigned k = i; k < j; ++k) {
    }

}

int main() {

    std::string name;
    while (std::cin >> name) {
        unsigned n;
        std::cin >> n;

        for (unsigned i = 0; i < n; ++i) {
            programs.push_back(Programme{name});
            std::cin >> programs.back().begin >> programs.back().length;
        }
    }

    std::sort(programs.begin(), programs.end());


}
