#include "CellMappingHcal.hh"


namespace CALICE {

void CellMappingHcal::print(  std::ostream& os ){

  os << " elec_channel: ["  << std::hex  << getElecChannel() << "]" <<  std::dec 
     << ", cellID: " <<  getCellID() 
     << std::endl ;
  os << " crateID: "  << getCrateID() <<  std::endl;
    os << " slotID: "  << getSlotID() <<  std::endl;
  os << " FE: "  << getFeID() <<  std::endl;
  os << " Position in Multiplexing series: "  << getMulID() <<  std::endl;
  os << " ADC Number (0-11): "  << getAdcID() <<  std::endl;
  os << " CellIndex i: " << getCellIndex("i") << " CellIndex j: " 
     << getCellIndex("j") <<  " CellIndex k: " << getCellIndex("k") << std::endl; 




 
}

short CellMappingHcal::getCellIndex(std::string indexStr)
{

  _index = -1;  

  if (indexStr != "i" && indexStr != "j" && indexStr != "k") {
    std::cout << "Please use characters i,j or k !!!" << std::endl;
    std::cout << "Will return -1" << std::endl;
    return _index;
  } 

   
  if (indexStr == "i" ) _index = ( (obj()->getIntVal(1) >> ICOORDSHIFT) & 0xff ); 
  if (indexStr == "j" ) _index = ( (obj()->getIntVal(1) >> JCOORDSHIFT) & 0xff ); 
  if (indexStr == "k" ) _index = ( (obj()->getIntVal(1) >> KCOORDSHIFT) & 0xff ); 
  
  return _index;
}

}

