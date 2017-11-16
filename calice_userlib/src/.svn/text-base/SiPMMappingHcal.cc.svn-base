#include "SiPMMappingHcal.hh"

namespace CALICE {

void SiPMMappingHcal::print(  std::ostream& os , string remark){

  os << " SiPM: " <<  getSiPMID() <<  std::dec << ", cellID: " <<  getCellID() 
     << std::endl ;
  os << "Cassette: " << getcassetteID() << std::endl;
  os << " CellIndex i: " << getCellIndex("i") << " CellIndex j: " 
     << getCellIndex("j") <<  " CellIndex k: " << getCellIndex("k") << std::endl; 
  os << "Comment: " << remark << std::endl;

}

short SiPMMappingHcal::getCellIndex(std::string indexStr)
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
