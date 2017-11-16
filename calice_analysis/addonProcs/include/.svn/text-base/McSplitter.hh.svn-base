#ifndef MCSPLITTER_HH
#define MCSPLITTER_HH 1

#include <marlin/Processor.h>
#include <EVENT/LCEvent.h>

#include "McSplitterComp.hh"
#include "McInvertedComp.hh"

#include <string>
#include <vector>

class McSplitter
    : public marlin::Processor
{
public:
    McSplitter();
    virtual ~McSplitter();

    McSplitter* newProcessor() { return new McSplitter(); }

    virtual void init();
    virtual void processEvent( lcio::LCEvent* );
    virtual void end();

    bool isInt( std::string s );
    int toInt( const std::string s );

private:
    std::string _inColName;

    std::vector<std::string> _initComp;

    std::vector<McSplitterComp*> _components;

    float _tot_eSum;
    std::vector<float> _eSums;
    unsigned _tot_nEvents;
};

#endif
