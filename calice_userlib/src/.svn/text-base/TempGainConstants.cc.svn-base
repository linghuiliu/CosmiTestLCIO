#include "TempGainConstants.hh"

#include <cstdio>

namespace CALICE {

void TempGainConstants::print(std::ostream& os) {
  char mystring[500];
  sprintf(mystring,"gainTemp=(%.1f+-%.1f) C, cellTemp=(%.1f+-%.1f) C, chip %2d, channel %2d, gain: (%.1f+-%.1f) ADC ch., Tslope=(%.1f+-%.1f) ADC ch./K, corrGain=(%.1f+-%.1f) ADC ch.",
	  getGainTemp(),getGainTempError(),getCellTemp(),getCellTempError(),getChip(),getChannel(),
	  getGainValue(),getGainError(),getTslope(),getTslopeError(),getCorrGain(),getCorrGainError());
  os << mystring << std::endl;
};

}
