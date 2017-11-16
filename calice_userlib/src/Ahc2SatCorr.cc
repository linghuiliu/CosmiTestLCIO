#include "Ahc2SatCorr.hh"

#include <TMath.h>

namespace CALICE {

  /******************************************************
    construct
  ******************************************************/
  Ahc2SatCorr::Ahc2SatCorr()
  {
    _effNpix = 0.;
  }

  /******************************************************
    construct with SiPM Saturation Parameters
  ******************************************************/
  Ahc2SatCorr::Ahc2SatCorr(SaturationParameters *saturationParameters)
  {
    _effNpix = saturationParameters->getNpix();
  }

  /************************************************
   destructor
  ************************************************/
  Ahc2SatCorr::~Ahc2SatCorr() {}


  /************************************************
   deSaturate
  ************************************************/
  float Ahc2SatCorr::deSaturate( const float saturatedSignal ) const
  {
    
    /*  protect against negative/zero input*/
    if ( saturatedSignal <= 0 ) return saturatedSignal;

    /* linearisation threshold */
    const float ratio = 0.95;

    if(saturatedSignal < ratio * _effNpix)
      {
	/* desaturate using inverse function */
	float unSaturatedSignal = - _effNpix * TMath::Log(1 - saturatedSignal / _effNpix);
	return unSaturatedSignal;
      }
    else
      {
	/* desaturate using linear continuation function for hits above linearisation threshold */
	float unSaturatedSignal = 1/( 1 - ratio ) * (saturatedSignal - ratio * _effNpix) - _effNpix * TMath::Log( 1 - ratio );
	return unSaturatedSignal;
      }

  }



  /************************************************
   deSaturatedError
  ************************************************/
  float Ahc2SatCorr::deSaturatedError( const float saturatedSignal,
                                       const float saturatedSignalError ) const
  {
    /*  relative error same as input*/
    return saturatedSignalError / saturatedSignal
      * deSaturate( saturatedSignal );
  }


  /************************************************
   saturate
  ************************************************/
  float Ahc2SatCorr::saturate( const float unSaturatedSignal ) const
  {
    /*  protect against negative/zero input*/
    if ( unSaturatedSignal <= 0 ) return unSaturatedSignal;

    /* calculate saturated signal using simple exponential formula */
    float saturatedSignal = _effNpix * ( 1. - TMath::Exp( - unSaturatedSignal / _effNpix ) );

    return saturatedSignal;
  }


  /************************************************
   saturatedError
  ************************************************/
  float Ahc2SatCorr::saturatedError( const float unSaturatedSignal,
                                     const float unSaturatedSignalError ) const
  {
    /*  relative error same as input*/
    return unSaturatedSignalError / unSaturatedSignal
      * saturate( unSaturatedSignal );
  }
  
}  /* namespace CALICE */



