#ifndef BmlCaenConfigurationBlock_hh
#define BmlCaenConfigurationBlock_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"

using namespace lcio ;
using namespace CALICE;

namespace CALICE {

  enum EBmlCaenIntValues {kBmlCaenConfBoardID,
			     kBmlCaenConfBaseAddress,
			     kBmlCaenConfRecordLabel,
			     kBmlCaenConfControlRegister,
			     kBmlCaenConfInterruptRegister,
			     kBmlCaenConfCountRegister,
			     kBmlCaenConfSpare,
			     kBmlCaenConfIntValues};

/**Class to store configuration data of the Caen TDCs (767, 1290)
 * Replaces BmlCaen767ConfigurationData.
 *  @author R.Poeschl (LAL Orsay)
 *  @date Oct 31 2010 
 */


class BmlCaenConfigurationBlock : public LCGenericObjectImplExt{


public:

  BmlCaenConfigurationBlock() { 
    //Do some initialization
    setBoardID(0);
    setBaseAddress(0);
    setRecordLabel(0);
    setControlRegister(0); 
    setInterruptRegister(0); 
    setCountRegister(0); 
    setSpare(0); 
  }; 

  BmlCaenConfigurationBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 
  

  BmlCaenConfigurationBlock &setBoardID(int boardid) {
    obj()->setIntVal(kBmlCaenConfBoardID, boardid); 
    return *this;
  }    
  
  
  int getBoardID() {return getIntVal(kBmlCaenConfBoardID);}
  
  
    BmlCaenConfigurationBlock &setBaseAddress(int baseaddress) {
       obj()->setIntVal(kBmlCaenConfBaseAddress, baseaddress);
       return *this;
    };

    short getBaseAddress() const {
      return (short) ( getIntVal(kBmlCaenConfBaseAddress) );
    };



  BmlCaenConfigurationBlock &setRecordLabel(int label) {
    obj()->setIntVal( kBmlCaenConfRecordLabel, label);
    return *this;
  };

  short getRecordLabel() const {
    return (short) ( getIntVal( kBmlCaenConfRecordLabel) & 0xff);
  };
  BmlCaenConfigurationBlock &setControlRegister(int controlregister) {
    obj()->setIntVal(kBmlCaenConfControlRegister, controlregister); 
    return *this;
  }    
  
  
  int getControlRegister() {return getIntVal(kBmlCaenConfControlRegister);}
  
  
  
  BmlCaenConfigurationBlock &setInterruptRegister(int interruptregister) {
    obj()->setIntVal(kBmlCaenConfInterruptRegister, interruptregister); 
    return *this;
  }    
  
  
  int getInterruptRegister() {return getIntVal(kBmlCaenConfInterruptRegister);}



  BmlCaenConfigurationBlock &setCountRegister(int countregister) {
    obj()->setIntVal(kBmlCaenConfCountRegister, countregister); 
    return *this;
  }    
  
  
  int getCountRegister() {
    //Like that this class can be savely used for the 767 only database entries (<year 2010)
    if(obj()->getNInt() < kBmlCaenConfInterruptRegister+2) return 0;
    return getIntVal(kBmlCaenConfInterruptRegister);
  }

  BmlCaenConfigurationBlock &setSpare(int spare) {
    obj()->setIntVal(kBmlCaenConfSpare, spare); 
    return *this;
  }    
  
  
  int getSpare() {
    //Like that this class can be savely used for the 767 only database entries (<year 2010)
    if(obj()->getNInt() < kBmlCaenConfInterruptRegister+2) return 0;
    return getIntVal(kBmlCaenConfSpare);
  }
  
  
  
  void print(  std::ostream& os) {
    os << "BmlCaenConfigurationData: " << std::hex << std::endl;
    os << "BoardID: " << BoardID(getBoardID()) << std::endl;
    os << "Base Address: " << getBaseAddress() << std::endl;
    os << "Record Label: " << getRecordLabel() << std::endl;
    os << "Control Register: " <<  getControlRegister() << std::endl;
    os << "Interrupt Register: " << getInterruptRegister() << std::endl;
    os << "Count Register: " << getCountRegister() << std::endl;
    os << "Spare: " << getSpare() << std::dec << std::endl;    

  };
  
  
   // -------- need to implement abstract methods from LCGenericObject
  
  const std::string getTypeName() const { 
    return"BmlCaenConfigurationBlock" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:controlregister,i:interruptregister,i:countregister,i:spare";}
 };
  

}

#endif
