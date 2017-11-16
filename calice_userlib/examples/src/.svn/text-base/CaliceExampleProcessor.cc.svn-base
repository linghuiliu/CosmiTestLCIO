#ifdef USE_LCCD
#include <sstream>
#include "CaliceExampleProcessor.hh"
#include "DBInitString.hh"
//Interface classes
#include "AdcBlock.hh"
#include "CellMappingHcal.hh"
#include "AhcSlowReadoutBlock.hh"
#include "AhcVfeConfigurationBlock.hh"
#include "BeTrgConf.hh"
#include "FeConfigurationBlock.hh"
#include "ReadOutConfigurationBlock.hh"
#include "TrgReadoutConfigurationBlock.hh"
#include "EmcStageDataBlock.hh"
#include "BmlEventData.hh"
#include "BmlCaenConfigurationBlock.hh"
#include "BmlCaenReadoutConfigurationBlock.hh"
#include "AhcSlowReadoutModBlock.hh"
#include "SceSlowReadoutModBlock.hh"
#include "BmlSlowRunDataBlock.hh"
#include "DaqTypeDataBlock.hh"
#include "DaqRunSummaryBlock.hh"
//
#include "lccd/LCConditionsMgr.hh"
#include "marlin/ConditionsProcessor.h"

#include "TriggerHandlerCalice.hh"

#include "ErrorBits.hh"
#include "TriggerBits.hh"

#include "to_binary_bitops.hh"


using namespace std;
using namespace lcio;
using namespace marlin;
using namespace CALICE;
using namespace lccd;

//namespace marlin {

CaliceExampleProcessor aCaliceExampleProcessor;

CaliceExampleProcessor::CaliceExampleProcessor() : Processor("CaliceExampleProcessor"),
#ifdef HAVE_ROOT
						   //_blockMin(90),
						   //_blockMax(107),
#endif
                                                   //Initialise the ChangeDelegators
						   _runInfoChanged(this, &CaliceExampleProcessor::setRunInfoConfigCol), _runInfoConfCol(0),
						   _beTrgConfigurationChanged(this, &CaliceExampleProcessor::setBeTrgConfigCol), _beTrgConfCol(0),
						   _emcFeConfigurationChanged(this, &CaliceExampleProcessor::setEmcFeConfigCol), _emcFeConfCol(0),
						   _ahcFeConfigurationChanged(this, &CaliceExampleProcessor::setAhcFeConfigCol), _ahcFeConfCol(0),
						   _feConfigurationChanged(this, &CaliceExampleProcessor::setFeConfigCol), _feConfCol(0),
						   _ahcVfeConfigurationChanged(this, &CaliceExampleProcessor::setAhcVfeConfigCol), _ahcVfeConfCol(0),
						   _ahcSroDataChanged(this, &CaliceExampleProcessor::setAhcSroDataCol), _ahcSroDataCol(0),
						   _ahcSroModDataChanged(this, &CaliceExampleProcessor::setAhcSroModDataCol), _ahcSroModDataCol(0),
						   _sceSroModDataChanged(this, &CaliceExampleProcessor::setSceSroModDataCol), _sceSroModDataCol(0),
						   _sceSroTempDataChanged(this, &CaliceExampleProcessor::setSceSroTempDataCol), _sceSroTempDataCol(0),
						   _emcStgDataChanged(this, &CaliceExampleProcessor::setEmcStgDataCol), _emcStgDataCol(0),
						   _roConfDataChanged(this, &CaliceExampleProcessor::setRoConfDataCol), _roConfDataCol(0),
						   _trgroConfDataChanged(this, &CaliceExampleProcessor::setTrgroConfDataCol), _trgroConfDataCol(0),
						   _bmlCaenConfDataChanged(this, &CaliceExampleProcessor::setBmlCaenConfDataCol), _bmlCaenConfDataCol(0),
						   _bmlCaenRoConfDataChanged(this, &CaliceExampleProcessor::setBmlCaenRoConfDataCol), _bmlCaenRoConfDataCol(0),
						   //Note that this part will be put into a BeamLineHandler 
						   _bmlSroRunDataChanged(this, &CaliceExampleProcessor::setBmlSroRunDataCol), _bmlSroRunDataCol(0)

{
  _description = "Processor containing examples on how to access the CALICE Data";
  //some steering parameters 
  
  //      registerProcessorParameter( "Channel_Map" , 
  //  				"Relation of elec. channel to geom. channel"  ,
  //				_channelMapCol ,
  //				std::string("channelmap") ) ;
  
  registerProcessorParameter( "RunInfo" , 
                              "Name of the conditions collection for Run Information."  ,
                              _colRunInfo,
                              std::string(COL_RUN_INFO) ) ;
  
  
  registerProcessorParameter( "ADC_CollectionName" , 
                              "Name of the collection for the ADC Values."  ,
                              _colAdcVals ,
                              std::string(COL_ADC) ) ;
  
  //Register the collections names for the two caen tdcs
  //_bmlCollections.push_back(std::string(COL_BML_RUNDATACERN));
  _colTdcVals.push_back(std::string(COL_CAEN767TDC));
  _colTdcVals.push_back(std::string(COL_CAENTDC));
  std::stringstream description_tdc;
  description_tdc << "Name of the collection for the TDC Values (Caen 767, 1290 - CERN running)." << std::endl;
  registerProcessorParameter( "TDC_CollectionName" , 
                              description_tdc.str(),
                              _colTdcVals ,
                              _colTdcVals ) ;
  
  registerProcessorParameter( "BETRG_Configuration" , 
                              "Name of the conditions collection for the BeTrg Configuration data."  ,
                              _colBeTrgConf ,
                              std::string(COL_TRIGGER_CONF) ) ;
  
  
  registerProcessorParameter( "AHC_FE_Configuration" , 
                              "Name of the conditions collection for the AHC FE configuration data."  ,
                              _colAhcFeConf ,
                              std::string(COL_AHC_FE_CONF) ) ;
  
  registerProcessorParameter( "EMC_FE_Configuration" , 
                              "Name of the conditions collection for the EMC FE configuration data."  ,
                              _colEmcFeConf ,
                              std::string(COL_EMC_FE_CONF) ) ;
  
  
  registerProcessorParameter( "FE_Configuration" , 
                              "Name of the conditions collection for the AHC FE configuration data."  ,
                              _colFeConf ,
                              std::string(COL_FE_CONF) ) ;
  
  
  registerProcessorParameter( "AHC_VFE_Configuration" , 
                              "Name of the conditions collection for the AHC VFE configuration data."  ,
                              _colAhcVfeConf ,
                              std::string(COL_AHC_VFE_CONF) ) ;
  
  
  registerProcessorParameter( "AHC_SRO_Data" , 
                              "Name of the conditions collection for the AHC SlowConfiguration data."  ,
                              _colAhcSroData ,
                              std::string(COL_AHC_SRO_DATA) ) ;

  
  registerProcessorParameter( "AHC_SRO_ModData" , 
                              "Name of the conditions collection for the AHC Slow r/o data."  ,
                              _colAhcSroModData ,
                              std::string(COL_AHC_SRO_MOD) ) ;

  registerProcessorParameter( "SCE_SRO_ModData" , 
                              "Name of the conditions collection for the SCE Slow r/o data."  ,
                              _colSceSroModData ,
                              std::string(COL_SCE_SRO_MOD) ) ;

  registerProcessorParameter( "SCE_SRO_TempData" , 
                              "Name of the conditions collection for the SCE Temperatures data."  ,
                              _colSceSroTempData ,
                              std::string(COL_SCE_SRO_TEMP) ) ;
  
  registerProcessorParameter( "EMC_STG_Data" , 
                              "Name of the conditions collection for the EMCStage data."  ,
                              _colEmcStgData ,
                              std::string(COL_EMC_STAGE_DATA) ) ;
  
  registerProcessorParameter( "RO_CONF_Data" , 
                              "Name of the conditions collection for the Crc ReadoutConfiguration data."  ,
                              _colRoConfData ,
                              std::string(COL_RO_CONF) ) ;
  
  registerProcessorParameter( "TRGRO_CONF_Data" , 
                              "Name of the conditions collection for the Trg ReadoutConfiguration data."  ,
                              _colTrgRoConfData ,
                              std::string(COL_TRGRO_CONF) ) ;
  
  registerProcessorParameter( "BMLSRO_RUN_DataCern" , 
                              "Name of the conditions collection for the Bml SlowReadoutRun data (CERN)."  ,
                              _colBmlSroRunDataCern ,
                              std::string(COL_BML_RUNDATACERN) ) ;

  registerProcessorParameter( "BMLCAEN_CONF_Data" , 
                              "Name of the conditions collection for the Caen Configuration data (CERN)."  ,
                              _colBmlCaenConfData,
                              std::string(COL_CAENTDC_CONF) ) ;

  registerProcessorParameter( "BMLCAEN_ROCONF_Data" , 
                              "Name of the conditions collection for the Caen r/o Configuration data (CERN)."  ,
                              _colBmlCaenRoConfData,
                              std::string(COL_CAENTDC_ROCONF) ) ;

  //Register the collections names from which we retrieve the beamline information
  //_bmlCollections.push_back(std::string(COL_BML_RUNDATACERN));
  _bmlCollections.push_back(std::string(COL_BML_RUNDATAFNAL));
  _bmlCollections.push_back(std::string(COL_BML_ACQDATAFNAL));
  std::stringstream description;
  description << "Collections dor beamline parametersm site specific" << std::endl;
  registerProcessorParameter( "BmlColNames" , description.str(), _bmlCollections , _bmlCollections);
  
  registerProcessorParameter( "TriggerConfigurationName" , 
			      "Name of the event parameter which contains the current trigger configuration bits."  ,
			      _parNameTriggerConf ,
			      std::string(PAR_TRIGGER_CONF) ) ;
  
  registerProcessorParameter( "TriggerAndEnableName" , 
			      "Name of the event parameter which will be filled with the current enabled trigger bits."  ,
			      _parNameTriggerAndEnable ,
			      std::string(PAR_TRIGGER_ANDENABLE) ) ;
  
  registerProcessorParameter( "TriggerEventName" , 
			      "Name of the event parameter which contains the current trigger event bits."  ,
			      _parNameTriggerEvent ,
			      std::string(PAR_TRIGGER_EVENT) ) ;
  
  registerProcessorParameter( "TriggerMainWordName" , 
			      "Name of the parameter which will be set to the trigger main word."  ,
			      _parNameTriggerMainWord ,
			      std::string(PAR_TRIGGER_MAIN_WORD) ) ;
  
  registerProcessorParameter( "TriggerPreHistoryName" , 
			      "Name of the parameter which contains the trigger post history."  ,
			      _parNameTriggerPreHistory ,
			      std::string(PAR_TRIGGER_PRE_HISTORY) ) ;
  
  registerProcessorParameter( "TriggerPostHistoryName" , 
			      "Name of the parameter which contains the trigger post history."  ,
			      _parNameTriggerPostHistory ,
			      std::string(PAR_TRIGGER_POST_HISTORY) ) ;
  
  
  registerProcessorParameter( "EventErrorsName" , 
			      "Name of the event parameter which contains the error bits of the event."  ,
			      _parNameErroBits ,
			      std::string(PAR_ERROR_BITS) ) ;
  
  registerProcessorParameter("DBInit",    
			     "Database initialization string",
			     _dbInit,
			     DBInitString() );
  
  registerProcessorParameter("RunSummaryFolder",    
			     "Folder which contains the run summary",
			     _runsumfolder,
			     std::string("/expert_calice_v0402/CALDAQ_RunSummary"));
  
#ifdef HAVE_ROOT
  // ROOT stuff
  //_hfile = new TFile("histos.root","RECREATE","file title");
  _hfile = new TFile("trhistos.root","RECREATE");
  //_hfile->SetCompressionLevel(1);
  _hpre = new TH1F("hpre","Dirty Bits in Pre Hist Dist.1 ",32,-0.5,31.5);
  _hpost = new TH1F("hpost","Dirty Bits in Post Hist Dist.1 ",32,-0.5,31.5);
  _hbit = new TH2F("hbit","Trigger Bit history ",256,-0.5,255.5,32,-0.5,31.5);
  
  //_hall = new TH1F("hall","ADC counts",1000,-500,1500);
  //_hlimits = new TH1F("hlimits","ADC counts extrema",1000,-10000,50000);
#endif

  //For the beamline parameter we need multiplechangedelegators which we initialise here  
  unsigned set_i(0);
  for (lcio::StringVec::const_iterator name_iter=_bmlCollections.begin();
       name_iter!=_bmlCollections.end();
       name_iter++, set_i++) {
    _bmlParamChangeDelegatorVec.push_back(MultipleConditionsChangeDelegator<CaliceExampleProcessor>(this,&CaliceExampleProcessor::setVBmlSroRunDataCol,set_i));
    //initialise the map which holds the relation between the collection and the delegator
    _bmlParamsIndexMap[set_i]=*name_iter;
  }
  

}


CaliceExampleProcessor::~CaliceExampleProcessor() {}

void CaliceExampleProcessor::init() {
     //Some Initialization
     _cellMapPtr = new CellMap( &CellMappingHcal::getElecChannel ) ;
     //Register Change Listeners for Conditions Data accessed in this processor 
     //marlin::ConditionsProcessor::registerChangeListener(_cellMapPtr, _channelMapCol);   
      marlin::ConditionsProcessor::registerChangeListener(&_runInfoChanged, _colRunInfo);
      marlin::ConditionsProcessor::registerChangeListener(&_beTrgConfigurationChanged, _colBeTrgConf );
      marlin::ConditionsProcessor::registerChangeListener(&_emcFeConfigurationChanged, _colEmcFeConf );
      marlin::ConditionsProcessor::registerChangeListener(&_ahcFeConfigurationChanged, _colAhcFeConf );
      marlin::ConditionsProcessor::registerChangeListener(&_feConfigurationChanged, _colFeConf );
      marlin::ConditionsProcessor::registerChangeListener(&_ahcVfeConfigurationChanged, _colAhcVfeConf );
      marlin::ConditionsProcessor::registerChangeListener(&_ahcSroDataChanged, _colAhcSroData);
      marlin::ConditionsProcessor::registerChangeListener(&_ahcSroModDataChanged, _colAhcSroModData);
      marlin::ConditionsProcessor::registerChangeListener(&_sceSroModDataChanged, _colSceSroModData);
      marlin::ConditionsProcessor::registerChangeListener(&_sceSroTempDataChanged, _colSceSroTempData);
      marlin::ConditionsProcessor::registerChangeListener(&_emcStgDataChanged, _colEmcStgData);
      marlin::ConditionsProcessor::registerChangeListener(&_roConfDataChanged, _colRoConfData);
      marlin::ConditionsProcessor::registerChangeListener(&_trgroConfDataChanged, _colTrgRoConfData);
      marlin::ConditionsProcessor::registerChangeListener(&_bmlSroRunDataChanged,_colBmlSroRunDataCern);
      marlin::ConditionsProcessor::registerChangeListener(&_bmlCaenConfDataChanged,_colBmlCaenConfData);
      marlin::ConditionsProcessor::registerChangeListener(&_bmlCaenRoConfDataChanged,_colBmlCaenRoConfData);
      //register the change listeners for the bml collections
      lcio::StringVec::const_iterator name_iter=_bmlCollections.begin();
      for (std::vector<CALICE::MultipleConditionsChangeDelegator<CaliceExampleProcessor> >::iterator change_delegator_iter=_bmlParamChangeDelegatorVec.begin();
           change_delegator_iter!=_bmlParamChangeDelegatorVec.end();
           change_delegator_iter++, name_iter++) {
	marlin::ConditionsProcessor::registerChangeListener(&(*change_delegator_iter),*name_iter);
      }
}



void  CaliceExampleProcessor::setVBmlSroRunDataCol(EVENT::LCCollection *vbmlSroRunDataCol, unsigned int set_i) {
   _bmlParamsCollectionMap[(*_bmlParamsIndexMap.find(set_i)).second]= vbmlSroRunDataCol;
}


void CaliceExampleProcessor::processEvent(LCEvent* evt){


    //Some Event info
    cout << "Event Number: " << evt->getEventNumber() << endl;   
    LCTime EventTime(evt->getTimeStamp());
    cout << "Event Time: " << EventTime.getDateString() << endl;   


    //Some runinfo from the runinfo condtitions data
    if(_runInfoConfCol) {
      stringstream runnum_str; 
      runnum_str << PAR_RUN_NUMBER << ": " ;
      std::cout << runnum_str.str() << _runInfoConfCol->getParameters().getIntVal(PAR_RUN_NUMBER) << std::endl;
      
      stringstream fefirm_str; 
      fefirm_str << PAR_RUN_FE_FIRM << ": " ;
      std::cout << fefirm_str.str() << _runInfoConfCol->getParameters().getIntVal(PAR_RUN_FE_FIRM) << std::endl;
      
      stringstream betrgfirm_str; 
      betrgfirm_str << PAR_RUN_BETRG_FIRM << ": " ;
      std::cout << betrgfirm_str.str() << _runInfoConfCol->getParameters().getIntVal(PAR_RUN_BETRG_FIRM) << std::endl;
      
      stringstream vmefirm_str; 
      vmefirm_str << PAR_RUN_VME_FIRM << ": " ;
      std::cout << vmefirm_str.str() << _runInfoConfCol->getParameters().getIntVal(PAR_RUN_VME_FIRM) << std::endl;
      
      
      stringstream runtype_str;
      runtype_str << PAR_RUN_TYPE << ": " ;
      std::cout << runtype_str.str() << _runInfoConfCol->getParameters().getStringVal(PAR_RUN_TYPE) << std::endl; 
      
      stringstream maj_runtype_str;
      maj_runtype_str << PAR_RUN_MAJ_TYPE << ": " ;
      std::cout << maj_runtype_str.str() << _runInfoConfCol->getParameters().getStringVal(PAR_RUN_MAJ_TYPE) << std::endl; 
      
      stringstream sub_runtype_str;
      sub_runtype_str << PAR_RUN_SUB_TYPE << ": " ;
      std::cout << sub_runtype_str.str() << _runInfoConfCol->getParameters().getStringVal(PAR_RUN_SUB_TYPE) << std::endl; 
 

   
      //Access the runsummary
      std::string till_string =  _runInfoConfCol->getParameters().getStringVal((lccd::DBTILL).c_str());
      LCTime till(static_cast<long64>(strtoll(till_string.c_str(),0,0)));
      lccd::IConditionsHandler* conData = new lccd::DBCondHandler(_dbInit,_runsumfolder, "RunSummary", "" ) ;
      try{ 
	conData->update( static_cast<lccd::LCCDTimeStamp>(static_cast<long64>(strtoll(till_string.c_str(),0,0))) ) ;
	LCCollection* col = conData->currentCollection();
	DaqRunSummaryBlock daq_summary(col->getElementAt(0));
	daq_summary.print(std::cout);
      }
      catch (lcio::Exception & aExc) {
      }
    }

//Information on the event trigger(s)
    TriggerHandlerCalice::getInstance()->printTriggerDefinitions(evt);
//This works if the event trigger info is reliable
    TriggerBits trigger_event(((LCEvent *) evt)->getParameters().getIntVal(_parNameTriggerEvent));
    std::cout << "TriggerEvent - Is Pedestal Trigger ?: " << trigger_event.isPedestalTrigger() << std::endl;         
    std::cout << "TriggerEvent - Is Beam Trigger ?: " << trigger_event.isBeamTrigger() << std::endl;         
    std::cout << "TriggerEvent - Is Cherenkov Trigger ?: " << trigger_event.isCherenkovTrigger() << std::endl;         
    std::cout << "TriggerEvent - Is Cherenkov2 Trigger ?: " << trigger_event.isCherenkov2Trigger() << std::endl;         
//Information on the trigger configuration
    TriggerBits trigger_conf(((LCEvent *) evt)->getParameters().getIntVal(_parNameTriggerConf));
    std::cout << "Is Pedestal Trigger Conf.?: " << trigger_conf.isPedestalTrigger() << std::endl;
    std::cout << "Is Beam Trigger Conf.?: " << trigger_conf.isBeamTrigger() << std::endl;
    std::cout << "Is Cosmics Trigger Conf.?: " << trigger_conf.isCosmicsTrigger() << std::endl;
    std::cout << "Is Calib Set ?: " << trigger_conf.isCalibTrigger() << std::endl;
    //Enabled bits from event header
    TriggerBits trigger_andenable(evt->getParameters().getIntVal(_parNameTriggerAndEnable));
    std::cout << "And Enabled Bits: " << trigger_andenable << std::endl;


    ErrorBits error(evt->getParameters().getIntVal(_parNameErroBits));
    std::cout << "Error Bits in this Event: " << error.getBits() << std::endl;
 
    std::cout << "Fifo cont at mainword: " << std::hex <<
       TriggerHandlerCalice::getInstance()->getTriggerFifoContMainWord()
       << std::dec << " " ; 
       to_binary_bitops(TriggerHandlerCalice::getInstance()->getTriggerFifoContMainWord()); 
       //get Mainword position from event header
       std::cout << "Main word from Eventheader: " << evt->getParameters().getIntVal(_parNameTriggerMainWord) << std::endl;



    //to check the access of a an arbitrary word
    std::cout << "Fifo cont at word 195: " << std::hex <<
      TriggerHandlerCalice::getInstance()->getTriggerFifoContaWord(195) << std::dec << std::endl;
    //..or the main word using the Event Header Information
    std::cout << "Fifo cont at main word using Event Header Information: " << std::hex <<
      TriggerHandlerCalice::getInstance()->getTriggerFifoContaWord(evt->getParameters().getIntVal(_parNameTriggerMainWord)) << std::dec << " ";
    to_binary_bitops(TriggerHandlerCalice::getInstance()->getTriggerFifoContaWord(evt->getParameters().getIntVal(_parNameTriggerMainWord))); 


    //... and so forth check interface of the TriggerHandlerCalice Class
    bool isSWTriggerOn = TriggerHandlerCalice::getInstance()->isSWTriggerOn();
    std::cout << "Software Trigger: " <<  isSWTriggerOn << std::endl;  

    //Obtain some advanced trigger info 
    //This is the configuration mask which has been interpreted by the TriggerBits Class 
    //It is recommended to use the TriggerBits Class !!!
    std::cout << "Trigger Configuration: " << std::hex << TriggerHandlerCalice::getInstance()->getTrgConfigurationMask() << std::dec << std::endl;
    //Test the trigger fifo content
    IntVec fifoContent = TriggerHandlerCalice::getInstance()->getTriggerFifoContent();
    //Fifoloop 
    int icount(0); 

    //if(!isSWTriggerOn) {
      //if(fifoContent.size() > 0 && evt->getEventNumber() > 1044) {
    std::cout << "fifo size: " << fifoContent.size() << std::endl;
    if(fifoContent.size() > 0) {
      for(IntVec::iterator vec_iter = fifoContent.begin();vec_iter != fifoContent.end(); vec_iter++){
	//if(*vec_iter) {std::cout << "Fifo Element at " <<  icount << " is: " << "in hex: " << std::hex << *vec_iter << std::dec << " in ";
	//to_binary_bitops(*vec_iter); } 
        std::cout << "Fifo Element at " <<  icount << " is: " << "in hex: " << std::hex << *vec_iter << std::dec << " in ";
     	to_binary_bitops(*vec_iter);  
	for (unsigned int itrig = 0;itrig<32;itrig++)  {
          if((1<<itrig) & (*vec_iter)) _hbit->Fill(icount,static_cast<float>(itrig));	    
        }
	icount++;
      }
    }
    //}


    //The trigger hsitory attached to the event as analyzed by the TriggerHandler
    //PreTrigger History
    IntVec pretrig_pos;
    IntVec pretrig_bits;
    evt->getParameters().getIntVals(std::string(_parNameTriggerPreHistory)+"Pos", pretrig_pos);
    evt->getParameters().getIntVals(std::string(_parNameTriggerPreHistory)+"Bits", pretrig_bits);
    //std::cout << "Size of pretrig pos: " << pretrig_pos.size() << std::endl;
    //std::cout << "Size of pretrig bits: " << pretrig_bits.size() << std::endl;

    //create a mask to exclude triggers from the following checks
    unsigned int excludedTriggers(0);
    excludedTriggers |= (1 << TriggerBits::getBit("Spill"));
    excludedTriggers |= (1 << TriggerBits::getBit( "GENERICBIT"));
    excludedTriggers = ~excludedTriggers;
    std::cout << "excluded triggers: " << std::hex << excludedTriggers << std::dec << std::endl; 

    if(pretrig_pos.size() == pretrig_bits.size() ) {
      std::cout << "Pre Trigger Bits " << std::endl;
      for (unsigned int ipre = 0; ipre < pretrig_pos.size(); ipre++) {
	std::cout << "Distance to delay+jitter corrected main word: " << pretrig_pos.at(ipre) << std::setw(3) << TriggerBits(pretrig_bits.at(ipre) ) << std::endl;
	 if(pretrig_pos.at(ipre)==1 && evt->getEventNumber()>1000 && (pretrig_bits.at(ipre) & excludedTriggers)) {
	  //Fill a histo with dirty (non excluded) bits
#ifdef HAVE_ROOT
	  for (unsigned int itrig = 0;itrig<32;itrig++) {
	    if((1<<itrig) & pretrig_bits.at(ipre) && ( (1<<itrig) & excludedTriggers)) _hpre->Fill(itrig);
          }
#else
	  //icount=0;
	  //	  if(fifoContent.size() > 0) {
	  //	    for(IntVec::iterator vec_iter = fifoContent.begin();vec_iter != fifoContent.end(); vec_iter++){
	  //	      if(*vec_iter) {std::cout << "Fifo Element at " <<  icount << " is: " << "in hex: " << std::hex << *vec_iter << std::dec << " in ";
	  //	      to_binary_bitops(*vec_iter); } 
	  //	      for (unsigned int itrig = 0;itrig<32;itrig++) {
	  //		_hbit->Fill(icount,static_cast<float>((1<<itrig) & (*vec_iter)));
	  //	      }
	  //	      icount++;
	  //	    }
	  //	  }
#endif
	 }
      }
    } else std::cout << "Size of Pretrig distance vector and bit vector do not agree, bug in Trigger Handling !!!!" << std::endl;

    //PostTrigger History
    IntVec posttrig_pos;
    IntVec posttrig_bits;
    evt->getParameters().getIntVals(std::string(_parNameTriggerPostHistory)+"Pos", posttrig_pos);
    evt->getParameters().getIntVals(std::string(_parNameTriggerPostHistory)+"Bits", posttrig_bits);
    if(posttrig_pos.size() == posttrig_bits.size() ) {
      std::cout << "Post Trigger Bits " << std::endl;
      TriggerBits postBits;      
      std::cout << "Cerenkov on: " << postBits.isCherenkovTrigger() << std::endl;

      for (unsigned int ipost = 0; ipost < posttrig_pos.size();
	   ipost++) {
	std::cout << "Distance to delay+jitter corrected main word: " << posttrig_pos.at(ipost) << TriggerBits(posttrig_bits.at(ipost) ) << std::endl;

        TriggerBits postBits(posttrig_bits.at(ipost));
	//std::cout << "Cerenkov on: " << postBits.isCherenkovTrigger() << std::endl;    
	//std::cout << "Cerenkov2 on: " << postBits.isCherenkov2Trigger() << std::endl;    
        if(posttrig_pos.at(ipost)==1 && evt->getEventNumber()>1000 && (posttrig_bits.at(ipost) & excludedTriggers)) {
#ifdef HAVE_ROOT
	  for (unsigned int itrig = 0;itrig<32;itrig++) {
	    if((1<<itrig) & posttrig_bits.at(ipost) && ( (1<<itrig) & excludedTriggers)) _hpost->Fill(itrig);
          }
#else
	  icount=0;
	  if(fifoContent.size() > 0) {
	    for(IntVec::iterator vec_iter = fifoContent.begin();vec_iter != fifoContent.end(); vec_iter++){
	      if(*vec_iter) {std::cout << "Fifo Element at " <<  icount << " is: " << "in hex: " << std::hex << *vec_iter << std::dec << " in ";
	      to_binary_bitops(*vec_iter); } 
	      
	      icount++;
	    }
	   }
#endif
        }
      }
    } else std::cout << "Size of Posttrig distance vector and bit vector do not agree, bug in Trigger Handling !!!!" << std::endl;

     printMiscInformation(evt);

  }

void CaliceExampleProcessor::printMiscInformation(LCEvent* evt) {
    //Extract the data from the various condition data sets
    //TrgConfData
       
    if(_trgroConfDataCol) {
     TrgReadoutConfigurationBlock trgro_datablock = TrgReadoutConfigurationBlock(_trgroConfDataCol->getElementAt(0));
     std::cout << " TrgReadoutConfigurationData: "  << std::endl;
     std::cout << " is Enable: " << trgro_datablock.getEnable() << std::endl;
     std::cout << " ClearBeTrg: " << trgro_datablock.getClearBeTrg() << std::endl;
     std::cout << " BeTrgSoftTrg: " << trgro_datablock.getBeTrgSoftTrg() << std::endl; 
     std::cout << " BeTrgSoftSpill: " << trgro_datablock.getBeTrgSoftSpill() << std::endl;
     std::cout << " BeTrgSquirt: " << trgro_datablock.getBeTrgSquirt() << std::endl;
     std::cout << " BeTrgVlink: " << trgro_datablock.getBeTrgVlink() << std::endl;
     std::cout << " SpillInvert: " << trgro_datablock.getSpillInvert() << std::endl;
     std::cout << " ReadPeriod: " << trgro_datablock.getReadPeriod() << std::endl;
     std::cout << " ReadCPeriod: " << trgro_datablock.getReadCounterPeriod() << std::endl;
     std::cout << " BeTrgPollNumber: " << trgro_datablock.getBeTrgPollNumber() << std::endl;
     std::cout << " BeTrgPollTime: " << trgro_datablock.getBeTrgPollTimeSec() << "." << trgro_datablock.getBeTrgPollTimeMus()<< std::endl; 
     std::cout << " BeTrgPollNumber: " << trgro_datablock.getBeTrgSpillNumber() << std::endl;
     std::cout << " BeTrgSpillTime: " << trgro_datablock.getBeTrgSpillTimeSec() << "." << trgro_datablock.getBeTrgSpillTimeMus()<< std::endl; 
      


    }
    

    //Emc Stage Data (if present)
    int emcstg_datablocks = 0;
    if(_emcStgDataCol) {
      emcstg_datablocks = _emcStgDataCol->getNumberOfElements();
      for (int iblock = 0; iblock < emcstg_datablocks; iblock++) {
	EmcStageDataBlock emcStgDat(_emcStgDataCol->getElementAt(iblock)); 
	std::cout << std::hex << "Header: " << emcStgDat.getHeader() << std::endl;  
	std::cout << " Header byte 0: " <<  ( (emcStgDat.getHeader()&0xFF) >> 0) << std::endl;
	std::cout << " Header byte 1: " <<  ( (emcStgDat.getHeader()&0xFF00) >> 8) << std::endl;
	std::cout << " Header byte 2: " <<  ( (emcStgDat.getHeader()&0xFF0000) >> 16) << std::endl;
	std::cout  << " Header byte 3: " <<  ((emcStgDat.getHeader()&0xFF000000) >> 24) << std::dec << std::endl;
	std::cout << " XIndexerStatus: " << emcStgDat.getXIndexerStatus() << std::endl;
	std::cout << " YIndexerStatus: " << emcStgDat.getYIndexerStatus() << std::endl;
	std::cout << " xStandPosition/mm: " << (float) emcStgDat.getXStandPosition()*0.1 << std::endl;
	std::cout << " yStandPosition/mm: " << (float) emcStgDat.getYStandPosition()*0.1 << std::endl;
	std::cout << " xBeamPosition/mm: " << (float) emcStgDat.getXBeamPosition()*0.1 << std::endl;
	std::cout << " yBeamPosition/mm: " << (float) emcStgDat.getYBeamPosition()*0.1 << std::endl;
	std::cout << " CheckSum: " << emcStgDat.getCheckSum() << std::endl;
      }


    }
    

    //Readout configuration Data
    
    int roconf_datablocks = 0;
    if(_roConfDataCol) {
      roconf_datablocks = _roConfDataCol->getNumberOfElements();
      for (int iblock = 0; iblock < roconf_datablocks; iblock++) {
	ReadOutConfigurationBlock roConfDat(_roConfDataCol->getElementAt(iblock)); 
	std::cout << std::hex << "Slot Enable: " << roConfDat.getSlotEnable() << std::endl;  
	std::cout << " Slot FeEnable 0: " <<  roConfDat.getFeSlotEnable(0) << std::endl;
	std::cout << " Slot FeEnable 1: " <<  roConfDat.getFeSlotEnable(1) << std::endl;
	std::cout << " Slot FeEnable 2: " <<  roConfDat.getFeSlotEnable(2) << std::endl;
	std::cout << " Slot FeEnable 3: " <<  roConfDat.getFeSlotEnable(3) << std::endl;
	std::cout << " Slot FeEnable 4: " << roConfDat.getFeSlotEnable(4) << std::endl;
	std::cout << " Vme Period: " << roConfDat.getVmePeriod() << std::endl;
	std::cout << " Be Period: " << roConfDat.getBePeriod() << std::endl;
	std::cout << " Bec Period " << roConfDat.getBecPeriod() << std::endl;
	std::cout << " FePeriod: " <<  roConfDat.getFePeriod() << std::endl;
	std::cout << " BeSoftTrigger: " << roConfDat.getBeSoftTrigger() << std::endl;
	std::cout << " Fe Broadcast Soft Trigger: " << roConfDat.getFeBrcSoftTrigger() << std::endl;
	std::cout << " Fe Broadcast Soft Trigger: " << roConfDat.getFeBrcSoftTrigger() << std::endl;
	std::cout << " Conf Mode: " << roConfDat.getConfMode() << std::dec << std::endl;
      }


      }
    

    
    //BeTrgConfiguration Data (would be better to access these by the
    //interface class TriggerHandler but here we go ...
   
    int betrg_confblocks = 0;
    if(_beTrgConfCol && evt->getEventNumber()>-1) {
      betrg_confblocks = _beTrgConfCol->getNumberOfElements();
      std::cout << "Data Description BeTrgConf: " << _beTrgConfCol->getParameters().getStringVal("DataDescription") << endl;   
    for (int ibt = 0; ibt < betrg_confblocks; ibt++) {
      BeTrgConf beTrgConf(_beTrgConfCol->getElementAt(ibt));  
        
      std::cout << " board: " << std::hex << beTrgConf.getBoardID() << std::dec << std::endl;
      std::cout << " Record Label=" << beTrgConf.getRecordLabel() << std::endl;
      std::cout << " enabled mask=" << beTrgConf.getInputEnableMask() << std::endl;
      std::cout << " osc. enable=" << beTrgConf.getOscillatorEnable() << std::endl;
      std::cout << " general enable=" << beTrgConf.getGeneralEnable() << std::endl;
      std::cout << " osc. period=" << beTrgConf.getOscillationPeriod() << std::endl;
      std::cout << " burst counter=" << beTrgConf.getBurstCounter() << std::endl;
      std::cout << " configuration=" << beTrgConf.getConfiguration() << std::endl;
      std::cout << " fifo idle depth=" << beTrgConf.getFifoIdleDepth() << std::endl;
      std::cout << " Busy Timeout=" << beTrgConf.getBusyTimeout() << std::endl;
      std::cout << " Burst Timeout=" << beTrgConf.getBurstTimeout() << std::endl;
      std::cout << " and Enable 0=" << std::hex << beTrgConf.getAndEnable(0) << std::endl;
      std::cout << " and Enable 1=" << beTrgConf.getAndEnable(1) << std::endl;
      std::cout << " and Enable 2=" << beTrgConf.getAndEnable(2) << std::endl;
      std::cout << " and Enable 3=" << beTrgConf.getAndEnable(3) << std::dec << std::endl;
      std::cout << " ext Beammode=" << beTrgConf.getExtBeamMode() << std::endl;
      std::cout << " Input Invert=" << beTrgConf.getInputInvert() << std::endl;
      std::cout << " QDR Configuration=" << beTrgConf.getQdrConfiguration() << std::endl;
      std::cout << " Sequencer Control=" << beTrgConf.getSequencerControl() << std::dec << std::endl;


    }

    }
    

    
    //EmcFeConfiguration
    
    int emcfe_confblocks = 0;
    if(_emcFeConfCol) {
      emcfe_confblocks = _emcFeConfCol->getNumberOfElements();
      std::cout << "Data Description FeConf: " << _emcFeConfCol->getParameters().getStringVal("DataDescription") << endl;   

    
    for (int ife = 0; ife < emcfe_confblocks; ife++) {

      FeConfigurationBlock feConfBlock(_emcFeConfCol->getElementAt(ife));  
      std::cout << "EMC FeConfiguration: CrateID " << feConfBlock.getCrateID() << std::endl;   
      std::cout << "EMC FeConfiguration: SlotID " << feConfBlock.getSlotID() << std::endl;   
      std::cout << "EMC FeConfiguration: BoardComponentNumber " << feConfBlock.getBoardComponentNumber() << std::endl;   
      std::cout << "EMC FeConfiguration: RecordLabel " << feConfBlock.getRecordLabel() << std::endl;   
      std::cout << "calibStart: " << feConfBlock.getCalibStart() << std::endl;
      std::cout << "calibWidth: " << feConfBlock.getCalibWidth() << std::endl;
      std::cout << "calibEnable: " << feConfBlock.isCalibEnable() << std::endl;
      std::cout << "holdStart: " << feConfBlock.getHoldStart() << std::endl;
      std::cout << "holdWidth: " << feConfBlock.getHoldWidth() << std::endl;
      std::cout << "holdInvert: " << feConfBlock.isHoldInvert() << std::endl;
      std::cout << "vfeResetStart: " << feConfBlock.getVfeResetStart() << std::endl;
      std::cout << "vfeResetEnd: " << feConfBlock.getVfeResetEnd() << std::endl;
      std::cout << "vfeSrinStart: " << feConfBlock.getVfeSrinStart() << std::endl;
      std::cout << "fveSrinEnd: " << feConfBlock.getVfeSrinEnd() << std::endl;
      std::cout << "vfeMplexClockStart: " << feConfBlock.getVfeMplexClockStart() << std::endl;
      std::cout << "vfeMplexClockMark: " << feConfBlock.getVfeMplexClockMark() << std::endl;
      std::cout << "vfeMplexClockSpace: " << feConfBlock.getVfeMplexClockSpace() << std::endl;
      std::cout << "vfeMplexClockPulses: " << feConfBlock.getVfeMplexClockPulses() << std::endl;
      std::cout << "adcStart: " << feConfBlock.getAdcStart() << std::endl;
      std::cout << "adcEnd: " << feConfBlock.getAdcEnd() << std::endl;
      std::cout << "adccontrol: " << feConfBlock.getAdcControlBits() << std::endl;
      std::cout << "adcDelay: " << feConfBlock.getAdcDelay() << std::endl;
      std::cout << "dacData(CrcFeConfigurationData::top): " << feConfBlock.getDacDataTop() << std::endl;
      std::cout << "dacData(CrcFeConfigurationData::top): " << feConfBlock.getDacDataBottom()<< std::endl;
      std::cout << "frameSyncDelay: " << feConfBlock.getFrameSyncDelay() << std::endl;
      std::cout << "qdrDataDelay: " << feConfBlock.getQdrDataDelay() << std::endl;
      std::cout << "qdrDataDelay: " << feConfBlock.getQdrDataDelay() << std::endl;
      std::cout << "vfeInfo: " << std::hex << std::showbase << feConfBlock.getVfeInfo() << std::dec << std::endl;

      for (int ichip = 0; ichip < 6; ichip++) {
	std::cout << "emcVfeEnable Bottom Connector, Chip " << ichip
      << ": " <<  feConfBlock.emcVfeBottomEnable(ichip) << std::endl; 
	std::cout << "emcVfeEnable Top Connector, Chip " << ichip 
      << ": " <<  feConfBlock.emcVfeTopEnable (ichip) << std::endl; 
      }
      std::cout << "LowGain Bit set ? Bottom Connector: " 
                << feConfBlock.emcVfeLowGainBottom()  << std::endl;
      std::cout << "LowGain Bit set ? Top Connector: " 
                << feConfBlock.emcVfeLowGainTop()  << std::endl;

      
    } 

    }
    

    
    //AhcFeConfiguration and 
    int fe_confblocks = 0;
    if(_ahcFeConfCol){
      fe_confblocks = _ahcFeConfCol->getNumberOfElements();
      std::cout << "Data Description FeConf: " << _ahcFeConfCol->getParameters().getStringVal("DataDescription") << endl;   
      
      
      for (int ife = 0; ife < fe_confblocks; ife++) {
	
	FeConfigurationBlock feConfBlock(_ahcFeConfCol->getElementAt(ife));  
	std::cout << "AHC FeConfiguration: CrateID " << feConfBlock.getCrateID() << std::endl;   
	std::cout << "AHC FeConfiguration: SlotID " << feConfBlock.getSlotID() << std::endl;   
	std::cout << "AHC FeConfiguration: BoardComponentNumber " << feConfBlock.getBoardComponentNumber() << std::endl;   
	std::cout << "AHC FeConfiguration: RecordLabel " << feConfBlock.getRecordLabel() << std::endl;   
	std::cout << "calibStart: " << feConfBlock.getCalibStart() << std::endl;
	std::cout << "calibWidth: " << feConfBlock.getCalibWidth() << std::endl;
	std::cout << "calibEnable: " << feConfBlock.isCalibEnable() << std::endl;
	std::cout << "holdStart: " << feConfBlock.getHoldStart() << std::endl;
	std::cout << "holdWidth: " << feConfBlock.getHoldWidth() << std::endl;
	std::cout << "holdInvert: " << feConfBlock.isHoldInvert() << std::endl;
	std::cout << "vfeResetStart: " << feConfBlock.getVfeResetStart() << std::endl;
	std::cout << "vfeResetEnd: " << feConfBlock.getVfeResetEnd() << std::endl;
	std::cout << "vfeSrinStart: " << feConfBlock.getVfeSrinStart() << std::endl;
	std::cout << "fveSrinEnd: " << feConfBlock.getVfeSrinEnd() << std::endl;
	std::cout << "vfeMplexClockStart: " << feConfBlock.getVfeMplexClockStart() << std::endl;
	std::cout << "vfeMplexClockMark: " << feConfBlock.getVfeMplexClockMark() << std::endl;
	std::cout << "vfeMplexClockSpace: " << feConfBlock.getVfeMplexClockSpace() << std::endl;
	std::cout << "vfeMplexClockPulses: " << feConfBlock.getVfeMplexClockPulses() << std::endl;
	std::cout << "adcStart: " << feConfBlock.getAdcStart() << std::endl;
	std::cout << "adcEnd: " << feConfBlock.getAdcEnd() << std::endl;
	std::cout << "adccontrol: " << feConfBlock.getAdcControlBits() << std::endl;
	std::cout << "adcDelay: " << feConfBlock.getAdcDelay() << std::endl;
	std::cout << "dacData(CrcFeConfigurationData::top): " << feConfBlock.getDacDataTop() << std::endl;
	std::cout << "dacData(CrcFeConfigurationData::bottom): " << feConfBlock.getDacDataBottom()<< std::endl;
	std::cout << "frameSyncDelay: " << feConfBlock.getFrameSyncDelay() << std::endl;
	std::cout << "qdrDataDelay: " << feConfBlock.getQdrDataDelay() << std::endl;
	std::cout << "qdrDataDelay: " << feConfBlock.getQdrDataDelay() << std::endl;
	std::cout << "vfeInfo: " << std::hex << std::showbase << feConfBlock.getVfeInfo() << std::dec << std::endl;	
      } 

    }


    //FeConfiguration and 
    fe_confblocks = 0;
    if(_feConfCol){
      fe_confblocks = _feConfCol->getNumberOfElements();
      std::cout << "Data Description FeConf: " << _feConfCol->getParameters().getStringVal("DataDescription") << endl;   
      
      for (int ife = 0; ife < fe_confblocks; ife++) {
	
	FeConfigurationBlock feConfBlock(_feConfCol->getElementAt(ife));  
	std::cout << " FeConfiguration: CrateID " << feConfBlock.getCrateID() << std::endl;   
	std::cout << " FeConfiguration: SlotID " << feConfBlock.getSlotID() << std::endl;   
	std::cout << " FeConfiguration: BoardComponentNumber " << feConfBlock.getBoardComponentNumber() << std::endl;   
	std::cout << " FeConfiguration: RecordLabel " << feConfBlock.getRecordLabel() << std::endl;   
	std::cout << "calibStart: " << feConfBlock.getCalibStart() << std::endl;
	std::cout << "calibWidth: " << feConfBlock.getCalibWidth() << std::endl;
	std::cout << "calibEnable: " << feConfBlock.isCalibEnable() << std::endl;
	std::cout << "holdStart: " << feConfBlock.getHoldStart() << std::endl;
	std::cout << "holdWidth: " << feConfBlock.getHoldWidth() << std::endl;
	std::cout << "holdInvert: " << feConfBlock.isHoldInvert() << std::endl;
	std::cout << "vfeResetStart: " << feConfBlock.getVfeResetStart() << std::endl;
	std::cout << "vfeResetEnd: " << feConfBlock.getVfeResetEnd() << std::endl;
	std::cout << "vfeSrinStart: " << feConfBlock.getVfeSrinStart() << std::endl;
	std::cout << "fveSrinEnd: " << feConfBlock.getVfeSrinEnd() << std::endl;
	std::cout << "vfeMplexClockStart: " << feConfBlock.getVfeMplexClockStart() << std::endl;
	std::cout << "vfeMplexClockMark: " << feConfBlock.getVfeMplexClockMark() << std::endl;
	std::cout << "vfeMplexClockSpace: " << feConfBlock.getVfeMplexClockSpace() << std::endl;
	std::cout << "vfeMplexClockPulses: " << feConfBlock.getVfeMplexClockPulses() << std::endl;
	std::cout << "adcStart: " << feConfBlock.getAdcStart() << std::endl;
	std::cout << "adcEnd: " << feConfBlock.getAdcEnd() << std::endl;
	std::cout << "adccontrol: " << feConfBlock.getAdcControlBits() << std::endl;
	std::cout << "adcDelay: " << feConfBlock.getAdcDelay() << std::endl;
	std::cout << "dacData(CrcFeConfigurationData::top): " << feConfBlock.getDacDataTop() << std::endl;
	std::cout << "dacData(CrcFeConfigurationData::bottom): " << feConfBlock.getDacDataBottom()<< std::endl;
	std::cout << "frameSyncDelay: " << feConfBlock.getFrameSyncDelay() << std::endl;
	std::cout << "qdrDataDelay: " << feConfBlock.getQdrDataDelay() << std::endl;
	std::cout << "qdrDataDelay: " << feConfBlock.getQdrDataDelay() << std::endl;
	std::cout << "vfeInfo: " << std::hex << std::showbase << feConfBlock.getVfeInfo() << std::dec << std::endl;	
      } 

    }
    

    
    //AhcVfeConfiguration
    
    int vfe_confblocks = 0;
    if(_ahcVfeConfCol) {
      vfe_confblocks = _ahcVfeConfCol->getNumberOfElements();
      std::cout << "Data Description Ahc VfeConf: " << _ahcVfeConfCol->getParameters().getStringVal("DataDescription") << endl;   
      
      
      for (int ivfe = 0; ivfe < vfe_confblocks; ivfe++) {
	AhcVfeConfigurationBlock vfeConfBlock(_ahcVfeConfCol->getElementAt(ivfe));  
	std::cout << "AHC VfeConfiguration: CrateID " << vfeConfBlock.getCrateID() << std::endl;   
	std::cout << "AHC VfeConfiguration: SlotID " << vfeConfBlock.getSlotID() << std::endl;   
	std::cout << "AHC VfeConfiguration: BoardComponentNumber " << vfeConfBlock.getBoardComponentNumber() << std::endl;   
	std::cout << "AHC VfeConfiguration: RecordLabel " << vfeConfBlock.getRecordLabel() << std::endl;   
	std::cout << "AHC VfeConfiguration: Verification Data " << vfeConfBlock.getVerificationData() << std::endl; 
	
	for (int ishift = 0; ishift < NAHCSHREGDATA; ishift++) {
	  std::cout << "AHC VfeConfiguration: Shift Register " << ishift << ":" << vfeConfBlock.getShiftRegisterData(ishift) << std::endl;   
	}
	
      }
      
    }
    
   
    
    
    if(_ahcSroDataCol) {
      AhcSlowReadoutBlock ahcSroDat(_ahcSroDataCol->getElementAt(0)); 
      std::cout << "Moving stage position x[mm]: " << ahcSroDat.get_xPosition_mm() << std::endl;
      std::cout << "Moving stage position y[mm]: " << ahcSroDat.get_yPosition_mm() << std::endl;
      std::cout << "z Position[mm]: " << ahcSroDat.get_zPosition_mm() << std::endl;
      std::cout << "rotated Position[deg]: " << ahcSroDat.get_cPosition_deg() << std::endl;

    }
    


    
    if(_ahcSroModDataCol) {
      //Retrieve the collection with Ahc Slow control data (Temps, Volts)    
      //_ahcSroModDataCol(evt->getCollection(COL_AHC_SRO_MOD));
      for (unsigned int imod=0; imod < static_cast<unsigned int>(_ahcSroModDataCol->getNumberOfElements()); imod++) {
	AhcSlowReadoutModBlock ahcSroModBlock(_ahcSroModDataCol->getElementAt(imod)); 
	std::cout << "Ahc SlowReadout Module Data" << std::endl;
	std::cout << "Module Number: " << ahcSroModBlock.getModuleNumber() << std::endl;
	std::cout << "Time Stamp: " << ahcSroModBlock.getTimeStamp().getDateString() << std::endl;
	std::cout << "Led Setting: " << std::hex << "hex: " << ahcSroModBlock.getLedSetting() << " " << std::dec; 
	to_binary_bitops(ahcSroModBlock.getLedSetting());
	std::cout << "CMB Width: " <<  ahcSroModBlock.getCmbWidth() << std::endl;
	std::cout << "CMB Height: " << ahcSroModBlock.getCmbHeight() << std::endl;
	std::cout << "*** CMB Temperatures: ***" << std::endl;
	
	//Print the cmb temperatures
	int icmbtemps(0);
	std::vector<double> cmbTemperatures(ahcSroModBlock.getCmbTemperatures());
	for(std::vector<double>::iterator vec_iter = cmbTemperatures.begin(); vec_iter !=
	      cmbTemperatures.end(); vec_iter++) {
	  if(icmbtemps < 5 ) std::cout << "CMB Temperature [" << icmbtemps+1 << "]: " << (*vec_iter) << " C" << std::endl;
	  if(icmbtemps == 5) std::cout << "CMB Temperature lower: " << (*vec_iter) << std::endl;
	  if(icmbtemps == 6) std::cout << "CMB Temperature upper: " << (*vec_iter) << std::endl;
	  if(icmbtemps > 6)  std::cout << "CMB Temperature UNKNOWN TYPE [" << icmbtemps+1 << "]: " << (*vec_iter) << " C" << std::endl;
	  icmbtemps++;
	}
	
	
	std::cout << "*** CMB Voltages: ****" << std::endl;
	//Print the cmb voltages
	int icmbvolts(0);
	std::vector<double> cmbVoltages(ahcSroModBlock.getCmbVoltages());
	for(std::vector<double>::iterator vec_iter = cmbVoltages.begin(); vec_iter !=
	      cmbVoltages.end(); vec_iter++) {
	  if(icmbvolts==0) std::cout << "CMB calib U041: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==1) std::cout << "CMB 12V power: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==2) std::cout << "CMB 1.235V: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==3) std::cout << "CMB VLDA upper: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==4) std::cout << "CMB VLDB upper: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==5) std::cout << "CMB VLDC upper: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==6) std::cout << "CMB VLDD lower: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==7) std::cout << "CMB 10 V bias: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==8) std::cout << "CMB calib U051 voltage: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts > 8)  std::cout << "CMB Voltage UNKNOWN TYPE [" << icmbvolts+1 << "]: " << (*vec_iter) << " C" << std::endl;
	  icmbvolts++;
	}
	std::cout << "*** CMB Values: ***" << std::endl;
	//Print the cmb values
	int icmbvals(0);
	std::vector<double> cmbValues(ahcSroModBlock.getCmbValues());
	for(std::vector<double>::iterator vec_iter = cmbValues.begin(); vec_iter !=
	      cmbValues.end(); vec_iter++) {
	  if(icmbvals==0) std::cout << "CMB 12V external 1: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvals==1) std::cout << "CMB 12V external 2: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvals>1)  std::cout << "CMB Values UNKNOWN TYPE [" << icmbvals+1 << "]: " << (*vec_iter) << " V" << std::endl;
	  icmbvals++;
	}
	
	std::cout << "*** HBAB Temperatures: ***" << std::endl;
	//Print the hbab temperatures
	int ihbabtemps(0);
	std::vector<double> hbabTemperatures(ahcSroModBlock.getHbabTemperatures());
	for(std::vector<double>::iterator vec_iter = hbabTemperatures.begin(); vec_iter !=
	      hbabTemperatures.end(); vec_iter++) {
	  if(ihbabtemps==0) std::cout << "HBAB Temperature top 1: " << (*vec_iter) << " C" << std::endl;
	  if(ihbabtemps==1) std::cout << "HBAB Temperature top 2: " << (*vec_iter) << " C" << std::endl;
	  if(ihbabtemps==2) std::cout << "HBAB Temperature bot 1: " << (*vec_iter) << " C" << std::endl;
	  if(ihbabtemps==3) std::cout << "HBAB Temperature bot 2: " << (*vec_iter) << " C" << std::endl;
	  if(ihbabtemps>3)  std::cout << "HBAB Temperature bot UNKNOWN TYPE [" << ihbabtemps+1 << "]: " << (*vec_iter) << " X" << std::endl;
	  ihbabtemps++;
	}
	
	
	std::cout << "*** HBAB Voltages: ***" << std::endl;
	//Print the hbab voltages/currents values
	int ihbabvolts(0);
	std::vector<double> hbabVoltages(ahcSroModBlock.getHbabVoltages());
	for(std::vector<double>::iterator vec_iter = hbabVoltages.begin(); vec_iter !=
	      hbabVoltages.end(); vec_iter++) {
	  if(ihbabvolts==0) std::cout << "HBAB HV top voltage: " << (*vec_iter) << " V" << std::endl;
	  if(ihbabvolts==1) std::cout << "HBAB HV bot voltage: " << (*vec_iter) << " V" << std::endl;
	  if(ihbabvolts==2) std::cout << "HBAB HV top current: " << (*vec_iter) << " A" << std::endl;
	  if(ihbabvolts==3) std::cout << "HBAB HV bot current: " << (*vec_iter) << " A" << std::endl;
	  if(ihbabvolts==4) std::cout << "HBAB LV top voltage: " << (*vec_iter) << " V" << std::endl;
	  if(ihbabvolts==5) std::cout << "HBAB LV bot voltage: " << (*vec_iter) << " V" << std::endl;
	  if(ihbabvolts==6) std::cout << "HBAB LV top current: " << (*vec_iter) << " A" << std::endl;
	  if(ihbabvolts==7) std::cout << "HBAB LV bot current: " << (*vec_iter) << " A" << std::endl;
	  if(ihbabvolts==8) std::cout << "HBAB LVn top voltage: " << (*vec_iter) << " V" << std::endl;
	  if(ihbabvolts==9) std::cout << "HBAB LVn bot voltage: " << (*vec_iter) << " V" << std::endl;
	  if(ihbabvolts==10) std::cout << "HBAB LVn top current: " << (*vec_iter) << " A" << std::endl;
	  if(ihbabvolts==11) std::cout << "HBAB LVn bot current: " << (*vec_iter) << " A" << std::endl;
	  if(ihbabvolts>11)  std::cout << "HBAB Voltage UNKNOWN TYPE [" << ihbabtemps+1 << "]: " << (*vec_iter) << " X" << std::endl;
	  ihbabvolts++;
	}
	
	
      }

    }
    //ahcSroMod End    

    //sce mod begins
    if(_sceSroModDataCol) {
      //Retrieve the collection with Sce Slow control data (Temps, Volts)    
      //_ahcSroModDataCol(evt->getCollection(COL_AHC_SRO_MOD));
      for (unsigned int imod=0; imod < static_cast<unsigned int>(_sceSroModDataCol->getNumberOfElements()); imod++) {
	SceSlowReadoutModBlock sceSroModBlock(_sceSroModDataCol->getElementAt(imod)); 
	std::cout << "Sce SlowReadout Module Data" << std::endl;
	std::cout << "Module Number: " << sceSroModBlock.getModuleNumber() << std::endl;
	std::cout << "Time Stamp: " << sceSroModBlock.getTimeStamp().getDateString() << std::endl;
	std::cout << "Led Setting: " << std::hex << "hex: " << sceSroModBlock.getLedSetting() << " " << std::dec; 
	to_binary_bitops(sceSroModBlock.getLedSetting());
	std::cout << "CMB Width: " <<  sceSroModBlock.getCmbWidth() << std::endl;
	std::cout << "CMB Height: " << sceSroModBlock.getCmbHeight() << std::endl;
	std::cout << "*** CMB Temperatures: ***" << std::endl;
	
	//Print the cmb temperatures
	int icmbtemps(0);
	std::vector<double> cmbTemperatures(sceSroModBlock.getCmbTemperatures());
	for(std::vector<double>::iterator vec_iter = cmbTemperatures.begin(); vec_iter !=
	      cmbTemperatures.end(); vec_iter++) {
	  if(icmbtemps == 0) std::cout << "CMB Temperature lower: " << (*vec_iter) << " C" << std::endl;
	  if(icmbtemps == 1) std::cout << "CMB Temperature upper: " << (*vec_iter) << " C" << std::endl;
	  icmbtemps++;
	}
	
	
	std::cout << "*** CMB Voltages: ****" << std::endl;
	//Print the cmb voltages
	int icmbvolts(0);
	std::vector<double> cmbVoltages(sceSroModBlock.getCmbVoltages());
	for(std::vector<double>::iterator vec_iter = cmbVoltages.begin(); vec_iter !=
	      cmbVoltages.end(); vec_iter++) {
	  if(icmbvolts==0) std::cout << "CMB calib U041: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==1) std::cout << "CMB 12V power: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==2) std::cout << "CMB 1.235V: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==3) std::cout << "CMB VLDA upper: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==4) std::cout << "CMB VLDB upper: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==5) std::cout << "CMB VLDC upper: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==6) std::cout << "CMB VLDD lower: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==7) std::cout << "CMB 10 V bias: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts==8) std::cout << "CMB calib U051 voltage: " << (*vec_iter) << " V" << std::endl;
	  if(icmbvolts > 8)  std::cout << "CMB Voltage UNKNOWN TYPE [" << icmbvolts+1 << "]: " << (*vec_iter) << " C" << std::endl;
	  icmbvolts++;
	}
	

	std::cout << "*** HBAB Voltages and Currents: ***" << std::endl;
	//Print the hbab voltages/currents values
	//int ihbabvolts(0);
	std::vector<double> hbabVoltages(sceSroModBlock.getHbabVoltages());
	std::vector<double> hbabCurrents(sceSroModBlock.getHbabCurrents());
	if(hbabVoltages.size() == hbabCurrents.size()) {
	  std::vector<double>::iterator veccur_iter=hbabCurrents.begin();
	  for(std::vector<double>::iterator vecvol_iter = hbabVoltages.begin(); vecvol_iter != hbabVoltages.end(); vecvol_iter++, veccur_iter++) {
	    std::cout << "HBAB SCE HV voltage: " << (*vecvol_iter) << " V" << std::endl;
	    std::cout << "HBAB SCE HV voltage: " << (*veccur_iter) << " A" << std::endl;    
	  }
	} else {
	  std::cout << "WARNING - Number of measured HBAB Currents does not correspond to number of measured HBAB Voltages!!!" << std::endl;
	  std::cout << "Number of measured HBAB Currents is: " << hbabCurrents.size() << std::endl;
	  std::cout << "Number of measured HBAB Voltages is: " << hbabVoltages.size() << std::endl;
	}
      }
    }
    //sce mod ends
    
    //sce temps begins
    if(_sceSroTempDataCol) {
      std::cout << "*********************SCE Temperatures**************************************" << std::endl;
      //Retrieve the collection with Sce Temperature Values    
      for (unsigned int imod=0; imod < static_cast<unsigned int>(_sceSroTempDataCol->getNumberOfElements()); imod++) {
	DaqTypeDataBlock sceSroTempBlock(_sceSroTempDataCol->getElementAt(imod)); 
	//Loop over the entries for a given type
	//Timestamps
	TimeStampMap_t timeStampMap(sceSroTempBlock.getTimeStamps());
	for(TimeStampMap_t::iterator timestamp_iter= timeStampMap.begin(); timestamp_iter!= timeStampMap.end(); timestamp_iter++) std::cout << (*timestamp_iter).first << ": " << (*timestamp_iter).second.getDateString() << std::endl;
	//Int values
	DaqTypeDataIntMap_t theIntMap(sceSroTempBlock.getIntArrays());
	for( DaqTypeDataIntMap_t::iterator slowmap_iter = theIntMap.begin(); slowmap_iter != theIntMap.end(); slowmap_iter++) { 
	  std::cout << "Datatype is: " << (*slowmap_iter).first << std::endl;
	  for(std::vector<int>::const_iterator thevals_iter =
		(*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ) std::cout << "Value: " << (*thevals_iter) << std::endl;
	}
	//Double Values
	DaqTypeDataDblMap_t theDblMap(sceSroTempBlock.getDblArrays());
	for( DaqTypeDataDblMap_t::iterator slowmap_iter = theDblMap.begin(); slowmap_iter != theDblMap.end(); slowmap_iter++) { 
	  std::cout << "Datatype is: " << (*slowmap_iter).first << std::endl;
	  for(std::vector<double>::const_iterator thevals_iter =
		(*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ) std::cout << "Value: " << (*thevals_iter) << std::endl;
	}
	//For completeness ... Float Values 
	DaqTypeDataFloatMap_t theFloatMap(sceSroTempBlock.getFloatArrays());
	for( DaqTypeDataFloatMap_t::iterator slowmap_iter = theFloatMap.begin(); slowmap_iter != theFloatMap.end(); slowmap_iter++) { 
	  std::cout << "Datatype is: " << (*slowmap_iter).first << std::endl;
	  for(std::vector<float>::const_iterator thevals_iter =
		(*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ) std::cout << "Value: " << (*thevals_iter) << std::endl;
	}
      }
    }
    //sce temps end
    
    //bml begins
    if(_bmlSroRunDataCol) {
      //Retrieve the collection with BmlSlowReadoutRunData (Beam Parameters (CERN))    
      //_bmlSroRunDataCol(evt->getCollection(COL_AHC_SRO_MOD));
      std::cout << "*********************(Cern) Beamline parameters **************************************" << std::endl;
      for (unsigned int ival=0; ival < static_cast<unsigned int>(_bmlSroRunDataCol->getNumberOfElements()); ival++) {
	BmlSlowRunDataBlock bmlSlowRunDataBlock(_bmlSroRunDataCol->getElementAt(ival)); 
	
	
        std::cout << "BmlSlow Run Data" << std::endl;
	std::cout << "Time Stamp:       " << bmlSlowRunDataBlock.getTimeStamp().getDateString() <<" h"  << std::endl;
	std::cout << "AbsorberPostition: " << bmlSlowRunDataBlock.getAbsorberPosition() << " mm" << std::endl;
	std::cout << "T4Position:       " << bmlSlowRunDataBlock.getT4Position() << " mm" << std::endl;
	std::cout << "TargetPosition:   " << bmlSlowRunDataBlock.getTargetPosition() << " mm" << std::endl;
	
	
	unsigned int idev(0);
	std::vector<double> sextapoleCurrents(bmlSlowRunDataBlock.getSextapoleCurrents());
	for(unsigned int isextcurs = 0; isextcurs < sextapoleCurrents.size()-1; isextcurs+=2) {
	  std::cout << "Sextapole Magnets Current[" << idev+1 << "] - Measurement:  " << sextapoleCurrents[isextcurs]  << " A" << std::endl;
	  std::cout << "Sextapole Magnets Current[" << idev+1 << "] - Reference:    " << sextapoleCurrents[isextcurs+1]  << " A" << std::endl;
	  idev++;
	}
    	
	
   
	std::cout << "*** Bending Magnets Current: ***" << std::endl; 
	idev=0;
	std::vector<double> bendCurrents(bmlSlowRunDataBlock.getBendCurrents());
	for(unsigned int ibendcurs = 0; ibendcurs < bendCurrents.size()-1; ibendcurs+=2) {
	  std::cout << "Bending Magnets Current["  << idev+1 << "] - Measurement:  " << bendCurrents[ibendcurs]  << " A" << std::endl;
	  std::cout << "Bending Magnets Current["  << idev+1 << "] - Reference:    " << bendCurrents[ibendcurs+1]  << " A" << std::endl;
	  idev++;
	}
	
	
	std::cout << "*** Quadupole Magnets Current: ***" << std::endl; 
	idev=0;
	std::vector<double> quadCurrents(bmlSlowRunDataBlock.getQuadrupoleCurrents());
	for(unsigned int iquadcurs = 0; iquadcurs < quadCurrents.size()-1; iquadcurs+=2) {
	  std::cout << "Quadrupole Magnets Current["  << idev+1 << "] - Measurement:  " << quadCurrents[iquadcurs]  << " A" << std::endl;
	  std::cout << "Quadrupole Magnets Current["  << idev+1 << "] - Reference:    " << quadCurrents[iquadcurs+1]  << " A" << std::endl;
	  idev++;
	}
	
	
	
	std::cout << "*** Trim Magnets Current: ***" << std::endl; 
	idev=0;
	std::vector<double> trimCurrents(bmlSlowRunDataBlock.getTrimCurrents());
	for(unsigned int itrimcurs = 0; itrimcurs < trimCurrents.size()-1; itrimcurs+=2) {
	  std::cout << "Trim Magnets Current["  << idev+1 << "] - Measurement:  " << trimCurrents[itrimcurs]  << " A" << std::endl;
	  std::cout << "Trim Magnets Current["  << idev+1 << "] - Reference:    " << trimCurrents[itrimcurs+1] << " A"  << std::endl;
	  idev++;
	}
	
	
	std::cout << "*** Collimator Settings: ***" << std::endl; 
	idev=0;
	std::vector<double> colPositions(bmlSlowRunDataBlock.getCollimatorPositions());
	for(unsigned int icolpos = 0; icolpos < colPositions.size()-3; icolpos+=4) {
	  std::cout << "Collimator Position x["  << idev+1 << "] - Measurement:  " << colPositions[icolpos]  << " mm" << std::endl;
	  std::cout << "Collimator Position x["  << idev+1 << "] - Reference:    " << colPositions[icolpos+1] << " mm"  << std::endl;
	  std::cout << "Collimator Position y["  << idev+1 << "] - Measurement:  " << colPositions[icolpos+2] << " mm" << std::endl;
	  std::cout << "Collimator Position y["  << idev+1 << "] - Reference:    " << colPositions[icolpos+3] << " mm"  << std::endl;
	  idev++;
	}
	
	
	std::cout << "*** H6A Experiment Count: ***" << std::endl; 
	//Print the h6a experiment count
	int ih6aexpcounts(0);
	std::vector<int> h6aExpCounts(bmlSlowRunDataBlock.getH6aExperimentCounts());
	for(std::vector<int>::iterator vec_iter = h6aExpCounts.begin(); vec_iter !=
          h6aExpCounts.end(); vec_iter++) {
	  if(ih6aexpcounts < 4 ) std::cout << "H6A Experiment Count [" << ih6aexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
	  if(ih6aexpcounts > 3)  std::cout << "H6A Experiment Count UNKNOWN TYPE [" << ih6aexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
	  ih6aexpcounts++;
	}
	
	
	
	std::cout << "*** H6B Experiment Counts: ***" << std::endl; 
	//Print the h6b experiment counts
	int ih6bexpcounts(0);
	std::vector<int> h6bExpCounts(bmlSlowRunDataBlock.getH6bExperimentCounts());
	for(std::vector<int>::iterator vec_iter = h6bExpCounts.begin(); vec_iter !=
	      h6bExpCounts.end(); vec_iter++) {
	  if(ih6bexpcounts < 4 ) std::cout << "H6B Experiment Count [" << ih6bexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
	  if(ih6bexpcounts > 3)  std::cout << "H6B Experiment Count UNKNOWN TYPE [" << ih6bexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
	  ih6bexpcounts++;
	}
	
	
	std::cout << "*** H6C Experiment Counts: ***" << std::endl; 
	//Print the h6c experiment counts
	int ih6cexpcounts(0);
	std::vector<int> h6cExpCounts(bmlSlowRunDataBlock.getH6cExperimentCounts());
	for(std::vector<int>::iterator vec_iter = h6cExpCounts.begin(); vec_iter !=
	      h6cExpCounts.end(); vec_iter++) {
	  if(ih6cexpcounts < 4 ) std::cout << "H6B Experiment Count [" << ih6cexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
	  if(ih6cexpcounts > 3)  std::cout << "H6B Experiment Count UNKNOWN TYPE [" << ih6cexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
	  ih6cexpcounts++;
	}
	
	
	std::cout << "*** RP Experiment Counts: ***" << std::endl; 
	//Print the rp experiment counts
	int irpexpcounts(0);
	std::vector<int> rpExpCounts(bmlSlowRunDataBlock.getRpExperimentCounts());
	for(std::vector<int>::iterator vec_iter = rpExpCounts.begin(); vec_iter !=
	      rpExpCounts.end(); vec_iter++) {
	  if(irpexpcounts < 8 ) std::cout << "RP Experiment Count [" << irpexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
	  if(irpexpcounts > 7)  std::cout << "RP Experiment Count UNKNOWN TYPE [" << irpexpcounts+1 << "]: " << (*vec_iter) << " Counts" << std::endl;
	  irpexpcounts++;
	}
	
	
	std::cout << "*** Scintillator Counts: ***" << std::endl; 
	//Print the scintillator counts
	int iscintcounts(0);
	std::vector<int> scintCounts(bmlSlowRunDataBlock.getScintillatorCounts());
	for(std::vector<int>::iterator vec_iter = scintCounts.begin(); vec_iter !=
	      scintCounts.end(); vec_iter++) {
	  if(iscintcounts < 9 ) std::cout << "Scintillator Counter [" << iscintcounts+1 << "]: " << (*vec_iter) << std::endl;
	  if(iscintcounts > 8)  std::cout << "Scintillator Counter UNKNOWN TYPE [" << iscintcounts+1 << "]: " << (*vec_iter) << std::endl;
	  iscintcounts++;
	}
      }
    }
    
    

    //Extract the beam parameters, note that here we extract beam parameters using the general DaqType... class which is applied for
    //the fnal parameters (and will be from now on to all future beamlines) 
    //The cern beam parameters 06/07 above could also be treated like that but are treated seperately for backward compatibility
    //The access to beam parameters will be handled by a Beamline manager class later on
    //Check whether we have bml parameters available
    if(_bmlParamsCollectionMap.size() > 0) {
      std::cout << "*********************Beamline Parameters Temperatures**************************************" << std::endl;
      //Loop over the bml parameter types 
      for (BmlParamsCollectionMap_t::iterator paramcol_iter = _bmlParamsCollectionMap.begin(); paramcol_iter != _bmlParamsCollectionMap.end(); paramcol_iter++){
	if((*paramcol_iter).second) {
	  std::cout << "Displaying: " << (*paramcol_iter).first << std::endl;
          for (unsigned int ival=0; ival < static_cast<unsigned int>((*paramcol_iter).second->getNumberOfElements()); ival++) {
	    DaqTypeDataBlock bmlSlowRunDataBlock((*paramcol_iter).second->getElementAt(ival)); 
            //Loop over the entries for a given type
            //Timestamps
            TimeStampMap_t timeStampMap(bmlSlowRunDataBlock.getTimeStamps());
	    for(TimeStampMap_t::iterator timestamp_iter= timeStampMap.begin(); timestamp_iter!= timeStampMap.end(); timestamp_iter++) std::cout << (*timestamp_iter).first << ": " << (*timestamp_iter).second.getDateString() << std::endl;
	    //Int values
            DaqTypeDataIntMap_t theIntMap(bmlSlowRunDataBlock.getIntArrays());
            for( DaqTypeDataIntMap_t::iterator slowmap_iter = theIntMap.begin(); slowmap_iter != theIntMap.end(); slowmap_iter++) { 
	      std::cout << "Datatype is: " << (*slowmap_iter).first << std::endl;
	      for(std::vector<int>::const_iterator thevals_iter =
	            (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ) std::cout << "Value: " << (*thevals_iter) << std::endl;
	    }
            //Double Values
            DaqTypeDataDblMap_t theDblMap(bmlSlowRunDataBlock.getDblArrays());
            for( DaqTypeDataDblMap_t::iterator slowmap_iter = theDblMap.begin(); slowmap_iter != theDblMap.end(); slowmap_iter++) { 
	      std::cout << "Datatype is: " << (*slowmap_iter).first << std::endl;
	      for(std::vector<double>::const_iterator thevals_iter =
	            (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ) std::cout << "Value: " << (*thevals_iter) << std::endl;
	    }
	    //For completeness ... Float Values 
            DaqTypeDataFloatMap_t theFloatMap(bmlSlowRunDataBlock.getFloatArrays());
            for( DaqTypeDataFloatMap_t::iterator slowmap_iter = theFloatMap.begin(); slowmap_iter != theFloatMap.end(); slowmap_iter++) { 
	      std::cout << "Datatype is: " << (*slowmap_iter).first << std::endl;
	      for(std::vector<float>::const_iterator thevals_iter =
	            (*slowmap_iter).second.begin(); thevals_iter != (*slowmap_iter).second.end(); thevals_iter++ ) std::cout << "Value: " << (*thevals_iter) << std::endl;
	    }
	  }          
	}	
      }
    }       
    //End of bml parameters display



    //TDC Data
    //Not that this demonstrates the access to the raw data class  
    //as written for the CAEN TDC which is used at CERN
    //No further handling has been applied so far  
    //The calcalation of positions, however, will be/is based on
    //the following data
    //The dedicated evaluation of the data  
    //would be subject to a dedicated processor which
    //would then present the data in a way to allow for a track reconstruction  
    
    
    for(lcio::StringVec::iterator string_iter=_colTdcVals.begin(); string_iter != _colTdcVals.end(); string_iter++) {
      std::cout << "BmlEventData: String: " << *string_iter << std::endl;
    try {
      LCCollection* caen767col(evt->getCollection(*string_iter));
      for (unsigned int ielm=0; ielm < static_cast<unsigned int>(caen767col->getNumberOfElements()); ielm++) {
	BmlEventData bmlEventData(caen767col->getElementAt(ielm));
	std::cout << " BmlEventData: "  << std::endl;
        //New October 2010: Adding and retrieving supplementary information
        bmlEventData.addSupplementaryInformation(evt, ielm);
	std::cout << "TDC Type: " << bmlEventData.getTDCType() << std::endl;  
        //End New October
       	std::cout << " BoardID: " << std::hex << BoardID(bmlEventData.getBoardID()) << std::endl;
	std::cout << " Base Address: " << bmlEventData.getBaseAddress() << std::dec << std::endl;
	std::cout << " Record Label: " << bmlEventData.getRecordLabel() << std::endl;
	std::cout << " Status Register: " << std::hex << bmlEventData.getStatusRegister() << std::dec << std::endl;
	std::cout << " Number of words: " << bmlEventData.getNumberOfWords() << std::endl; 
	std::cout << " Geo Address: " << std::hex << bmlEventData.getGeoAddress() << std::dec << std::endl; 
	std::cout << " Event Number: " << bmlEventData.getEventNumber() << std::endl; 
	std::cout << " Status: " << bmlEventData.getStatus() << std::endl;
	std::cout << " EventDataCounter: " << bmlEventData.getEventDataCounter() << std::endl;
        //New October 2010, supplementary information
	std::cout << "Word Count: " << bmlEventData.getWordCount() << std::endl;
        std::cout << " Event Count: " << bmlEventData.getEventCount() << std::endl;
        std::cout << " BunchID: " << bmlEventData.getBunchID() << std::endl;
        std::cout << " EventID TDC trailer: " << bmlEventData.getEventIDTrailer() << std::dec << std::endl;
        std::cout << " TDC errors: " << bmlEventData.getTDCErrors() << std::dec << std::endl;
        std::cout << " Buffer overflow: " << bmlEventData.getBufferOverflow() << std::endl;
        std::cout << " Trigger lost: " << bmlEventData.getTriggerLost() << std::endl;
        std::cout << " Number of trailer words: " << bmlEventData.getTDCWordCountTrailer() << std::endl; 
        //End New October 2010
	std::cout << " Number of channels with signal: " << bmlEventData.getNumberOfSignalChannels() << std::endl;
	if( bmlEventData.getNumberOfSignalChannels() > 0) { 
	  std::cout << " The channels follow: " << std::endl;
	  std::cout << " Falling edges are indicated by a minus sign " << std::endl;
	  TDCChannelContainer_t tdcChannelContainer = bmlEventData.getTDCChannelContainer();
	  for(TDCChannelContainer_t::iterator tdcchan_iter = tdcChannelContainer.begin(); tdcchan_iter != tdcChannelContainer.end(); tdcchan_iter++){
	    std::cout << "TDC Channel Number: " << static_cast<unsigned int>((*tdcchan_iter).first) << std::endl;
	    std::cout << "Number of measured signals: " << (*tdcchan_iter).second.size() <<std::endl;
	    for (std::vector< std::pair<bool,int> >::iterator tdcvec_iter = (*tdcchan_iter).second.begin();  tdcvec_iter != (*tdcchan_iter).second.end();
		 tdcvec_iter++ ) {
	      std::pair<bool, int> thepair = (*tdcvec_iter);
	      std::cout << "is StartTime?: " << thepair.first << std::endl;
	      std::cout << "Measured time: " << thepair.second << std::endl;
	    }   
	  }//tdc channel iteration
	} //num sigchannels
      } //loop over elements
    } catch (lcio::DataNotAvailableException err) {}
    } 

          
    if(_bmlCaenConfDataCol) {
      for (unsigned int ival=0; ival < static_cast<unsigned int>(_bmlCaenConfDataCol->getNumberOfElements()); ival++) {
       	BmlCaenConfigurationBlock bmlCaenConfDataBlock(_bmlCaenConfDataCol->getElementAt(ival)); 
	std::cout << "BmlCaenConfigurationData: " << std::hex << std::endl;
	std::cout << "BoardID: " << bmlCaenConfDataBlock.getBoardID() << std::endl;
	std::cout << "Base Address: " << bmlCaenConfDataBlock.getBaseAddress() << std::endl;
	std::cout << "Record Label: " << bmlCaenConfDataBlock.getRecordLabel() << std::endl;
	std::cout << "Control Register: " <<  bmlCaenConfDataBlock.getControlRegister() << std::endl;
	std::cout << "Interrupt Register: " << bmlCaenConfDataBlock.getInterruptRegister() << std::endl;
	std::cout << "Count Register: " << bmlCaenConfDataBlock.getCountRegister() << std::endl;
	std::cout << "Spare: " << bmlCaenConfDataBlock.getSpare() << std::dec << std::endl;    
      }
    }


    if(_bmlCaenRoConfDataCol) {
      for (unsigned int ival=0; ival < static_cast<unsigned int>(_bmlCaenRoConfDataCol->getNumberOfElements()); ival++) {
       	BmlCaenReadoutConfigurationBlock bmlCaenRoConfDataBlock(_bmlCaenRoConfDataCol->getElementAt(ival)); 
        std::cout << "BmlCaenReadoutConfigurationData: " << std::hex << std::endl;
	std::cout << "Crate Number Address: " << bmlCaenRoConfDataBlock.getCrateNumber() << std::dec << std::endl;
	std::cout << "Readout Period: " << bmlCaenRoConfDataBlock.getReadPeriod() << std::dec << std::endl;
	std::cout << "Enabled? " << bmlCaenRoConfDataBlock.isEnabled() << std::endl;
	std::cout << "Softtrigger? " <<  bmlCaenRoConfDataBlock.isSoftTrigger() << std::endl;
	std::cout << "Blt Readout? " <<  bmlCaenRoConfDataBlock.isBltReadout() << std::endl;
	std::cout << "Array Readout? " <<  bmlCaenRoConfDataBlock.isArrayReadout() << std::endl;
	std::cout << "Mode word (contains info above): " << std::hex << bmlCaenRoConfDataBlock.getMode() << std::dec << std::endl;
      }
    }



    const StringVec* strVec = evt->getCollectionNames() ;
    for( StringVec::const_iterator name = strVec->begin() ; name !=
	   strVec->end() ; name++){
      if(*name == _colAdcVals) {
        _adcCol = evt->getCollection(_colAdcVals) ;
        HandleADCCollections(evt);
      }

    }
}
  



void CaliceExampleProcessor::HandleADCCollections(LCEvent* evt) {

      int nadc_blocks =  _adcCol->getNumberOfElements() ; 

#ifdef HAVE_ROOT
      char bufnam[30] = "block xxx, channel xx\0";
      char bufi[30] = "blxxxchxx\0";
#endif

      for( int i=0 ; i<nadc_blocks  ; i++ ){
	AdcBlock adc_block(_adcCol->getElementAt(i));
	cout.flags ( ios_base::right | ios_base::dec | ios_base::showbase );
        //Example on how to get some more info on the ADC channel
	//cout << "Crate Number: " <<  adc_block.getCrateID() << endl;
	//cout << "Slot Number: " <<  adc_block.getSlotID() << endl;
	for (int kadc = 0; kadc < 12; kadc++) {
	  short adc_val = adc_block.getAdcVal(kadc);
	  int elec_channel = adc_block.getElecChannel(kadc);
	  //cout << "ADC-Channel[" << std::hex << elec_channel << "]: " << std::dec << adc_val << " ADC Counts" << endl;

#ifdef HAVE_ROOT
          // fill histo
	  /*_hall->Fill(adc_val);
      if(adc_val>20000 || adc_val<-4000) _hlimits->Fill(adc_val);
      if(adc_val>400 && adc_val<1400) _hpeak->Fill(adc_val);
      if(i>=_blockMin && i<=_blockMax) {
	int ihist = 1000*i + kadc;
	TH1F* h = _hmap.find( ihist )->second;
	if( h==NULL ) {
	
	  bufnam[6] = '0' + i/100;
	  bufnam[7] = '0' + i%100/10;
	  bufnam[8] = '0' + i%10;
	  bufnam[19] = '0' + kadc/10;
	  bufnam[20] = '0' + kadc%10;
	  bufi[2] = bufnam[6];
	  bufi[3] = bufnam[7];
	  bufi[4] = bufnam[8];
	  bufi[7] = bufnam[19];
	  bufi[8] = bufnam[20];

	  h = new TH1F( bufi, bufnam, 1500, -50, 1450);
	  _hmap.insert( pair<int,TH1F*>( ihist, h ) );
	}

	// fill channel histo
	h->Fill(adc_val);
      }
	  */
#endif


          /* Taken out until a valid cell mapping is available
	  try{  
	    //This might produce a lot of error messages in case the
            //cell map is not correct
	     int cellID = _cellMapPtr->find( elec_channel ).getCellID() ;  
	     if(evt->getEventNumber() < 10) cout << "Adc channel: " << std::hex << elec_channel << " " << "CellId: " << std::dec << cellID << endl; 
            long64 evttime = evt->getTimeStamp()*1LL; 
            //long64 testtime = 75690*1000000LL;
	    //if( evttime  > testtime ) cout << "Adc channel: " << std::hex << elec_channel << " " << "CellId: " << std::dec << cellID << endl; 

	  }
	  catch(Exception& e ){
	    //_chMap->print( std::cout ) ;
	    //std::cout << " Exception: " << e.what() << std::endl ;
	    }*/
	}
	//cout << "Transmission Status: " << adc_block.getTransmissionStatus() << endl;
      }

}


#endif

