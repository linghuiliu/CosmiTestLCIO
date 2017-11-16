#ifndef __VetoFraction_hh__
#define __VetoFraction_hh__ 1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>



namespace CALICE{



  /**  @brief  Class to store the fraction of events that are cut with a certain purity setting of the veto. Thanks go to Sebastian Richter for the suggestion of the clas name.
   *
   *   @attention Take care of the units of the return function!
   *
   *   @author Benjamin.Lutz@desy.de
   *   @version 1.0
   *   @date   September 2008
   */
  class VetoFraction : public UTIL::LCFixedObject<0, 2, 0> {
  public:
    /**  Constructor with initial values
     */
    VetoFraction( const float purity, const float fraction) {
      obj()->setFloatVal(0,purity);
      obj()->setFloatVal(1,fraction);
    }

    /**  Constructor from LCObject
     */
    VetoFraction( EVENT::LCObject* object) : UTIL::LCFixedObject<0,2,0>( object ){
    }

    /**  Destructor
     */
    ~VetoFraction(){
    }

    void setPurity(float purity);
    void setFraction(float fraction);

    /**
     * @brief purity in [@%]
     */
    float getPurity() const;
    /**
     * @brief fraction
     *
     * @remarks As the name suggests the unit is @b not @%. Values should be in [0,1].
     */
    float getFraction() const;

    const std::string getTypeName() const;
    const std::string getDataDescription() const;

  };  // class VetoFraction


}  // namespace CALICE

#endif

