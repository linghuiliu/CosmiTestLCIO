#ifndef FeInfo_hh
#define FeInfo_hh 1

#include "lcio.h"
#include "UTIL/LCFixedObject.h"
#include "BoardID.hh"


//Header which allows us to build an md5 checksum
#include "md5.hh"

#ifdef VALUE_CHECK
#  include <cassert>
#endif
// #define NINT  // see enum EIntValues
#define NFLOAT 0
#define NDOUBLE 0

using namespace lcio ;

namespace CALICE {

  /** The indices of all the stored values.*/

enum EIntValues {kIntValFeInfoBoardId, kIntValFeInfoLabel, kIntValFeInfoFeID,
		 kIntValFeInfoFrameSyncOutPacked0,kIntValFeInfoFrameSyncOutPacked2,
		 kIntValFeInfoFeLength,
		 kIntValFeInfoFrameSyncOutPacked4,
		 kIntValFeInfoFifo,
		 kIntValFeInfoTriggerCounter,
                 kNIntValuesFeInfo};

/** Stores the configuration data of the front-end of one CERC board.
 * Based on @ref AdcBlock.
 * This is just a copy of the Calice DAQ class CrcFeConfigurationData. Since 
 * I don't have a profound understanding of the meaning of its members the 
 * documentation is mostly missing or at least very poor.
 * 
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */
class FeInfo : public LCFixedObject<kNIntValuesFeInfo,NFLOAT,NDOUBLE> {
  
public: 
    

  /** Convenient c'tor.
   * @param boardID Crc board ID (VME crate, slot, fe))
   */
  FeInfo(int boardID) {
    obj()->setIntVal( kIntValFeInfoBoardId , boardID  ) ;
  };

  /** Get the Crc Board ID.
   */
  int  getBoardID() const {return getIntVal(kIntValFeInfoBoardId);};
  

  /** 'Copy constructor' needed to interpret LCCollection read from file/database.
   */
  FeInfo(LCObject* obj) : LCFixedObject<kNIntValuesFeInfo,NFLOAT,NDOUBLE>(obj) { } 
  
  /** Set the fe link label(?). 
   * @todo needs better explanation.
   */
  FeInfo &  setLabel(int label) {obj()->setIntVal(kIntValFeInfoLabel,label);return *this;};

  /** Get the Fe link label(?).
   * @todo needs better explanation.
   */
  int  getLabel() const {return getIntVal(kIntValFeInfoLabel);};

  /** Set the front-end id.
   * This field is redundant since the front-end id is also encoded in the board ID.
   */
  FeInfo &  setFeID(int fe_id) {obj()->setIntVal(kIntValFeInfoFeID,fe_id);return *this;};

  /** Get the front-end id.
   * This field is redundant since the front-end id is also encoded in the board ID.
   */
  int  getFeID() const {return getIntVal(kIntValFeInfoFeID);};
  
  /** Set the four frame sync out half words.
   * @param sync_words_01 the first and the second half word packed in one 32bit integer.
   * @param sync_words_23 the third and the forth half word packed in one 32bit integer.
   * @param sync_word_4   the fifth half word packed in the lower 16 bits of a 32bit integer.
   
   * The first word should be in the lower 16 bit of the first argument, the second word in the
   * upper 16 bits of the first argument and so forth:
   * <P>
   * <PRE>
   * sync_words_01= (word_0 & 0xffff) + ((word_1 & xffff)<<16);
   * </PRE>
   */
  FeInfo &  setFrameSyncOut(int sync_words_01, int sync_words_23, int sync_word_4) {
    obj()->setIntVal(kIntValFeInfoFrameSyncOutPacked2,sync_words_01);
    obj()->setIntVal(kIntValFeInfoFrameSyncOutPacked0,sync_words_23);
    obj()->setIntVal(kIntValFeInfoFrameSyncOutPacked4,sync_word_4);
    return *this;
  };

  /** Get the frame sync out half words.
   * @param word_i the index of the word (valid range 0-4).
   * @return the specified 16 bit half word.
   */
  int  getFrameSyncOut(int word_i) const {
#ifdef BOUNDARY_CHECK
    assert(static_cast<unsigned int>(word_i)<5);
#endif
    if (word_i==4) {
      return (getIntVal(kIntValFeInfoFrameSyncOutPacked4) ) & 0xffff;
    }
    else {
      word_i = (word_i >= 2 ? word_i-2 : word_i+2);
      if (word_i & 1) {
	// high half word
	return (getIntVal(kIntValFeInfoFrameSyncOutPacked0+(word_i>>1)) >> 16 ) & 0xffff;
      }
      else {
	// low half word
	return (getIntVal(kIntValFeInfoFrameSyncOutPacked0+(word_i>>1)) ) & 0xffff;
      }
    }
  };
  

  /** Set the fe length field (?).
   * @todo needs better explanation.
   */
  FeInfo &  setFeLength(int fe_length) {  
    // the fe length is sotred in the high half word.
    obj()->setIntVal(kIntValFeInfoFeLength,getIntVal(kIntValFeInfoFeLength) & 0xffff
					   + ((fe_length & 0xffff)<<16) );
    return *this;
  };

  /** Get the fe length field (?).
   * @todo needs better explanation.
   */
  int  getFeLength() const {
    // the fe length is sotred in the high half word.
    return (getIntVal(kIntValFeInfoFeLength)>>16) & 0xffff;
  };


  /** Set the back-end status word.
   * @todo needs better explanation.
   */
  FeInfo &  setBeStatus(int be_status) {
    // the be status high half word is stored in the low half word of the fe length field
    obj()->setIntVal(kIntValFeInfoFeLength,  getIntVal(kIntValFeInfoFeLength) & 0xffff0000
		                             + ((be_status>>16) & 0xffff) );

    // the be status low half word is stored in the high half word of the forth frame sync out word:
    obj()->setIntVal(kIntValFeInfoFrameSyncOutPacked4,  getIntVal(kIntValFeInfoFrameSyncOutPacked4) & 0xffff
		                                     + ((be_status & 0xffff)<<16) );
    return *this;
  };

  /** Get the back-end status word.
   * @todo needs better explanation.
   */
  int  getBeStatus() const {return   ((getIntVal(kIntValFeInfoFeLength) & 0xffff)<<16)
			           + ((getIntVal(kIntValFeInfoFrameSyncOutPacked4)>>16) & 0xffff);};


  /** Set the trigger counter.
   * The trigger counter can be used to identify front-end data of the same event.
   */
  FeInfo &  setTriggerCounter(int trigger_counter) {obj()->setIntVal(kIntValFeInfoTriggerCounter,trigger_counter);return *this;};

  /** Get the trigger counter.
   * The trigger counter can be used to identify front-end data of the same event.
   */
  int  getTriggerCounter() const {return getIntVal(kIntValFeInfoTriggerCounter);};

  /** Set the fifo status, and the front-end id.
   * @param fifo the packed word which contains the front-end id, and the fifo status (overflow/underflow bits, fifo words).
   */
  FeInfo &  setFifo(int fifo) {obj()->setIntVal(kIntValFeInfoFifo,fifo);return *this;};

  /** Get the front-end id from the fifo status word.
   * If this front-end id does not match the result of @ref getFeID then all the front-end
   * data for this particular event is corrupt.
   */
  int  getFifoFeID() const {return getIntVal(kIntValFeInfoFifo) & 0xff;};

  /** Get the fifo status (without front-end ID.
   */
  int  getFifoStatus() const {return getIntVal(kIntValFeInfoFifo) >> 8;};

  /** Get number of words in the fifo
   */
  int  getNumberOfFifoWords() const {
    return ((getIntVal(kIntValFeInfoFifo) >> 8) &0xffff);
  };

  /** Check whether the fifo is full.
   * @return true if the fifo is full;
   */
  bool  isFifoFull() const {return getIntVal(kIntValFeInfoFifo) & (1<<25);};

  /** Check whether the fifo is empty.
   * @return true if the fifo is empty;
   */
  bool  isFifoEmpty() const {return getIntVal(kIntValFeInfoFifo) & (1<<24);};

  /** Convenient print method 
   */
  void print(  std::ostream& os) const;

  // -------- need to implement abstract methods from LCGenericObject

  /** Return the type of the class 
   */
  const std::string getTypeName() const { 
    return"FeInfo" ;
  } 
  
  /** Return a brief description of the data members 
   */
  const std::string getDataDescription() const {
    return "i:BoardID,RecordLabel,fe; s:packed[5]:frameSyncOutPacked; s:beStatus,feLength; i:fifo, trigger counter" ;
  }

}; // class

inline std::ostream &operator <<(std::ostream &os, const FeInfo &a) {
  a.print(os);
  return os;
}


}
#endif 
