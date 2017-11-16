#ifndef _EventStatList_t_hh_
#define _EventStatList_t_hh_

#include <FeStatList_t.hh>

/** Statistics about event data record errors.
 * The object is meant to be used together with @ref FeStatList_t.
 * This object collects statistics about bad event data records
 * The following errors are counted: 
 *  the same event was read out twice;
 *  the trigger counter does not match the trigger counter of another Crc Board;
 *  the record is corrupt.
 * @author Goetz Gaycken, LLR - Ecole polytechnique
 * @date feb, 2006
 */
class EventStat_t : public StatObj_t
{
public:
  EventStat_t() :
  _sameEvent(0),_corruptRecord(0),_triggerCounterMismatch(0),_slotTriggerCounterMismatch(0),_missingFeData(0),_nFeData(0), _nBadHeader(0), _nDesCrcVLink(0) {};

  /** does nothing.
   */
  void finish() {
  }

  /** Print the table header.
   */
  void printTableHeader(std::ostream &out) const;

  /** Print the statistics of one object (one table row).*/
  void print(std::ostream &out);

  /** Return true if at least one error was detected for the corresponding front-end.*/
  bool hasInfo() const {return error()>0;};

  /** Notify that the same event was read out twice.
   */
  void sameEvent() {_sameEvent++;};

  /** Notify that the record is corrupt.
   */
  void corruptRecord() {_corruptRecord++;};
  
  /** Notify about a trigger counter mismitch wrt. to the last "good" front-end.
   */
  void triggerCounterMismatch() {_triggerCounterMismatch++;};

  /** Notify about a trigger counter mismitch wrt. to the previous Crc board.
   */
  void slotTriggerCounterMismatch() {_slotTriggerCounterMismatch++;};

  /** Notify missing front-end event data.
   * The front-end event data is needed to identify corrupt records and find trigger counter mismatches. 
   */
  void noFeData() {_missingFeData++;};

  /** Notify about available front-end event data.
   * If front-end data is missing always than it is not an error.
   */
  void feData() {_nFeData++;};
  
  /** Notify bad Vlink Headers according to Pauls verify function */

  void setBadVLinkHeader() {_nBadHeader++;}

  /** Notify completetly corrupt ADC records (e.g. at beginning of
      config */

  void setDestroyedCrcVLinkRecords() {_nDesCrcVLink++;}

  /** The total number of events for which the previous event was read again instead of a new one.
   */
  unsigned int refreshErrors() const {return _sameEvent;};

  /** The total number of events for which the event data record was corrupt.
   */
  unsigned int recordErrors() const {return _corruptRecord;};

  /** The total number of events for which the trigger counter did not match the one of the previous front-end.
   */
  unsigned int triggerCounterErrors() const {return _triggerCounterMismatch;};

  /** The total number of events for which the trigger counter did not match the one of the previous slot.
   */
  unsigned int slotTriggerCounterErrors() const {return _slotTriggerCounterMismatch;};

  /** The total number of events for which the front-end event data was missing.
   */
  unsigned int eventsWithoutFeData() const {return ( _nFeData==0 ? false : _missingFeData);};

  /** Return the number of bad VLink Headers */

  unsigned int badVLinkHeaders() {return _nBadHeader;}

  /** Return the number of destroyed CrcVLinkData */

  unsigned int destroyedCrcVLinkRecords() {return _nDesCrcVLink;}

  /** The total number of errors.
   */
  unsigned int error() const {return
      _sameEvent+_corruptRecord+( _nFeData==0 ? false : _missingFeData)+_triggerCounterMismatch +
  _nBadHeader + _nDesCrcVLink;};

private:
  unsigned int _sameEvent;
  unsigned int _corruptRecord;
  unsigned int _triggerCounterMismatch;
  unsigned int _slotTriggerCounterMismatch;
  unsigned int _missingFeData;
  unsigned int _nFeData;
  /** variables counting missing header info */
  unsigned int _nBadHeader;
  unsigned int _nDesCrcVLink;


};

typedef FeStatList_t<EventStat_t> EventStatList_t;
  
#endif
