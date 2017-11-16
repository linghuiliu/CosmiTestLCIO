#ifndef __MCPARTICLEWRITEENGINE_HH__
#define __MCPARTICLEWRITEENGINE_HH__

#include <string>

#include "marlin/Processor.h"
#include "RootTreeWriter.hh"
#include "RootWriteEngine.hh"

#include "TTree.h"

/**
 *
 * This engine adds information about the MonteCarlo particle to the ROOT tree.
 *
 * The added branches are
 *
 * -> startPos[3]/D : the production vertex of the particle in [mm]
 * -> endPos[3]/D : the endpoint of the particle in [mm]
 * -> PDG/I : the PDG code of the particle
 *
*/
namespace marlin
{
  class MCParticleWriteEngine : public RootWriteEngine
  {
  public:

    MCParticleWriteEngine( RootTreeWriter* host, std::string engineName = "MCParticleWriteEngine" ):
      RootWriteEngine(host), _engineName( engineName ) {}

    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );

    virtual void fillVariables( EVENT::LCEvent* );

    // tree variables
    double _startPos[3];
    double _endPos[3];
    int _PDG;

private:
    std::string _engineName;
    std::string _enginePrefix;
    std::string _inColNameMCParticle;

  };

} //namespace marlin

#endif
