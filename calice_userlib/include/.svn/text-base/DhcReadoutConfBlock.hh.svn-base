#ifndef DhcReadoutConfBlock_hh
#define DhcReadoutConfBlock_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
#include "LCGenericObjectImplExt.hh"
#include "UTIL/LCTime.h"
#include <map>
#include <cassert>

using namespace lcio ;
using namespace CALICE;

namespace CALICE {


  enum EDhcreadoutConfigurationIntValues {kDhcRoConfIntNumbers,
					  kDhcRoConfIntSlotEnableMask, 
					  kDhcRoConfIntSlotEnable, 
					  kDhcRoConfIntNumSlots, 
					  kDhcRoConfIntBeTimeDif, 
					  kDhcRoConfIntValues};


 /** Class to interface the configuration of the Dhcal fes 
   *
   * @author R. Pöschl (LAL Orsay), J. Smith (ANL) 
   * @date Apr  2011
   * @todo Attention we may introduce a platform dependency here!!!! The structure of the object assumes that LCGenericObjects are aligned on 32 bits 
   * Need maybe revision for assumptions on data alignment, could be added as collection parameters,  
   */
class DhcReadoutConfBlock : public LCGenericObjectImplExt {
  
public: 


  /** Simple Constructor */   
  DhcReadoutConfBlock() {
    //Initialisation of data members
    obj()->setIntVal(kDhcRoConfIntNumbers, 0);
    obj()->setIntVal(kDhcRoConfIntSlotEnableMask, 0);
    obj()->setIntVal(kDhcRoConfIntSlotEnable, 0);
    obj()->setIntVal(kDhcRoConfIntNumSlots, 0);
    _numSlotsInitialised=false;
  };
  
  /**'Copy constructor' needed to interpret LCCollection read from file/database.*/
  DhcReadoutConfBlock(LCObject* obj) : LCGenericObjectImplExt(obj) 
  { 
    assert ( _obj->getNFloat() == 0 );
    assert ( _obj->getNDouble() == 0 );
    //assert ( _obj->getNInt() == kDhcRawChipIntValues );
    _numSlotsInitialised=true;
    fillSlotToWordsAssociationMask(getSlotEnableMask(), false);
  } 

   /**The destructor */
   virtual ~DhcReadoutConfBlock() {}

  /** Set the numbers*/
  DhcReadoutConfBlock &setNumbers(unsigned int numbers) {
    obj()->setIntVal(kDhcRoConfIntNumbers, static_cast<int>(numbers)); 
    return *this;
  }
  
  /** Get the numbers*/
  unsigned int getNumbers() const {
    return static_cast<unsigned int>(getIntVal(kDhcRoConfIntNumbers));
}

  /**Return the crate number, can be obtained from numbers */
  unsigned int getCrateNumber() {
    return static_cast<unsigned int>(getIntVal(kDhcRoConfIntNumbers) & 0xff);
  }


  /** Set the slot enable mask, which tells which slots in could (in principle) have been enabled*/
  DhcReadoutConfBlock &setSlotEnableMask(unsigned int slotenablemask) {
    obj()->setIntVal(kDhcRoConfIntSlotEnableMask, static_cast<int>(slotenablemask)); 
    //fill association map
    fillSlotToWordsAssociationMask(slotenablemask, true);
    return *this;
  }

  /** Get the slot enable mask*/
  unsigned int getSlotEnableMask() const {
    return static_cast<unsigned int>(getIntVal(kDhcRoConfIntSlotEnableMask));
}


  /** Set the slot enable, i.e. the slots which are really enabled*/
  DhcReadoutConfBlock &setSlotEnable(unsigned int slotenable) {
    obj()->setIntVal(kDhcRoConfIntSlotEnable, static_cast<int>(slotenable)); 
    return *this;
  }

  /** Get the slot enable*/
  unsigned int getSlotEnable() const {
    return static_cast <unsigned int>(getIntVal(kDhcRoConfIntSlotEnable));
}


  /** Set the number of slots, note that this number will have to be multiplied by two, leave it for the time being*/
  DhcReadoutConfBlock &setNumberOfSlots(unsigned int numslots) {
    obj()->setIntVal(kDhcRoConfIntNumSlots, static_cast<int>(numslots)); 
    _numSlotsInitialised=true;
    return *this;
  }

 /** Get the numbers of slots*/
  unsigned int getNumberOfSlots() const {
    return static_cast<unsigned int>(getIntVal(kDhcRoConfIntNumSlots));
  }


  /** Set the enabled fes for this slot */
  DhcReadoutConfBlock &setSlotFeEnable(unsigned int islot, unsigned int val) { 
    fillMethod(islot, val, 0, sizeof(unsigned int)/sizeof(unsigned short));
    return *this;
  }
  
  /** Get the enabled fes for this slot */ 
  unsigned short getSlotFeEnable(unsigned int islot) { 
    return getMethod(islot, 0, sizeof(unsigned int)/sizeof(unsigned short));
  }
  
  /** Set the revision number of the fes for this slot*/
  DhcReadoutConfBlock &setSlotFeRevision(unsigned int islot, unsigned char val) { 
    unsigned int offset(_islotfillMap.size()/2+_islotfillMap.size()%2);
    fillMethod(islot, static_cast<unsigned int>(val), offset, sizeof(unsigned int)/sizeof(unsigned char));
    return *this;
  }

  /** Set the revision number of the fes for this slot*/
  unsigned char getSlotFeRevision(unsigned int islot) { 
    unsigned int offset(_islotfillMap.size()/2+_islotfillMap.size()%2);
    return static_cast<unsigned char>(getMethod(islot, offset, sizeof(unsigned int)/sizeof(unsigned char)));
  }
  
  /** Set the Be wait interval */
  DhcReadoutConfBlock& setBeWaitInterval(unsigned int timeinmus) {
    obj()->setIntVal(kDhcRoConfIntBeTimeDif, static_cast<int>(timeinmus)); 
    return *this;
  }
   
  /** Return the Be wait interval, in mus */
  LCTime getBeWaitInterval() {
    LCTime finaltime(static_cast<long64>(static_cast<unsigned int>(getIntVal(kDhcRoConfIntBeTimeDif)*1000)));
    return finaltime;
  }

  /** Convenient print method*/
  std::ostream& print(std::ostream &ostrm);
  
  const std::string getTypeName() const { return "DhcReadoutConfBlock";} 
  
  /** Return a brief description of the data members*/
  const std::string getDataDescription() const {
    return "i:numbers,i:slotenablemask,i:slotenable,i:numslots,i:betimediff,i[numslots/2]:slotfeenable,i[numref/4]:ferev" ;
  }

private: 
  /** map which associates the slots with the position in the LCGenericObject */
  std::map<unsigned int, unsigned int> _islotfillMap;
  /** is the number of slots initialised (makes only sense during the filling sequence)*/
  bool _numSlotsInitialised;
  /** Association mask */
  void fillSlotToWordsAssociationMask(unsigned int, bool);
  /** A convenient method to fill fe eanble and fe revision values */
  void fillMethod(unsigned int, unsigned int, unsigned int, unsigned int); 
  /** A convenient method to fill fe eanble and fe revision values */
  unsigned short getMethod(unsigned int, unsigned int, unsigned int); 

}; // class

}
#endif 
