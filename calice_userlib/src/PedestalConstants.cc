#include "PedestalConstants.hh"

using namespace lcio;

namespace CALICE {

void PedestalConstants::print(std::ostream& os) {

  os << " chip: " << std::hex << getChip() << ", channel: " << getChannel() << " " << getCellKey() << std::dec
     << ", pedestal: " << getPedestalValue() << "+-" << getPedestalError() << std::endl;

};

}
