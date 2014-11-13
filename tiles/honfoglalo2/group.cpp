#include "common.h"

Group::Group( Ertek ert , Termeszetes csopid ):
    ertek(ert),
    csopID(csopid)
{

}

Termeszetes Group::size() const
{
    return tagok.size();
}
