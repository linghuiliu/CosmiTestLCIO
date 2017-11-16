#include "AhcSlowReadoutModMapping.hh"


namespace CALICE{

  void AhcSlowReadoutModMapping::setModule(int module) {
    obj()->setIntVal(0,module);
  }
  void AhcSlowReadoutModMapping::setCMBlabel(int label) {
    obj()->setIntVal(1,label);
  }
  void AhcSlowReadoutModMapping::setHVlabel(int label) {
    obj()->setIntVal(2,label);
  }
  void AhcSlowReadoutModMapping::setLVlabel(int label) {
    obj()->setIntVal(3,label);
  }


  int AhcSlowReadoutModMapping::getModule() const {
    return getIntVal(0);
  }
  int AhcSlowReadoutModMapping::getCMBlabel() const {
    return getIntVal(1);
  }
  int AhcSlowReadoutModMapping::getHVlabel() const {
    return getIntVal(2);
  }
  int AhcSlowReadoutModMapping::getLVlabel() const {
    return getIntVal(3);
  }

  /**  Implementation of LCGenericObject::getTypeName
   */
  const std::string AhcSlowReadoutModMapping::getTypeName() const {
    return "AhcSlowReadoutModMapping";
  }

  /**  Implementation of LCGenericObject::getDataDescription
   */
  const std::string AhcSlowReadoutModMapping::getDataDescription() const {
    return "i:module,i:CMBlabel,i:HVlabel,i:LVlabel";
  }



}  // namespace CALICE


