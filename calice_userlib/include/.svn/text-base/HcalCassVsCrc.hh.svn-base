#ifndef HcalCassVsCrc_h
#define HcalCassVsCrc_h 1

#include <string>
#include <cmath>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NCRINT 3 
#define NCRFLO 0
#define NCRDBL 0




using namespace lcio ;
using namespace std;


namespace CALICE {

class HcalCassVsCrc;

/** Mapping class based on the LCFixedObject template
 *  that relates hcal-cassette <-> crc board
 */
class HcalCassVsCrc : public LCFixedObject<NCRINT,NCRFLO,NCRDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  HcalCassVsCrc(int cassetteID, int slotID, int feID) {

    obj()->setIntVal( 0 , cassetteID  ) ;
    obj()->setIntVal( 1 , slotID  ) ;
    obj()->setIntVal( 2 , feID  ) ;
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  HcalCassVsCrc(LCObject* obj) : LCFixedObject<NCRINT,NCRFLO,NCRDBL>(obj) { } 

  /** Important for memory handling*/
  virtual ~HcalCassVsCrc() { /* no op*/  }
  

  /** the class interface:
    */
  int getCassetteID()   { return obj()->getIntVal(0) ;  } 
  int getSlotID()   { return obj()->getIntVal(1) ;  } 
  int getFeID()   { return obj()->getIntVal(2) ;  } 

  /** convenient print function
   */ 
  void print(  std::ostream& os ) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  const std::string getTypeName() const { 
    return"HcalCassVsCrc" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:CassetteID,i:SlotID,i:FeID" ;
  }

}; // class

}
#endif 
//=============================================================================
