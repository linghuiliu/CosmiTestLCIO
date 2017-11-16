#include <EmcStageDataBlock.hh>

namespace CALICE {

  void EmcStageDataBlock::print(std::ostream& os) {
       os << std::hex << "Header: " << getHeader() 
	    << " Header byte 0: " <<  ( (getHeader()&0xFF) >> 0) 
            << " Header byte 1: " <<  ( (getHeader()&0xFF00) >> 8) 
            << " Header byte 2: " <<  ( (getHeader()&0xFF0000) >> 16) 
            << " Header byte 3: " <<  ( (getHeader()&0xFF000000) >> 24) 
	    << std::dec
            << " XIndexerStatus: " << getXIndexerStatus()
            << " YIndexerStatus: " << getYIndexerStatus()
	    << " xStandPosition: " << getXStandPosition()
            << " yStandPosition: " << getYStandPosition()
	    << " xStandPosition/mm: " << (float) getXStandPosition()*0.1
            << " yStandPosition/mm: " << (float) getYStandPosition()*0.1
	    << " xBeamPosition/mm: " << (float) getXBeamPosition()*0.1
            << " yBeamPosition/mm: " << (float) getYBeamPosition()*0.1
	    << " CheckSum: " << getCheckSum() 
	    << std::endl;


   }


}
