#include "TilesItepSaturation.hh"

namespace CALICE {


void TilesItepSaturation::print(std::ostream& os){

  os << " SiPM: " <<  getSIPMID() << std::endl; 
  os << " Saturation curve values: "<<std::endl;
  for ( int i=0;i<20;i++) {
    os << i << " SiPM : " << getSipmPixelSat(i) << " [pixels], PMT : " << getPmtMipSat(i) << " [MIP]" << std::endl; 
  }
  
  os << "*********************************************************************" << std::endl;

}

}



