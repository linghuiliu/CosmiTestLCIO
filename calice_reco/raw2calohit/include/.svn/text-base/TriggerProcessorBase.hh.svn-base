#ifdef USE_LCCD

#ifndef _TriggerProcessorBase_hh_
#define _TriggerProcessorBase_hh_

#include <string>
#include "marlin/Processor.h"
#include "LCIOSTLTypes.h"

namespace CALICE {
  class TriggerHandlerCalice;
}


namespace CALICE {

/** Base class which registers all the parameters which are neccessary to deal with trigger data.
 * A processor which needs trigger data can derive from this class.
 * The functionality is provided by the trigger handler.
 * @sa TriggerHandlerCalice
 * @author: R. Pöschl DESY
 * @date Dec 7 2005
 */

  class TriggerProcessorBase : public marlin::Processor {

public:
  //  virtual Processor* newProcessor() { return new TriggerProcessorBase;}
protected:
  TriggerProcessorBase(const std::string &processor_name);
public:
  ~TriggerProcessorBase();
  virtual void init();  

  //   virtual void processEvent( LCEvent * evt ) ;

  //  virtual void end() {};

protected:
  CALICE::TriggerHandlerCalice* getTriggerHandler() {return _theTrigHandler;}

  const CALICE::TriggerHandlerCalice* getTriggerHandler() const {return _theTrigHandler;}
  
private:

  /** The TriggerHandler */
  CALICE::TriggerHandlerCalice* _theTrigHandler;

  /** Collections needed for trigger analysis */
  std::string _col_trigger_assignment;
  std::string _col_trigger_check; 
  std::string _colBeTrgConf;

  std::string _colNameReadoutConf;
  std::string _colNameTrgReadoutConf;
    
  lcio::StringVec _colNameModuleConnection;
  lcio::StringVec _colNameFeConf;
  
  std::string _colNameTriggerEvent;

protected:
  std::string _parNameTriggerConf;
  std::string _parNameTriggerAndEnable; 
  std::string _parNameTriggerEvent;

};
}
#endif

#endif
