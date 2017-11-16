#ifndef __VetoThreshold_hh__
#define __VetoThreshold_hh__ 1

#include "UTIL/LCFixedObject.h"
#include "EVENT/LCObject.h"
#include <string>



namespace CALICE{



  /**
   * @brief  Container class to store the threshold necessary to achive a certain purity
   *
   * The class contains two values fot the purity. One is the value which was
   * requested. The limited step width of the threshold calculation leads to a
   * small difference in the aimed at and obtained purity. Therefore, the
   * effective purity reached with this threshold is saved in addition.
   *
   * The threshold error is half the width of the binning in the determination
   * process.
   *
   * @author Benjamin.Lutz@desy.de
   * @version 1.0
   * @date   September 2008
   */
  class VetoThreshold : public UTIL::LCFixedObject<0, 4, 0> {
  public:
    /**  Constructor with initial values
     */
    VetoThreshold( const float purity, const float effectivePurity, const float threshold, const float thresholdError) {
      obj()->setFloatVal(0,purity);
      obj()->setFloatVal(1,effectivePurity);
      obj()->setFloatVal(2,threshold);
      obj()->setFloatVal(3,thresholdError);
    }

    /**  Constructor from LCObject
     */
    VetoThreshold( EVENT::LCObject* object) : UTIL::LCFixedObject<0,4,0>( object ){
    }

    /**  Destructor
     */
    ~VetoThreshold(){
    }

    void setPurity(float purity);
    void setEffectivePurity(float effectivePurity);
    void setThreshold(float threshold);
    void setThresholdError(float thresholdError);

    /**
     * @brief purity in [@%]
     */
    float getPurity() const;

    /**
     * @brief effective purity in [@%]
     */
    float getEffectivePurity() const;

    /**
     * @brief threshold in [adc]
     */
    float getThreshold() const;

    /**
     * @brief threshold error [adc]
     */
    float getThresholdError() const;

    const std::string getTypeName() const;
    const std::string getDataDescription() const;

  };  // class VetoThreshold


}  // namespace CALICE

#endif

