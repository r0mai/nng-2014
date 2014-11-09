#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

typedef unsigned long UL;
typedef std::vector<UL> VUL;

UL x,y;

UL count = 0;
std::stringstream ss;

void csere(UL e , UL k)
{
    //std::cout << e <<" " << k << std::endl;
    ++count;
    ss << e/y << " " << e%y << " " << k/y << " " << k%y << std::endl;
}


int main()
{
    std::cerr << "ADJ EGY PERMUTACIOT A BEMENET ELOTT!! (pl 1 0 2)" << std::endl;
    UL p1,p2,p3;
    std::cin >> p1 >> p2 >> p3;
    VUL permback{ p1,p2,p3 };
    VUL permofthree{ permback[p1],permback[p2],permback[p3] };

    std::cin >> x >> y;
    VUL m(x*y);

    VUL db(3);

    for( auto& z : m )
    {
        std::cin >> z;
        ++db[permback[z]];
    }
    db[2] += db[1] += db[0];

    UL i = 0;
    UL j = db[0];
    UL k = db[2] - 1;

    while( i < db[0] || j < db[1] || k >= db[1] )
    {
        //std::cout << "ittene: " << i <<" " << j <<" " << k << std::endl;
        while( i < db[0] && m[i]==permofthree[0] ) ++i;
        while( j < db[1] && m[j]==permofthree[1] ) ++j;
        while( k >= db[1] && m[k]==permofthree[2] ) --k;
        //std::cout << "ittenv: " << i <<" " << j <<" " << k << std::endl;

        if( i < db[0] && m[i] == permofthree[1] )
        {
            UL l = j;
            while( m[l] != permofthree[0] ) ++l;

            std::swap( m[i], m[l] );
            csere(i,l);

            if( j == l )
                ++j;
            ++i;
        }
        else if( i < db[0] && m[i] == permofthree[2] )
        {
            UL l = k;
            while( m[l] != permofthree[0] ) --l;

            std::swap( m[i], m[l] );
            csere(i,l);

            if( k == l )
                --k;
            ++i;
        }
        else if( j < db[1] )
        {
            std::swap( m[j], m[k] );
            csere(j,k);

            --k;
            ++j;
        }
    }
    std::ofstream ofx("test/tiles_out_checker_" + std::to_string( count ) + ".out");
    UL u = 0;
    for( auto z : m )
    {
        ofx << z << " ";
        if( ++u % y == 0 ) ofx << std::endl;
    }
    std::cout << count << std::endl << ss.str();

    return 0;
}
