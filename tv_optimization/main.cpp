
#include <map>
#include <tuple>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

struct Programme {
    std::string tv;
    unsigned begin, length;
};

bool operator<(const Programme& lhs, const Programme& rhs) {
    return lhs.begin + lhs.length < rhs.begin + rhs.length;
}

typedef std::vector<Programme> Programs;

Programs programs;

int main() {

    typedef std::vector<Programme> Programs;

    Programs programs;

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

    /*
     * Beware! Bela's ingenuity coming up!
     */

    // parcellaveghez  hozzarendelunk egy eddigi max osszeget + egy palyazok-szama vectort
    std::map<unsigned, std::pair<unsigned, std::vector<unsigned>>, std::greater<unsigned>> s;

    s[0] = std::make_pair(0 , std::vector<unsigned>());

    for (unsigned i = 0; i < programs.size(); ++i)
    {
        unsigned a = programs[i].begin;
        unsigned b = a + programs[i].length;
        unsigned f = programs[i].length;
        auto prev = s.upper_bound( a );
        if ( s.begin()->second.first < prev->second.first + f )
        {
            std::vector<unsigned> cpy = prev->second.second;
            cpy.push_back(i);
            s[b] = std::make_pair( prev->second.first + f , cpy );
        }
    }
    std::cout << s.begin()->second.first << std::endl;
    for (auto x : s.begin()->second.second)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}
