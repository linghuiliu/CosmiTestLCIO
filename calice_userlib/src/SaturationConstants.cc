#include "SaturationConstants.hh"

using namespace lcio;

namespace CALICE {

void SaturationConstants::print(std::ostream& os) {

  os << " chip: " << std::hex << getChip() << ", channel: " << getChannel() << " " << getCellKey() << std::dec
     << ", saturation parameter: " << getSaturationValue() << std::endl;

};

}
