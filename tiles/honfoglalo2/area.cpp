#include "common.h"

Area::Area( Node* beginNode , Ertek max ):
        hozzam_rendelt{ beginNode },
        ezt_gyujtjuk(beginNode->ertek),
        maximalis(max),
        szelen_vagyunk_e(beginNode->szelen_vagyok_e())
    {
        beginNode->foglalt = ezt_gyujtjuk;
    }

bool Area::lepes()
{
    return true;
}

void segitseg( Area& , Area& ){}
void csere( Area& , Area& , Area& ){}
