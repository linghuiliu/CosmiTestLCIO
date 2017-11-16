#ifndef FeEvent_hh
#define FeEvent_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"
#ifdef BOUNDARY_CHECK
#include <stdexcept>
#endif

//#define NINT  // see enum EIntValues
//#define NFLOAT 0
//#define NDOUBLE 0

using namespace lcio ;
using namespace CALICE;

namespace CALICE {


  enum EFeEventIntValues {kFeEventIntValBoardId,
			 kFeEventIntValRecLabel,
			 kFeEventIntValTriggerCounter,
			 kFeEventIntValSpyRegister,
			 kNFeEventIntValues};


  /** Stores the trigger history (i.e. fifo content).when it appears in the front end 
   *  as realized for runs starting in May 2006 (Ask Paul for
   *  firmware version)
   * in older versions of the DAQ these data came along wiith
   * the BeTrgEventData which still contains information related
   * to the data handled by the present interface class. 
   *
   * To acces the configuration:
   * <pre>
   *   void processEvent(LCEvent *event)  {
   *       try {
   *        // string _colName = "BeTrgEventData"
   *        LCCollection *col=event->getCollection(_colName);
   *        for (unsigned int element_i=0; element_i<col->getNumberOfElements(); element_i++) {
   *           FeEvent event(col->getElementAt(element_i));
   *           
   *        }
   *   }
   * </pre>
   *
   *
   * @author R. Pöschl (LAL Orsay) 
   * @date Jul  2006
   */
class FeEvent : public LCGenericObjectImplExt {
  
public: 


  /** Simple Constructor */   

  FeEvent() { 

    //Some initialization
    setBoardID(0);
    setRecordLabel(0);
    setTriggerCounter(0);
    setSpyRegister(0); 
  };

  /** UsefulConstructor
   *  @param board_id the packed board id (@ref CALICE::BoardID). 
   */
  FeEvent(unsigned int board_id) {


    //Some initialization
     setBoardID(board_id);
     setRecordLabel(0);
     setTriggerCounter(0);
     setSpyRegister(0); 
     
  };
  
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
      FeEvent(LCObject* obj) : LCGenericObjectImplExt(obj) { } 


  /** Set the packed board id.
   * @sa BoardID
   */
  FeEvent &setBoardID(int boardID) {obj()->setIntVal(kFeEventIntValBoardId,boardID); return *this;};

  /**Get the board id*/
  int getBoardID() const {return getIntVal(kFeEventIntValBoardId);};


 /** Set the Record Label*/
  FeEvent& setRecordLabel(int label) {
    obj()->setIntVal(kFeEventIntValRecLabel, label);
    return *this;
  };

  /** Return the Record Label */
  short getRecordLabel() const {
    return (short) ( getIntVal(kFeEventIntValRecLabel) & 0xff);
  };
  
  /** Set the trigger counter*/
  
  FeEvent &setTriggerCounter(int trigger_counter) 
  {obj()->setIntVal(kFeEventIntValTriggerCounter, trigger_counter); return *this;};
  
  /** Get the Trigger Counter */
  int getTriggerCounter() const {return getIntVal(kFeEventIntValTriggerCounter);};

  /** Set the Spy Register Value (Whatever this is ....*/

  FeEvent &setSpyRegister(int spyregister) 
  {obj()->setIntVal( kFeEventIntValSpyRegister, spyregister); return *this;};

  /** Get the Spy Register Value */
  int getSpyRegister() const {return getIntVal(kFeEventIntValSpyRegister);};


  /** Convenient print method.
   */
  void print(  std::ostream& os) const;  

  const std::string getTypeName() const { 
    return "FeEvent" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:board_ID, i:record_label, i:TriggerCounter, i:SpyRegister" ;
  }

}; // class

}
#endif 
