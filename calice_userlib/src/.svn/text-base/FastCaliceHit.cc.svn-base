#include "FastCaliceHit.hh"
#include <iostream>
#include <iomanip>

using namespace lcio;

namespace CALICE {

FastCaliceHitMemoryPool theFCHMemPool;

FastCaliceHitMemoryPool::~FastCaliceHitMemoryPool() {
  for(std::vector<void*>::iterator it = mempool.begin(); it != mempool.end(); ++it)
    free(*it);
}


void FastCaliceHit::print(std::ostream& os) {
  os << " module: " << std::hex << getModule() << std::dec << ", chip: " << getChip() << ", "
     << " channel: " << getChannel() << ", E=" << getEnergyValue() << "+-"<< getEnergyError()
     << ", time stamp: " << getTimeStamp() << std::endl;
}

  std::ostream& operator<<(std::ostream& out, FastCaliceHit fch) {

    out << " module: "  << std::setw(2) << std::setfill('0') << fch.getModule() 
        << " chip: "    << std::setw(2) << std::setfill('0') << fch.getChip()
        << " channel: " << std::setw(2) << std::setfill('0') << fch.getChannel()
        << " amplitude: " << fch.getEnergyValue() 
        << " +- "         << fch.getEnergyError()
        << " time stamp: " << fch.getTimeStamp() ;

    return out;
        
  }

}
