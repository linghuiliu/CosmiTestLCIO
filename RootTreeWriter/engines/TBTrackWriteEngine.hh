#include "RootWriteEngine.hh"
#include "marlin/Processor.h"
#include "RootTreeWriter.hh"

#include <string>

#include "TTree.h"

namespace marlin
{
  class TBTrackWriteEngine: public RootWriteEngine
  {
  public:

    TBTrackWriteEngine( RootTreeWriter* host ):RootWriteEngine(host),
					    _engineName("TBTrackWriteEngine")
    {}
    

    virtual const std::string& getEngineName()
    { return _engineName; }

    virtual void registerParameters();

    virtual void registerBranches( TTree* hostTree );

    virtual void fillVariables( EVENT::LCEvent* );

    /* tree variables*/
    struct
    {
      float XSlope;
      float XOffset;
      float XSlopeError;
      float XOffsetError;
      float XChi2;
      float XHcalImpact;
      int XNoDof;  /**< number of degrees of freedom*/
      unsigned int XNoHits; /**< number of hits used in fit*/
      int XNumElements;
      float XProbability;
      
      float YSlope;
      float YOffset;
      float YSlopeError;
      float YOffsetError;
      float YChi2;
      float YHcalImpact;
      int YNoDof;  /**< number of degrees of freedom*/
      unsigned int YNoHits; /**< number of hits used in fit*/
      int YNumElements;
      float YProbability;
      
    } _hitsFill;
    
        

  private:
    std::string _engineName;
    std::string _enginePrefix;

    /* processor parameters*/
    std::string _colXName;
    std::string _colYName;
    float _z_of_Hcal;
    float _rotationAngle;
  };

}/*namespace marlin*/
