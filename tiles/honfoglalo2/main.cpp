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

    std::vector<NodePtr> beginNs( 3 );
    Ertekek maxx( 3 );
    int i = 0;
    for( auto gr : groups )
    {
        if( maxx[gr.ertek] < gr.size() )
        {
            if( ++i % 2 )
            {
                beginNs[gr.ertek] = gr.tagok[gr.size()/2];
                maxx[gr.ertek] = gr.size();
            }
        }
    }
    areas.push_back( Area( beginNs[0] , db[0] ) );
    areas.push_back( Area( beginNs[1] , db[1] ) );
    areas.push_back( Area( beginNs[2] , db[2] ) );


    for( Termeszetes i = 0;
        areas[0].vege() &&
        areas[1].vege() &&
        areas[3].vege() ; ++i %= 3)
    {
        if( !areas[i].lepes() )
        {
            Ertek kitol = areas[i].lop();
            areas[kitol].lepes();
        }
    };
    kiir( nodeMat );
    rendez( areas );
    kiir( nodeMat );

    std::cout << "db : " << count << std::endl;
    std::ofstream ofs("ki_"+std::to_string(count)+".out");

    ofs << count << std::endl << ss.str();

}
