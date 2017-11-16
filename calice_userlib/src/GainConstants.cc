#include "GainConstants.hh"

using namespace lcio;

namespace CALICE {

void GainConstants::print(std::ostream& os) {

  os << " chip: " << std::hex << getChip() << ", channel: " << getChannel() << " (cellKey: " << getCellKey() << std::dec
     << "), gain: " << getGainValue() << "+-" << getGainError() << std::endl;

};

}
