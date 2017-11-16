#include "SatCorrItep.hh"

#include <stdexcept>
#include <cassert>

namespace CALICE {

  /******************************************************
    construct from LCObject
  ******************************************************/
  SatCorrItep::SatCorrItep( EVENT::LCObject* obj ) : _scaling( 1.0 ), _procedureType ( 0 )
  {
   
    EVENT::LCGenericObject* gObj = dynamic_cast< EVENT::LCGenericObject* >( obj );

    if ( gObj == NULL )
      throw std::invalid_argument( "SatCorrItep: this LCObject is not LCGenericObject" );

    if ( gObj->getNInt() != 1 || gObj->getNFloat()%2 != 0 )
      throw std::invalid_argument( "SatCorrItep: suspicious number of elements in this LCGenericObject" );

    setIntVal( 0, gObj->getIntVal( 0 ) );

    for ( int i = 0; i != gObj->getNFloat(); ++i )
      setFloatVal( i, gObj->getFloatVal( i ) );
      
    setFunParameters(1156.,0.8,0.7); //2-exp default parameters
  }


  /**************************************************
    construct from HcalTileIndex
  ************************************************/
  SatCorrItep::SatCorrItep( const HcalTileIndex& hti,
                            const unsigned int nPoints,
                            const float* pixs,
                            const float* pmts ) : _scaling( 1.0 ), _procedureType ( 0 )
  {
    setIndex( hti );

    for ( unsigned int i = 0; i != nPoints; ++i )
      {
        setPix( i, pixs[i] );
        setPmt( i, pmts[i] );
      }
   
    setFunParameters(1156.,0.8,0.7); //2-exp default parameters 
  }

  /************************************************
   destructor
  ************************************************/
  SatCorrItep::~SatCorrItep() {}


  /************************************************
   deSaturate
  ************************************************/
  float SatCorrItep::deSaturate( const float saturatedSignal ) const
  {
    
    /*  protect against negative/zero input*/
    if ( saturatedSignal <= 0 ) return saturatedSignal;

    /*  scale the input value by 1/_scaling,
        equivalent to scaling with _scaling of all pixel values in curve
    */
    float satScale = saturatedSignal / _scaling;
    
    /*  if below first point in curve, do nothing*/
    if ( satScale < getPix( 0 ) )
      return saturatedSignal * getPmt( 0 ) / getPix( 0 );

    int nPairs = getNFloat()/2;
    

    /*************************************************************************/
    /*  check for cut-off beyond last measured point
     */
    //if ( satScale >= getPix( nPairs - 1 ) ) return getPmt( nPairs - 1 ) * _scaling;
    /*------------------------------------------------------------------------*/
    /*do not cut-off, but do a linear extrapolation from the last 2 points
      Slope must be positive. If not, take the next lower points...
    */
    
    if ( satScale >= getPix( nPairs - 1 ) )
      {
        float slopeLast   = 0;
        int iLastPositive = 0;

        for (int i = nPairs - 1; i > 0; --i)
          {
            slopeLast = ( getPmt(i) - getPmt(i - 1) ) / ( getPix(i) - getPix(i - 1) );

            if (slopeLast > 0)
              {
                iLastPositive = i;
                break;
              }
          }
        
        float extrapolationLast =  getPmt(iLastPositive - 1) + slopeLast * ( satScale - getPix(iLastPositive - 1) );
        return _scaling * extrapolationLast;

      }

    // find index of the measured point closest but lower to the input
    int iUpper = 0;
    while ( satScale >= getPix( iUpper ) )  ++iUpper;
    int iLower = iUpper - 1;

    //  return linear interpolation between points iLower and iLower+1
    float slope = ( getPmt( iUpper ) - getPmt( iLower ) )
      / ( getPix( iUpper ) - getPix( iLower ) );

    float extrapolation =
      getPmt( iLower ) + slope * ( satScale - getPix( iLower ) );
 
    return _scaling * extrapolation;
    
  }



  /************************************************
   deSaturatedError
  ************************************************/
  float SatCorrItep::deSaturatedError( const float saturatedSignal,
                                       const float saturatedSignalError ) const
  {
    /*  relative error same as input*/
    return saturatedSignalError / saturatedSignal
      * deSaturate( saturatedSignal );
  }


  /************************************************
   saturate
  ************************************************/
  float SatCorrItep::saturate( const float unSaturatedSignal ) const
  {
    
    /*  protect against negative/zero input*/
    if ( unSaturatedSignal <= 0 ) return unSaturatedSignal;

    /*  curve should be scaled - scale input instead*/
    float linScale = unSaturatedSignal / _scaling;
    
    /*  if below first point in curve, do nothing*/
    if ( linScale < getPmt( 0 ) )
      return unSaturatedSignal * getPix( 0 ) / getPmt( 0 );

    if ( _procedureType == 0 ) { //make the interpolation 


    //  check for cut-off beyond last measured point
    int nPairs = getNFloat()/2;
    //if ( linScale >= getPmt( nPairs - 1 ) ) return getPix( nPairs - 1 ) * _scaling;

    //do not cut-off, but use linear extrapolation of the last 2 points
    if ( linScale >= getPmt( nPairs - 1 ) )
      {
        float slopeLast   = 0;
        int iLastPositive = 0;

        for (int i = nPairs - 1; i > 0; --i)
          {
            slopeLast = ( getPix(i) - getPix(i-1)) / (getPmt(i) - getPmt(i-1));
            if (slopeLast > 0)
              {
                iLastPositive = i;
                break;
              }
          }
          
        float extrapolationLast = getPix(iLastPositive - 1) + slopeLast * (linScale - getPmt(iLastPositive - 1));
        
        return _scaling * extrapolationLast;
      }

    //  find index of the measured point closest but lower to the input
    int iUpper = 0;
    while ( linScale >= getPmt( iUpper ) )
      ++iUpper;
    int iLower = iUpper - 1;

    //  extract linear interpolation between points iLower and iLower+1
    float slope = ( getPix( iUpper ) - getPix( iLower ) )
      / ( getPmt( iUpper ) - getPmt( iLower ) );
    float extrapolation = getPix( iLower )
      + slope * ( linScale - getPmt( iLower ) );
      
    //  pixel axis is scaled by _scaling: 
    return _scaling * extrapolation;
      
    } //_procedureType == 0
    
    else { //make pure curve saturation
        float p0 = getFunParam0();
        float p1 = getFunParam1();
        float p2 = getFunParam2();
        float x = linScale;
        //double exp
        float extrapolation = p0*(p1*(1-exp(-p2*x/p0)) + (1-p1)*(1-exp(-(1-p2*p1)*x/(p0*(1-p1)))) );
           
    //  pixel axis is scaled by _scaling:
    return _scaling * extrapolation;
    }
    

    
  }


  /************************************************
   saturatedError
  ************************************************/
  float SatCorrItep::saturatedError( const float unSaturatedSignal,
                                     const float unSaturatedSignalError ) const
  {
    /*  relative error same as input*/
    return unSaturatedSignalError / unSaturatedSignal
      * saturate( unSaturatedSignal );
  }
  
}  /* namespace CALICE */



