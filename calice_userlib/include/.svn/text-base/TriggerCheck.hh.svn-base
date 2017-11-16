#ifndef TriggerCheck_h
#define TriggerCheck_h 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"

#define NTCINT 4 
#define NTCFLO 0 
#define NTCDBL 0 

using namespace lcio ;

namespace CALICE {

class TriggerCheck ;

/** Example for a simple class based on the LCFixedObject template.
 *  <p>
 *  This is an interface to the parameters needed for the trigger
 *  checking. 
 *  Trigger Checking consists of
 *  a) the word in the fifo where we expect the fired trigger (main word)
 *  b) the distance from the last appearance of the pattern in the
 *     main word where we allow one the mainword bits to be set
 *     'Tolerance'
 *     This accounts for finite propagation time of signals in the hardware
 *  c) The number of words upwards and downwards of the mainword to be checked
 *  
 */
class TriggerCheck : public LCFixedObject<NTCINT,NTCFLO,NTCDBL> {
  
public: 
  
  /** Convenient c'tor.
   */
  TriggerCheck(int main_word, int tolerance, int up, int down) {
    obj()->setIntVal( 0 , main_word  ) ;
    obj()->setIntVal( 1 , tolerance  ) ;
    obj()->setIntVal( 2 , up  ) ;
    obj()->setIntVal( 3 , down  ) ;
  }

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  TriggerCheck(LCObject* obj) : LCFixedObject<NTCINT,NTCFLO,NTCDBL>(obj) { } 

  /** Important for memory handling*/
  virtual ~TriggerCheck() { /* no op*/  }
  

  /** get the 'main fifo word' */
  int getMainWord()   { return obj()->getIntVal(0) ;  } 
  /** get the distance word (see above) */
  int getTolerance()   { return obj()->getIntVal(1) ;  } 
  /** number of words to be searched in upwards direction */
  int getUpSearch()   { return obj()->getIntVal(2) ;  } 
  /** number of words to be searched in downwards direction */
  int getDownSearch()   { return obj()->getIntVal(3) ;  } 
  
  /** Convenient print method */
  void print(  std::ostream& os ) ;
  
  // -------- need to implement abstract methods from LCGenericObject

  /** Return type name */
  const std::string getTypeName() const { 
    return"Trigger_Check   " ;
  } 
  
  /** Return data description */
  const std::string getDataDescription() const {
       return "i:main_word,i:tolerance,i:up_search,i:down_search" ;
  }

}; // class

}
#endif 
//=============================================================================
