#ifndef MCINVERTEDCOMP_HH
#define MCINVERTEDCOMP_HH 1

#include <string>

#include "McSplitterComp.hh"

class McInvertedComp
    : public McSplitterComp
{
public: 
    McInvertedComp( std::string name );
    virtual ~McInvertedComp() {};
    virtual bool matches( const int pdg );
};

#endif
