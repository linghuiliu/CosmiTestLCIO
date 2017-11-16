#include "DeadNoiseConstants.hh"

using namespace lcio;

namespace CALICE {

void DeadNoiseConstants::print(std::ostream& os) {

  os << " chip: " << std::hex << getChip() << ", channel: " << getChannel() << " " << getCellKey() << std::dec
     << ", dead: " << getDeadLevel() << ", noise: " << getNoiseLevel() << std::endl;

};

}
