/* Processor which wraps the CaliceTriggerProcessor*/

#ifndef CALICETRIGGERPROCESSOR_h
#define CALICETRIGGERPROCESSOR_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 


#include "TriggerProcessorBase.hh"
 
#ifdef WITH_CONTROL_HISTOGRAMS
#  include "histmgr/Key_t.hh"
#endif


#include <string>
#include "TriggerProcessorBase.hh"

class TriggerHandlerCalice;

namespace marlin {

/** Class which adds basic trigger information to the event parameters
  * In order to make use of the trigger information this processor has
  * to run *before* each other user defined processor. 
  * It initializes the trigger handler through which the 
  * user can obtain more information on trigger issues in other processors. 
  * Here it adds the trigger which have fired the event to the event parameters.  
  * This trigger information is made persistent with the event and
  * can be later on used in higher level analysis.
  * @author: R. Pöschl DESY
  * @date Dec 7 2005
  *
  */


class CaliceTriggerProcessor : public CALICE::TriggerProcessorBase {

public:
  Processor* newProcessor() { return new CaliceTriggerProcessor;}
  CaliceTriggerProcessor();
  //  ~CaliceTriggerProcessor();
  void init();  
  void processEvent( LCEvent * evt ) ;
  void end();

private:
  std::string  _confTriggerBitsParName;         /**< par. name of the configuration trigger bits.*/
  std::string  _eventTriggerBitsParName;        /**< par. name of the event trigger bits.*/
  std::string  _parNameTriggerMainWord;         /**< Par. name of the trigger main word.*/
  std::string  _parNameTriggerPostHistory;      /**< Par, name of the trigger post history.*/
  std::string  _parNameTriggerPreHistory;       /**< Par, name of the trigger pre history.*/

  std::string  _parNameTriggerPostHistoryPos;
  std::string  _parNameTriggerPostHistoryBits;
  std::string  _parNameTriggerPreHistoryPos;
  std::string  _parNameTriggerPreHistoryBits;

  UInt_t       _noMainWord;                     /**< events for which the main word was not within the valid range.*/
  UInt_t       _eventsWithOutOfRangeTriggers;   /**< events for which possible trigger bits were out of range.*/

  StringVec _beamTriggerNames;            /**< Names of all trigger which indicated physics events and which will be ignored for pedestal calculation.*/
  unsigned int _beamTriggerBitMask;         /**< Bit mask of all triggers which indicated physics events and which will be ignored for pedestal calculation.*/
  IntVec _noTriggerActivityRange;           /**< The time in ns before and after the main word during which there is no beam trigger allowed.*/


#ifdef WITH_CONTROL_HISTOGRAMS
  histmgr::Key_t       _histGroupKey;            /**< Key for the histogram group.*/
  histmgr::Key_t       _histTriggerEventKey;     /**< Key for the histogram of the trigger event data. */
  histmgr::Key_t       _histTriggerConfKey;      /**< Key for the histogram of the trigger configuration. */
  histmgr::Key_t       _histTriggerPostKey;      /**< Key for the histogram of the trigger post history histograms. */
  histmgr::Key_t       _histTriggerPreKey;       /**< Key for the histogram of the trigger pre history histograms. */
#endif
};
}
#endif
