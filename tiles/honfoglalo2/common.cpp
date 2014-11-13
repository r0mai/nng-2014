#include "common.h"

Ertek invalid = static_cast<Ertek>(-1);
Ertek count = 0;
std::stringstream ss;

void kiir( const NodeMatrix& g )
{
    for( auto v : g )
    {
        for( auto n : v )
        {
            //std::cout << "\033[" << (4 >> ( n.csoport->csopID/7 % 3 ) ) << ";" << n.csoport->csopID%7 + 31 <<"m";
            switch (n.foglalt)
            {
                case 0:
                    std::cout << "\033[7;31m"; break;
                case 1:
                    std::cout << "\033[7;32m"; break;
                case 2:
                    std::cout << "\033[7;34m"; break;
            }
            std::cout << n.ertek << "\033[0m" << " ";
        }
        std::cout << std::endl;
    }
}
