#ifndef BmlCaen767ReadoutConfigurationBlock_hh
#define BmlCaen767ReadoutConfigurationBlock_hh 1

#include "lcio.h"
#include "LCGenericObjectImplExt.hh"


using namespace lcio ;
using namespace CALICE;

#define CROENABLEBIT 0
#define CROSOFTTRGBIT 1
#define CROBLTROBIT 2
#define CROARROBIT 3

namespace CALICE {

  /** The indices of all the stored values.
   */
  enum EBmlCaen767IntValues {kBmlCaen767RoConfCrateNumber,
			     kBmlCaen767RoConfReadPeriod,
			     kBmlCaen767RoConfBoolCol,
			     kBmlCaen767RoConfMode,
			     kBmlCaen767RoConfIntValues};

 /** Stores the configuration of the Caen767 TDC into the database
   * At the moment I am writing this class I don't know whether they'll
   * serve for something but it is a small uncomplicated class, so here
   * we go
   * @sa ConditionsChangeDelegator
   * @author R. Poeschl LAL (based on the other interface classes)
   *
   * @date Aug  2006
   */

class BmlCaen767ReadoutConfigurationBlock : public LCGenericObjectImplExt{


public:

  /** Default Constructor */
  BmlCaen767ReadoutConfigurationBlock() { 
    setCrateNumber(0);
    setReadPeriod(0);
    setBools(0,0,0,0);
    setMode(0);
  }; 

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  BmlCaen767ReadoutConfigurationBlock(LCObject* obj) : LCGenericObjectImplExt(obj) { } 
  
  /** set the packed board id.
   * @sa BoardID
   * For the Caen 767 TDC the boardID has a slightly different meaning compared with the CrcBoards
   * slotID and board component numbers are just the MSB and LSB of
   * the base address.
   */
  /** Set the crate number */
  BmlCaen767ReadoutConfigurationBlock &setCrateNumber(int cratenumber) {
    obj()->setIntVal(kBmlCaen767RoConfCrateNumber, cratenumber); 
    return *this;
  }    
  
  /**Return the crate number */
  int getCrateNumber() {return getIntVal(kBmlCaen767RoConfCrateNumber);}


   /** Set the Read Period */
   BmlCaen767ReadoutConfigurationBlock &setReadPeriod(int readperiod) {
    obj()->setIntVal( kBmlCaen767RoConfReadPeriod, readperiod);
    return *this;
  };

  /** Return the Read Period */
     unsigned int getReadPeriod() const {
      return (unsigned int) ( getIntVal(kBmlCaen767RoConfReadPeriod) );
    };

  /** Store the indicators of the readout mode */
  BmlCaen767ReadoutConfigurationBlock &setBools(bool enable, bool softtrigger, bool bltro, bool arrayro) {
     int boolword(0);

     boolword =  ((((boolword | ((static_cast<int>(enable) & 0x1) << CROENABLEBIT)) | 
                                ((static_cast<int>(softtrigger) & 0x1) << CROSOFTTRGBIT)) | 
                                ((static_cast<int>(bltro) & 0x1) << CROBLTROBIT)) |
                                ((static_cast<int>(arrayro) & 0x1) << CROARROBIT));
       obj()->setIntVal(kBmlCaen767RoConfBoolCol, boolword);
       return *this;
  }

  bool isEnabled() {
    return (static_cast<bool> (   (getIntVal(kBmlCaen767RoConfBoolCol) >> CROENABLEBIT) & 0x1 ));
  }

  bool isSoftTrigger() {
    return (static_cast<bool> (   (getIntVal(kBmlCaen767RoConfBoolCol) >> CROSOFTTRGBIT) & 0x1 ));
  }

  bool isBltReadout() {
    return (static_cast<bool> (   (getIntVal(kBmlCaen767RoConfBoolCol) >> CROBLTROBIT) & 0x1 ));
  }

  bool isArrayReadout() {
    return (static_cast<bool> (   (getIntVal(kBmlCaen767RoConfBoolCol) >> CROARROBIT) & 0x1 ));
  }

  /** Store the readout mode word
   * note that this word contains the information which you also
   * get with the isSoftTrigger, isEnable, getCrateNumber etc. methods
   * we add the word in case meaning of the bits therein have changed
   * or new inidcators are added
   * and we weren't aware of it. Hence this is just a safety net
   *
   */
   BmlCaen767ReadoutConfigurationBlock &setMode(int mode) {
    obj()->setIntVal(kBmlCaen767RoConfMode, mode); 
    return *this;
  }    
  
  
  /**Return the readout mode word*/
  int getMode() {return getIntVal(kBmlCaen767RoConfMode);}
  
  /** Convenient print method */
  void print(  std::ostream& os) {
    os << "BmlCaenReadoutConfigurationData: " << std::hex << std::endl;
    os << "Crate Number Address: " << getCrateNumber() << std::dec << std::endl;
    os << "Readout Period: " << getReadPeriod() << std::dec << std::endl;
    os << "Enabled? " << isEnabled() << std::endl;
    os << "Softtrigger? " <<  isSoftTrigger() << std::endl;
    os << "Blt Readout? " <<  isBltReadout() << std::endl;
    os << "Array Readout? " <<  isArrayReadout() << std::endl;
    os << "Mode word (contains info above): " << std::hex << getMode() << std::dec << std::endl;
    
  };
  
  /** Return the type of the class*/
  const std::string getTypeName() const { 
    return"BmlCaenReadoutConfigurationBlock" ;
  } 
  
  /** Return a brief description of the data members  */
  const std::string getDataDescription() const {
    return "i:cratenumber,i:readperiod,i:boolword,i:mode";}
  
  
  
};
  

}

#endif
