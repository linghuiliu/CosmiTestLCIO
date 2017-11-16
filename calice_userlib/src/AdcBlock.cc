#include "AdcBlock.hh"

using namespace std ;
using namespace lcio;


namespace CALICE {

unsigned AdcBlock::getElecChannel(int i) const {

  //int ipos = 3 + i/2;
  //unsigned mod = (unsigned) fmod( (float) i, 2);
  unsigned boardID = (unsigned) getIntVal(0) ;
  unsigned Fe = (unsigned) getIntVal(2) ;
  unsigned Imul = (unsigned) getIntVal(3) ;
  
  //Need to do some bit masking since boardID contains also
  //Values like CRC Component
  unsigned elec_channel = ( ( (boardID<<8) & 0xffff0000) | (Fe<<12) | ( Imul << 4 ) | (i << 0) );
  return elec_channel;
}


void AdcBlock::print(  std::ostream& os, int i ){

  os << " elec_channel: ["  << std::hex  << getElecChannel(i) << "]" <<  std::dec 
     << ", cellID: " <<  getAdcVal(i) 
     << std::endl ;
  
}

}
