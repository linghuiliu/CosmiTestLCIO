#include "TcmtSaturationConstants.hh"

using namespace lcio;

namespace CALICE {

void TcmtSaturationConstants::print(std::ostream& os) {

  os << " cassette=" << std::hex << getChip()
     << ", strip=" << getChannel()
     << ", cellKey=" << getCellKey() << std::dec
     << ", Cpix: " << getCpixValue() << " +- " << getCpixError() << std::endl;

};

}
