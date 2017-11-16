#include "TilesItep.hh"

namespace CALICE {

void TilesItep::print(  std::ostream& os ){

  os << " SiPM: " <<  getSIPMID() << ", TileSize: " <<  getTileSize() << std::endl;
  os << " Voltage: " <<  getVoltage() << " Voltage Breakdown: "<<getVoltageBreakdown() << std::endl;
  os << " Delta_SPE: " <<  getDelta_SPE() << std::endl;
  os << " Phe/Mip : " <<  getPhe_MIP() << ", Temperature: " <<  getTemp() << std::endl;
  os << " SiPM current :" <<getCurrent() << " +/- " << getCurrentRMS() << " uA"<< std::endl; 
  os << " dark rate at 0.5 pixels : "<< getDarkRate0() <<" kHz " << std::endl; 
  os << " dark rate at 0.5 MIP : "<< getDarkRateHalf() <<" kHz " << std::endl; 
  os << " dark rate at 0.5 MIP @ T=22 C : "<< getDarkRateHalfCorr() <<" kHz " << std::endl;
  os << " Pedestal RMS : " << getPedRMS()  <<" unit of gain" << std::endl; 
  os << " single pixel width : " << getPeakWidth()  << std::endl; 
  os << " interpixel cross-talk : " << getXTalk()  << std::endl; 
  os << "*********************************************************************" << std::endl;

}

}



