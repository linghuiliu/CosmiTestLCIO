#include "CaliceConditions.hh"

using namespace lcio;

namespace CALICE {

void CaliceConditions::print(std::ostream& os) {

  os << " module: " << std::hex << getModuleID() << std::dec << ", moduleNr: "<< getModuleNr() << ",\n"
     << "calibStart: " << getCalibStart() << ", calibWidth: " << getCalibWidth() 
     << ", calibEnabled: " << isCalibEnabled() << ",\n"
     << "hold: " << getHold() << ", holdWidth: " << getHoldWidth() << ",\n"
     << "multiplex: " << getMultiplex() << "x,\n" 
     << "vcalib: " << getVcalib() << ",\n"
     << "verification data:" << getVerification() << std::endl;
  for (int i = 0; i<12; i++)
    os << "SR hab(" << i << ")=" << getSR(i) << ", ";
  os << std::endl;

}

}
