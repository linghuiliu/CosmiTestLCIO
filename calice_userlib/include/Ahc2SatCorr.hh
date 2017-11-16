#ifndef __Ahc2SatCorr_hh__
#define __Ahc2SatCorr_hh__

#include "SaturationParameters.hh"

namespace CALICE {

  class Ahc2SatCorr
  {
  public:

    Ahc2SatCorr();
    Ahc2SatCorr(SaturationParameters *saturationParameters);

    virtual ~Ahc2SatCorr();

    float deSaturate( const float saturatedSignal ) const;
    float deSaturatedError( const float saturatedSignal,
                            const float saturatedSignalError ) const;

    float saturate( const float unsaturatedSignal ) const;
    float saturatedError( const float unsaturatedSignal,
                          const float unsaturatedSignalError ) const;

    float getNeffPix() {return _effNpix;}

  protected:

  private:
    float _effNpix; //Number of effective pixels
    
  };  // class Ahc2SatCorr

}  // namespace CALICE

#endif   // __Ahc2SatCorr_hh__
