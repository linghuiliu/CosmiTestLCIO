#ifndef __HODOSCOPEWRITEENGINE_HH__
#define __HODOSCOPEWRITEENGINE_HH__

#include "hodoscopeBlock.hh"
#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"
#include "TMath.h"

#include <string>

#include "TTree.h"
#include "TF1.h"

#define pitch 5.0

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
    void recoverDeadCh();
    void reconstruct(int ihod);
    double trackDistance(double x1, double y1, double x2, double y2, float * pos);
    void chAt(int fiber, int ihod, int axis, int *ch);
    double thetaWith(int fiber);
    double fiberWith(double theta);
    double argWith(double x, double y);
    double edgeCorrection(double x);

    /* tree variables*/
    const static unsigned int MAXCELLS  = 7609; /*should be big enough for all detectors!*/

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
      double trueRecoX[2];
      double trueRecoY[2];
      double trueRecoZ[2];
      int    maxPass;
    } _hitsFill;
    
    struct
    {
      int nHits;
      int hitI[MAXCELLS];
      int hitJ[MAXCELLS];
      int hitK[MAXCELLS];
      float hitEnergy[MAXCELLS];
      float hitPos[MAXCELLS][3];
    } _ahcHits;
    
  private:
    void resetHitsFill();

  
    std::string _engineName;
    std::string _enginePrefix;

    /* processor parameters*/
    std::string _prefix[2];
    std::string _hitColName[2];
    std::string _ahcColName;

    int height1, height2;

    TF1 *corrF1, *corrF2;

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
