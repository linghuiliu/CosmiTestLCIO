#include "VetoFraction.hh"


namespace CALICE{

  void VetoFraction::setPurity(float purity) {
    obj()->setFloatVal(0,purity);
  }
  void VetoFraction::setFraction(float fraction) {
    obj()->setFloatVal(1,fraction);
  }

  float VetoFraction::getPurity() const {
    return getFloatVal(0);
  }

  float VetoFraction::getFraction() const {
    return getFloatVal(1);
  }

  /**  Implementation of LCGenericObject::getTypeName
   */
  const std::string VetoFraction::getTypeName() const {
    return "VetoFraction";
  }

  /**  Implementation of LCGenericObject::getDataDescription
   */
  const std::string VetoFraction::getDataDescription() const {
    return "f:purity,f:fraction";
  }



}  // namespace CALICE


