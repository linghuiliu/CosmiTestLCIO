#include "DhcReadoutConfBlock.hh"


std::ostream& DhcReadoutConfBlock::print(std::ostream &ostrm){
     ostrm << " DhcReadoutConfBlock: "  << std::endl;
     ostrm << " Numbers: " << getNumbers() << std::endl; 
     ostrm << " Crate number: " << getCrateNumber() << std::endl; 
     ostrm << " Number of slots: " << getNumberOfSlots() << std::endl; 
     ostrm << " Slot enable mask: " << std::hex << getSlotEnableMask() << std::dec << std::endl; 
     ostrm << " Slot enable: " << std::hex << getSlotEnable() << std::dec << std::endl; 
     for(unsigned int islot=0; islot < getNumberOfSlots(); islot++) { 
         std::cout << " Slot number: " << islot << std::endl; 
         std::cout << " Enabled fes: " << getSlotFeEnable(islot) << std::endl; 
         std::cout << " Fe revision: " << static_cast<unsigned int>(getSlotFeRevision(islot)) << std::endl; 
     }
     ostrm << " Be wait interval [mus]: " <<  (getBeWaitInterval().timeStamp())/1000 << std::endl; 
     return ostrm;
}


void DhcReadoutConfBlock::fillSlotToWordsAssociationMask(const unsigned int slotenablemask, bool initvals) {
  /** using the slot enable mask we create the map which associates the slots to the position in the LCGenericObject */
  _islotfillMap.clear();
  unsigned int iword(0);
  if(_numSlotsInitialised) {
    for (unsigned int islot=0; islot < getNumberOfSlots(); islot++) {
      if( (0x1 << islot) &  slotenablemask){
	_islotfillMap[islot]=iword;     
	iword++;
      }
    }
    if(initvals) {
      for (unsigned int ipos=0; ipos < iword/2+1; ipos++) obj()->setIntVal(kDhcRoConfIntValues+ipos,0);
      unsigned int offset(_islotfillMap.size()/2+_islotfillMap.size()%2);
      for (unsigned int ipos=0; ipos < iword/4+1; ipos++) obj()->setIntVal(kDhcRoConfIntValues+offset+ipos,0);
    }
  } else std::cout <<   "Warning number of slots not initialised please, class not correctly filled" << std::endl;
}

void DhcReadoutConfBlock::fillMethod(unsigned int islot, unsigned int val, unsigned int offset, unsigned int bitalign) {
#ifdef RECO_DEBUG
  std::cout << "offset " << offset << std::endl;  
  std::cout << "bitalign " << bitalign<< std::endl;  
  std::cout << "val " << std::hex << val << std::dec << std::endl;  
#endif
  //if(islot < static_cast<unsigned int>(getIntVal(kDhcRoConfIntNumSlots))) obj()->setIntVal(kDhcRoConfIntValues+offset+(_islotfillMap.find(islot)->second)/bitalign, getIntVal(kDhcRoConfIntValues+offset+(_islotfillMap.find(islot)->second)/bitalign) | (val << ((_islotfillMap.find(islot)->second)%bitalign)*(32/bitalign)));
if( (0x1 << islot) & getSlotEnableMask() ) { obj()->setIntVal(kDhcRoConfIntValues+offset+(_islotfillMap.find(islot)->second)/bitalign, getIntVal(kDhcRoConfIntValues+offset+(_islotfillMap.find(islot)->second)/bitalign) | (val << ((_islotfillMap.find(islot)->second)%bitalign)*(32/bitalign)));
#ifdef RECO_DEBUG
  std::cout << "Shift val: " <<  ((_islotfillMap.find(islot)->second)%bitalign)*(32/bitalign) << std::endl;
  std::cout << "Word val: " <<  (_islotfillMap.find(islot)->second)/bitalign << std::endl;
  std::cout << "Cur val: " << getIntVal(kDhcRoConfIntValues+offset+(_islotfillMap.find(islot)->second)/bitalign) << std::endl;
#endif
  } else std::cout << "DhcReadoutConfBlock Warning - Trying to access disabled or non-existing slot! Slot enable mask " << std::hex << getSlotEnableMask() << " got " << std::dec << islot << std::endl;  

}

unsigned short DhcReadoutConfBlock::getMethod(unsigned int islot, unsigned int offset, unsigned int bitalign) {
#ifdef RECO_DEBUG
  std::cout << "offset " << offset << std::endl;  
  std::cout << "bitalign " << bitalign<< std::endl;  
#endif
    unsigned short feenable(0);
    if( (0x1 << islot) & getSlotEnableMask() ) { feenable = static_cast<unsigned short>(((getIntVal(kDhcRoConfIntValues+offset+(_islotfillMap.find(islot)->second)/bitalign)) >> ((_islotfillMap.find(islot)->second)%bitalign)*(32/bitalign)) & 0xffff); 
#ifdef RECO_DEBUG
    std::cout << "Read val " << std::hex << getIntVal(kDhcRoConfIntValues+offset+(_islotfillMap.find(islot)->second)/bitalign) << std::dec << std::endl;      
    std::cout << "Word val: " <<  (_islotfillMap.find(islot)->second)/bitalign << std::endl;
    std::cout << "Shift val " << ((_islotfillMap.find(islot)->second)%bitalign)*(32/bitalign) << std::endl;
#endif
    }
    else std::cout << "DhcReadoutConfBlock Warning - Trying to access disabled or non-existing slot! Slot enable mask " << std::hex << getSlotEnableMask() << " got " << std::dec << islot << std::endl;  
#ifdef RECO_DEBUG
    std::cout << "retval " << std::hex << feenable << std::dec << std::endl;  
#endif
    return feenable;

}
