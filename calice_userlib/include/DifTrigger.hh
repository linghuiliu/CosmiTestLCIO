#ifndef DifTrigger_hh
#define DifTrigger_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
#include "LCGenericObjectImplExt.hh"
#include <cassert>

using namespace lcio ;
using namespace CALICE;

namespace CALICE {


  enum EDifTriggerIntValues {kDifTriggerCounter,
			 kDifTriggerIntValues};


  /** Simple class which stores the trigger counter retrieved from the DIF cards 
   *
   * @author R. Pöschl (LAL Orsay) 
   * @date Dec  2010
   */
class DifTrigger : public LCGenericObjectImplExt {
  
public: 


  /** Simple Constructor */   
  DifTrigger() {};
  
  /**'Copy constructor' needed to interpret LCCollection read from file/database.*/
  DifTrigger(LCObject* obj) : LCGenericObjectImplExt(obj) 
  { 
    assert ( _obj->getNFloat() == 0 );
    assert ( _obj->getNDouble() == 0 );
    assert ( _obj->getNInt() == kDifTriggerIntValues );
  } 

   /**The destructor */
   virtual ~DifTrigger() 
   { 
    
   }



  /** Set the trigger counter*/
  DifTrigger &setTriggerCounter(int triggercounter) {obj()->setIntVal(kDifTriggerCounter, triggercounter); return *this;};

  /** Get the trigger counter*/
  int getTriggerCounter() const {return getIntVal(kDifTriggerCounter);}

  /** Convenient print method*/
  std::ostream& print(std::ostream &ostrm);
  
  const std::string getTypeName() const { return "DifTrigger";} 
  
  /** Return a brief description of the data members*/
  const std::string getDataDescription() const {
    return "i:triggercounter" ;
  }

}; // class

}
#endif 
