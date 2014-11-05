#include <boost/multiprecision/cpp_int.hpp>
typedef boost::multiprecision::cpp_int US;
typedef unsigned long long UL;
typedef std::vector< UL > VUS;

US res;
VUS ns;

int main()
{
    while ( std::cin >> res )
    {
        ns.push_back( res.convert_to<UL>() );
    }

    ns.pop_back();

    UL max = 1 << (ns.size() - 1);
    std::cout << max << " "  << (ns.size() - 1) << std::endl;
    for( UL i = 0 ; i < max ; ++i )
    {
        std::cout << ns[0];
        for( UL j = 1 ; j < ns.size() ; ++j )
        {
            std::cout << " " << (((i >> (j-1)) & 1)?'+':'*') << " " << ns[j];
        }
        std::cout << std::endl;
    }
}
