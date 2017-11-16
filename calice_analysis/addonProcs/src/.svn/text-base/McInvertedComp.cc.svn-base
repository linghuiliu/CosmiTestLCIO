#include "McInvertedComp.hh"

McInvertedComp::McInvertedComp( std::string name )
    : McSplitterComp(name)
{
}

bool McInvertedComp::matches( int pdg )
{
    return ! McSplitterComp::matches(pdg);
}
