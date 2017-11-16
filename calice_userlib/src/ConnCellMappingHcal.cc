#include "ConnCellMappingHcal.hh"

namespace CALICE {
void ConnCellMappingHcal::print(  std::ostream& os ){

  os << " Connector Pin: " <<  getConnpin() << std::endl ;

  os << " CellIndex Fine i: " << getCellIndex_fine("i") << " CellIndex Fine j: " << getCellIndex_fine("j") <<  " CellIndex k: " << getCellIndex_fine("k") << std::endl; 
  os << " CellIndex Coarse i: " << getCellIndex_fine("i") << " CellIndex Coarse j: " << getCellIndex_fine("j") <<  " CellIndex Coarse k: " << getCellIndex_fine("k") << std::endl; 
  std::cout << "************************************************" << std::endl;

}


short ConnCellMappingHcal::getCellIndex_fine(std::string indexStr)
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


short ConnCellMappingHcal::getCellIndex_coarse(std::string indexStr)
{

  _index = -1;  

  if (indexStr != "i" && indexStr != "j" && indexStr != "k") {
    std::cout << "Please use characters i,j or k !!!" << std::endl;
    std::cout << "Will return -1" << std::endl;
    return _index;
  } 

   
  if (indexStr == "i" ) _index = ( (obj()->getIntVal(2) >> ICOORDSHIFT) & 0xff ); 
  if (indexStr == "j" ) _index = ( (obj()->getIntVal(2) >> JCOORDSHIFT) & 0xff ); 
  if (indexStr == "k" ) _index = ( (obj()->getIntVal(2) >> KCOORDSHIFT) & 0xff ); 
  
  return _index;
}


}
