#ifndef __SatCorrItep_hh__
#define __SatCorrItep_hh__

#include "IMPL/LCGenericObjectImpl.h"

#include "HcalTileIndex.hh"
#include "TilesItepSaturation.hh"
#include "SatCorrFunction.hh"


namespace CALICE {

  class SatCorrItep
    : public IMPL::LCGenericObjectImpl,
      public SatCorrFunction {
  public:
    SatCorrItep( EVENT::LCObject* obj );
    SatCorrItep( const HcalTileIndex& hti,
                 const unsigned int nPoints,
                 const float* pixs,
                 const float* pmts );
    virtual ~SatCorrItep();

    // this const int is necessary because of the usage of the ConditionsMap in the IntegratedHcalCalibrationProcessor
    const int getIndex() const { return getIntVal( 0 ); }
    float getScaling() const { return _scaling; };

    float deSaturate( const float saturatedSignal ) const;
    float deSaturatedError( const float saturatedSignal,
                            const float saturatedSignalError ) const;
    float saturate( const float unsaturatedSignal ) const;
    float saturatedError( const float unsaturatedSignal,
                          const float unsaturatedSignalError ) const;

    const std::string getTypeName() const { return "SatCorrItep"; }
    const std::string getDataDescription() const { return "i:HcalTileIndex, f:pix_or_pmt[2][nFloat/2]"; }

    void setIndex( const HcalTileIndex& hti ) { setIntVal( 0, hti.getIndex() ); }
    void setScaling( const float scaling ) { _scaling = scaling; }
    
    void setProcedureType (const int procedureType ) {
      _procedureType = procedureType;
    }
    int  getProcedureType() const { return _procedureType;  };
    
    float getPix( const unsigned int i ) const {
      return getFloatVal( 2*i ); }
    float getPmt( const unsigned int i ) const {
      return getFloatVal( 2*i+1 ); }
    void setPix( const unsigned int i,
                 const float pix ) { setFloatVal( 2*i, pix ); }
    void setPmt( const unsigned int i,
                 const float pmt ) { setFloatVal( 2*i+1, pmt ); }

  
    float getFunParam0() const { return _funParam0; }
    float getFunParam1() const { return _funParam1; }
    float getFunParam2() const { return _funParam2; }
    
    void setFunParameters (const float p0, const float p1, const float p2) {
     _funParam0 = p0;
     _funParam1 = p1;
     _funParam2 = p2;
    }
    
    //void calculateFunctionParameters();
    
  protected:

  private:
    float _scaling;

    int   _procedureType; // 0 - interpolation (default), 1 - pure fit for saturation
        
    float _funParam0;
    float _funParam1;
    float _funParam2;
    
  };  // classs SatCorrItep

}  // namespace CALICE

#endif   // __SatCorrItep_hh__
