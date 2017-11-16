#ifndef ErrorBits_hh
#define ErrorBits_hh 1

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>

namespace CALICE {

/** Helper class to query or set the event errors.
 *
 * usage:
 *
 * ErrorBits error(((LCEvent *) evt)->getParameters().getIntVal(PAR_ERROR_BITS));
 *
 * @author Götz Gaycken LLR (Ecole Polytechnique)
 * @date Apr  2005
 */
class ErrorBits 
{
  
public: 

  enum EErrorBits {kNoEventData,            // 0
		   kMissingAdcBlock,        // 1
		   kMissingVLinkHeader,     // 2
		   kInvalidTrigger,         // 3
		   kBadConfigData,          // 4
		   kWrongTriggerCounter,    // 5
		   kCorruptEventRecord,     // 6
                   kMissingTriggerRecord,   // 7
		   kCorruptAcquisition,     // 8
		   kTDCOutOfSynch,          // 9
		   kDirtyEvent,             //10
		   kLargeNegativeSignal,    //11
		   kCorruptBmlRecord,       //12
		   kCorruptDifTrigCounter,  //13
		   kNErrorBits};

  ErrorBits(int trigger_bits) 
    : _errorBits(trigger_bits)
  {
  };

  ErrorBits() {};

  /** Return true if NO error was detected.
   */
  bool operator!() const {
    return _errorBits == 0;
  };

  /** Return true if an error was detected.
   */
  operator bool() const {
    return _errorBits != 0;
  };


  /** Was the event data missing for this event?
   * return true if all ADC blocks were missing;
   */
  bool noEventData() const { return getBit(kNoEventData);};

  /** Set the missing  event data flag
   */
  void setNoEventData() { setBit(kNoEventData);};

  /** Is one or more ADC blocks missing?
   * return true if one or more ADC blocks are missing.
   */
  bool missingADCBlock() const { return getBit(kMissingAdcBlock);};

  /** Set the error bit to flag missing ADC blocks.
   */
  void setMissingADCBlock() { setBit(kMissingAdcBlock);};

  /** Is the VLink Header missing?
   * return true if the VLink header is missing.
   */
  bool missingVLinkHeader() const { return getBit(kMissingVLinkHeader);};

  /** Set the error bit to flag missing VLink headers.
   */
  void setMissingVLinkHeader() { setBit(kMissingVLinkHeader);};

  /** Return true if the trigger counter did not change or is not the same for all "valid" front-ends.
   * Front-ends with missing front-end data are not considered also not connected front-ends. 
   */
  bool wrongTriggerCounter() const { return getBit(kWrongTriggerCounter);};

  /** Set the error bit to flag errors in the trigger counter field
   */
  void setWrongTriggerCounter() { setBit(kWrongTriggerCounter);};

  /** Return true if the event record of one front-end was corrupt.
   */
  bool corruptEventRecord() const { return getBit(kCorruptEventRecord);};

  /** Set the error bit to flag events with corrupt event records.
   */
  void setCorruptEventRecord() { setBit(kCorruptEventRecord);};


  /** Is the Vlink header missing ?.
   * return true if the VLink header is missing.
   */
  bool invalidTrigger() const { return getBit(kInvalidTrigger);};

  /** Set the error bit to flag missing VLink headers.
   */
  void setInvalidTrigger() { setBit(kInvalidTrigger);};

  /** Error in the verification of the Configuration Data 
    */
  bool badConfigurationData() const { return getBit(kBadConfigData);}; 

  /** Set the error bit to flag bad Configuration Data.
   */
  void setBadConfigurationData() { setBit(kBadConfigData);};

/** No trigger exists for a given event */
  bool noTrigger() const { return getBit(kMissingTriggerRecord);};

  /** Set the error bit to flag bad Configuration Data.
   */
  void setNoTrigger() { setBit(kMissingTriggerRecord);};

/** The acquisition sequence is screwed up
 * i.e. unequal numbers of event and trigger records
  */
  bool corruptAcquisition() const { return getBit(kCorruptAcquisition);};

  /** Set the error bit to flag a wrong acquistion.
   */
  void setCorruptAcquisition() { setBit(kCorruptAcquisition);};

/** The event number in the TDC does not agree with the 
 * the event number as derived from the CrcBoards
  */
  bool tdcOutOfSynch() const { return getBit(kTDCOutOfSynch);};

  /** Set the error bit to flag suspicious TDC data
   */
  void setTDCOutOfSynch() { setBit(kTDCOutOfSynch);};


  /** Return true if the event record of one front-end was corrupt.
   */
  bool corruptBmlRecord() const { return getBit(kCorruptBmlRecord);};

  /** Set the error bit to flag events with corrupt event records.
   */
  void setCorruptBmlRecord() { setBit(kCorruptBmlRecord);};

  /** The event contained a significant fraction of negative signals.
   * This may indicate pedestal drift, overlapping events, ...
  */
  bool largeNegativeSignal() const { return getBit(kLargeNegativeSignal);};

  /** Set this error bit if the event contains a significant fraction of negative signals.
   * This may indicate pedestal drift, overlapping events, ...
  */
  void setLargeNegativeSignal() { setBit(kLargeNegativeSignal);};

  /** Thers is additional trigger activity close to or at the main word.
  */
  bool dirtyEvent() const { return getBit(kDirtyEvent);};

  /** Set this error bit if there is additional trigger activity close to or at the main word.
  */
  void setDirtyEvent() { setBit(kDirtyEvent);};

  /** Return true if one of the dif trigger counters gets out of synch.
   */
  bool corruptDifTrigCounter() const { return getBit(kCorruptDifTrigCounter);};

  /** Set the error bit to flag events with corrupt dif counter.
   */
  void setCorruptDifTrigCounter() { setBit(kCorruptDifTrigCounter);};



  /** Has an error occurred?
   */
  bool isSet() const { return _errorBits != 0;};

  /** return the error bits.
   */
  int getBits() const {return _errorBits;};

  /** print the current errors.
   */
  std::ostream &print(std::ostream &out) const;

  /** Translate an error name into a bit.
   */
  static unsigned int getErrorBit(const std::string &name);

  /** Get the name assigned to an error  bit.
   */
  static const char *getErrorName(unsigned int bit_i) {
#ifdef BOUNDARY_CHECK
    assert ( bit_i < kNErrorBits );
#endif
    return __name[bit_i]; 
  }

  /** Create a error bit mask from a vector of error names.
   */
  static unsigned int getErrorBitMask(const std::vector< std::string> &error_names);


private:
  /** get an error bit.
   */
  bool getBit(unsigned int bit) const {
    return _errorBits & (1<<bit);
  };

  /** set an error bit.
   */
  void setBit(unsigned int bit) {
    _errorBits |= (1<<bit);
  };
  
  int _errorBits;  /**< The error bits. If non zero an error has occurred. */

  static const char *__name[kNErrorBits];
  
  static std::map<std::string, unsigned short> __nameToBit;

}; // class

  inline std::ostream &operator <<(std::ostream &os, const ErrorBits &error) {
    return error.print(os);
  }

}
#endif 
