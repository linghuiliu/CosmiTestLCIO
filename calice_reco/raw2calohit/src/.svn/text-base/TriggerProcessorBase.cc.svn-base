#include "TriggerHandlerCalice.hh"
#include "TriggerProcessorBase.hh"
//
#include "lccd/LCConditionsMgr.hh"
#include "marlin/ConditionsProcessor.h"
#include "TriggerHandlerCalice.hh"
#include <TriggerBits.hh>
#include <collection_names.hh>

using namespace std;
using namespace lcio;

namespace CALICE {

  // Not a real processor so no instance:
  //TriggerProcessorBase aTriggerProcessorBase;

  TriggerProcessorBase::TriggerProcessorBase(const std::string &processor_name) 
    : Processor(processor_name) 
#ifdef TRIGGER_HANDLER_IS_SINGLETON
#else
    , _theTrigHandler(new CALICE::TriggerHandlerCalice)
#endif
  {
    _description = "Processor to add Trigger information on CALICE data to the event";
    //some steering parameters 
    //trigger assignment
    registerProcessorParameter( "Trigger_Assignment" , 
                              "Name of the Collection for the Trigger Assignment"  ,
                                _col_trigger_assignment ,
                               std::string(COL_TRIGGER_ASSIGNMENT) ) ;


    //Parameters to be passed to the triggerhandler
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



}


 TriggerProcessorBase::~TriggerProcessorBase() {
#ifdef TRIGGER_HANDLER_IS_SINGLETON
#else
   delete _theTrigHandler;
#endif  
 }

  void TriggerProcessorBase::init() {
     //Some initialization
    printParameters();

    //Initialize the TriggerHandler
#ifdef TRIGGER_HANDLER_IS_SINGLETON
    _theTrigHandler = TriggerHandlerCalice::getInstance();
#endif

    _theTrigHandler->init( _col_trigger_assignment, _col_trigger_check, _colBeTrgConf, _colNameReadoutConf, _colNameTrgReadoutConf, _colNameFeConf, _colNameModuleConnection);
    _theTrigHandler->setColNameTriggerEvent( _colNameTriggerEvent );

}

  
}
