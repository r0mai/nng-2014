#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <unordered_map>
#include <iostream>
#include <vector>
#include <valarray>
#include <sstream>
#include <fstream>

struct Node;
struct Group;
struct Area;

typedef unsigned long Termeszetes;
typedef unsigned long Ertek;
typedef double Suly;
typedef std::pair<Ertek,Ertek> Koordinata;
typedef std::valarray<Ertek> Ertekek;

typedef Node* NodePtr;
typedef Group* GroupPtr;
typedef std::vector <Area> Areas;
typedef std::vector <Group> Groups;
typedef std::vector <Node> Nodes;
typedef std::vector <Nodes> NodeMatrix;

extern Ertek invalid;
extern Ertek count;
extern std::stringstream ss;


template<typename T>
void my_swap( T& a , T& b )
{
    std::swap((*a)->ertek,(*b)->ertek);
    ++count;
    ss << (*a)->koord.first << " " << (*a)->koord.second << " "
        << (*b)->koord.first << " " << (*b)->koord.second << std::endl;
}

void kiir( const NodeMatrix& g );
void segitseg( Area& , Area& );
void csere( Area& , Area& , Area& );
void rendez( Areas& );

struct Group
{
    std::vector < NodePtr > tagok;
    Ertek ertek;
    Termeszetes csopID;

    Group( Ertek ert , Termeszetes csopid );

    Termeszetes size() const;
};

struct Area
{
    std::vector< NodePtr > hozzam_rendelt;
    std::vector< Termeszetes > mibol_mennyi;
    Ertek ezt_gyujtjuk;
    Ertek maximalis;
    bool szelen_vagyunk_e;

    Area( NodePtr beginNode , Ertek max );

    bool lepes();
    bool vege();
    Ertek lop();
    void hozzaad( NodePtr );
    void elvesz( NodePtr );
    friend void csere( Area& , Area& , Area& );
};

struct Node
{
    Koordinata koord;
    Ertek ertek;
    Ertek foglalt;
    GroupPtr csoport;

    std::vector<NodePtr> szomszed;
    std::vector<NodePtr> atlos_szomszed;

    std::vector<NodePtr> rendezett_szomszed;

    Node();
    void beallit( const Koordinata&, Ertek, Ertek = invalid );

    bool szelen_vagyok_e();
    Termeszetes szelen_levo_szomszedok( Ertek ki );
    bool ket_szomszed_ugyanolyan_a_szelen( Ertek ki );
    bool tobb_helyrol_kozel_van( Ertek ki );

    void csoportosit( GroupPtr csop );
    void szomszed_rendez();
};


#endif // COMMON_H_INCLUDED
