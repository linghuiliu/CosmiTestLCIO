#include "TriggerHandlerCalice.hh"
#include "SimpleTriggerProcessor.hh"
//
#include "lccd/LCConditionsMgr.hh"
#include "marlin/ConditionsProcessor.h"
//#include "TriggerHandlerCalice.hh"
#include <TriggerBits.hh>
#include <collection_names.hh>

#include <iostream>
#include <iomanip>
#include <sstream>


namespace marlin {

SimpleTriggerProcessor aSimpleTriggerProcessor;

  SimpleTriggerProcessor::SimpleTriggerProcessor() : Processor("SimpleTriggerProcessor") 
  {

    _description = "Processor to add Trigger information on CALICE data to the event";


//some steering parameters 
    //Parameters to be passed to the triggerhandler
    //trigger assignment
    registerProcessorParameter( "Trigger_Assignment" , 
				"Name of the Collection for the Trigger Assignment"  ,
                                _col_trigger_assignment ,
				std::string(COL_TRIGGER_ASSIGNMENT) ) ;
    
    
    
    //trigger configuration collection 
    registerProcessorParameter( "Trigger_Check" , 
				"Name of the Collection for the Trigger Check"  ,
                                _col_trigger_check,
				std::string(COL_TRIGGER_CHECK) ) ;
    
    
    //trigger configuration collection 
    registerProcessorParameter( "BE_Trigger_Configuration" , 
				"Name of the conditions collection for the BE trigger configuration"  ,
                                _colBeTrgConf,
				std::string(COL_TRIGGER_CONF) ) ;
    
    //readout configuration collection 
    registerProcessorParameter( "Readout_Configuration" , 
				"Name of the conditions collection for the readout configuration"  ,
                                _colNameReadoutConf,
				std::string(COL_RO_CONF) ) ;
    
    //readout configuration collection 
    registerProcessorParameter( "TrgReadout_Configuration" , 
				"Name of the conditions collection for the trigger readout configuration"  ,
                                _colNameTrgReadoutConf,
				std::string(COL_TRGRO_CONF) ) ;
    
    _colNameFeConf.clear();
    _colNameFeConf.push_back(COL_EMC_FE_CONF);
    _colNameFeConf.push_back(COL_AHC_FE_CONF);
    
    registerProcessorParameter( "FE_Configuration" , 
				"List of names of the conditions collection for the FE configuration (CALDAQ_EmcFeConfiguration, CALDAQ_AhcFeConfiguration)." ,
                                _colNameFeConf,
                                _colNameFeConf);
    
    registerProcessorParameter( "TriggerConfigurationName" , 
                                "Name of the event parameter which will be filled with the current trigger configuration bits."  ,
                                _parNameTriggerConf ,
                                std::string(PAR_TRIGGER_CONF) ) ;
    
    registerProcessorParameter( "TriggerAndEnableName" , 
    				"Name of the event parameter which will be filled with the current enabled trigger bits."  ,
				_parNameTriggerAndEnable ,
    				std::string(PAR_TRIGGER_ANDENABLE) ) ;

    registerProcessorParameter( "TriggerEventName" , 
                                "Name of the event parameter which will be filled with the current trigger event bits."  ,
                                _parNameTriggerEvent ,
                                std::string(PAR_TRIGGER_EVENT) ) ;
    
    registerProcessorParameter( "ColTriggerEventName" , 
                                "Collection name of trigger event data."  ,
                                _colNameTriggerEvent ,
                                std::string(COL_FETRG) ) ;
    
    _colNameModuleConnection.clear();
    _colNameModuleConnection.push_back("EmcModuleConnection");
    _colNameModuleConnection.push_back("AhcModuleConnection");
    

    registerProcessorParameter( "ModuleConnectionCollectionName" , 
                                "List of names of the conditions data collection which describes the connection between modules and the DAQ front-ends."  ,
                                _colNameModuleConnection,
                                _colNameModuleConnection);
    
    
    registerProcessorParameter( "TriggerMainWordName" , 
    				"Name of the parameter which will be set to the trigger main word."  ,
				_parNameTriggerMainWord ,
    				std::string(PAR_TRIGGER_MAIN_WORD) ) ;
    
    registerProcessorParameter( "TriggerPreHistoryName" , 
    				"Name of the parameter which will contain the trigger pre history: will create parameters: [name]Pos, [name]Bits ."  ,
				_parNameTriggerPreHistory ,
    				std::string(PAR_TRIGGER_PRE_HISTORY) ) ;
    
    registerProcessorParameter( "TriggerPostHistoryName" , 
    				"Name of the parameter which will contain the trigger psat history: will create parameters: [name]Pos, [name]Bits ."  ,
				_parNameTriggerPostHistory ,
    				std::string(PAR_TRIGGER_POST_HISTORY) ) ;
    
    
  }
  

  // SimpleTriggerProcessor::~SimpleTriggerProcessor() {}

  void SimpleTriggerProcessor::init() {
    printParameters();

    //Initialize the TriggerHandler
    _theTrigHandler = TriggerHandlerCalice::getInstance();
    //Only collections which come from outside can be sent to the TriggerHandler
    _theTrigHandler->init( _col_trigger_assignment, _col_trigger_check, _colBeTrgConf, _colNameReadoutConf,
 _colNameTrgReadoutConf, _colNameFeConf, _colNameModuleConnection);
    _theTrigHandler->setColNameTriggerEvent( _colNameTriggerEvent );

    _noMainWord=0;
    _eventsWithOutOfRangeTriggers=0;
    
    assert( !_parNameTriggerPostHistory.empty() );
    _parNameTriggerPostHistoryPos=_parNameTriggerPostHistory;
    _parNameTriggerPostHistoryPos+="Pos";
    _parNameTriggerPostHistoryBits=_parNameTriggerPostHistory;
    _parNameTriggerPostHistoryBits+="Bits";
    
    assert( !_parNameTriggerPreHistory.empty() );
    _parNameTriggerPreHistoryPos=_parNameTriggerPreHistory;
    _parNameTriggerPreHistoryPos+="Pos";
    _parNameTriggerPreHistoryBits=_parNameTriggerPreHistory;
    _parNameTriggerPreHistoryBits+="Bits";

    


}




  void SimpleTriggerProcessor::processEvent(LCEvent* evt){

    if(evt->getEventNumber() > 1000) {
      std::cout << "Event gt 1000 reached" << std::endl;
    }

    // copy fifo from the event to the trigger handler
    _theTrigHandler->setTrigger(evt);

    // search around the nominal main word were the enabled trigger fired.
    UInt_t main_word=_theTrigHandler->searchTriggerMainWord();
    evt->parameters().setValue(_parNameTriggerMainWord, static_cast<int>(main_word));
    
    if (main_word >= _theTrigHandler->getTriggerFifoContent().size() ) {
      _noMainWord++;
    }


    
    // scan the history before and after the main word
    _theTrigHandler->searchTriggerHistory();

 
    
    CALICE::TriggerBits out_of_range_triggers=_theTrigHandler->getOutOfRangeTriggerBits();
    if ( out_of_range_triggers.getTriggerBits() & _theTrigHandler->getPrincipleTriggerMask() ) {
      _eventsWithOutOfRangeTriggers++;
    } 


    // get the history prior to the main word and copy it to the event header.
    const std::map<unsigned int, unsigned int > &pre_trigger_history = _theTrigHandler->getPreTriggerHistory();
    std::vector<int> temp_pos;
    std::vector<int> temp_bits;
    for (std::map<unsigned int, unsigned int>::const_iterator history_iter = pre_trigger_history.begin();
	 history_iter != pre_trigger_history.end();
	 history_iter++ ) {
      temp_pos.push_back(history_iter->first);
      temp_bits.push_back(history_iter->second);
    }

    
    if(temp_pos.size()>0) evt->parameters().setValues(_parNameTriggerPreHistoryPos , temp_pos);
    if(temp_bits.size()>0) evt->parameters().setValues(_parNameTriggerPreHistoryBits , temp_bits);

    
    // get the history posterior to the main word and copy it to the event header.
    const std::map<unsigned int, unsigned int > &post_trigger_history = _theTrigHandler->getPostTriggerHistory();
    temp_pos.clear();
    temp_bits.clear();
    for (std::map<unsigned int, unsigned int>::const_iterator history_iter = post_trigger_history.begin();
	 history_iter != post_trigger_history.end();
	 history_iter++ ) {
      temp_pos.push_back(history_iter->first);
      temp_bits.push_back(history_iter->second);
    }

    if(temp_pos.size()>0) evt->parameters().setValues(_parNameTriggerPostHistoryPos , temp_pos);
    if(temp_bits.size()>0) evt->parameters().setValues(_parNameTriggerPostHistoryBits , temp_bits);
    


    CALICE::TriggerBits trigger_conf(_theTrigHandler->getTriggerConfiguration());
    CALICE::TriggerBits trigger_andenable(_theTrigHandler->getAndEnableBits());
    CALICE::TriggerBits trigger_event(_theTrigHandler->getTriggerEvent());



    evt->parameters().setValue(_parNameTriggerConf , trigger_conf.getTriggerBits());
    evt->parameters().setValue(_parNameTriggerAndEnable , trigger_andenable.getTriggerBits());
    evt->parameters().setValue(_parNameTriggerEvent, trigger_event.getTriggerBits());

  }
  
  void SimpleTriggerProcessor::end()
  {
    std::cout << "--- " << name() << " Report :" << std::endl;
    std::cout << std::setw(8) << _noMainWord                    << " events for which no main word was found." << std::endl
              << std::setw(8) << _eventsWithOutOfRangeTriggers  << " events for which principle trigger inputs could have fired outside the fifo range." << std::endl;
  }
  
}
