#include "common.h"

Group::Group( NodePtr kezdo , Ertek ert , Termeszetes csopid ):
    ertek(ert),
    csopID(csopid)
{
    kezdo->csoportosit( this );
}

inline Termeszetes Group::size() const
{
    return tagok.size();
}
