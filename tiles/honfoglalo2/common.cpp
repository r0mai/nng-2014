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
            std::cout << "\033[" << (4 >> ( n.csoport->csopID/7 % 3 ) ) << ";" << n.csoport->csopID%7 + 31 <<"m";
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

void rendez(Areas& areas)
{
    auto begin0 = areas[0].hozzam_rendelt.begin();
    auto begin1 = areas[1].hozzam_rendelt.begin();
    auto begin2 = areas[2].hozzam_rendelt.begin();
    auto end0 = areas[0].hozzam_rendelt.end();
    auto end1 = areas[1].hozzam_rendelt.end();
    auto end2 = areas[2].hozzam_rendelt.end();


    while( begin0 < end0 || begin1 < end1 )
    {
        while ( begin0 < end0 && (*begin0)->ertek == 0 ) ++begin0;
        while ( begin1 < end1 && (*begin1)->ertek == 1 ) ++begin1;
        while ( begin2 < end2 && (*begin2)->ertek == 2 ) ++begin2;
        if( begin0 < end0 && (*begin0)->ertek == 1 )
        {
            auto it = begin1;
            while( it < end1 && (*it)->ertek != 0 ) ++it;
            if( it == end1 )
            {
                my_swap( begin0 , it = begin1 );
            }
            else
            {
                my_swap( begin0, it );
                ++begin0;
            }
            if( it == begin1 ) ++begin1;
        }
        else if( begin0 < end0 && (*begin0)->ertek == 2 )
        {
            auto it = begin2;
            while( it < end2 && (*it)->ertek != 0 ) ++it;
            if( it == end2 )
            {
                my_swap( begin0 , it = begin2 );
            }
            else
            {
                my_swap( begin0, it );
                ++begin0;
            }
            if( it == begin2 ) ++begin2;
        }
        else if( begin1 < end1 && begin2 < end2 )
        {
            my_swap( begin1 , begin2 );
            begin1++;
            begin2++;
        }
        else
        {
            break;
        }
    }
}
