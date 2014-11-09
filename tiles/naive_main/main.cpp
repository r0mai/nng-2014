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
    std::cin >> x >> y;
    VUL m(x*y);

    VUL db(3);

    for( auto& z : m )
    {
        std::cin >> z;
        ++db[z];
    }
    db[2] += db[1] += db[0];

    UL i = 0;
    UL j = db[0];
    UL k = db[2] - 1;

    while( i < db[0] || j < db[1] || k >= db[1] )
    {
        while( i < db[0] && m[i]==0 ) ++i;
        while( j < db[1] && m[j]==1 ) ++j;
        while( k >= db[1] && m[k]==2 ) --k;

        if( i < db[0] && m[i] == 1 )
        {
            UL l = j;
            while( m[l] != 0 ) ++l;

            std::swap( m[i], m[l] );
            csere(i,l);

            if( j == l )
                ++j;
            ++i;
        }
        else if( i < db[0] && m[i] == 2 )
        {
            UL l = k;
            while( m[l] != 0 ) --l;

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
    std::ofstream ofx("tiles_out_checker_" + std::to_string( count ) + ".out");
    UL u = 0;
    for( auto z : m )
    {
        ofx << z << " ";
        if( ++u % y == 0 ) ofx << std::endl;
    }
    std::cout << count << std::endl << ss.str();

    return 0;
}
