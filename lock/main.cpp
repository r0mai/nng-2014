#include <iostream>
#include <vector>

typedef unsigned long long US;
typedef unsigned long Index;
typedef std::vector< US > VUS;
typedef std::vector<VUS > MUS;

MUS m;
US res;
VUS ns;

bool req( Index x , Index y , US sum )
{
    if( y >= m.size() ) return false;

    sum += m[x][y];

    if( sum > res ) return false;
    if( sum == res && y == m.size() - 1 ) return true;

    if( req( y + 1 , y + 1 , sum ) )
    {
        std::cout << "+ ";
        return true;
    }
    if( req( x , y + 1 , sum ) )
    {
        std::cout << "* ";
        return true;
    }
    return false;
}

int main()
{
    while ( std::cin >> res )
    {
        ns.push_back( res );
    }
    res = ns.back();
    ns.pop_back();

    m.resize( ns.size() , VUS( ns.size() ) );
    for( Index i = 0; i < ns.size() ; ++i )
    {
        m[i][i] = ns[i];
        for( Index j = i + 1 ; j < ns.size() ; ++j )
        {
            m[i][j] = m[i][j-1] * ( ns[j] - 1 );
        }
    }
    req( 0 , 0 , 0 );
    return 0;
}
