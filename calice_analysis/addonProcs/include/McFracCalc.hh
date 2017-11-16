#ifndef MCFRACCALC_HH
#define MCFRACCALC_HH 1

#include <marlin/Processor.h>
#include <EVENT/LCEvent.h>

#include <string>
#include <vector>
#include <map>

class McFracCalc
    : public marlin::Processor
{
public:
    McFracCalc();
    virtual ~McFracCalc();

    McFracCalc* newProcessor() { return new McFracCalc(); }

    virtual void init();
    virtual void processEvent( lcio::LCEvent* );
    virtual void end();

    bool isInt( std::string s );
    int toInt( const std::string s );

private:
    std::string _inColName;
    std::string _noiseColName;
    std::string _afterDigiColName;
    std::string _inColBase;
    std::string _outColBase;

    std::vector<std::string> _components;

    float _tot_eSum;
    std::vector<float> _eSums;
    float _eSumNoise;
    unsigned _tot_nEvents;
    float _mipCut;

    std::map< unsigned, float > _noiseFractions;
    std::vector< std::map< lcio::long64, float > > _compFractions;
    std::string _compFracEncStr;

    void calcNoiseFractions( lcio::LCEvent* evt );
    void calcCompFractions( lcio::LCEvent* evt );
    void generatePseudoHits( lcio::LCEvent* evt );
};

#endif
