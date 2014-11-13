#include "common.h"

Area::Area( Node* beginNode , Ertek max ):
        hozzam_rendelt{ beginNode },
        mibol_mennyi(3),
        ezt_gyujtjuk(beginNode->ertek),
        maximalis(max),
        szelen_vagyunk_e(beginNode->szelen_vagyok_e())
    {
        beginNode->foglalt = ezt_gyujtjuk;
        ++mibol_mennyi[ezt_gyujtjuk];
    }

bool Area::vege()
{
    return hozzam_rendelt.size() == maximalis;
}
bool Area::lepes()
{

    return true;
}
Ertek Area::lop()
{
    throw "";
}

void Area::hozzaad( NodePtr p )
{
    hozzam_rendelt.push_back( p );
    ++mibol_mennyi[p->ertek];
    szelen_vagyunk_e |= p->szelen_vagyok_e();
}
void Area::elvesz( NodePtr p )
{
    auto it = std::find(
            hozzam_rendelt.begin() ,
            hozzam_rendelt.end() , p );
    *it = hozzam_rendelt.back();
    hozzam_rendelt.pop_back();

    --mibol_mennyi[p->ertek];
    szelen_vagyunk_e = p->szelen_levo_szomszedok( ezt_gyujtjuk );
}

void csere( Area& , Area& , Area& ){}
