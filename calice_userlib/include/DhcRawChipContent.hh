#ifndef DhcRawChipContent_hh
#define DhcRawChipContent_hh 1

#define SHIFT_V 0
#define SHIFT_DCAL 2
#define SHIFT_DCOL 7
#define SHIFT_DCON 11

#define MASK_V    0x00000002
#define MASK_DCAL 0x0000007c
#define MASK_DCOL 0x00000780
#define MASK_DCON 0x00007800

#define SHIFT_TRG 0
#define SHIFT_DBT 1
#define SHIFT_ERR 2
#define SHIFT_CHKSUM 6
#define SHIFT_VERSUM 14

#define MASK_TRG    0x00000001
#define MASK_DBT    0x00000002
#define MASK_ERR    0x0000003c
#define MASK_CHKSUM 0x00003fc0
#define MASK_VERSUM 0x003fc000


#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
#include "LCGenericObjectImplExt.hh"
#include <cassert>

using namespace lcio ;
using namespace CALICE;

namespace CALICE {


  enum EDhcRawChipContentIntValues {kDhcRawChipIntCrateNr,
                                    kDhcRawChipIntElecAdr,
                                    kDhcRawChipIntHitsHi, 
                                    kDhcRawChipIntHitsLo, 
                                    kDhcRawChipIntTimeStamp, 
			            kDhcRawChipIntMiscInfo,
			            kDhcRawChipIntValues};


 /** Simple class which stores the dhcal chip content 
   * Note that the dchal as it is now delivers the same time stamp  
   * for _all_ 64 channels served by a chip. That's why the LCIO RawCalorimeterHit
   * class is not convenient. Since it would lead to an increas of the data volume 
   * by a factor 64.
   *
   * @author R. Pöschl (LAL Orsay) 
   * @date Dec  2010
   */
class DhcRawChipContent : public LCGenericObjectImplExt {
  
public: 


  /** Simple Constructor */   
  DhcRawChipContent() {
    //Initialisation of data members
    obj()->setIntVal(kDhcRawChipIntCrateNr, 0);
    obj()->setIntVal(kDhcRawChipIntElecAdr, 0);
    obj()->setIntVal(kDhcRawChipIntHitsHi, 0);
    obj()->setIntVal(kDhcRawChipIntHitsLo, 0);
    obj()->setIntVal(kDhcRawChipIntTimeStamp, 0);
    obj()->setIntVal(kDhcRawChipIntMiscInfo, 0);
  };
  
  /**'Copy constructor' needed to interpret LCCollection read from file/database.*/
  DhcRawChipContent(LCObject* obj) : LCGenericObjectImplExt(obj) 
  { 
    assert ( _obj->getNFloat() == 0 );
    assert ( _obj->getNDouble() == 0 );
    assert ( _obj->getNInt() == kDhcRawChipIntValues );
  } 

   /**The destructor */
   virtual ~DhcRawChipContent() {}


  /** Set the high hits */ 
  DhcRawChipContent &setCrateNumber(unsigned int cratenr) {
    obj()->setIntVal(kDhcRawChipIntCrateNr, static_cast<int>(cratenr)); 
    return *this;
  }
  
  unsigned int getCrateNumber() const { 
    return static_cast<unsigned int>(getIntVal(kDhcRawChipIntCrateNr));
  }

  /** Set the electronic channel number*/
  DhcRawChipContent &setElecChannel(unsigned int vmead, unsigned int dcalad, unsigned int dcolad, unsigned int dconad) {
    unsigned int elecad (
			 ( ( (vmead) << SHIFT_V ) |
			 (   (dcalad) << SHIFT_DCAL ) |    
			 (   (dcolad) << SHIFT_DCOL ) |    
			 (   (dconad) << SHIFT_DCON ) ) & 0xffff);
    obj()->setIntVal(kDhcRawChipIntElecAdr, static_cast<int>(elecad)); 
    return *this;
  }
  
  /** Get the entire electronic channel number*/
  int getElecChannel() const {
    return getIntVal(kDhcRawChipIntElecAdr);
}


  /** Return the addresses of individual hardware components*/
  unsigned int getVmeAddress() const { 
    return static_cast<unsigned int>(  (getIntVal(kDhcRawChipIntElecAdr) & MASK_V) >> SHIFT_V);
  }

  unsigned int getDcalAddress() const { 
    return static_cast<unsigned int>(  (getIntVal(kDhcRawChipIntElecAdr) & MASK_DCAL) >> SHIFT_DCAL);
  }

  unsigned int getDcolAddress() const { 
    return static_cast<unsigned int>(  (getIntVal(kDhcRawChipIntElecAdr) & MASK_DCOL) >> SHIFT_DCOL);
  }

  unsigned int getDconAddress() const { 
    return static_cast<unsigned int>(  (getIntVal(kDhcRawChipIntElecAdr) & MASK_DCON) >> SHIFT_DCON);
  }

  /** Set the high hits */ 
  DhcRawChipContent &setHitsHi(unsigned int hitshi) {
    obj()->setIntVal(kDhcRawChipIntHitsHi, static_cast<int>(hitshi)); 
    return *this;
  }
  
  unsigned int getHitsHi() const { 
    return static_cast<unsigned int>(getIntVal(kDhcRawChipIntHitsHi));
  }

  /** Set the low hits */ 
  DhcRawChipContent &setHitsLo(unsigned int hitslo) {
    obj()->setIntVal(kDhcRawChipIntHitsLo, static_cast<int>(hitslo)); 
    return *this;
  }

  /** Return the low hits */
  unsigned int getHitsLo() const { 
    return static_cast<unsigned int>(getIntVal(kDhcRawChipIntHitsLo));
  }

  /** The core, the time stamp */ 
  DhcRawChipContent &setDhcTimeStamp(unsigned int timestamp) {
    obj()->setIntVal(kDhcRawChipIntTimeStamp, static_cast<int>(timestamp)); 
    return *this;
  }

  /** Return the time stamp */
  unsigned int getDhcTimeStamp() const { 
    return static_cast<unsigned int>(getIntVal(kDhcRawChipIntTimeStamp));
  }

  /** Set miscalleneous information */
  DhcRawChipContent &setMiscInfo(bool trg, bool dbt, unsigned char err, unsigned char chksum, unsigned char versum) {
    unsigned int miscinfo (
			 ( ( (trg) << SHIFT_TRG ) |
			 (   (dbt) << SHIFT_DBT ) |    
			 (   (err) << SHIFT_ERR ) |    
			 (   (chksum) << SHIFT_CHKSUM ) |
			 (   (versum) << SHIFT_VERSUM ) ) & 0xffffff);
    obj()->setIntVal(kDhcRawChipIntMiscInfo, static_cast<int>(miscinfo)); 
    return *this;
  }

  /** Return the trigger information */
  bool getTrgInfo() {
    return static_cast<bool>((getIntVal(kDhcRawChipIntMiscInfo) & MASK_TRG) >> SHIFT_TRG);
  }

  /** Return the dbt information */
  bool getDbtInfo() {
    return static_cast<bool>((getIntVal(kDhcRawChipIntMiscInfo) & MASK_DBT) >> SHIFT_DBT);
  }

  /** Return the error information */
  unsigned char getErrInfo() {
    return static_cast<unsigned int>((getIntVal(kDhcRawChipIntMiscInfo) & MASK_ERR) >> SHIFT_ERR);
  }


  /** Return the checksum information */
  unsigned char getChkSum() {
    return static_cast<unsigned char>((getIntVal(kDhcRawChipIntMiscInfo) & MASK_CHKSUM) >> SHIFT_CHKSUM);
  }

  /** Return the verification sum (to check checksum) */
  unsigned char getVerSum() {
    return static_cast<unsigned char>((getIntVal(kDhcRawChipIntMiscInfo) & MASK_VERSUM) >> SHIFT_VERSUM);
  }


  /** Convenient print method*/
  std::ostream& print(std::ostream &ostrm);
  
  const std::string getTypeName() const { return "DhcRawChipContent";} 
  
  /** Return a brief description of the data members*/
  const std::string getDataDescription() const {
    return "I:cratenr,i:elecaddr,i:hihits,i:lohits,i:timestamp,i:miscinfo" ;
  }

}; // class

}
#endif 
