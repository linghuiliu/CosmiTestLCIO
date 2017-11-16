/* Processor which wraps the CaliceTriggerHandler*/
#ifdef USE_LCCD

#ifndef SIMPLETRIGGERPROCESSOR_h
#define SIMPLETRIGGERPROCESSOR_h 1

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 


#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCEventImpl.h" 
#include "IMPL/LCRunHeaderImpl.h" 
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h" 
#include "IMPL/LCTOOLS.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "UTIL/LCTime.h"
#include "LCIOTypes.h"
#include "marlin/Processor.h"

#include "TriggerHandlerCalice.hh"
#include <collection_names.hh>
#include <string>

using namespace lcio;
using namespace marlin;
using namespace CALICE;


namespace marlin {

/** Class which adds basic trigger information to the event parameters
  * In order to make use of the trigger information this processor has
  * to run *before* each other user defined processor. 
  * It initializes the trigger handler through which the 
  * user can obtain more information on trigger issues in other processors. 
  * Here it adds the trigger which have fired the event to the event parameters.  
  * It repeats the functionality of the CaliceTriggerProcessor in the
  * reco package mainly for demnstration purposes and to allow
  * in principle for data analysis outside the reco package.  
  * @author: R. Pöschl LAL
  * @date Sep 2 2006
  *
  */


class SimpleTriggerProcessor : public Processor {

public:
  Processor* newProcessor() { return new SimpleTriggerProcessor;}
  SimpleTriggerProcessor();
  //  ~CaliceTriggerProcessor();
  void init();  
  void processEvent( LCEvent * evt ) ;
  void end();

private:
 /** The TriggerHandler */
  TriggerHandlerCalice* _theTrigHandler;


  std::string  _confTriggerBitsParName;         /**< par. name of the configuration trigger bits.*/
  std::string  _eventTriggerBitsParName;        /**< par. name of the event trigger bits.*/
  std::string  _parNameTriggerMainWord;         /**< Par. name of the trigger main word.*/
  std::string  _parNameTriggerPostHistory;      /**< Par, name of the trigger post history.*/
  std::string  _parNameTriggerPreHistory;       /**< Par, name of the trigger pre history.*/

  std::string  _parNameTriggerPostHistoryPos;
  std::string  _parNameTriggerPostHistoryBits;
  std::string  _parNameTriggerPreHistoryPos;
  std::string  _parNameTriggerPreHistoryBits;


  /** Collections needed for trigger analysis */
  std::string _col_trigger_assignment;
  std::string _col_trigger_check; 
  std::string _colBeTrgConf;

  std::string _colNameReadoutConf;
  std::string _colNameTrgReadoutConf;
    
  lcio::StringVec _colNameModuleConnection;
  lcio::StringVec _colNameFeConf;
  
  std::string _colNameTriggerEvent;

  std::string _parNameTriggerConf;
  std::string _parNameTriggerAndEnable;
  std::string _parNameTriggerEvent;

  UInt_t       _noMainWord;                     /**< events for which the main word was not within the valid range.*/
  UInt_t       _eventsWithOutOfRangeTriggers;   /**< events for which possible trigger bits were out of range.*/

};
}
#endif

#endif
