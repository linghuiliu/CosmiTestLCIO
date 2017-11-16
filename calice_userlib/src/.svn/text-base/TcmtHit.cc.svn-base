#include "TcmtHit.hh"
#include <iostream>

using namespace lcio;

namespace CALICE {

TcmtHitMemoryPool theTHMemPool;

TcmtHitMemoryPool::~TcmtHitMemoryPool() {
  for(std::vector<void*>::iterator it = mempool.begin(); it != mempool.end(); ++it)
    free(*it);
}


void TcmtHit::print(std::ostream& os) {

  if(getModuleID()!=0) {
    // HCAL case
    os << " module: " << std::hex << getModuleID() << std::dec << ", chip: " << getChip() << ", "
       << " channel: " << getChannel();
  }
  else {
    // TCMT case
    os << " module="<< getChip() <<" strip="<< getChannel();
  }

  os << ", E=" << getEnergyValue() << "+-"<< getEnergyError()
     << ", time stamp: " << getTimeStamp() << std::endl;

}

}
