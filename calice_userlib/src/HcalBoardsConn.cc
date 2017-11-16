#include "HcalBoardsConn.hh"

namespace CALICE {

void HcalBoardsConn::print(  std::ostream& os , string remark){

  os << " Connector Pin: " <<  getConnectorPin() << std::endl ;
  os << " HBAB: " << getHbabID() << " HAB: " << getHabID() << std::endl; 
  os << " Asic Input: " << getAsicInput() << " Analog Output: " << getAnalogOutput() << std::endl; 
  os << " Comment: " << remark << std::endl;
  os << "***********************************************************" << std::endl; 

}

}   



