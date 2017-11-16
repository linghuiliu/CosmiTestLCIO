#ifndef __TRACKFINDERWRITEENGINE_HH__
#define __TRACKFINDERWRITEENGINE_HH__

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
  class TrackFinderWriteEngine : public RootWriteEngine
  {
  public:

    TrackFinderWriteEngine( RootTreeWriter* host, std::string engineName = "TrackFinderWriteEngine" ):
      RootWriteEngine(host), _engineName( engineName ) {}

    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );

    virtual void fillVariables( EVENT::LCEvent* );

    // tree variables

private:
    std::string _engineName;
    std::string _enginePrefix;
    std::string _showerStartColName;
    std::string _trackColName;

    int _fNumHcalLayers;
    int _fShowerStart;
    int _fCalorimeterType;
    float _eSumTrack;
    int _numTrackHits;

    const static unsigned int MAXLAYERS = 69;   /*should be big enough for all detectors! ECAL+HCAL*/
    int _trackHitType[MAXLAYERS];
    float _trackHitEnergy[MAXLAYERS];

  };

} //namespace marlin

#endif
