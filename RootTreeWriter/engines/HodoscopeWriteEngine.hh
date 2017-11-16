#ifndef __HODOSCOPEWRITEENGINE_HH__
#define __HODOSCOPEWRITEENGINE_HH__

#include "hodoscopeBlock.hh"
#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"
#include "TMath.h"

#include <string>

#include "TTree.h"

namespace marlin
{
  class HodoscopeWriteEngine: public RootWriteEngine
  {
  public:
    
    HodoscopeWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
					   _engineName("HodoscopeWriteEngine")
    {}
    
    
    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );
    
    virtual void fillVariables( EVENT::LCEvent* );
    
    double adc2nph( int adc, int ihod, int ch );
    void reconstruct(int ihod);
    void chAt(int fiber, int ihod, int axis, int *ch);
    double thetaWith(int fiber);
    double fiberWith(double theta);
    double argWith(double x, double y);

    /* tree variables*/
    struct
    {
      int    cycle[2];
      int    tdc[2];
      int    accept[2];
      int    adc[2][64];
      double nph[2][64];
      int    nRecoX[2];
      int    nRecoY[2];
      double recoX[2][8];
      double recoY[2][8];
      
    } _hitsFill;
    
    
  private:
    void resetHitsFill();

  
    std::string _engineName;
    std::string _enginePrefix;

    /* processor parameters*/
    std::string _prefix[2];
    std::string _hitColName[2];
    std::string _ahcColName;

    int   cycle;
    int   tdc;
    int   accept;
    std::vector<int>    adc;
    std::vector<double> nph;
    double onePC[2][64];
    double pedestal[2][64];
    int    nReco[2];
    double recoPos[2][8];
    bool   isModInverted;

  };

}/*namespace marlin*/

#endif /* __HITWRITEENGINE_HH__*/
