#ifndef BmlCaen767ConfigurationBlock_hh
#define BmlCaen767ConfigurationBlock_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"

using namespace lcio ;
using namespace CALICE;

namespace CALICE {

  enum EBmlCaen767IntValues {kBmlCaen767ConfBoardID,
			     kBmlCaen767ConfBaseAddress,
			     kBmlCaen767ConfRecordLabel,
			     kBmlCaen767ConfControlRegister,
			     kBmlCaen767ConfInterruptRegister,
			     kBmlCaen767ConfIntValues};


/**Class to store configuration data of the 767 Caen TDC
 * obsolete as of 31/10/10, replaced by BmlCaenConfigurationBlock, 
 * kept for backward compatibility of the code 
 *  @author R.Poeschl (LAL Orsay)
 *  @date Aug 01 2006 
 */



class BmlCaen767ConfigurationBlock : public LCGenericObjectImplExt{


public:


  BmlCaen767ConfigurationBlock() { 
    //Do some initialization
    setBoardID(0);
    setBaseAddress(0);
    setRecordLabel(0);
    setControlRegister(0); 
    setInterruptRegister(0); 
  }; 
  BmlCaen767ConfigurationBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 
  

  BmlCaen767ConfigurationBlock &setBoardID(int boardid) {
    obj()->setIntVal(kBmlCaen767ConfBoardID, boardid); 
    return *this;
  }    
  
  
  int getBoardID() {return getIntVal(kBmlCaen767ConfBoardID);}
  
  
    BmlCaen767ConfigurationBlock &setBaseAddress(int baseaddress) {
       obj()->setIntVal(kBmlCaen767ConfBaseAddress, baseaddress);
       return *this;
    };

    short getBaseAddress() const {
      return (short) ( getIntVal(kBmlCaen767ConfBaseAddress) );
    };



  BmlCaen767ConfigurationBlock &setRecordLabel(int label) {
    obj()->setIntVal( kBmlCaen767ConfRecordLabel, label);
    return *this;
  };

  short getRecordLabel() const {
    return (short) ( getIntVal( kBmlCaen767ConfRecordLabel) & 0xff);
  };
  BmlCaen767ConfigurationBlock &setControlRegister(int controlregister) {
    obj()->setIntVal(kBmlCaen767ConfControlRegister, controlregister); 
    return *this;
  }    
  
  
  int getControlRegister() {return getIntVal(kBmlCaen767ConfControlRegister);}
  
  
  
  BmlCaen767ConfigurationBlock &setInterruptRegister(int interruptregister) {
    obj()->setIntVal(kBmlCaen767ConfInterruptRegister, interruptregister); 
    return *this;
  }    
  
  
  int getInterruptRegister() {return getIntVal(kBmlCaen767ConfInterruptRegister);}
  
  
  
  void print(  std::ostream& os) {
    os << "BmlCaen767ConfigurationData: " << std::hex << std::endl;
    os << "BoardID: " << BoardID(getBoardID()) << std::endl;
    os << "Base Address: " << getBaseAddress() << std::endl;
    os << "Record Label: " << getRecordLabel() << std::endl;
    os << "Control Register: " <<  getControlRegister() << std::endl;
    os << "Interrupt Register: " << getInterruptRegister() << std::dec << std::endl;
    
  };
  
  
   // -------- need to implement abstract methods from LCGenericObject
  
  const std::string getTypeName() const { 
    return"BmlCaen767ConfigurationBlock" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:controlregister,i:interruptregister";}
  
  
  
};
  

}

#endif
