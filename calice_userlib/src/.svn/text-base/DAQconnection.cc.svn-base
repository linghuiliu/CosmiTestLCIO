#include "DAQconnection.hh"


namespace CALICE{

  void DAQconnection::setChannel(unsigned int channel) {
    bitField["channel"] = channel;
    updateLCobject();
  }
  void DAQconnection::setChip(unsigned int chip){
    bitField["chip"] = chip;
    updateLCobject();
  }
  void DAQconnection::setFe(unsigned int fe){
    bitField["fe"] = fe;
    updateLCobject();
  }
  void DAQconnection::setSlot(unsigned int slot){
    bitField["slot"] = slot;
    updateLCobject();
  }
  void DAQconnection::setCrate(unsigned int crate){
    bitField["crate"] = crate;
    updateLCobject();
  }


  unsigned int DAQconnection::getChannel() const{
    return bitField["channel"];
  }
  unsigned int DAQconnection::getChip() const{
    return bitField["chip"];
  }
  unsigned int DAQconnection::getFe() const{
    return bitField["fe"];
  }
  unsigned int DAQconnection::getSlot() const{
    return bitField["slot"];
  }
  unsigned int DAQconnection::getCrate() const{
    return bitField["crate"];
  }

  void DAQconnection::updateLCobject(){
    obj()->setIntVal(0, bitField.highWord());
    obj()->setIntVal(1, bitField.lowWord());
  }

  const std::string DAQconnection::getEncodingString() const {
    return encodingString;
  }


  /**  Implementation of LCGenericObject::getTypeName
   */
  const std::string DAQconnection::getTypeName() const {
    return "DAQconnection";
  }
  
  /**  Implementation of LCGenericObject::getDataDescription
   */
  const std::string DAQconnection::getDataDescription() const {
    return "i:high,i:low";
  }
  


}  // namespace CALICE


