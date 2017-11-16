#ifndef __SatCorrItepCutOff_hh__
#define __SatCorrItepCutOff_hh__

#include "IMPL/LCGenericObjectImpl.h"

#include "HcalTileIndex.hh"
#include "TilesItepSaturation.hh"
#include "SatCorrFunction.hh"

namespace CALICE {

  class SatCorrItepCutOff
    : public IMPL::LCGenericObjectImpl,
      public SatCorrFunction {
  public:
    SatCorrItepCutOff( EVENT::LCObject* obj );
    SatCorrItepCutOff( const HcalTileIndex& hti,
                       const unsigned int nPoints,
                       const float* pixs,
                       const float* pmts );
    virtual ~SatCorrItepCutOff();

    int getIndex() const { return getIntVal( 0 ); }
    float getScaling() const { return _scaling; };

    float deSaturate( const float saturatedSignal ) const;
    float deSaturatedError( const float saturatedSignal,
                            const float saturatedSignalError ) const;
    float saturate( const float unsaturatedSignal ) const;
    float saturatedError( const float unsaturatedSignal,
                          const float unsaturatedSignalError ) const;

    const std::string getTypeName() const { return "SatCorrItepCutOff"; }
    const std::string getDataDescription() const { return "i:HcalTileIndex, f:pix_or_pmt[2][nFloat/2]"; }

    void setIndex( const HcalTileIndex& hti ) { setIntVal( 0, hti.getIndex() ); }
    void setScaling( const float scaling ) { _scaling = scaling; }

    float getPix( const unsigned int i ) const {
      return getFloatVal( 2*i ); }
    float getPmt( const unsigned int i ) const {
      return getFloatVal( 2*i+1 ); }
    void setPix( const unsigned int i,
                 const float pix ) { setFloatVal( 2*i, pix ); }
    void setPmt( const unsigned int i,
                 const float pmt ) { setFloatVal( 2*i+1, pmt ); }

  protected:

  private:
    float _scaling;

  };  // classs SatCorrItepCutOff

}  // namespace CALICE

#endif   // __SatCorrItepCutOff_hh__
