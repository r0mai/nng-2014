#include "common.h"

int main()
{
    Termeszetes x,y,tmp;
    std::cin >> x >> y;
    NodeMatrix nodeMat( x , Nodes(y) );
    Groups groups;
    groups.reserve( x*y );
    Areas areas;
    areas.reserve( 3 );
    Ertekek db(3);

    for( Termeszetes i = 0; i < x ; ++i )
    {
        for( Termeszetes j = 0; j < y ; ++j )
        {
            std::cin >> tmp;
            ++db[tmp];
            nodeMat[i][j].beallit( std::make_pair( i , j ) , tmp );

            if( i > 0 )
            {
                nodeMat[i][j].szomszed.push_back( &nodeMat[i-1][j] );
                nodeMat[i-1][j].szomszed.push_back( &nodeMat[i][j] );
            }
            if( j < y-1 )
            {
                nodeMat[i][j].szomszed.push_back( &nodeMat[i][j+1] );
                nodeMat[i][j+1].szomszed.push_back( &nodeMat[i][j] );
            }
            if( i > 0 && j > 0)
            {
                nodeMat[i][j].atlos_szomszed.push_back( &nodeMat[i-1][j-1] );
                nodeMat[i-1][j-1].atlos_szomszed.push_back( &nodeMat[i][j] );
            }
            if( i > 0 && j < y-1 )
            {
                nodeMat[i][j].atlos_szomszed.push_back( &nodeMat[i-1][j+1] );
                nodeMat[i-1][j+1].atlos_szomszed.push_back( &nodeMat[i][j] );
            }
        }
    }

    for( Termeszetes i = 0; i < x ; ++i )
    {
        for( Termeszetes j = 0; j < y ; ++j )
        {
            if( nodeMat[i][j].csoport == NULL )
            {
                groups.push_back( Group( nodeMat[i][j].ertek , groups.size() ) );
                nodeMat[i][j].csoportosit( &groups.back() );
            }
        }
    }
    kiir( nodeMat );
}
