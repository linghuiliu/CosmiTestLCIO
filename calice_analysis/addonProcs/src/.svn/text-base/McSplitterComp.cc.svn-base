#include "McSplitterComp.hh"

#include <sstream>

using namespace std;
using namespace EVENT;
using namespace IMPL;

McSplitterComp::McSplitterComp( const std::string name )
    : _name(name)
{
}

string McSplitterComp::name()
{
    return _name;
}

void McSplitterComp::addPDG( const int pdg )
{
    _accept.push_back( pdg );
}

bool McSplitterComp::matches( int pdg )
{
    for(unsigned i=0; i<_accept.size(); i++) {
        if( pdg == _accept[i] ) return true;
    }
    return false;
}

float McSplitterComp::energy()
{
    return _energy;
}

void McSplitterComp::addEnergy( float energy )
{
    _energy += energy;
}

void McSplitterComp::resetEnergy()
{
    _energy = 0;
}

void McSplitterComp::copyStuffFrom( EVENT::SimCalorimeterHit *hit )
{
    _cellID0 = hit->getCellID0();
    _cellID1 = hit->getCellID1();
     _pos[0] = hit->getPosition()[0];
     _pos[1] = hit->getPosition()[1];
     _pos[2] = hit->getPosition()[2];
}

SimCalorimeterHitImpl* McSplitterComp::createSimCalorimeterHitImpl()
{
    SimCalorimeterHitImpl *hit = new SimCalorimeterHitImpl;
    hit->setCellID0( _cellID0 );
    hit->setCellID1( _cellID1 );
    hit->setPosition( _pos );
    hit->setEnergy( _energy );
    return hit;
}
