#ifndef MCSPLITTERCOMP_HH
#define MCSPLITTERCOMP_HH 1

#include <string>
#include <vector>

#include <EVENT/SimCalorimeterHit.h>
#include <IMPL/SimCalorimeterHitImpl.h>

class McSplitterComp
{
public: 
    McSplitterComp( const std::string name );
    virtual ~McSplitterComp() {};

    virtual std::string name();
    virtual void addPDG( const int pdg );

    virtual bool matches( const int pdg );
    virtual void addEnergy( const float energy );
    virtual void resetEnergy();
    virtual float energy();

    virtual void copyStuffFrom( EVENT::SimCalorimeterHit* hit );
    virtual IMPL::SimCalorimeterHitImpl * createSimCalorimeterHitImpl();

private:

    std::string _name;
    std::vector<int> _accept;
    float _energy;

    int _cellID0;
    int _cellID1;
    float _pos[3];
};

#endif
