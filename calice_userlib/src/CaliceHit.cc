#include "CaliceHit.hh"
#include <iostream>

using namespace lcio;

namespace CALICE {

void CaliceHit::print(std::ostream& os) {

  os << " module: " << std::hex << getModuleID() << std::dec << ", chip: " << getChip() << ", "
     << " channel: " << getChannel() << ", E=" << getEnergyValue() << "+-"
     << getEnergyError() << ", time stamp: " << getTimeStamp() 
     << ", type: " << getType() << std::endl;

}

}
