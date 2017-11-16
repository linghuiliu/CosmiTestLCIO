#include "HodoscopeEventDataBlock.hh"

namespace CALICE {


HodoscopeEventDataBlock::HodoscopeEventDataBlock() : _hitMapsCreated(false) {/*no op*/}

void HodoscopeEventDataBlock::storeHitMaps(std::vector<int> &hitVecX, std::vector<int> &hitVecY) {
  obj()->setIntVal(kHodEvtNumHitsX, static_cast<int>(hitVecX.size())); 
  obj()->setIntVal(kHodEvtNumHitsY, static_cast<int>(hitVecY.size())); 
  int ihit(0);
  //Fill the hits in x direction
  for( std::vector<int>::iterator veciter=hitVecX.begin(); veciter!=hitVecX.end(); veciter++) {
    ihit++;
    //std::cout << "storing xvalue: " << std::hex << static_cast<int>((*veciter)) << std::dec << std::endl;
    obj()->setIntVal(kHodEvtNumHitsX+1+ihit, static_cast<int>((*veciter))); 
  }
  
  //Fill the hits in y direction
  for( std::vector<int>::iterator veciter=hitVecY.begin(); veciter!=hitVecY.end(); veciter++) {
    ihit++;
    obj()->setIntVal(kHodEvtNumHitsX+1+ihit, static_cast<int>((*veciter))); 
  }
  
}



void HodoscopeEventDataBlock::createHitMaps() {
  
  //Build the actual Hit Maps
  //X direction
  for(unsigned int ihitx=0; ihitx < getNumHitsX(); ihitx++) {
    //channel = (getIntVal(kHodEvtNumHitsX+2+ihitx) >> HODCHANNELSHIFT) & 0xffffffff;
    //value =    getIntVal(kHodEvtNumHitsX+2+ihitx) & 0xffffffff;
   _hitMapX[(getIntVal(kHodEvtNumHitsX+2+ihitx) >> HODCHANNELSHIFT) & 0xffff]=getIntVal(kHodEvtNumHitsX+2+ihitx) & 0xffff;  
  } 
  
  //Y direction
  int iposy(kHodEvtNumHitsY+getNumHitsX());
  for(unsigned int ihity=0; ihity < getNumHitsY(); ihity++) _hitMapY[(getIntVal(iposy+1+ihity) >> HODCHANNELSHIFT) & 0xffff]=getIntVal(iposy+1+ihity) & 0xffff;    
  
  _hitMapsCreated=true;
}

  void HodoscopeEventDataBlock::print(std::ostream& os) {
  
  //Check whether the hitmaps have been created
  if(!_hitMapsCreated) createHitMaps();

  //Print what we know about the hodoscope data for this event
  os << " Hodoscope - Status (hex): " << std::hex << getStatusWord() << std::dec << std::endl;
  os << " Hodoscope - Event Counter: " << getEventCounter() << std::endl;
  os << " Hodoscope - Time Counter: " << getTimeCounter() << std::endl;
  os << " Hodoscope - Hits in x direction: " << getNumHitsX() << std::endl;
  for(HodoscopeHitMap_t::iterator hodmapiter=_hitMapX.begin(); hodmapiter != _hitMapX.end(); hodmapiter++) os << "Hit in X - Channel: " << (*hodmapiter).first << " Value: " << (*hodmapiter).second << std::endl;
  std::cout << " Hodoscope - Hits in y direction: " << getNumHitsY() << std::endl;
  for(HodoscopeHitMap_t::iterator hodmapiter=_hitMapY.begin(); hodmapiter != _hitMapY.end(); hodmapiter++) os << "Hit in Y - Channel: " << (*hodmapiter).first << " Value: " << (*hodmapiter).second << std::endl;
  }

}
