#ifndef __SimpleValue_hh__
#define __SimpleValue_hh__ 1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>

namespace CALICE{

  /**  @brief  Class to store float value, its error, and status flag plus
   *           integer index in LCIO
   *
   *   This class provides an interface to store four numbers inside an LCIO
   *   object:
   *   - an integer identifier, e.g. the index of a calorimeter cell
   *   - a float value, e.g. a calibration constant
   *   - a float error, e.g. the uncertainty on the calibration
   *   - an integer status flag, e.g. to tell default from measured constants
   *
   *   @author Niels.Meyer@desy.de
   *   @author Benjamin.Lutz@desy.de
   *   @date   February 2008 (NM)
   *   @date   February 2010 added set functions, fixed some const errors (BL)
   */
  class SimpleValue : public UTIL::LCFixedObject<2, 2, 0> {
  public:
    /**  Empty constructor
     */
    SimpleValue(){}

    /**  Constructor with initial values
     */
    SimpleValue( const int id, const float val, const float err, const int stat ){
      obj()->setIntVal( 0, id );
      obj()->setFloatVal( 0, val );
      obj()->setFloatVal( 1, err );
      obj()->setIntVal( 1, stat );
    }

    /**  Constructor from LCObject
     */
    SimpleValue( EVENT::LCObject* obj ) : UTIL::LCFixedObject<2,2,0>( obj ){
    }


    /**  Destructor
     */
    ~SimpleValue(){
    }

    /**  Return index identifier
     */
    int getCellID() const {
      return getIntVal( 0 );
    }

    /**  Return the value
     */
    float getValue() const {
      return getFloatVal( 0 );
    }

    /**  Return the error on the value
     */
    float getError() const {
      return getFloatVal( 1 );
    }

    /**  Return the status
     */
    int getStatus() const {
      return getIntVal( 1 );
    }

    /**  set index identifier
     */
    void setCellID(const int ID) {
      obj()->setIntVal( 0, ID );
    }

    /** set the value
     */
    void setValue(const float value) {
      obj()->setFloatVal( 0, value );
    }

    /**  set the error on the value
     */
    void setError(const float error) {
      obj()->setFloatVal( 1, error );
    }

    /**  set the status
     */
    void setStatus(const int status) {
      obj()->setIntVal( 1, status );
    }

    /**  Implementation of LCGenericObject::getTypeName
     */
    virtual const std::string getTypeName() const {
      return "SimpleValue";
    }

    /**  Implementation of LCGenericObject::getDataDescription
     */
    virtual const std::string getDataDescription() const {
      return "i:ID,f:value,f:error,i:status";
    }

  };  // class SimpleValue

}  // namespace CALICE

#endif
