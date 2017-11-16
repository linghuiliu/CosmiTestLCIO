#ifndef SiPMVolCorr_h
#define SiPMVolCorr_h 1

#include <string>
#include <cmath>

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NVCINT 1 
#define NVCFLO 1
#define NVCDBL 0




using namespace lcio ;
using namespace std;



namespace CALICE {

class SiPMVolCorr;

/** voltage corrections for Hcal SiPMs 
 */
class SiPMVolCorr : public LCFixedObject<NVCINT,NVCFLO,NVCDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  SiPMVolCorr(int SiPMID, float voltage) {

    obj()->setIntVal( 0 , SiPMID  ) ;
    obj()->setFloatVal( 0 , voltage  ) ;
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  SiPMVolCorr(LCObject* obj) : LCFixedObject<NVCINT,NVCFLO,NVCDBL>(obj) { } 

  /** Important for memory handling*/
  virtual ~SiPMVolCorr() { /* no op*/  }
  

  /** the class interface:
    */
  int getSiPMID()   { return obj()->getIntVal(0) ;  } 
  float getVolCorr()   { return obj()->getFloatVal(0) ;  } 

  /** convenient print function
   */ 
  void print(  std::ostream& os ) ;
  

  // -------- need to implement abstract methods from LCGenericObject

  const std::string getTypeName() const { 
    return"SiPMVoltageCorrection" ;
  } 
  
  const std::string getDataDescription() const {
    return "i:SiPMID,f:voltage";
  }

}; // class

}
#endif 
//=============================================================================
