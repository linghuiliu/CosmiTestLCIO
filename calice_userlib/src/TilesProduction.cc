#include "TilesProduction.hh"

namespace CALICE {

void TilesProduction::print(std::ostream& os){

  os << " SiPM: " <<  getSIPMID() << ", TileSize: " <<  getTileSize() << std::endl;
  os << " (Module, Chip, Channel) =  (" <<  getModule() << "," <<  getChip()<< "," <<  getChan() << std::endl;
  os << "*********************************************************************" << std::endl;

}

}



