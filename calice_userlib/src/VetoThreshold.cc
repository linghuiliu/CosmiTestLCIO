#include "VetoThreshold.hh"


namespace CALICE{

  void VetoThreshold::setPurity(float purity) {
    obj()->setFloatVal(0,purity);
  }
  void VetoThreshold::setEffectivePurity(float effectivePurity) {
    obj()->setFloatVal(1,effectivePurity);
  }
  void VetoThreshold::setThreshold(float threshold) {
    obj()->setFloatVal(2,threshold);
  }
  void VetoThreshold::setThresholdError(float thresholdError) {
    obj()->setFloatVal(3,thresholdError);
  }

  float VetoThreshold::getPurity() const {
    return getFloatVal(0);
  }

  float VetoThreshold::getEffectivePurity() const {
    return getFloatVal(1);
  }

  float VetoThreshold::getThreshold() const {
    return getFloatVal(2);
  }

  float VetoThreshold::getThresholdError() const {
    return getFloatVal(3);
  }

  /**  Implementation of LCGenericObject::getTypeName
   */
  const std::string VetoThreshold::getTypeName() const {
    return "VetoThreshold";
  }

  /**  Implementation of LCGenericObject::getDataDescription
   */
  const std::string VetoThreshold::getDataDescription() const {
    return "f:purity,f:effectivePurity,f:threshold,f:thresholdError";
  }



}  // namespace CALICE


