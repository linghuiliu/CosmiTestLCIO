#ifndef BmlCaen1290ConfigurationBlock_hh
#define BmlCaen1290ConfigurationBlock_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"

using namespace lcio ;
using namespace CALICE;

namespace CALICE {

  enum EBmlCaen1290IntValues {kBmlCaen1290ConfBoardID,
			     kBmlCaen1290ConfBaseAddress,
			     kBmlCaen1290ConfRecordLabel,
			     kBmlCaen1290ConfControlRegister,
			     kBmlCaen1290ConfInterruptRegister,
			     kBmlCaen1290ConfCountRegister,
			     kBmlCaen1290ConfIntValues};


class BmlCaen1290ConfigurationBlock : public LCGenericObjectImplExt{


public:


  BmlCaen1290ConfigurationBlock() { 
    //Do some initialization
    setBoardID(0);
    setBaseAddress(0);
    setRecordLabel(0);
    setControlRegister(0); 
    setInterruptRegister(0); 
    setCountRegister(0); 
  }; 
  BmlCaen1290ConfigurationBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 
  

  BmlCaen1290ConfigurationBlock &setBoardID(int boardid) {
    obj()->setIntVal(kBmlCaen1290ConfBoardID, boardid); 
    return *this;
  }    
  
  
  int getBoardID() {return getIntVal(kBmlCaen1290ConfBoardID);}
  
  
    BmlCaen1290ConfigurationBlock &setBaseAddress(int baseaddress) {
       obj()->setIntVal(kBmlCaen1290ConfBaseAddress, baseaddress);
       return *this;
    };

    short getBaseAddress() const {
      return (short) ( getIntVal(kBmlCaen1290ConfBaseAddress) );
    };



  BmlCaen1290ConfigurationBlock &setRecordLabel(int label) {
    obj()->setIntVal( kBmlCaen1290ConfRecordLabel, label);
    return *this;
  };

  short getRecordLabel() const {
    return (short) ( getIntVal( kBmlCaen1290ConfRecordLabel) & 0xff);
  };

  BmlCaen1290ConfigurationBlock &setControlRegister(int controlregister) {
    obj()->setIntVal(kBmlCaen1290ConfControlRegister, controlregister); 
    return *this;
  }    
  
  
  int getControlRegister() {return getIntVal(kBmlCaen1290ConfControlRegister);}
  
  
  
  BmlCaen1290ConfigurationBlock &setInterruptRegister(int interruptregister) {
    obj()->setIntVal(kBmlCaen1290ConfInterruptRegister, interruptregister); 
    return *this;
  }    
  
  
  int getInterruptRegister() {return getIntVal(kBmlCaen1290ConfInterruptRegister);}
  
   BmlCaen1290ConfigurationBlock &setCountRegister(int countregister) {
    obj()->setIntVal(kBmlCaen1290ConfCountRegister, countregister); 
    return *this;
  }    
  
  
  int getCountRegister() {return getIntVal(kBmlCaen1290ConfCountRegister);}
 
  
  void print(  std::ostream& os) {
    os << "BmlCaen1290ConfigurationData: " << std::hex << std::endl;
    os << "BoardID: " << BoardID(getBoardID()) << std::endl;
    os << "Base Address: " << getBaseAddress() << std::endl;
    os << "Record Label: " << getRecordLabel() << std::endl;
    os << "Control Register: " <<  getControlRegister() << std::endl;
    os << "Interrupt Register: " << getInterruptRegister() << std::endl;
    os << "Count Register: " <<  getCountRegister() << std::dec << std::endl;
    
  };
  
  
   // -------- need to implement abstract methods from LCGenericObject
  
  const std::string getTypeName() const { 
    return"BmlCaen1290ConfigurationBlock" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:controlregister,i:interruptregister";}
  
  
  
};
  

}

#endif
