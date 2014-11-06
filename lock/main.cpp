#include <iostream>
#include <vector>
#include <unordered_map>
#include <boost/multiprecision/cpp_int.hpp>

typedef boost::multiprecision::cpp_int US;

typedef unsigned long Index;
typedef std::vector< US > VUS;
typedef std::vector<VUS > MUS;

typedef std::unordered_map< Index , std::pair< US , US > > UNS;
typedef std::vector<  UNS > VUNS;
typedef std::vector< VUNS > MUNS;

MUS m;
US res;
VUS ns;
US counter = 0;
MUNS x;

std::string chars;

bool doit( Index i , Index j , US max = res )
{   
    if( i == ns.size() )
    {
        return true;
    }
    for( auto n : x[i][j] )
    {
        if( n.second.first <= max && max <= n.second.second )
        {
            if ( doit( j + 1 , n.first , max - m[i][j] ) )
            {
                chars += std::string( j - i , '*' ) + '+';
                std::cerr << "multiply from " << i << " to " << j << " =  " << m[i][j]  << std::endl; 
                return true;
            }
        }
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
    x.resize( ns.size() , VUNS( ns.size() ) );


    for( Index i = 0; i < ns.size() ; ++i )
    {
        m[i][i] = ns[i];
        for( Index j = i + 1 ; j < ns.size() ; ++j )
        {
            m[i][j] = m[i][j-1] * ns[j];
        }
        x[i][ns.size()-1].insert( std::make_pair ( ns.size() , std::make_pair( m[i][ns.size()-1] , m[i][ns.size()-1] ) ) );
    }

    for( Index j = ns.size() - 1 ; j > 0 ; )
    {
        --j;
        for( Index i = 0 ; i <= j ; ++i )
        {
            for( Index sj = j + 1 ; sj < ns.size() ; ++sj )
            {
                auto it = std::begin( x[j+1][sj] );
                US min = it->second.first;
                US max = it->second.second;
                for( ++it ; it != std::end( x[j+1][sj] ) ; ++it )
                {
                    if( it->second.first < min )
                        min = it->second.first;
                    if( it->second.second > max )
                        max = it->second.second;

                }
                min += m[i][j];
                max += m[i][j];
                x[i][j].insert( std::make_pair ( sj , std::make_pair( min, max ) ) );
                //std::cout << i <<" " << j << " " << sj <<" min:" << min << " max:" << max << std::endl;

            }
        }
    }

    for( Index j = ns.size() - 1 ; j > 0 ; )
    {
        --j;
        std::cerr << j << std::endl;
        if ( doit( 0 , j ) ) break;
    }

    for( std::string::reverse_iterator rit = chars.rbegin() + 1 ; rit != chars.rend() ; ++rit )
    {
        std::cout << *rit << " ";
    }
    std::cout << std::endl;
    return 0;
}
