#include "FeInfo.hh"
#include <iostream>
#include <iomanip>

namespace CALICE {
  void FeInfo::print(  std::ostream& os) const {
    unsigned int fe_id=getFeID();
    BoardID board_id(getBoardID());
    os << board_id
       << std::setw(9) << ( (fe_id != static_cast<unsigned int>(getFifoFeID()) || fe_id != static_cast<unsigned int>(board_id.getBoardComponentNumber())) ? "corrupt" : "ok")
      //       << std::setw(2) << "fe from:: fifo:" <<  getFifoFeID()
      //       << std::setw(2) << " field:" <<  getFeID()
       << " label=" << std::setw(8) << getLabel()
       << " frame sync out=";
    os << std::hex;
    for (unsigned int word_i=0; word_i<5; word_i++) {
      os << std::setw(8) << getFrameSyncOut(word_i) << " ";
    }
    os << std::dec;
    os << " length=" << std::setw(8) << getFeLength()
       << " be.stat.=" << std::setw(8) << std::hex << getBeStatus() << std::dec
       << " fifo=" << std::setw(9) << (isFifoFull() ? (isFifoEmpty() ? "corrupt" : "full") : (isFifoEmpty() ? "empty " : ""))
       << getNumberOfFifoWords()
       << " trigger cnt=" << getTriggerCounter();
  }

}
