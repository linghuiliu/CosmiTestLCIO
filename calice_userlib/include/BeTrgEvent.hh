#ifndef BeTrgEvent_hh
#define BeTrgEvent_hh 1

#include "lcio.h"
//#include "UTIL/LCFixedObject.h"
#include "LCGenericObjectImplExt.hh"
#include "BoardID.hh"

#include <cassert>

#ifdef BOUNDARY_CHECK
#  include <stdexcept>
#endif

//#define NINT  // see enum EIntValues
//#define NFLOAT 0
//#define NDOUBLE 0

using namespace lcio ;
using namespace CALICE;

namespace CALICE {

/** The indices of all the stored values.
 */
/*  enum EBeTrgEventIntValues {kBeTrgEventIntValBoardId,
			     kBeTrgEventIntValRecLabel,
			     kBeTrgEventIntValPreBusyTriggerCounter,
			     kBeTrgEventIntValTriggerCounter,
			     kBeTrgEventIntValInputStatus,
			     kBeTrgEventIntValInputCatch,
			     kBeTrgEventIntValTriggerBusyStatus,
			     kBeTrgEventIntValInitialFifoStatus,
			     kBeTrgEventIntValFinalFifoStatus,
			     kNBeTrgEventIntValues};
*/



  enum EBeTrgEventIntValues {kBeTrgEventIntValBoardId,
			     kBeTrgEventIntValRecLabel,
			     kBeTrgEventIntValInputStatus,
			     kBeTrgEventIntValInputCatch,
			     kBeTrgEventIntValprebusyCatch,
			     kBeTrgEventIntValsignalCatch,
			     kBeTrgEventIntValcontrol,
			     kBeTrgEventIntValInitialFifoStatus,
			     kBeTrgEventIntValFinalFifoStatus,
			     kBeTrgEventIntValtriggerCounter,
			     kBeTrgEventIntValpreBusyTriggerCounter,
			     kNBeTrgEventIntValues};


  /** Stores the trigger event data.
   *
   * To acces the configuration:
   * <pre>
   *   void processEvent(LCEvent *event)  {
   *       try {
   *        // string _colName = "BeTrgEventData"
   *        LCCollection *col=event->getCollection(_colName);
   *        for (unsigned int element_i=0; element_i<col->getNumberOfElements(); element_i++) {
   *           BeTrgEvent event(col->getElementAt(element_i));
   *           
   *        }
   *   }
   * </pre>
   *
   * @sa ConditionsChangeDelegator
   *
   * @author Götz Gaycken LLR (Ecole Polytechnique)
   * @date Sep  2005
   */
class BeTrgEvent : public LCGenericObjectImplExt {
  
public: 
    

  /** UsefulConstructor
   *  @param board_id the packed board id (@ref CALICE::BoardID). 
   */
  BeTrgEvent(unsigned int board_id) {

     setBoardID(board_id);
  };
  
  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  BeTrgEvent(LCObject* obj) : LCGenericObjectImplExt(obj) { 
    assert ( _obj->getNFloat() == 0);
    assert ( _obj->getNDouble() == 0);
    assert ( _obj->getNInt() >= kNBeTrgEventIntValues);
  } 

  /** set the packed board id.
   * @sa BoardID
   */
  BeTrgEvent &setBoardID(int boardID) {obj()->setIntVal(kBeTrgEventIntValBoardId,boardID); return *this;};

  /**Information on the FE chip the data was received
   */
  int getBoardID() const {return getIntVal(kBeTrgEventIntValBoardId);};

  /** Set the pre busy trigger counter (what ever that is ;-).
   */
  //BeTrgEvent &setPreBusyTriggerCounter(int pre_busy_counter) 
  //{obj()->setIntVal(kBeTrgEventIntValPreBusyTriggerCounter,pre_busy_counter); return *this;};

  /** Get  the pre busy trigger counter (what ever that is ;-).
   */
  //int getPreBusyTriggerCounter() const {return getIntVal(kBeTrgEventIntValPreBusyTriggerCounter);};

  /** Set the trigger counter.
   */
  //BeTrgEvent &setTriggerCounter(int counter) 
  //{obj()->setIntVal(kBeTrgEventIntValTriggerCounter,counter); return *this;};

  /** Get  the trigger counter.
   */
  //int getTriggerCounter() const {return getIntVal(kBeTrgEventIntValTriggerCounter);};

  /** Set the input status.
   */
  BeTrgEvent &setInputStatus(int input_status) 
  {obj()->setIntVal(kBeTrgEventIntValInputStatus,input_status); return *this;};

  /** Get the input status.
   */
  int getInputStatus() const {return getIntVal(kBeTrgEventIntValInputStatus);};

  /** Set the input catch (what ever that is ;-).
   */
  BeTrgEvent &setInputCatch(int input_catch) 
  {obj()->setIntVal(kBeTrgEventIntValInputCatch,input_catch); return *this;};

  /** Get the input catch (what ever that is ;-).
   */
  int getInputCatch() const {return getIntVal(kBeTrgEventIntValInputCatch);};

  /** Set the trigger busy status.
   */
  //BeTrgEvent &setTriggerBusyStatus(int busy_status) 
  //{obj()->setIntVal(kBeTrgEventIntValTriggerBusyStatus,busy_status); return *this;};

  /** Get the trigger busy status.
   */
  //int getTriggerBusyStatus() const {return getIntVal(kBeTrgEventIntValTriggerBusyStatus);};

  /** Set the prebusy Catch.
   */
  BeTrgEvent &setPreBusyCatch(int prebusy_catch) 
  {obj()->setIntVal(kBeTrgEventIntValprebusyCatch,prebusy_catch); return *this;};

  /** Get the prebusy Catch.
   */
   int getPrebusyCatch() const {return getIntVal(kBeTrgEventIntValprebusyCatch);};

  /** Set the signal Catch.
   */
  BeTrgEvent &setSignalCatch(int signal_catch) 
  {obj()->setIntVal(kBeTrgEventIntValsignalCatch,signal_catch); return *this;};

  /** Get the signal Catch.
   */
   int getSignalCatch() const {return getIntVal(kBeTrgEventIntValsignalCatch);};

  /** Set the contrl value.
   */
  BeTrgEvent &setControl(int control) 
  {obj()->setIntVal(kBeTrgEventIntValcontrol,control); return *this;};

  /** Get the control value
   */
   int getControl() const {return getIntVal(kBeTrgEventIntValcontrol);};


  /** Set the inital fifo status.
   */
  BeTrgEvent &setInitialFifoStatus(int initial_fifo_status) 
  {obj()->setIntVal(kBeTrgEventIntValInitialFifoStatus,initial_fifo_status); return *this;};

  /** Get the initial fifo status.
   */
  int getInitialFifoStatus() const {return getIntVal(kBeTrgEventIntValInitialFifoStatus);};

  /** Set the final fifo status.
   */
  BeTrgEvent &setFinalFifoStatus(int final_fifo_status) 
  {obj()->setIntVal(kBeTrgEventIntValFinalFifoStatus,final_fifo_status); return *this;};

  /** Get the final fifo status.
   */
  int getFinalFifoStatus() const {return getIntVal(kBeTrgEventIntValFinalFifoStatus);};

  /** Set the fifo word.
   * @param word_index of the word in the fifo.
   * @param word the value of the word.
   */
  BeTrgEvent &setFifoWord(unsigned int word_index, unsigned int word) 
  {obj()->setIntVal(kNBeTrgEventIntValues+word_index,word); return *this;};

  /** Set all the fifo words.
   * @param n_words the number of words in the fifo.
   * @param word_list a pointer to the fifo words.
   */
  BeTrgEvent &setAllFifoWords(unsigned int n_words, const unsigned int * const word_list) 
  { 
    

    for(unsigned int word_i=0; word_i<n_words; word_i++) {
      obj()->setIntVal(kNBeTrgEventIntValues+word_i,word_list[word_i]); 
    }
    return *this;
  };

  /** Get the number of words contained in the fifo.
   */
  int getNFifoWords() const {
    return getNInt()-kNBeTrgEventIntValues;
  };

  /** Get a fifo word
   * @param word_index of the word in the fifo
   */
  int getFifoWord(unsigned int word_index) const {
#ifdef BOUNDARY_CHECK
    if (word_index >= static_cast<unsigned int>(getNFifoWords())) {
      throw std::out_of_range("BeTrgEvent::getFifoWord> word index out of range!");
    }
#endif
    return getIntVal(kNBeTrgEventIntValues+word_index);
  };
  
  /** Get a fifo word.
   * @param dest reference to a vector of integers which will be cleared in 
   *        filled with the contents of the fifo
   * @return for convenience a reference to the filled vector (i.e. is the one
   *         given as argument).
   *
   * NOTE: Unless a vector is needed to pass the data to some function, it is
   * adviced to get the individual values with getFifoWord() since this avoids
   * copying (On the other hand, each access has the penalty of a virtual 
   * function call, so in some cases it might be advantages to make a copy
   * which will allow faster element acces in subsequent calls.)
   */
  IntVec &getFifoWords(IntVec &dest) const {
    dest.clear();
    for (unsigned int word_i=0; word_i < static_cast<unsigned int>( getNFifoWords() ); word_i++) {
      dest.push_back(getFifoWord(word_i));
    }
    return dest;
  };


  /** Set the trigger counter.
   */
  BeTrgEvent &setTriggerCounter(int trigger_counter) 
  {obj()->setIntVal(kBeTrgEventIntValtriggerCounter,trigger_counter); return *this;};

  /** Get the trigger counter.
   */
  int getTriggerCounter() const {return getIntVal(kBeTrgEventIntValtriggerCounter);};


  /** Set the prebusy Trigger counter.
   */
  BeTrgEvent &setPreBusyTriggerCounter(int prebusy_counter) 
  {obj()->setIntVal(kBeTrgEventIntValpreBusyTriggerCounter,prebusy_counter); return *this;};

  /** Get the prebusy trigger counter.
   */
  int getPreBusyTriggerCounter() const {return getIntVal(kBeTrgEventIntValpreBusyTriggerCounter);};



  /** Convenient print method.
   */
  void print(  std::ostream& os) {
    /*    os << "board: " << BoardID(getBoardId()) <<std::endl
       << " pre busy trigger counter=" << getPreBusyTriggerCounter() << std::endl
       << " trigger counter="          << getTriggerCounter()        << std::endl
       << " trigger busy status="      << getTriggerBusyStatus()     << std::endl
       << " input status="             << getInputStatus()           << std::endl
       << " input catch="              << getInputCatch()            << std::endl
       << " initial fifo status="      << getInitialFifoStatus()     << std::endl
       << " final fifo status="        << getFinalFifoStatus()       << std::endl;
    */
    for (unsigned int fifo_i=0; fifo_i < static_cast<unsigned int>(getNFifoWords()); fifo_i++) {
      os << " " << std::setw(8) << std::hex << getFifoWord(fifo_i);
      if (fifo_i%4==3) {
	os << std::endl;
      }
    }

    os << std::endl;
  };


  const std::string getTypeName() const { 
    return"BeTrgEvent" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:board_ID, i:record_label, i:preBusyCounter, i:triggerCounter, i:inputstatus, i:inputcatch, i:triggerbusystatus, i:initialfifostatus,i:finalfifostatus,i[n]:fifowords" ;
  }

}; // class

}
#endif 
