#include <unordered_map>
#include <iostream>
#include <vector>
#include <valarray>
#include <sstream>
#include <fstream>

typedef unsigned long UL;
typedef std::pair<UL,UL> PUL;
typedef std::valarray<UL> VUL;

UL count = 0;
std::stringstream ss;

template<typename T>
void my_swap( T& a , T& b )
{
    std::swap((*a)->ertek,(*b)->ertek);
    ++count;
    ss << (*a)->koord.first << " " << (*a)->koord.second << " "
        << (*b)->koord.first << " " << (*b)->koord.second << std::endl;
}

struct Node;

struct Group
{
    std::vector < Node* > tagok;
    UL ertek;
    UL csopID;

    Group( UL ert , UL csopid ):ertek(ert),csopID(csopid){}

    inline UL size() const { return tagok.size(); }

};

struct Node
{
    PUL koord;
    UL ertek;
    UL marked;
    Group* csoport;
    bool osztalyoz;
    std::vector<Node*> szomszed;
    std::vector<Node*> atlos_szomszed;

    std::vector<Node*> rendezett_szomszed;
    Node():marked(-1),csoport(NULL),osztalyoz(false){}

    bool out()
    {
        return szomszed.size() == 3 || szomszed.size() == 2;
    }

    bool neigh_mine( UL mark )
    {
        if( !out() ) return true;
        for( auto sz : szomszed )
        {
            if( sz->out() && sz->marked == mark ) return true;
        }
        return false;
    }

    bool least_three_of_mine( UL mark )
    {
        UL res = 0;
        for( auto sz : szomszed )
        {
            if( sz->marked == mark ) ++res;
        }
        return !tobbhely( mark ) && res > 2;
    }

    bool tobbhely( UL mark )
    {
        if( rendezett_szomszed.empty() )
        {
            rendezett_szomszed = szomszed;
            rendezett_szomszed.insert( rendezett_szomszed.end() , atlos_szomszed.begin() , atlos_szomszed.end() );
            std::sort( rendezett_szomszed.begin() , rendezett_szomszed.end() ,
                [& ,this](Node* f , Node* s)
                {
                    PUL f_s{ f->koord.first - koord.first + 1 , f->koord.second - koord.second + 1 };
                    UL f_e = f_s.second >= f_s.first ? f_s.second + f_s.first : 8 - (f_s.second + f_s.first);
                    PUL s_s{ s->koord.first - koord.first + 1 , s->koord.second - koord.second + 1 };
                    UL s_e = s_s.second >= s_s.first ? s_s.second + s_s.first : 8 - (s_s.second + s_s.first);
                    return  f_e < s_e;
                });
        }
        UL valtoz = 0;
        UL kezdet = rendezett_szomszed.back()->marked;
        for( auto sz : rendezett_szomszed )
        {
            if( kezdet != sz->marked )
            {
                if( kezdet == mark || sz->marked == mark )
                {
                    ++valtoz;
                }
            }
            kezdet = sz->marked;
        }
        return valtoz > 2;
    }

    void grouping(Group* gr)
    {
        csoport = gr;
        gr->tagok.push_back( this );

        for( auto sz : szomszed )
        {
            if( sz->ertek == ertek && sz->csoport == NULL )
            {
                sz->grouping( gr );
            }
        }
    }

    void talal( UL mit , std::unordered_map< Node* , UL >& mapp , UL melyen , UL tole = UL(-1) )
    {
        osztalyoz = true;
        for( auto sz : szomszed )
        {
            if( sz->marked == tole )
            {
                UL suly = sz->osztaly( mit , melyen , tole );
                if( mapp[ sz ] < suly )
                    mapp[ sz ] = suly;
            }
        }
        osztalyoz = true;
    }

    UL osztaly( UL mi_szerint , UL melyen , UL tole )
    {
        if( tole == UL(-1) && marked != UL(-1) ) return 0;

        UL res = (ertek == mi_szerint) ? 200 * csoport->size() : 20;

        if( tole != UL(-1) ) res *= marked == UL(-1) ? 200 * csoport->size() : 3;

        if( melyen )
        {
            osztalyoz = true;
            for( auto sz : szomszed )
            {
                if( sz->osztalyoz == false )
                {
                    res += sz->osztaly( mi_szerint , melyen - 1 , tole ) / 20;
                }
            }
            osztalyoz = false;
        }
        return res;
    }
};

struct Area
{
    UL need;
    UL max;
    bool out;
    Area( Node* beginNode , UL max ):
        need(beginNode->ertek),
        max(max),
        out(beginNode->out()),
        mine{ beginNode }
    {
        beginNode->marked = need;
    }

    bool step(  )
    {
        if(mine.size() == max) return true;

        std::unordered_map<Node*,UL> lehets;

        for( auto it = mine.begin() ; it != mine.end() ; ++it )
        {
            (*it)->talal( need , lehets , 6 );
        }

        UL max = 0;
        Node* max_n = NULL;
        for( auto leh : lehets )
        {
            if( leh.second > max )
            {
                if( leh.first->tobbhely( need ) )
                {
                    //std::cout << need << " OUT BC tobbhely" << leh.first->koord.first << "  - " << leh.first->koord.second << std::endl;
                    continue;
                }
                if( out && !leh.first->neigh_mine( need ) )
                {
                    //std::cout << need << " OUT BC neighb" << leh.first->koord.first << "  - " << leh.first->koord.second << std::endl;
                    continue;
                }

                max_n = leh.first;
                max = leh.second;
            }
        }

        if( max_n == NULL ) return false;
        out|= max_n->out();
        max_n->marked = need;

        mine.push_back( max_n );

        return true;
    }

    bool help( Area& neki )
    {
        std::unordered_map<Node*,UL> lehets;

        for( auto it = mine.begin() ; it != mine.end() ; ++it )
        {
            (*it)->talal( need , lehets , 8 , neki.need );
        }

        UL max = 0;
        Node* max_n = NULL;
        for( auto leh : lehets )
        {
            if( leh.second > max )
            {
                // ++ feltetelek!!!
                if( leh.first->tobbhely( need ) ) continue;
                if( out && !leh.first->neigh_mine( need ) ) continue;
                if( !leh.first->least_three_of_mine( neki.need ) ) continue;

                max_n = leh.first;
                max = leh.second;
            }
        }

        if( max_n == NULL ) return false;

        neki.torol( max_n );
        out|= max_n->out();
        max_n->marked = need;

        mine.push_back( max_n );

        return true;
    }

    void torol( Node* p )
    {
        mine.erase( std::find( mine.begin() , mine.end() , p ) );
    }
    std::vector<Node*> mine;
};
typedef std::vector<Area> Areas;
typedef std::vector<Group> Groups;
typedef std::vector <Node> Nodes;
typedef std::vector <Nodes> Graph;

void kiir( const Graph& g )
{
    for( auto v : g )
    {
        for( auto n : v )
        {
            std::cout << "\033[" << (4 >> ( n.csoport->csopID/7 % 3 ) ) << ";" << n.csoport->csopID%7 + 31 <<"m";

            switch (n.marked)
            {
                case 0:
                    std::cout << "\033[7;31m"; break;
                case 1:
                    std::cout << "\033[7;32m"; break;
                case 2:
                    std::cout << "\033[7;34m"; break;
            }

            std::cout << n.ertek;

            std::cout << "\033[0m" << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    UL x,y;
    std::cin >> x >> y;
    Graph nodeMat( x , Nodes(y ) );
    Groups groups;
    groups.reserve( x*y );
    Areas areas;
    areas.reserve( 3 );
    VUL db(3);

    for( UL i = 0; i < x ; ++i )
    {
        for( UL j = 0; j < y ; ++j )
        {
            std::cin >> nodeMat[i][j].ertek;
            ++db[nodeMat[i][j].ertek];

            nodeMat[i][j].koord = std::make_pair( i , j );
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

    for( UL i = 0; i < x ; ++i )
    {
        for( UL j = 0; j < y ; ++j )
        {
            if( nodeMat[i][j].csoport == NULL )
            {
                groups.push_back( Group( nodeMat[i][j].ertek , groups.size() ) );
                nodeMat[i][j].grouping( &groups.back() );
            }
        }
    }


    std::vector<Node*> beginNs( 3 );
    VUL maxx( 3 );
    for( auto gr : groups )
    {
        if( maxx[gr.ertek] < gr.size() )
        {
            beginNs[gr.ertek] = gr.tagok[0];
            maxx[gr.ertek] = gr.size();
        }
    }
    areas.push_back( Area( beginNs[0] , db[0] ) );
    areas.push_back( Area( beginNs[1] , db[1] ) );
    areas.push_back( Area( beginNs[2] , db[2] ) );

    kiir( nodeMat );
    UL n_it = db.max();

    for( UL i = 0 ; i < n_it ; ++i )
    {
        for( auto it = areas.begin() ; it != areas.end() ; ++it )
        {
            if( !it->step() )
            {
                //std::cout << "SEGITSEG" << std::endl;
                auto it2 = it ;
                while( ++it2 != it )
                {
                    if( it2 == areas.end() ) it2 = areas.begin();
                    if( it->help( *it2 ) )
                    {
                        it2->step();
                        break;
                    }
                }
            }
        }
    //    std::cin.get();
    }
    kiir( nodeMat );

    auto begin0 = areas[0].mine.begin();
    auto begin1 = areas[1].mine.begin();
    auto begin2 = areas[2].mine.begin();
    auto end0 = areas[0].mine.end();
    auto end1 = areas[1].mine.end();
    auto end2 = areas[2].mine.end();


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
    std::cout << std::endl << std::endl << std::endl;
    kiir( nodeMat );
    std::cout << "db : " << count << std::endl;
    std::ofstream ofs("ki_"+std::to_string(count)+".out");

    ofs << count << std::endl << ss.str();
    return 0;
}

