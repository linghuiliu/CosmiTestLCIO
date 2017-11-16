#ifndef __SaturationParameters_hh__
#define __SaturationParameters_hh__ 1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

namespace CALICE{

  /**  @brief  Class to store float values for Saturation Correction Curves plus
   *           integer cell index in LCIO
   *
   *   This class provides an interface to store four numbers inside an LCIO
   *   object:
   *   - an integer identifier, e.g. the index of a calorimeter cell
   *   - a 1st float value, [0], e.g. a maximum number of effective SiPM pixels
   *   - a 2nd float value, [1], e.g. a fraction of 2-exp function
   *   - a 3rd float value, [2], e.g. an epsilon of 2-exp function
   *   The 2-exp function is used:
   *   N = [0]*([1]*(1-exp(-[2]*x/[0]))+(1-[1])*(1-exp(-(1-[2]*[1])*x/([0]*(1-[1])))))
   *   where x is the unsaturated amplitude, N is the saturated amplitude 
   *   
   *   @author Sergey Morozov
   *   @date   August 2013
   */
  class SaturationParameters : public UTIL::LCFixedObject<1, 3, 0> {
  public:
    /**  Empty constructor
     */
    SaturationParameters(){}

    /**  Constructor with initial values
     */
    SaturationParameters( const int id, const float npix, const float fraction, const float epsilon ){
      obj()->setIntVal( 0, id );
      obj()->setFloatVal( 0, npix );
      obj()->setFloatVal( 1, fraction );
      obj()->setFloatVal( 2, epsilon );
    }

    /**  Constructor from LCObject
     */
    SaturationParameters( EVENT::LCObject* obj ) : UTIL::LCFixedObject<1,3,0>( obj ){
    }


    /**  Destructor
     */
    ~SaturationParameters(){
    }

    /**  Return index identifier
     */
    int getCellID() const {
      return getIntVal( 0 );
    }

    /**  Return the value
     */
    float getNpix() const {
      return getFloatVal( 0 );
    }

    /**  Return the error on the value
     */
    float getFraction() const {
      return getFloatVal( 1 );
    }

    /**  Return the status
     */
    float getEpsilon() const {
      return getFloatVal( 2 );
    }

    /**  set index identifier
     */
    void setCellID(const int ID) {
      obj()->setIntVal( 0, ID );
    }

    /** set the value
     */
    void setNpix(const float npix) {
      obj()->setFloatVal( 0, npix );
    }

    /**  set the error on the value
     */
    void setFraction(const float fraction) {
      obj()->setFloatVal( 1, fraction );
    }

    /**  set the status
     */
    void setEpsilon(const float epsilon) {
      obj()->setFloatVal( 2, epsilon );
    }

    /**  Implementation of LCGenericObject::getTypeName
     */
    virtual const std::string getTypeName() const {
      return "SaturationParameters";
    }

    /**  Implementation of LCGenericObject::getDataDescription
     */
    virtual const std::string getDataDescription() const {
      return "i:ID,f:npix,f:fraction,f:epsilon";
    }

  };  // class SaturationParameters

}  // namespace CALICE

#endif
