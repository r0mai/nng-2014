#include "common.h"

Node::Node(){}

void Node::beallit( const Koordinata& k , Ertek e , Ertek f )
{
    koord = k;
    ertek = e;
    foglalt = f;
    csoport = nullptr;
}

bool Node::szelen_vagyok_e()
{
    return szomszed.size() < 4;
}

Termeszetes Node::szelen_levo_szomszedok( Ertek ki )
{
    Termeszetes res = 0;
    for( auto sz : szomszed )
    {
        if( sz->szelen_vagyok_e() &&
            sz->foglalt == ki )
            ++res;
    }
    return res;
}

bool Node::ket_szomszed_ugyanolyan_a_szelen( Ertek ki )
{
    return szelen_levo_szomszedok( ki ) > 1;
}

bool Node::tobb_helyrol_kozel_van( Ertek ki )
{
    Termeszetes valtoz = 0;
    Ertek kezdet = rendezett_szomszed.back()->foglalt;
    for( auto sz : rendezett_szomszed )
    {
        if( kezdet != sz->foglalt &&
            ( kezdet == ki || sz->foglalt == ki ) )
        {
            ++valtoz;
        }
        kezdet = sz->foglalt;
    }
    return valtoz > 2;
}

void Node::csoportosit( GroupPtr csop )
{
    csoport = csop;
    csop->tagok.push_back( this );

    for( auto sz : szomszed )
    {
        if( sz->ertek == ertek && sz->csoport == NULL )
        {
            sz->csoportosit( csop );
        }
    }
}

void Node::szomszed_rendez()
{
    if( rendezett_szomszed.empty() )
    {
        rendezett_szomszed = szomszed;
        rendezett_szomszed.insert( rendezett_szomszed.end() , atlos_szomszed.begin() , atlos_szomszed.end() );
        std::sort( rendezett_szomszed.begin() , rendezett_szomszed.end() ,
            [& ,this](NodePtr f , NodePtr s)
            {
                Koordinata f_s{ f->koord.first - koord.first + 1 , f->koord.second - koord.second + 1 };
                Termeszetes f_e = f_s.second >= f_s.first ? f_s.second + f_s.first : 8 - (f_s.second + f_s.first);
                Koordinata s_s{ s->koord.first - koord.first + 1 , s->koord.second - koord.second + 1 };
                Termeszetes s_e = s_s.second >= s_s.first ? s_s.second + s_s.first : 8 - (s_s.second + s_s.first);
                return  f_e < s_e;
            });
    }
}
