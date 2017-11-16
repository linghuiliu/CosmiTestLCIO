#ifndef __SHOWERSHAPENGINE_HH__
#define __SHOWERSHAPEENGINE_HH__

#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"
#include "TMath.h"

#include <string>

#include "TTree.h"

namespace marlin
{
  class ShowerShapeEngine: public RootWriteEngine
  {
  public:
    
    ShowerShapeEngine( RootTreeWriter* host ):RootWriteEngine(host),
					   _engineName("ShowerShapeEngine")
    {}
    
    
    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );
    
    virtual void fillVariables( EVENT::LCEvent* );
    
    /* tree variables*/
    const static unsigned int MAX_CLUSTERS = 20; /*<maximum number of clusters to be saved in the tree*/

    
  private:
    std::string _engineName;
    std::string _enginePrefix;

    /* processor parameters*/
    std::string _prefix;
    std::string _showerShapeColName;

    float _cogX[MAX_CLUSTERS];
    float _cogY[MAX_CLUSTERS];
    float _cogZ[MAX_CLUSTERS];
    float _energy[MAX_CLUSTERS];
    int   _nHits[MAX_CLUSTERS];
    int   _nClusters;

  };

}/*namespace marlin*/

#endif /* __HITWRITEENGINE_HH__*/
