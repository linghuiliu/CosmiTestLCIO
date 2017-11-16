#ifndef FeTrgData_hh
#define FeTrgData_hh 1

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


  enum EFeTrgDataIntValues {kFeTrgDataIntValBoardId,
			     kFeTrgDataIntValRecLabel,
			     kFeTrgDataIntValHeader,
			     kFeTrgDataIntValTrailer,
			     kFeTrgDataIntValTrigCounter,
			     kNFeTrgDataIntValues};


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
   *           BeTrgEvent event(col->getElementAt(element_i));
   *           
   *        }
   *   }
   * </pre>
   *
   *
   * @author R. Pöschl (LAL Orsay) 
   * @date Jul  2006
   */
class FeTrgData : public LCGenericObjectImplExt {
  
public: 
    

  /** UsefulConstructor
   *  @param board_id the packed board id (@ref CALICE::BoardID). 
   */
  FeTrgData(unsigned int board_id) {

     setBoardID(board_id);
     setRecordLabel(0);
     setHeaderWord(0);
     setTrailerWord(0);
     setTriggerCounter(0);


  };
  
    /** 'Copy constructor' needed to interpret LCCollection read from file/database.
     */
  FeTrgData(LCObject* obj) : LCGenericObjectImplExt(obj) { 
    assert ( _obj->getNFloat() == 0 );
    assert ( _obj->getNDouble() == 0 );
    assert ( _obj->getNInt() >= kNFeTrgDataIntValues );
  } 


  /** set the packed board id.
   * @sa BoardID
   */
  FeTrgData &setBoardID(int boardID) {obj()->setIntVal(kFeTrgDataIntValBoardId,boardID); return *this;};

  /**Get the board id*/
  int getBoardID() const {return getIntVal(kFeTrgDataIntValBoardId);};

  /** set the record label.*/
  FeTrgData &setRecordLabel(int label)
  {obj()->setIntVal(kFeTrgDataIntValRecLabel, label); return *this;};

  /** get the record label*/
  int getRecordLabel() const {return getIntVal(kFeTrgDataIntValRecLabel);};


  /** The header which comes along with the CrcVlinkTrgData*/

  FeTrgData &setHeaderWord(int header) 
  {obj()->setIntVal(kFeTrgDataIntValHeader, header); return *this;};

  /** Get the header word
   */
  int getHeaderWord() const {return getIntVal(kFeTrgDataIntValHeader);};

  /** The trailer which comes along with the CrcVlinkTrgData*/
  FeTrgData &setTrailerWord(int trailer) 
  {obj()->setIntVal(kFeTrgDataIntValTrailer, trailer); return *this;};

  /** Get the trailer word
   */
  int getTrailerWord() const {return getIntVal(kFeTrgDataIntValTrailer);};

  /** Set the trigger counter (=trigger number)*/
  FeTrgData &setTriggerCounter(int trigcount) 
  {obj()->setIntVal(kFeTrgDataIntValTrigCounter, trigcount); return *this;};

  /** Get the trigger number 
   */
  int getTriggerCounter() const {return getIntVal(kFeTrgDataIntValTrigCounter);};

  /** Set the fifo word.
   * Remmeber the fifo contains the -trigger history-
   * @param word_index of the word in the fifo.
   * @param word the value of the word.
   */
  FeTrgData &setFifoWord(unsigned int word_index, unsigned int word) 
  {obj()->setIntVal(kNFeTrgDataIntValues+word_index,word); return *this;};

  /** Set all the fifo words.
   * @param n_words the number of words in the fifo.
   * @param word_list a pointer to the fifo words.
   */
  FeTrgData &setAllFifoWords(unsigned int n_words, const unsigned int * const word_list) 
  { 
    

    for(unsigned int word_i=0; word_i<n_words; word_i++) {
      obj()->setIntVal(kNFeTrgDataIntValues+word_i,word_list[word_i]); 
    }
    return *this;
  };

  /** Get the number of words contained in the fifo.
   */
  int getNFifoWords() const {
    return getNInt()-kNFeTrgDataIntValues;
  };

  /** Get a fifo word.
   * @param word_index of the word in the fifo.
   */
  int getFifoWord(unsigned int word_index) const {
#ifdef BOUNDARY_CHECK
    if (word_index>static_cast<unsigned int>(getNFifoWords()) ) {
      throw std::out_of_range("FeTrgData::getFifoWord> word index out of range!");
    }
#endif
    return getIntVal(kNFeTrgDataIntValues+word_index);
  };
  
  /** Get a fifo word.
   * @param dest reference to a vector of integers whiich will be cleared in 
   *        filled with the contents of the fifo.
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





  /** Convenient print method.
   */
  void print(  std::ostream& os) {
    os << " board: " << BoardID(getBoardID()) <<std::endl
       << " record label: " << getRecordLabel() << std::endl
       << " header=" << getHeaderWord() << std::endl
       << " trailer" << getTrailerWord() << std::endl
       << " trigger counter" << getTriggerCounter() << std::endl;

      for (unsigned int fifo_i=0; fifo_i < static_cast<unsigned int>(getNFifoWords()); fifo_i++) {
	os << " " << std::setw(8) << std::hex << getFifoWord(fifo_i);
	if (fifo_i%4==3) {
	  os << std::endl;
	}
      }
    
    os << std::endl;
  };
  

  const std::string getTypeName() const { 
    return "FeTrgData" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:board_ID, i:record_label, i:Header, i:Trailer, i:Triggercounter, i[n]:fifowords" ;
  }

}; // class

}
#endif 
