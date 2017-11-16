#ifndef BeEvent_hh
#define BeEvent_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"
#ifdef BOUNDARY_CHECK
#include <stdexcept>
#endif
#include <cassert>

//#define NINT  // see enum EIntValues
//#define NFLOAT 0
//#define NDOUBLE 0

using namespace lcio ;
using namespace CALICE;

namespace CALICE {


  enum EBeEventIntValues {kBeEventIntValBoardId,
			 kBeEventIntValRecLabel,
			 kBeEventIntValStatusWord,
			 kBeEventIntValL1aCounter,
			 kBeEventIntValBxCounter,
			 kBeEventIntValQdrFrameCounter,			 
			 kBeEventIntValQdrDataCounter,			 
			 kBeEventIntValTotalFrameCounter,			 
			 kNBeEventIntValues};


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
   *           BeEvent event(col->getElementAt(element_i));
   *           
   *        }
   *   }
   * </pre>
   *
   *
   * @author R. Pöschl (LAL Orsay) 
   * @date Jul  2006
   */
class BeEvent : public LCGenericObjectImplExt {
  
public: 


  /** Simple Constructor */   

  BeEvent() { 

    //Some initialization
    setBoardID(0);
    setRecordLabel(0);
    setStatusWord(0);
    setL1aCounter(0); 
    setBxCounter(0);
    setQdrFrameCounter(0);
    setQdrDataCounter(0);
    setTotalFrameCounter(0);
  };
  /** UsefulConstructor
   *  @param board_id the packed board id (@ref CALICE::BoardID). 
   */
  BeEvent(unsigned int board_id) {


    //Some initialization
     setBoardID(board_id);
     setRecordLabel(0);
     setStatusWord(0);
     setL1aCounter(0); 
     setBxCounter(0);
     setQdrFrameCounter(0);
     setQdrDataCounter(0);
     setTotalFrameCounter(0);
     
  };
  
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
      BeEvent(LCObject* obj) : LCGenericObjectImplExt(obj) 
  { 
    assert ( _obj->getNFloat() == 0 );
    assert ( _obj->getNDouble() == 0 );
    assert ( _obj->getNInt() == kNBeEventIntValues );
  } 


  /** Set the packed board id.
   * @sa BoardID
   */
  BeEvent &setBoardID(int boardID) {obj()->setIntVal(kBeEventIntValBoardId,boardID); return *this;};

  /**Get the board id*/
  int getBoardID() const {return getIntVal(kBeEventIntValBoardId);};


 /** Set the Record Label*/
  BeEvent& setRecordLabel(int label) {
    obj()->setIntVal(kBeEventIntValRecLabel, label);
    return *this;
  };

  /** Return the Record Label */
  short getRecordLabel() const {
    return (short) ( getIntVal(kBeEventIntValRecLabel) & 0xff);
  };
  
  /** Set the status word*/
  
  BeEvent &setStatusWord(int status) 
  {obj()->setIntVal(kBeEventIntValStatusWord, status); return *this;};
  
  /** Get the status word
   */
  int getStatusWord() const {return getIntVal(kBeEventIntValStatusWord);};

  /** Set the l1acounter 
    * This number is used to check integrity of trigger counters 
    */

  BeEvent &setL1aCounter(int l1acounter) 
  {obj()->setIntVal(kBeEventIntValL1aCounter, l1acounter); return *this;};

  /** Get the l1acounter */
  int getL1aCounter() const {return getIntVal(kBeEventIntValL1aCounter);};


  /** Set the bxcounter (bunch crossing !?)*/
  BeEvent &setBxCounter(int bxcount) 
  {obj()->setIntVal(kBeEventIntValBxCounter, bxcount); return *this;};

  /** Get the bxcounter 
   */
  int getBxCounter() const {return getIntVal(kBeEventIntValBxCounter);};


  /** Set the QdrFrameCounter (Whatever this is ...) */
  BeEvent &setQdrFrameCounter(int qdrframecount) 
  {obj()->setIntVal(kBeEventIntValQdrFrameCounter, qdrframecount); return *this;};

  /** Get the QdrFrameCounter*/
  int getQdrFrameCounter() const {return getIntVal(kBeEventIntValQdrFrameCounter);};


  /** Set the QdrDataCounter (Whatever this is ...) */
  BeEvent &setQdrDataCounter(int qdrdatacount) 
  {obj()->setIntVal(kBeEventIntValQdrDataCounter, qdrdatacount); return *this;};

  /** Get the QdrDataCounter*/
  int getQdrDataCounter() const {return getIntVal(kBeEventIntValQdrDataCounter);};


  /** Set the TotalFrameCounter (Whatever this is ...) */
  BeEvent &setTotalFrameCounter(int totalframecount) 
  {obj()->setIntVal(kBeEventIntValTotalFrameCounter, totalframecount); return *this;};

  /** Get the TotalFrameCounter*/
  int getTotalFrameCounter() const {return getIntVal(kBeEventIntValTotalFrameCounter);};



  /** Convenient print method.
   */
  void print(  std::ostream& os) {
    os << " board: " << BoardID(getBoardID()) << std::endl
       << " record label: " << getRecordLabel() << std::endl 
       << " status: " << getStatusWord() << std::endl
       << " l1acounter: " << getL1aCounter() << std::endl
       << " bxcounter: " << getBxCounter() << std::endl
       << " qdrframecounter: " << getQdrFrameCounter() << std::endl
       << " qdrdatacounter: " << getQdrDataCounter() << std::endl
       << " totalframecounter: " << getTotalFrameCounter() << std::endl;
    os << std::endl;
  };
  

  const std::string getTypeName() const { 
    return "BeEvent" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:board_ID, i:record_label, i:Status, i:L1aCounter, i:BxCounter, i:QdrFraeCounter, i:QdrDataCounter, i:TotalFrameCounter" ;
  }

}; // class

}
#endif 
