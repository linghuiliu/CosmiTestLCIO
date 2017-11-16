#ifndef TRIGGERHANDLER_H
#define TRIGGERHANDLER_H 1


/** Think it is safe to make the class depending on LCIO
  */
#include "lcio.h"
#include "UTIL/LCTime.h"

#include <string>


using namespace std;
using namespace lcio;


namespace CALICE {

/** Base Class for the user access to trigger decisions. 
  * It defines the core interface to the trigger information. 
  * The concrete implementations have to be singletons
  * such that the TriggerHandling can be plugged in anywhere in
  *
  * @author R. Pöschl DESY
  * @date May 9 2005
  *
  */

class TriggerHandler {


public:

  /** Destructor
   */
  virtual ~TriggerHandler() {/*no op*/} 

  /** This adds the trigger information to the event
    */
  virtual void setTrigger(LCEvent*) = 0;

  /**Trigger Validation 
   */
  virtual bool isValidTrigger(const LCEvent*) = 0;


  /** A initialization method
    * Don't know whether it will be useful 
    */  
  virtual void init() = 0; 


private:
  /** The private constructor
    */
  //TriggerHandler() {/*no op*/}
  
};

}


#endif
