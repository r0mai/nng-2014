#include <iostream>
#include <utility>

#include <memory>
#include <vector>

typedef unsigned long UL;
typedef std::vector <UL> VUL;
typedef std::vector<VUL> MUL;
typedef std::pair< UL , UL > Coord;

int main()
{
    UL x,y;
    std::cin >> x >> y;

    MUL m( x , VUL(y) );
    VUL g(3);
    for( auto& x : m )
    {
        for( auto& y : x)
        {
            std::cin >> y;
            ++g[y];
            //std::cout << y << " ";
        }
        //std::cout << std::endl;
    }
    //std::cout << std::endl;
    std::cout << "information" << std::endl 
            << "0 db:" << g[0] << std::endl 
            << "1 db:" << g[1] << std::endl 
            << "2 db:" << g[2] << std::endl; 

    UL c;
    std::cin >> c;

    for( UL i = 0; i < c ; ++i )
    {
        UL q,w,e,r;
        std::cin >> q >> w >> e >> r;
        std::swap( m[q][w] , m[e][r] );
    }

    for( auto& x : m )
    {
        for( auto& y : x)
        {
            std::cout << y << " ";
        }
        std::cout << std::endl;
    }


    std::vector<std::vector<std::shared_ptr<UL> > > sets( x , std::vector<std::shared_ptr<UL> >(y) );

    UL count = 1;
    UL uniq = 1;
    sets[0][0] = std::shared_ptr<UL>(new UL(0));

    for( UL j = 1 ; j < y ; ++j )
    {
        sets[0][j] = m[0][j] == m[0][j-1] ? sets[0][j-1] : ( ++count , std::shared_ptr<UL>(new UL(uniq++)) );
    }
    for( UL i = 1; i < x ; ++i )
    {
        sets[i][0] = m[i][0] == m[i-1][0] ? sets[i-1][0] : ( ++count , std::shared_ptr<UL>(new UL(uniq++)) );

        for( UL j = 1 ; j < y ; ++j )
        {
            bool yprev = m[i][j] == m[i][j-1];
            bool xprev = m[i][j] == m[i-1][j];
            if( yprev && xprev )
            {
                sets[i][j] = sets[i][j-1];

                if( *sets[i][j-1] != *sets[i-1][j] )
                {
                    *sets[i][j-1] = *sets[i-1][j];
                    --count; // WARNING!!
                }
            }
            else if( yprev )
            {
                sets[i][j] = sets[i][j-1];
            }
            else if( xprev )
            {
                sets[i][j] = sets[i-1][j];
            }
            else
            {  
                ++count;
                sets[i][j] = std::shared_ptr<UL>(new UL(uniq++));
            }
        }
    }
    sets.clear();

    std::cout << "count : " << count << std::endl;

    std::cout << "is ok? :" << std::boolalpha << (count == 3) << std::endl;
    return 0;
}
