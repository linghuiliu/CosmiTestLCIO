#ifdef USE_LCCD
#include <ModuleConnection.hh>
#include "TriggerHandlerCalice.hh"
#include <marlin/ConditionsProcessor.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

#include "collection_names.hh"
//Access class for CrcBeTrgConfigurationsData
//#include "BeTrgConfAccess.hh"
#include "BeTrgConf.hh"
//Access class for CrcBeTrgEventData
//#include "BeTrgEventAccess.hh"
#include "BeTrgEvent.hh"
//Interface to trigger check conditions objects
#include "TriggerCheck.hh"
//We need to check for the FE Configurations, too
#include "FeConfigurationBlock.hh"
//We need to check for s/w triggers in the Crc ReadOutConfigurations, too
#include "ReadOutConfigurationBlock.hh"
//Access class to trigger info contained in FEs
#include "FeTrgData.hh"
//Access to BeTrgPollData, mainly to check whether the
//Trigger Polling has generated a timeout => s/w trigger
#include "BeTrgPollData.hh"

#include "TrgReadoutConfigurationBlock.hh"

#include "to_binary_bitops.hh"

namespace CALICE {

#ifdef TRIGGER_HANDLER_IS_SINGLETON
//initialize data members which are static in this class
TriggerHandlerCalice* TriggerHandlerCalice::_thandler_instance = 0;
#endif

//TriggerHandlerCalice::TriggerDefinitionMap_t* TriggerHandlerCalice::_trdefMap = 0;

TriggerHandlerCalice::TriggerHandlerCalice() : _triggerAssignment(this, &TriggerHandlerCalice::CreateTriggerDefinitionMap),
                                               _triggerConfChange(this, &TriggerHandlerCalice::ExtractTriggerEnvironment),
                                               _triggerCheck(this, &TriggerHandlerCalice::ExtractTriggerCheckConditions),
					       _calibrationOn(false),
                                               _roConfChange(this, &TriggerHandlerCalice::readoutConfChange),
					       _roConfCol(0),
					       _trgroConfChange(this, &TriggerHandlerCalice::ExtractGenSoftTrigConfiguration),
					       _swTriggerOnInd(false),
					       _swTriggerOnGen(false),
                                               _main_word(0),
					       _word_up(0),
                                               _word_down(0),
                                               _principleTriggerMask(0),
                                               _confTriggerMask(0),
                                               _genericTriggerMask(0),
                                               _theTime(0),
                                               _isInitialized(false),
                                               _event_triggerBits(0),
                                               _conf_triggerBits(0),
                                               _andenable_triggerBits(0),
					       _triggerPollTimeout(false),
					       _nConfigurationChanges(0),
					       _colNameTriggerEvent( COL_FETRG ),
					       _errorCounter(0),
					       _noTriggerEvents(0),
					       _trgEventClass(kUnknown),
					       _verbose(true)
{
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kPedestalBit)]   =CALICE::TriggerBits::kPedestal;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kCrcOscillatorBit)]=CALICE::TriggerBits::kCrcOscillator;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kBeamBit)]       =CALICE::TriggerBits::kBeam;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kCosmicsBit)]    =CALICE::TriggerBits::kCosmics;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kGenericBit)]    =CALICE::TriggerBits::kGeneric;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSpillBit)]      =CALICE::TriggerBits::kSpill;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kCherenkovBit)]  =CALICE::TriggerBits::kCherenkov;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kVetoBit)]       =CALICE::TriggerBits::kVeto;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC1_3x3Bit)]    =CALICE::TriggerBits::kSC1_3x3;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC2_3x3Bit)]    =CALICE::TriggerBits::kSC2_3x3;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC1_10x10Bit)]  =CALICE::TriggerBits::kSC1_10x10;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC2_10x10Bit)]  =CALICE::TriggerBits::kSC2_10x10;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC1_100x100Bit)]=CALICE::TriggerBits::kSC1_100x100;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC2_100x100Bit)]=CALICE::TriggerBits::kSC2_100x100;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kExternalLEDBit)]=CALICE::TriggerBits::kExternalLED;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kFingerHorizontBit)]=CALICE::TriggerBits::kFingerHorizont;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kFingerVertBit)]=CALICE::TriggerBits::kFingerVert;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kVetoULBit)]=CALICE::TriggerBits::kVetoUL;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kVetoDRBit)]=CALICE::TriggerBits::kVetoDR;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kVetoURBit)]=CALICE::TriggerBits::kVetoUR;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kVetoDLBit)]=CALICE::TriggerBits::kVetoDL;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC1_20x20Bit)]  =CALICE::TriggerBits::kSC1_20x20;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kVeto_20x20Bit)]  =CALICE::TriggerBits::kVeto_20x20;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kCherenkov2Bit)]  =CALICE::TriggerBits::kCherenkov2;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC1_50x50Bit)]  =CALICE::TriggerBits::kSC1_50x50;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC2_50x50Bit)]  =CALICE::TriggerBits::kSC2_50x50;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kUTA1_19x19Bit)]  =CALICE::TriggerBits::kUTA1_19x19;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kUTA2_19x19Bit)]  =CALICE::TriggerBits::kUTA2_19x19;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kMuon_ExtBit)]  =CALICE::TriggerBits::kMuon_Ext;
  _knownTrigger[CALICE::TriggerBits::getName(CALICE::TriggerBits::kBeam_ExtBit)]  =CALICE::TriggerBits::kBeam_Ext;
}  


#ifdef TRIGGER_HANDLER_IS_SINGLETON
//create or return an instance of the trigger handler
TriggerHandlerCalice* TriggerHandlerCalice::getInstance() {
  if ( _thandler_instance == 0) _thandler_instance = new TriggerHandlerCalice();
  return _thandler_instance;
}
#endif

//#ifdef TRIGGER_HANDLER_IS_SINGLETON
// need to implement interface from abstract basis class
//initialization
void TriggerHandlerCalice::init() {

  lcio::StringVec temp_col_name_fe_conf;
  temp_col_name_fe_conf.push_back(COL_EMC_FE_CONF);
  temp_col_name_fe_conf.push_back(COL_AHC_FE_CONF);
  temp_col_name_fe_conf.push_back(COL_FE_CONF);

  lcio::StringVec temp_col_name_module_connection;
  temp_col_name_module_connection.push_back(string("EmcModuleConnection"));
  temp_col_name_module_connection.push_back(string("AhcModuleConnection"));

  init( string(COL_TRIGGER_ASSIGNMENT), 
	string(COL_TRIGGER_CHECK), 
	string(COL_TRIGGER_CONF), 
	string(COL_RO_CONF),
	string(COL_TRGRO_CONF),
	temp_col_name_fe_conf,
	temp_col_name_module_connection);
}
//#endif

//initialization (collection names from external source)
  void TriggerHandlerCalice::init(const string& col_trigger_assignment, 
				  const string& col_trigger_check, 
				  const string& colBeTrgConf,
				  const string& col_name_readout_conf,
				  const string& col_name_trgro_conf,
				  const lcio::StringVec& col_name_fe_conf, 
				  const lcio::StringVec& col_name_module_connection)
  {

  //get the handlers for the desired trigger conditions data 
  //these names have to be given in the MARLIN steering !!!
  //FIXME: need to throw exceptions

  //needs to be initalized only once
  if(!_isInitialized) {

    if (col_name_module_connection.size() != col_name_fe_conf.size() ) {
      throw std::runtime_error("TriggerHandlerCalice::init> For each front-end conditions data there must be defined exactly one module connection conditions data.");
    }

    std::stringstream message;
    message << "TriggerConfiguration::init> No conditions data handler registered handler to listen for ";

    // count errors upon handler registration
    unsigned int cond_error=0;
  
    if (!marlin::ConditionsProcessor::registerChangeListener(&_triggerAssignment, col_trigger_assignment) ) {
      message  << (cond_error++>0 ? "," : "") << col_trigger_assignment;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener(&_triggerConfChange, colBeTrgConf)) {
      message  << (cond_error++>0 ? "," : "") << colBeTrgConf;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener(&_triggerCheck, col_trigger_check)) {
      message  << (cond_error++>0 ? "," : "") << col_trigger_check;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener(&_roConfChange, col_name_readout_conf)) {
      message  << (cond_error++>0 ? "," : "") << col_name_readout_conf;
    }

    if(!marlin::ConditionsProcessor::registerChangeListener(&_trgroConfChange, col_name_trgro_conf)) {
      message  << (cond_error++>0 ? "," : "") << col_name_trgro_conf;
    }



    // to register the conditions change handlers
    // first all conditions change delegators need to be created.
    // Since they are stored in a vector, the address of the delegator objects
    // may change if the vector is extended. So first all delegators are created
    // then they are registered.

    unsigned int set_i=0;
    for (lcio::StringVec::const_iterator name_iter=col_name_fe_conf.begin();
	 name_iter!=col_name_fe_conf.end();
	 name_iter++, set_i++) {
      _feConfChange.push_back(MultipleConditionsChangeDelegator<TriggerHandlerCalice>(this,&TriggerHandlerCalice::feConfigurationChange,set_i));
    }

    set_i=0;
    for (lcio::StringVec::const_iterator name_iter=col_name_module_connection.begin();
	 name_iter!=col_name_module_connection.end();
	 name_iter++, set_i++) {
      _moduleConnectionChange.push_back(MultipleConditionsChangeDelegator<TriggerHandlerCalice>(this,&TriggerHandlerCalice::moduleConnectionChange,set_i));
    }

    _nFrontEnds.resize(col_name_module_connection.size());
    _swTriggerOnVec.resize(col_name_module_connection.size());
    _calibrationOnVec.resize(col_name_module_connection.size());
    _feConfCol.resize(col_name_module_connection.size());

    // listen for changes of front-end configurations
    {
      lcio::StringVec::const_iterator name_iter=col_name_fe_conf.begin();
      for (std::vector<CALICE::MultipleConditionsChangeDelegator<TriggerHandlerCalice> >::iterator change_delegator_iter=_feConfChange.begin();
	   change_delegator_iter!=_feConfChange.end();
	   change_delegator_iter++, name_iter++) {

	assert( name_iter != col_name_fe_conf.end() );

	if (!marlin::ConditionsProcessor::registerChangeListener(&(*change_delegator_iter), *name_iter)) {
	  message  << (cond_error++>0 ? "," : "") << *name_iter;
	}
      }
    }
    
    // listen for changes of module connection changes
    {
      lcio::StringVec::const_iterator name_iter=col_name_module_connection.begin();
      for (std::vector<CALICE::MultipleConditionsChangeDelegator<TriggerHandlerCalice> >::iterator change_delegator_iter=_moduleConnectionChange.begin();
	   change_delegator_iter!=_moduleConnectionChange.end();
	   change_delegator_iter++, name_iter++) {

	assert( name_iter != col_name_module_connection.end() );

	if (!marlin::ConditionsProcessor::registerChangeListener(&(*change_delegator_iter), *name_iter)) {
	  message  << (cond_error++>0 ? "," : "") << *name_iter;
	}
      }
    }


    if (cond_error>0) {
      message << ".";
      //CRP Temporary fix to to avoid memery leak caused by LCCD or CondDBMySQL
      //which appears if a listener is registered but no data are found
      //On the other hand the trigger handler did throw a runtime error
      //if there is a missing changelistener but if a detector missing
      //some record types did not appear in the datastream 
      //or no cd data saet was entered into the db for that period
      //throw std::runtime_error(message.str());
      std::cout << "Warning: " << message.str() << std::endl;
    }


    _isInitialized = true;
  }

}
  
  void TriggerHandlerCalice::setTriggerEventClass() 
  {
    if (_colNameTriggerEvent == COL_TRIGGER_EVENT) {
      _trgEventClass=kBeTrgEvent;
    }
    else if (_colNameTriggerEvent == COL_FETRG) {
      _trgEventClass=kFeTrgEvent;
    }
    else {
      _trgEventClass=kUnknown;
    }
  }



//adding the trigger information to the event
  void TriggerHandlerCalice::setTrigger(LCEvent* evt) {

    if (_trdefMap.empty()) {
   cout << "Event Number: " << evt->getEventNumber() << endl;   
   LCTime EventTime(evt->getTimeStamp());
   cout << "Event Time: " << EventTime.getDateString() << endl;   
   throw std::runtime_error("TriggerHandlerCalice::SetTrigger - No Trigger Definition found. Will leave program now!!!");
   }

#ifdef TRIGGER_HANDLER_IS_SINGLETON
    //check whether the init method has already been called 
    //if not do it
    if(!_isInitialized) init();
#endif
        
    PrepareTriggerEventData(evt);
    


#ifdef RECO_DEBUG
    std::cout << "Last Conf Change: " << _lastConfigurationChange << std::endl; 
    std::cout << "Conf Change Id: " << _nConfigurationChanges << std::endl;       
#endif
    
    
    //Set the "configured" trigger 
    _lastConfigurationChange = _nConfigurationChanges;


    _event_triggerBits.reset();

    // also check the trigger history in case of poll timeouts

#ifdef RECO_DEBUG
      cout << "Size of Fifo: " << getUsedFifoDepth() << std::endl;
#endif
    if (getUsedFifoDepth() < (unsigned) (_main_word + 1) ) {
#ifdef RECO_DEBUG
      cout << "Runnumber: " << evt->getRunNumber() << " Eventnumber: " << evt->getEventNumber() << " Time: " << _theTime.getDateString() << endl;
      cout << "Number of words in fifo too small to extract Main Word" << endl;
      cout << "Size of Fifo: " << getUsedFifoDepth() << std::endl;
      cout << "Main Word at: " << _main_word << std::endl;
#endif
    }

  }

  // search in the fifo for a word which for which the configured mask is on.
  // The search starts at the main word. It is performed in first 
  // in increasing time direction, then in decreasing time direction.

  unsigned int TriggerHandlerCalice::searchTriggerMainWord()
  {
      
    unsigned int actualMask =  _genericTriggerMask | _confTriggerMask;
#ifdef RECO_DEBUG
    std::cout << "actual mask: " << actualMask << std::endl;    
    std::cout << "Default main word:" << _main_word << std::endl;
#endif
    // \todo throw an exception ?
    if (actualMask == 0 ) return _main_word;
    
    unsigned int used_fifo_depth=getUsedFifoDepth();
    if (used_fifo_depth<2) return 0;
    
    unsigned int this_trigger_main_word  = used_fifo_depth+1;
    unsigned int this_trigger_off  = used_fifo_depth+1;


    unsigned int main_word_value=0;
    
    //search trigger main word
    unsigned int last_trigger_word=_main_word+abs(_word_up)+1;
   if (last_trigger_word>used_fifo_depth) {
      last_trigger_word=used_fifo_depth;
    }
    
    unsigned int first_trigger_word=0;
    if (abs(_word_down)<_main_word) {
      first_trigger_word=_main_word-abs(_word_down);
    }
    
    // search up 
    for (unsigned int iword=_main_word ; iword<last_trigger_word; iword++) {
      
      // allow for a jitter in one direction
      // \todo also in the other ?
      unsigned int testWord = static_cast<unsigned int>(_theTriggerFifoCont.at( iword )) | static_cast<unsigned int>(_theTriggerFifoCont.at( iword + 1 ));
      
      // \todo allow for the generic bit ?
      if ( (testWord & actualMask)  && (testWord & _genericTriggerMask )) {
	if ( ( _theTriggerFifoCont.at( iword) & actualMask) == 0 ) {
	  //std::cout << "iword In trigger search: " << iword << std::endl;
    	  iword++;
	}
	this_trigger_main_word=iword;
	this_trigger_off=iword;
	main_word_value = testWord;
	
	// now find positions where it goes off ;
	for (unsigned int jword=this_trigger_main_word+1; jword < last_trigger_word; jword++) {
	  
	  testWord = _theTriggerFifoCont.at( jword ) | _theTriggerFifoCont.at( jword + 1 );
	  // testWord should have still the bits enabled of the enabled trigger inputs which were enabled at the firs location
	  if ( !(testWord & main_word_value & actualMask ) || !(testWord & _genericTriggerMask )) break;
	  main_word_value |= testWord;
	  this_trigger_off=jword;
	}
	
	if (this_trigger_main_word>0) {
	  
	  // now find positions where it goes on ;
	  unsigned int start_down_search=this_trigger_main_word;
	  if (start_down_search>0) start_down_search--;
	  for (unsigned int jword=start_down_search; jword-->0; ) {
	    
	    testWord = _theTriggerFifoCont.at( jword ) | _theTriggerFifoCont.at( jword + 1 );
	    if ( !(testWord & main_word_value & actualMask ) || !(testWord & _genericTriggerMask )) break;
	    main_word_value |= testWord;
	    this_trigger_main_word = jword+1;
	  }
	  
	}
	
	break;
      }
    }
    
    if (this_trigger_main_word>=last_trigger_word) {
      
      unsigned int first_trigger_word=0;
      if (abs(_word_down)<_main_word) {
	first_trigger_word=_main_word-abs(_word_down);
      }
      
      // search down
      for (unsigned int iword=_main_word ; iword-->first_trigger_word; ) {
	
	// allow for a jitter in one direction
	// \todo also in the other ?
	unsigned int testWord = static_cast<unsigned int>(_theTriggerFifoCont.at( iword )) | static_cast<unsigned int>(_theTriggerFifoCont.at( iword + 1 ));
	
	// \todo allow for or ? 
	if ( (testWord & actualMask)  && (testWord & _genericTriggerMask )) {
	  this_trigger_main_word=iword;
	  if ( ( _theTriggerFifoCont.at( iword) &  actualMask) == 0) {
	    iword++;
	  }
	  this_trigger_off=iword;
	  main_word_value = testWord;
	  
	  // now find positions where it goes off ;
	  for (unsigned int jword=this_trigger_main_word+1; jword < last_trigger_word; jword++) {
	    
	    testWord = _theTriggerFifoCont.at( jword ) | _theTriggerFifoCont.at( jword + 1 );
	    if ( !(testWord & main_word_value & actualMask ) || !(testWord & _genericTriggerMask )) break;
	    main_word_value |= testWord;
	    this_trigger_off=jword;
	  }
	  
	  if (this_trigger_main_word>0) {
	    
	    // now find positions where it goes on ;
	    unsigned int start_down_search=this_trigger_main_word;
	    if (start_down_search>0) start_down_search--;
	    for (unsigned int jword=start_down_search; jword-->0; ) {
	      
	      testWord = _theTriggerFifoCont.at( jword ) | _theTriggerFifoCont.at( jword + 1 );
	      if ( !(testWord & main_word_value & actualMask ) || !(testWord & _genericTriggerMask )) break;
	      main_word_value |= testWord;
	      this_trigger_main_word = jword+1;
	    }
	    
	  }
	  
	  break;
	}
	
      }
    }

    
#ifdef RECO_DEBUG
    std::cout << "Found Main Word: " << this_trigger_main_word << std::endl;
#endif
    if(this_trigger_main_word ==_theTriggerFifoCont.size()-1 || this_trigger_main_word == 0) {
      _this_trigger_main_word = -1;
      _this_trigger_length = 0;   
    } else {
      _this_trigger_main_word=static_cast<int>(this_trigger_main_word);
      _this_trigger_length=this_trigger_main_word-this_trigger_off;
    }
    
    
#ifdef RECO_DEBUG
    std::cout << "The main word: " <<  _this_trigger_main_word << std::endl;
    int icount(0);
    if(_theTriggerFifoCont.size() > 0) {
      for(IntVec::iterator vec_iter = _theTriggerFifoCont.begin();vec_iter != _theTriggerFifoCont.end(); vec_iter++){
        std::cout << "Fifo Element at " <<  icount << " is: " << "in hex: " << std::hex << *vec_iter << std::dec << " in ";
        to_binary_bitops(*vec_iter);  
        icount++;
      }
    }
#endif
    return _this_trigger_main_word;
    
    
  }


//the validation check
bool TriggerHandlerCalice::isValidTrigger(const LCEvent* evt) {

#ifdef TRIGGER_HANDLER_IS_SINGLETON
   //check whether the init method has already been called 
   //if not do it
   if(!_isInitialized) init();
#endif

   std::cout << "Warning - TriggerHandlerCalice isValidtrigger: " << std:: endl;
   std::cout << "The Trigger Check will maybe re-implemented in future versions. " << std::endl;
   std::cout << "This method will always return true" << std::endl;

   bool isValidTrigger = true;


   bool isImplemented=false;
   assert ( isImplemented );
   
   return isValidTrigger;
}


  void TriggerHandlerCalice::searchTriggerHistory() {

  // \todo need to define the first trigger word;
  unsigned int first_trigger_word=0;
  //  if (_this_trigger_main_word > _word_down) {
  //    first_trigger_word = _word_down;
  //  }

  _distanceToPreTrigger.clear();
  _distanceToPostTrigger.clear();
  unsigned int dirty_triggerBits(0); 
  _outOfRange_triggerBits = 0;
  

  //define the maximal pulslength a bit could have been on
  //FIXME this has to become a steering parameter, in the extreme case
  //separate for each known trigger 
  unsigned int pulselength(2); 


  if (static_cast<unsigned int>(_this_trigger_main_word)>=getUsedFifoDepth()) {
    _outOfRange_triggerBits = getPrincipleTriggerMask();
    return;
  }

  unsigned int start_trigger_word=static_cast<unsigned int>(_this_trigger_main_word);
  unsigned int future_search_start_trigger_word=static_cast<unsigned int>(_this_trigger_main_word);

  unsigned int event_trigger_bits=0;
  if (_this_trigger_main_word>255) {
    _outOfRange_triggerBits=0xffffff;
    _event_triggerBits=0;
    //return _distanceToPreTrigger;
    return;
  }
  unsigned int out_of_range_trigger_bits =0;

  // find fifo word of the slowest "trigger" which should corresponds to the main word.
  for(TriggerDefinitionMap_t::const_iterator trdef_iter = _trdefMap.begin(); 
      trdef_iter != _trdefMap.end(); 
      ++trdef_iter) {

    unsigned int a_start_trigger_word;
    if ( trdef_iter->first.delay() < 0 && static_cast<unsigned int>(-trdef_iter->first.delay()) > static_cast<unsigned int>(_this_trigger_main_word) )  {
      a_start_trigger_word=0;
      out_of_range_trigger_bits |= trdef_iter->second.bitMask();
    }
    else {
      a_start_trigger_word= static_cast<unsigned int>(_this_trigger_main_word) + trdef_iter->first.delay();
    }

    if (a_start_trigger_word>=static_cast<unsigned int>(trdef_iter->first.jitter())) a_start_trigger_word -= trdef_iter->first.jitter();
    else a_start_trigger_word=0;

    if ( a_start_trigger_word >= getUsedFifoDepth()) {
	start_trigger_word=getUsedFifoDepth();
	// \todo issue error that expected trigger position outside fifo size?
	//	std::cout << "WARNING: Expected trigger  position of " << trdef_iter->second.name() 
	//		  << " is outside fifo depth ." << std::endl;
	out_of_range_trigger_bits |= trdef_iter->second.bitMask();
    }
    else {
      
      unsigned int a_last_word = a_start_trigger_word+2*trdef_iter->first.jitter()+1;
      if (a_last_word > getUsedFifoDepth()) a_last_word = getUsedFifoDepth();
      
      // check the fifo around the expected position whether the bit was on or off.
      for (unsigned int jword=a_start_trigger_word; jword< a_last_word ; jword++) {
      // value should be the trigger bits corresponding to the trigger;
       if ( (_theTriggerFifoCont[jword] & trdef_iter->first.mask()) ) {
	  event_trigger_bits |= trdef_iter->second.bitMask();
	}
      }

      if (a_start_trigger_word > start_trigger_word) {
	start_trigger_word= a_start_trigger_word;
      }
      if (a_last_word < future_search_start_trigger_word) {
	future_search_start_trigger_word= a_last_word;
      }
      
    }


  }

  _outOfRange_triggerBits = out_of_range_trigger_bits;
  _event_triggerBits=event_trigger_bits;

  // do not distinguish sub detector types:
  if(isGenSWTriggerOn() || isSWTriggerOn() || _triggerPollTimeout) {
    unsigned int smeared_trigger_word=getSmearedTriggerMainWord();
    // only assume that it is a software trigger if non of the enabled inputs has fired.
    if ( (smeared_trigger_word & getTrgConfigurationMask() & ~_genericTriggerMask) == 0) {
      _event_triggerBits.setPedestalTrigger();
    }
    else {
      std::cout << "Software trigger but conf=" << std::hex << getTrgConfigurationMask() << " and event=" << smeared_trigger_word << std::dec << std::endl;
    }
  }
  
  // \todo calibration pulse will be only there in a short time window 
  // so if the trigger main word is outside this window, there is probably
  // no calibration pulse.
  
  // btw. calib is not really a trigger
  // and it is switch on by every trigger also beam triggers 
  if(isCalibrationOn()) {
    _event_triggerBits.setCalibTrigger();
  }
  
  
  if (start_trigger_word< getUsedFifoDepth() ) {
    // now get the _distanceToPreTrigger (post history) of all configured trigger bits to the main word
    unsigned int new_trigger_mask=~event_trigger_bits;
    unsigned foundFirstOff(0);
    //bool updatetriggermask(false);
    for (unsigned int iword=start_trigger_word; iword-- > first_trigger_word; ) {
      
      unsigned int this_triggers=0;
      if ( _theTriggerFifoCont[iword] & _principleTriggerMask ) {
	
	for(TriggerDefinitionMap_t::const_iterator  trdef_iter = _trdefMap.begin(); 
	    trdef_iter != _trdefMap.end(); 
	    ++trdef_iter) {
	  
	  // the trigger is delayed but it may jittering to a shorter delay:
	  //  iword < _this_trigger_main_word  + trdef_iter->first.delay() - trdef_iter->first.jitter()
	  // To avoid a subtraction of unsigned :
	  if ( iword +  trdef_iter->first.jitter() < static_cast<unsigned int>(_this_trigger_main_word)  + trdef_iter->first.delay() ) {
	    //updatetriggermask=true;
	    // build mask of all triggers which are on to mask them when looking at the next word.
	    this_triggers |= _theTriggerFifoCont[iword] &  trdef_iter->first.mask();

	    //In the next check only trigger bits 'survive' the 'if statement'
	    //which were set in the mainword 
	    if(!(foundFirstOff & trdef_iter->first.mask()) && (event_trigger_bits & trdef_iter->second.bitMask())) {
	      //if(evt->getEventNumber() > 1000 &&
	      // trdef_iter->first.mask() == 0x8000) {
	      //std::cout << std::hex << "First cond: " << (_theTriggerFifoCont[iword] & trdef_iter->first.mask()) << std::endl; 
	      //}
	      //If the bit is still on continue to the next trigger bit
	      if((_theTriggerFifoCont[iword] & trdef_iter->first.mask())) {
                //This means that it has ever been on during our check
                //So we declare this bit to be dirty
		if (iword == first_trigger_word) dirty_triggerBits |= trdef_iter->second.bitMask();
		continue;} else {
		  //ok this trigger was on in the main word but is now off 
		  //Check how often is has been on from iword+1 until
		  //the delay corrected main word taking the
		  //pulselength into account
		  unsigned int ion(0);
		  unsigned int iowords[pulselength];
		  //Attention need to sneak one into the post history
		  for(unsigned int icheck = iword+1; icheck < static_cast<unsigned int>(_this_trigger_main_word)+trdef_iter->first.delay()+pulselength; icheck++){
		    //count how often it was on
		    if(_theTriggerFifoCont[icheck] & trdef_iter->first.mask()) ion++;
		    if(ion-1 < pulselength) iowords[ion-1] = icheck; 
		    
		  }
		  //The bit was on more often than its pulslength
		  //This is already an indicator that something is wrong
		  if (ion > pulselength)  dirty_triggerBits |= trdef_iter->second.bitMask();
		  else { 
		    
		    //This is a bug (or a very strange situation) and has to be declared like that
                    //Well, this is a bug if we would with relative delays
		    /*if(ion == 0) { 
		      std::stringstream message;
		      message << "TriggerHandlerCalice::searchTriggerHistory> ";              
		      message << "PreTriggerHistorySearch doesn't find trigger Bit "; 
		      message << trdef_iter->second.name() ;
		      message << " although present in the main word???" << std::endl;
#ifdef RECO_DEBUG
		      std::cout << "main word = " << _this_trigger_main_word << "  event= " << _event_triggerBits << " conf=" << _conf_triggerBits << std::endl;
		      int icount(0); 
		      for(IntVec::iterator vec_iter = _theTriggerFifoCont.begin();vec_iter != _theTriggerFifoCont.end(); vec_iter++){
			std::cout << "Fifo Element at " <<  icount << " is: " << "in hex: " << std::hex << *vec_iter << std::dec << " in ";
			to_binary_bitops(*vec_iter);  
			
			icount++;
		      }
#endif
		      throw std::runtime_error(message.str());
		      }*/ 
		    //if its smaller or equal the pulslength
		    //check the distance between the words
		    //FIXME; the situation gets more complicated if the  
		    //pulslength is larger then two, in that case we'd
		    //need to check for consecutive pulses
		    if(ion > 0) {if( static_cast<unsigned
				     int>(abs(static_cast<int>(iowords[0]-iowords[ion-1])))
				     > pulselength)  dirty_triggerBits |=
						       trdef_iter->second.bitMask();}
		  }          
		  
		  
		}
	      //Here we have identify a trigger that is off for the
	      //first time after the main word while present in the main word
	      foundFirstOff |= trdef_iter->first.mask();  
	      //Prevent further testing if this trigger bit is off for the first
	      //time after the main word
	      continue;
	    } //end of check for bits in the main word
	    
	    //if(evt->getEventNumber() > 1000 && trdef_iter->first.mask()
	    //== 0x8000) std::cout << "Spill trigger???" << iword << std::endl;

	    if ( ( _theTriggerFifoCont[iword] & new_trigger_mask) &  trdef_iter->first.mask()) {
	      
	      // ignore the 
	      int a_distance = _this_trigger_main_word - (iword - trdef_iter->first.delay() );
	      if (a_distance>1 ) { 
		
		std::map<unsigned int,unsigned int>::iterator the_distance=_distanceToPreTrigger.find(a_distance);
		if (the_distance == _distanceToPreTrigger.end()) {
		  _distanceToPreTrigger[a_distance] = trdef_iter->second.bitMask();
		}
		else {
		  the_distance->second |= trdef_iter->second.bitMask();
		}
	      }
	    }
	  }
	}
      }
      // mask all trigger which are on
      //if(updatetriggermask) new_trigger_mask = ~this_triggers;
      new_trigger_mask = ~this_triggers;
      
    }
  }

  //By construction the smallest distance filled into the distanceTo... is 2
  //so we use distance=1 for the results on bits which were on in the
  //main word  
  if(dirty_triggerBits) _distanceToPreTrigger[1] = dirty_triggerBits;
  dirty_triggerBits=0;

  //_distanceToPostTrigger.clear();

  if ( future_search_start_trigger_word > 0 && future_search_start_trigger_word < getUsedFifoDepth() ) {

  // now search for triggers which occured in the "future".

  unsigned int new_trigger_mask=~event_trigger_bits;
  //an unsigned in which we store whether we have found the first off for
  //this trigger
  unsigned foundFirstOff(0);
  for (unsigned int iword=future_search_start_trigger_word; iword < getUsedFifoDepth(); iword++ ) {


    unsigned int this_triggers=0;
    if ( _theTriggerFifoCont[iword] & _principleTriggerMask ) {

      for(TriggerDefinitionMap_t::const_iterator  trdef_iter = _trdefMap.begin(); 
	  trdef_iter != _trdefMap.end(); 
	  ++trdef_iter) {

	// the trigger is delayed but it may jitter to a smaller delay:
	//  iword < _this_trigger_main_word  + trdef_iter->first.delay() + trdef_iter->first.jitter()
	if ( iword  > static_cast<unsigned int>(_this_trigger_main_word)  + trdef_iter->first.delay() +  trdef_iter->first.jitter() ) {

	  this_triggers |= _theTriggerFifoCont[iword] &  trdef_iter->first.mask();

          //In the next check only trigger bits 'survive' the 'if statement'
          //which were set in the mainword 
          if(!(foundFirstOff & trdef_iter->first.mask()) && (event_trigger_bits & trdef_iter->second.bitMask())) {
	    //if(evt->getEventNumber() > 1000 &&
	    //   trdef_iter->first.mask() == 0x8000) {
	    //  std::cout << std::hex << "First cond: " << (_theTriggerFifoCont[iword] & trdef_iter->first.mask()) << std::endl; 
	    //}
	    //If the bit is still on continue to the next trigger bit
	    if((_theTriggerFifoCont[iword] & trdef_iter->first.mask())) {
                //This means that it has ever been on during our check
                //So we declare this bit to be dirty
	      if(iword==getUsedFifoDepth()-1) dirty_triggerBits |= trdef_iter->second.bitMask();
	      continue;} else {
		//ok this trigger was on in the main word but is now off 
		//Check how often is has been on from iword-1 until
		//the delay corrected main word taking the
		//pulselength into account
		unsigned int ion(0);
		unsigned int iowords[pulselength];
		//Here we need to check only up to the (delay corrected)
                //main word since part of the range up to (corr.)
                //mainword-pulslength is already covered by preHistorySearch
		for(unsigned int icheck = iword-1; icheck > static_cast<unsigned int>(_this_trigger_main_word)
		      + trdef_iter->first.delay()-pulselength+1; icheck--){
		  //count how often it was on
		  if(_theTriggerFifoCont[icheck] & trdef_iter->first.mask()) ion++;
		  if(ion-1 < pulselength) iowords[ion-1] = icheck; 
		  
		}
		//The bit was on more often than its pulslength
		//This is already an indicator that something is wrong
		if (ion > pulselength)  dirty_triggerBits |= trdef_iter->second.bitMask();
		else { 
		  
		  //This is a bug (or a very strange situation) and has to be declared like that
                  //Well, this is a bug if we would worl with realtive delays
		  /*if(ion == 0) { 
		    std::stringstream message;
		    message << "TriggerHandlerCalice::searchTriggerHistory> ";              
		    message << "PostTriggerHistorySearch doesn't find trigger Bit "; 
		    message << trdef_iter->second.name() ;
		    message << " although present in the main word???" << std::endl;
#ifdef RECO_DEBUG
		    std::cout << "main word = " << _this_trigger_main_word << "  event= " << _event_triggerBits << " conf=" << _conf_triggerBits << std::endl;
		    int icount(0); 
		    for(IntVec::iterator vec_iter = _theTriggerFifoCont.begin();vec_iter != _theTriggerFifoCont.end(); vec_iter++){
		      std::cout << "Fifo Element at " <<  icount << " is: " << "in hex: " << std::hex << *vec_iter << std::dec << " in ";
		      to_binary_bitops(*vec_iter);  
		      
		      icount++;
		    }
#endif
		    //throw std::runtime_error(message.str());
		    }*/ 
		  //if its smaller or equal the pulslength
		  //check the distance between the words
		  //FIXME; the situation gets more complicated if the  
		  //pulslength is larger then two, in that case we'd
		  //need to check for consecutive pulses
		  if( ion > 0) {if( static_cast<unsigned
				    int>(abs(static_cast<int>(iowords[0]-iowords[ion-1])))
				    > pulselength)  dirty_triggerBits |=
						      trdef_iter->second.bitMask();}
		}          
		
		
	      }
	    //Here we have identify a trigger that is off for the
	    //first time after the main word while present in the main word
	    foundFirstOff |= trdef_iter->first.mask();  
	    //Prevent further testing if this trigger bit is off for the first
	    //time after the main word
	    continue;
	  } //end of check for bits in the main word



	  if ( ( _theTriggerFifoCont[iword] & new_trigger_mask) &  trdef_iter->first.mask()) {

	    unsigned int a_distance = (iword - trdef_iter->first.delay() ) - _this_trigger_main_word;
	    if (a_distance>1) { 

	      std::map<unsigned int,unsigned int>::iterator the_distance=_distanceToPostTrigger.find(a_distance);
	      if (the_distance == _distanceToPostTrigger.end()) {
		_distanceToPostTrigger[a_distance] = trdef_iter->second.bitMask();
	      }
	      else {
		the_distance->second |= trdef_iter->second.bitMask();
	      }
	    }
	  }
	}
      }
    }
    new_trigger_mask = ~this_triggers;
  }
  }

  //By construction the smallest distance filled into the distanceTo... is 2
  //so we use distance=1 for the results on bits which were on in the
  //main word  
  if(dirty_triggerBits) _distanceToPostTrigger[1] = dirty_triggerBits;


#ifdef RECO_DEBUG
  std::cout << "main word = " << _this_trigger_main_word << "  event= " << _event_triggerBits << " conf=" << _conf_triggerBits << std::endl;


  for (unsigned int iword=0; iword < _theTriggerFifoCont.size(); iword++ ) {

    unsigned int bits=0;
    unsigned int value=_theTriggerFifoCont[iword];
    for(TriggerDefinitionMap_t::const_iterator  trdef_iter = _trdefMap.begin(); 
	trdef_iter != _trdefMap.end(); 
	++trdef_iter) {
      if (value & trdef_iter->first.mask()) {
	bits |= trdef_iter->second.bitMask();
      }
      
    }
    if (bits ) {
      std::cout << setw(3) << iword << ":" << CALICE::TriggerBits(bits) << std::endl;
    }
  }


  std::cout << "pre trigger:" << std::endl;
  for (std::map<unsigned int, unsigned int>::const_iterator hist_iter=_distanceToPreTrigger.begin();
       hist_iter!=_distanceToPreTrigger.end();
       hist_iter++) {
    std::cout << setw(3) << static_cast<int>(hist_iter->first) << " " << TriggerBits(hist_iter->second) << std::endl;
  }
  std::cout << "post trigger:" << std::endl;
  for (std::map<unsigned int, unsigned int>::const_iterator hist_iter=_distanceToPostTrigger.begin();
       hist_iter!=_distanceToPostTrigger.end();
       hist_iter++) {
    std::cout << setw(3) << static_cast<int>(hist_iter->first) << " " << TriggerBits(hist_iter->second) << std::endl;
  }
  std::cout << endl;

#endif  
  //return _distanceToPreTrigger;
  }

void TriggerHandlerCalice::printTriggerDefinitions(const LCEvent* evt) {

#ifdef TRIGGER_HANDLER_IS_SINGLETON
   //check whether the init method has already been called 
   //if not do it
   if(!_isInitialized) init();
#endif

   LCTime theTime(evt->getTimeStamp());

   //print the trigger map
   
   TriggerDefinitionMap_t::iterator trdef_iter;

   std::cout << "******* Trigger Definition for Run: " << evt->getRunNumber() << " and Event: " << evt->getEventNumber() << " *********" << std::endl;   
   std::cout << "******* at Time: " << theTime.getDateString() << " *********" << std::endl;      
   //std::cout << "TriggerType         BitPosition" << std::endl; 
   std::cout << "TriggerType         TriggerMask     TriggerDelay    TriggerJitter" << std::endl; 
   std::cout << "-----------         -----------     ------------    -------------" << std::endl; 
   for(trdef_iter = _trdefMap.begin(); trdef_iter != _trdefMap.end(); ++trdef_iter) {
     std::cout << trdef_iter->second.name() << std::setw(20) << std::hex
	       << trdef_iter->first.mask() << std::setw(20) 
     //	       << std::endl;
     
     // knwon trigger definitions which will appear in the event header
     //if (trdef_iter->second.bitMask()!=0) {
     //  std::cout << " * ";
       //}
     // if standard trigger definition
     //     for (UInt_t type_i=0; type_i<kNTriggerTypes; type_i++) {
     //       if (trdef_iter->second.name() == _triggerNames[type_i]) {
     //	 std::cout << " * ";
     //       }
     //     }

     << trdef_iter->first.delay() << std::setw(20) << trdef_iter->first.jitter() << std::endl; 

   }
   
   std::cout << "Principle Trigger Mask: " << std::hex << _principleTriggerMask << std::dec << std::endl;

   std::cout << "Generic Trigger Mask: " << std::hex << _genericTriggerMask << std::dec << std::endl;
  std::cout << "Configuration Mask: " << std::hex << _confTriggerMask << std::dec << std::endl;


#ifdef BOUNDARY_CHECK
  assert ( _nFrontEnds.size() == _swTriggerOnVec.size() );
#endif

  std::cout << "Individual Software Trigger: ";
  unsigned int sub_det_i=0;
  for (std::vector< unsigned int >::const_iterator sub_det_iter=_swTriggerOnVec.begin();
       sub_det_iter != _swTriggerOnVec.end();
       sub_det_iter++, sub_det_i++) {
    std::cout << sub_det_i << ":";
    if ( *sub_det_iter== _nFrontEnds[sub_det_i] ) {
      std::cout << "on ";
    }
    else {
      if ( *sub_det_iter==0 ) {
	std::cout << "off ";
      }
      else  {
	std::cout << "on=" << *sub_det_iter << "/" << _nFrontEnds[sub_det_i] << " ";
      }
    }
  }

  
  std::cout << " -> " << ( _swTriggerOnInd   ? "on " : "off ")  << std::endl;


  std::cout << "General Software Trigger: " << ( _swTriggerOnGen   ? "on " : "off ")  << std::endl; 

   std::cout << "Calibration : ";
  sub_det_i=0;
  for (std::vector<bool>::const_iterator sub_det_iter=_calibrationOnVec.begin();
       sub_det_iter != _calibrationOnVec.end();
       sub_det_iter++, sub_det_i++) {
    std::cout << sub_det_i << ":" << ( *sub_det_iter   ? "on " : "off ");
  }
  std::cout << "  -> " << ( _calibrationOn   ? "on " : "off ")  << std::endl;


  std::cout << "A software trigger will be regarded as a PEDESTAL Trigger and superseeds the definitions above!!! " << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;


}

void TriggerHandlerCalice::TriggerConditionsListener(){

  //Do the check only in case there is a trigger definitions map and 
  //after the trigger configuration has been retrieved from the db
  if ( !_trdefMap.empty() && _nConfigurationChanges > 0) {


    if ( (_principleTriggerMask & _confTriggerMask) != _confTriggerMask ) {   
      std::stringstream message;
      message << "TriggerHandlerCalice TriggerConditions Listener: Trigger Configuration Requires non defined Trigger inputs:";
      unsigned int mask=1;
      //Get undefined bits
      unsigned int undefined = _confTriggerMask & (~_principleTriggerMask);
      for (unsigned int bit_i=0; bit_i<32; bit_i++) {
	if(undefined & mask) {
	  message << bit_i << " ";
	}
	mask <<= 1;
      }
      std::cout << "Error Occured for Trigger Configuration No: " << _nConfigurationChanges  << std::endl;
      throw std::runtime_error ( message.str() ); 
    }
  }
  
}

void TriggerHandlerCalice::CreateTriggerDefinitionMap(EVENT::LCCollection *TrAssCol) {

  bool isObsoleteFormat(false);
  _trdefMap.clear();


  /*Get Trigger Types, delays and jitters from the collection
   The method checks whether an oblsolete format is accidentally accessed
   and raise a warning if so.
   This check you will find all over the place
  */

  if (!TrAssCol) throw std::runtime_error("TriggerHandlerCalice::CreateTriggerDefinitionMap - No Trigger Definition found. Will leave program now!!!");

  StringVec trigger_types;
  TrAssCol->getParameters().getStringVals(PAR_TRIGGER_TYPE_NAMES, trigger_types);
  //Check for the trigger types parameters under a different name
  //This parameter has been stored under a different name in older
  //Versions of the db!!!
  if(trigger_types.size() == 0) TrAssCol->getParameters().getStringVals("Trigger_Types", trigger_types);
  if(trigger_types.size() == 0) throw std::runtime_error("No Trigger Types defined. Will leave program now !!!");

  IntVec trigger_mask;
  IntVec trigger_delay;
  IntVec trigger_jitter;

  //Used to check for old/obsolete format
  IntVec trigger_value;
  IntVec trigger_position;


  TrAssCol->getParameters().getIntVals(PAR_TRIGGER_BITS, trigger_position);
  TrAssCol->getParameters().getIntVals(PAR_TRIGGER_VALUE, trigger_value);

  if(trigger_position.size() > 0 || trigger_value.size() > 0) {
    std::cout << "You have an obsolete version of the TriggerDefinition at hand" << std::endl;
    std::cout << "This version of the TriggerHandler needs: " << std::endl;  
    std::cout << "TriggerType TriggerMask TriggerDelay TriggerJitter" << std::endl;
    std::cout << "The TriggerAssignment at hand contains: " << std::endl;
    std::cout << "TriggerType TriggerMask(Position) TriggerValue " << std::endl;
    std::cout << "Will set defaults for Delay=0 and Jitter=1" << std::endl;
    isObsoleteFormat=true;
#ifdef RECO_DEBUG
  cout << "Trigger Position size: " << trigger_position.size() << std::endl;
  cout << "Trigger Types size: " << trigger_types.size() << std::endl;
#endif
   }


  TrAssCol->getParameters().getIntVals(PAR_TRIGGER_MASK, trigger_mask);
  TrAssCol->getParameters().getIntVals(PAR_TRIGGER_DELAY, trigger_delay);
  TrAssCol->getParameters().getIntVals(PAR_TRIGGER_JITTER, trigger_jitter);

  //This is really crazy and should never !!! happen
  if(isObsoleteFormat && ( trigger_delay.size() > 0 || trigger_jitter.size() > 0))
    {
    std::stringstream message;
    message << "The conditions data for the trigger assignment are utterly wrong." << std::endl
            << "Current Format and obsolete format mixed Will leave program now !!!"
	    << "Trigger Types size: " << trigger_types.size() << std::endl
	    << "Trigger Mask size: " << trigger_mask.size() << std::endl
	    << "Trigger Delay size: " << trigger_delay.size() << std::endl
	    << "Trigger jitter size: " << trigger_jitter.size() << std::endl
	    << "Trigger Position size: " << trigger_position.size() << std::endl
	    << "Trigger Value size: " << trigger_value.size() << std::endl;
		 throw std::runtime_error(message.str());
    }
#ifdef RECO_DEBUG
  cout << "Trigger Mask size: " << trigger_mask.size() << std::endl;
  cout << "Trigger Delay size: " << trigger_delay.size() << std::endl;
  cout << "Trigger Jitter size: " << trigger_jitter.size() << std::endl;
#endif

    //This is a real severe error !!! Here we need a dedicated error handling !!!
    if(trigger_position.size() > 0) {
      if( trigger_position.size() != trigger_types.size() ) 
        {
          cout << "Trigger Types size: " << trigger_types.size() << std::endl;
          cout << "Trigger Position size: " << trigger_position.size() << std::endl;
          throw std::runtime_error("The conditions data for the trigger assignment are utterly wrong. Will leave program now !!!");
        }
    }
    else {
      if( (trigger_mask.size()  != trigger_types.size() ) || 
	  (trigger_mask.size() != trigger_types.size() ) ||
	  (trigger_delay.size() > 0 && trigger_delay.size() != trigger_types.size() ) ||
	  (trigger_jitter.size() > 0 && trigger_jitter.size() != trigger_types.size() ) ) {
	
	std::stringstream message;
	message << "The conditions data for the trigger assignment are utterly wrong. Will leave program now !!!"
		<< "Trigger Types size: " << trigger_types.size() << std::endl
		<< "Trigger Mask size: " << trigger_mask.size() << std::endl
		<< "Trigger Delay size: " << trigger_delay.size() << std::endl
		<< "Trigger jitter size: " << trigger_jitter.size() << std::endl;
	
	throw std::runtime_error(message.str());
	
      }
    }

   //We loop over one vector and access simultaneously the elements
   //of the other vectors. This is safe since we've 
  //checked that the 
   //all vectors are of equal size.
   StringVec::iterator trtyp_iter; 
   unsigned int itype = 0;

   //Initialize the trigger masks
   _principleTriggerMask = 0;   
   _genericTriggerMask = 0;   

   std::map<std::string, unsigned int>::const_iterator genericTriggerIter = _knownTrigger.find( CALICE::TriggerBits::getName(CALICE::TriggerBits::kGenericBit) );
   assert ( genericTriggerIter != _knownTrigger.end());

   for(trtyp_iter = trigger_types.begin(); trtyp_iter != trigger_types.end(); ++trtyp_iter){ 


//Create the actual trigger map
#ifdef RECO_DEBUG
     std::cout << "Trigger Type: " << *trtyp_iter << std::endl;
     if(trigger_position.size() > 0) {
       std::cout << "Trigger Position: " << std::hex << trigger_position.at(itype) << std::dec << std::endl;
     } else {
       std::cout << "Trigger Mask: " << std::hex << trigger_mask.at(itype) << std::endl;
     }
#endif

     short delay=0;
     if (trigger_delay.size()>0) {
       delay=static_cast<short>(trigger_delay[itype]);
     }

     unsigned short jitter=1;
     if (trigger_jitter.size()>0) {
       jitter=static_cast<unsigned short>(trigger_jitter[itype]);
     }

     pair<TriggerDefinitionMap_t::iterator,bool > update_trmap; 
     if(trigger_position.size() > 0) {
       update_trmap = _trdefMap.insert( make_pair(TriggerValue_t(1 << trigger_position.at(itype),
								 delay, jitter),
						  TriggerDef_t(*trtyp_iter)));
     } else {
       update_trmap = _trdefMap.insert( make_pair(TriggerValue_t(trigger_mask.at(itype), 
								 delay,jitter), 
						  TriggerDef_t(*trtyp_iter) ) );
     }

     //This should never happen but anyway
     if(!update_trmap.second) {
       throw std::runtime_error ( "TriggerHandlerCalice CreateTriggerDefinitionMap: Creation of TriggerMap failed"); 
     }			    

     if (*trtyp_iter != "UNKNOWN") { 

       if(trigger_position.size() > 0 ) {
	 _principleTriggerMask = _principleTriggerMask | ( 1 << trigger_position.at(itype));  
       } else {
	 _principleTriggerMask = _principleTriggerMask | trigger_mask.at(itype);     
       }

     }

     if (*trtyp_iter == genericTriggerIter->first ) {

       if(trigger_position.size() > 0) {
	 _genericTriggerMask = _genericTriggerMask | ( 1 << trigger_position.at(itype));  
       } else {
	 _genericTriggerMask = _genericTriggerMask | trigger_mask.at(itype);  
       }

     }
     itype++;
   }

   // copy the trigger bits which will be set in the event header.
   unsigned int trigger_i = 0; 
   for (TriggerDefinitionMap_t::iterator trigger_type_iter=_trdefMap.begin();
	trigger_type_iter!=_trdefMap.end();
	trigger_type_iter++, trigger_i++) {

     std::cout << " TriggerType: " << trigger_type_iter->second.name() << std::endl; 

     std::map<std::string, unsigned int>::const_iterator known_trigger=_knownTrigger.find(trigger_type_iter->second.name());
     if (known_trigger!=_knownTrigger.end()) {
       trigger_type_iter->second.setBitMask(known_trigger->second);
       if (_verbose) {
	 std::cout << "Trigger Mask  Assigned Bit(Offline)       Trigger Type" << std::endl;
	 std::cout << std::hex << setw(9) << trigger_type_iter->first.mask()  
		   << " -> "   << setw(9) << known_trigger->second << " : " 
		   << setw(25) << trigger_type_iter->second.name() 
		   << std::dec << std::endl;
       }
     }
     else {
       std::stringstream message;
       message << "TriggerHandlerCalice::CreateTriggerDefinitionMap> Assignment of unknown trigger: " << trigger_type_iter->second.name();
       throw std::runtime_error(message.str());
     }

   }

  updateTriggerConfBits();
  TriggerConditionsListener();

}

void TriggerHandlerCalice::ExtractTriggerEnvironment(EVENT::LCCollection *confCol) {


  //initialize the Configuration Mask
  _confTriggerMask = 0;
  //..and the anad Enable word  
  _andEnable0 = 0;

  //Analyze coniguration words 
  int num_elmnts = confCol->getNumberOfElements() ; 


  //CRP Patch define a vector which contains written boardids
  std::vector<int> vec_board_id;
  vec_board_id.resize(0);

  //if there is only one element in the collection we take
  //what we get
  if (num_elmnts == 1) { 
     BeTrgConf beTrgConf(confCol->getElementAt(0));
    _confTriggerMask = beTrgConf.getInputEnableMask(); 
  }


  //Check first non-zero element with 'read' label
  //and assume that this contains the configured trigger(s)
  //If no entry matches we assume that a software trigger was configured
  //TODO: Check whether this is the right strategy
  if (num_elmnts > 1) {
#ifdef RECO_DEBUG
    std::cout << "Info: TriggerHandlerCalice - ExtractTriggerEnvironment " << std::endl;
    std::cout << "More than one element in TriggerConfiguration Collection " << std::endl;
    std::cout << "Will use first non-zero Element with 'read' RecordLabel" << std::endl;
   std::cout << "In addition we check that the board was also written" << std::endl;
    std::cout << "Otherwise will set configuration to 0, i.e. Software Trigger" << std::endl;
#endif
    for (int iconf = 0; iconf < confCol->getNumberOfElements(); iconf++)
      {
	BeTrgConf beTrgConf(confCol->getElementAt(iconf));
#ifdef RECO_DEBUG
	cout << "BoardID: " << std::hex << beTrgConf.getBoardID() << std::dec << std::endl; 
	cout << "_confTriggerMask: " << iconf << ", " << std::hex << beTrgConf.getInputEnableMask() << std::dec << std::endl; 
	cout << "RecordLabel: " << beTrgConf.getRecordLabel() << std::endl; 
#endif
        if(beTrgConf.getRecordLabel() == 1 && beTrgConf.getInputEnableMask() != 0) vec_board_id.push_back(beTrgConf.getBoardID());
	if (beTrgConf.getInputEnableMask() != 0 && beTrgConf.getRecordLabel() == 0 )  { 
	  //As a patch we check whether this value appears also
          //in the written data (i.e. RecordLabel= 1)
          //All this is a coplete mess but anyway ... 
          //This strategy assumes that only one board get BeTrgconf
  //data input
  //This strategy has to be revised but works for the time being
          if (vec_board_id.size() == 1) {  

              if(vec_board_id.at(0) == beTrgConf.getBoardID()) {
		_confTriggerMask = beTrgConf.getInputEnableMask();
                _andEnable0 = beTrgConf.getAndEnable(0);
		std::cout << "And Enable COnd:" << _andEnable0 << std::endl;
              } else {
		std::cerr << "Warning, different boards for write (" << beTrgConf.getBoardID() << ") and read " << vec_board_id.at(0) << std::endl; 
              }

	  } else {
	    std::cerr << "Warning more than one board contains" << std::endl;
	    std::cerr << " (written) BeTrgConfdata. Will take first board which appears" << std::endl;
	    std::cerr << " in board vector" << std::endl;

          }
		

	}

      }

  }
  if (_verbose) {
    std::cout << "Trigger configuration  " << _nConfigurationChanges << " enabled bits:";
    unsigned int mask=1;
    for (unsigned int bit_i=0; bit_i<32; bit_i++) {
      if(_confTriggerMask & mask) {
	std::cout << bit_i << " ";
      }
      mask <<= 1;
    }
    std::cout <<std::endl;

      
  }
  //Count configuration Changes
  _nConfigurationChanges++;

  //Tell the world about the great things we have found
#ifdef RECO_DEBUG
  std::cout << "TriggerHandlerCalice::ExtractTriggerEnvironment Change No.: " << _nConfigurationChanges << std::endl;
  std::cout << "TriggerHandlerCalice::ExtractTriggerEnvironment ConfigTriggerMask: " << std::hex <<  _confTriggerMask << std::dec << std::endl;
#endif
  updateTriggerConfBits();
  TriggerConditionsListener();

}


  // set the predefined trigger bits (TriggerBits) according to the current configuration. 
  void TriggerHandlerCalice::updateTriggerConfBits()
  {

    //Set the bits enabled for coincidence
    unsigned int andenable_trigger_bits(0);
    
    for(TriggerDefinitionMap_t::const_iterator trdef_iter = _trdefMap.begin(); trdef_iter != _trdefMap.end(); ++trdef_iter) {
      if ( (_andEnable0 & trdef_iter->first.mask()) ) {  
	andenable_trigger_bits |= trdef_iter->second.bitMask();
      }
    }
    
    
    _andenable_triggerBits=CALICE::TriggerBits(andenable_trigger_bits);



    unsigned int conf_trigger_bits=0;
    
    for(TriggerDefinitionMap_t::const_iterator trdef_iter = _trdefMap.begin(); trdef_iter != _trdefMap.end(); ++trdef_iter) {
      if ( (_confTriggerMask & trdef_iter->first.mask()) ) {  
	conf_trigger_bits |= trdef_iter->second.bitMask();
      }
    }
    
    
    _conf_triggerBits=CALICE::TriggerBits(conf_trigger_bits);
    
    if (_swTriggerOnInd) {
      _conf_triggerBits.setPedestalTrigger();
    }
    else if (_confTriggerMask == 0 ) {

      // does this happen ? No input enabled, no visible software trigger configuration ? 
      // yes this can happen: assert ( false );
      // but is pedestal trigger correct ?
      _conf_triggerBits.setPedestalTrigger();
    }
    
    if (_calibrationOn) {
      _conf_triggerBits.setCalibTrigger();
    }
    
    if (_verbose) {
      std::cout << "Trigger configuration bits:" << _conf_triggerBits << std::endl;
    }
  }

void TriggerHandlerCalice::feConfigurationChange(EVENT::LCCollection *feConfCol, unsigned int set_i) {
#ifdef BOUNDARY_CHECK
  assert(set_i < _feConfCol.size());
#endif
  _feConfCol[set_i]=feConfCol;

  checkForCalibrationOn();
}


// Verify whether the calibration chip is switched on
//
// Logic: loop over all connected FE. Set for each front end the calibration chip
// change status and wehther it was turned on or off. This is done per sub detector
// i.e. all FE conntect to one sub detector will toggle the calibration status of
// the sub detector.
// At the end, loop over all sub detectors find out whether the calibration chip
// was changed if so set the new chip status for the sub detector. If the 
// calibration is on for one sub detector set the global "calibration on" flag.

bool TriggerHandlerCalice::checkForCalibrationOn()
{
  std::vector<unsigned int > calib_change;
  // Set the calibration status as unchanged and to "off"
  calib_change.resize(_feConfCol.size(),0);

  // loop over all connected FE and set calibration status of the connected sub detector to
  // changed and if the calibration chip is on to "on"
  for (std::vector<LCCollection *>::const_iterator fe_col_iter=_feConfCol.begin();
       fe_col_iter != _feConfCol.end();
       fe_col_iter++) {
    if (*fe_col_iter) {
      //_feConfCol=col;
      
      for (UInt_t fe_i=0; fe_i<static_cast<UInt_t>((*fe_col_iter)->getNumberOfElements()); fe_i++) {
	CALICE::FeConfigurationBlock a_fe_conf((*fe_col_iter)->getElementAt(fe_i));
	CALICE::BoardID board_id(a_fe_conf.getBoardID());

	CrateList_t::const_iterator a_crate_iter=_connections.find(board_id.getCrateID());

	if (   a_crate_iter==_connections.end() 
	    || static_cast<unsigned int>(board_id.getSlotID()) >= a_crate_iter->second.slotList().size() 
	    || static_cast<unsigned int>(board_id.getBoardComponentNumber()) >= a_crate_iter->second.slotList()[board_id.getSlotID()].size() ) continue;

	unsigned int sub_det_i=a_crate_iter->second.slotList()[board_id.getSlotID()][board_id.getBoardComponentNumber()];
	if (sub_det_i >=   _calibrationOnVec.size() ) continue;

	calib_change[sub_det_i] |= 2;
	if (a_fe_conf.isCalibEnable()) {
	  calib_change[sub_det_i] |= 1;
	}

      }
    }
  }

  // If the calibration status of a sub detector was changed update the status of the calibration 
  // chip for the sub detector.
  // Also set the global calibration flag
  _calibrationOn=false;
  for (unsigned int sub_det_i=0; sub_det_i<_calibrationOnVec.size(); sub_det_i++) {
    if (calib_change[sub_det_i] & 2) {
      _calibrationOnVec[sub_det_i]=(calib_change[sub_det_i] & 1);
      if (calib_change[sub_det_i] & 1) {
	_calibrationOn=true;
      }
    }
  }

  // print out the result
  if (_verbose) {
    
    std::cout << "Front end configuration:  calib " ;
    if (_calibrationOn) {
      std::cout << "on for sub dectors:";
      for (unsigned int sub_det_i=0; sub_det_i<_calibrationOnVec.size(); sub_det_i++) {
	if (_calibrationOnVec[sub_det_i]) {
	  std::cout << sub_det_i << " ";
	}
      }
    }
    else {
      std::cout << " off.";
    }
    std::cout <<std::endl;

      
  }

  // Finally update the configuration bits;

  // clear bit in trigger configuration
  _conf_triggerBits=CALICE::TriggerBits(_conf_triggerBits.getTriggerBits() & ~CALICE::TriggerBits::kCalib);

  if (_calibrationOn) {
    _conf_triggerBits.setCalibTrigger();
  }

  return _calibrationOn;

}

void TriggerHandlerCalice::readoutConfChange(EVENT::LCCollection *roConfCol) {
  _roConfCol=roConfCol;
  checkForSoftwareTriggerOn();
}


// Check for software front-end or back-end triggers
//
// logic:
// Consider only readout configuration data of connected crates.
// If the back-end software trigger of one crate is set,  update the software
// trigger status of all the sub detector which are connected to the crate.
// Proceed in a similar way with the front-end software triggers. 
// If the front-end trigger is configured, then change the software
// trigger status of all sub detectors which are connected to a front-end
// in any of the slots which has the same front-end ID.
// If the software trigger is enabled for one sub detector then the
// global flag is set to on.

bool TriggerHandlerCalice::checkForSoftwareTriggerOn()
{
#ifdef RECO_DEBUG
  cout << "In ExtractSoftTrigConf: " << endl;
#endif

  _swTriggerOnInd = false;

  for (std::vector< unsigned int >::iterator soft_trigger_iter= _swTriggerOnVec.begin();
       soft_trigger_iter != _swTriggerOnVec.end();
       soft_trigger_iter++ ) {
    *soft_trigger_iter=0;
  }

  //We will check only for be softtriggers 
  //This might be too naive and the checks performed here need to 
  //be much more complicated

  if(_roConfCol) {
    //If we find at least one crate with be s/w Trigger enabled we
    //declare s/w trigger to be on 
    // loop of readout elements 

    for (UInt_t ro_i=0; ro_i<static_cast<UInt_t>(_roConfCol->getNumberOfElements()); ro_i++) {
      CALICE::ReadOutConfigurationBlock ro_conf(_roConfCol->getElementAt(ro_i));

      //Check for be softtriggers  
#ifdef CONV_DEBUG
      std::cout << "ro_i: " << ro_i << std::endl;
      std::cout << "SoftTriggerWord: " <<  ro_conf.getBeSoftTrigger() << std::endl;
#endif

      unsigned int crate_id  = static_cast<UInt_t>(ro_conf.getCrateID());
      CrateList_t::const_iterator crate_iter = _connections.find(crate_id);
      
      // do not bother with unconnected crates
      if ( crate_iter == _connections.end())  continue;

      // back-end soft triggers
      if ( ro_conf.getBeSoftTrigger() ) {
	_swTriggerOnInd = true; 
	for (UInt_t sub_det_i=0; sub_det_i<_swTriggerOnVec.size(); sub_det_i++) {
	  if (crate_iter->second.isConnectedToSubDetector(sub_det_i)) {
	    _swTriggerOnVec[sub_det_i]++;
	  }
	}
      }

      // front-end soft triggers
      for (UInt_t fe_i=0; fe_i<8; fe_i++) {
	if (ro_conf.getFeSoftTrigger(fe_i)) {
	  for (UInt_t slot_i=ro_conf.getFirstSlot(); slot_i<=ro_conf.getLastSlot(); slot_i++) {

	    if (ro_conf.isFeEnabled(slot_i,fe_i)) {

	      // do not bother with unconnected front-ends
	      if (crate_iter->second.slotList().size()<=slot_i || crate_iter->second.slotList()[slot_i].size()<=fe_i) continue;
	      
	      unsigned int sub_det_i=crate_iter->second.slotList()[slot_i][fe_i];
	      if (sub_det_i<_swTriggerOnVec.size()) {
		_swTriggerOnVec[sub_det_i]=true;
	      }
	      break;
	    }
	  }
	}

      }

    }
    

    bool mixed=false;
    unsigned int sub_det_i=0;
    for (vector<unsigned int >::const_iterator soft_trigger_iter= _swTriggerOnVec.begin();
	 soft_trigger_iter != _swTriggerOnVec.end();
	 soft_trigger_iter++,sub_det_i++ ) {
      if (*soft_trigger_iter>0) _swTriggerOnInd=true;
      if (    (*soft_trigger_iter>0) !=  _swTriggerOnInd 
	   || (*soft_trigger_iter>0 &&  *soft_trigger_iter!=_nFrontEnds[sub_det_i] ) ) {
	mixed=true;
	break;
      }
    }
    
    // produce warning if soft trigger is configured differently for the different sub detectors.
    if (mixed) {

      UInt_t sub_det_i=0;
      std::cerr << "TriggerHandlerCalice::ExtractSoftTrigConf> WARNING soft trigger not the same for all sub detectors : ";
      for (vector<unsigned int >::const_iterator soft_trigger_iter= _swTriggerOnVec.begin();
	   soft_trigger_iter != _swTriggerOnVec.end();
	   soft_trigger_iter++, sub_det_i++ ) {
	std::cerr << sub_det_i;
	if ( *soft_trigger_iter==_nFrontEnds[sub_det_i]) std::cerr << ":on ";
	else if ( *soft_trigger_iter==0) std::cerr << ":off ";
	else std::cerr << ":" << *soft_trigger_iter << "/" << _nFrontEnds[sub_det_i] << "on";
      }
      std::cerr << std::endl;

    }

  }
  
  if (_verbose) {
    std::cout << "Individual Software trigger";
    if (_swTriggerOnInd) {
      std::cout << " on for sub detectors: ";
      unsigned int sub_det_i=0;
      for (vector<unsigned int >::const_iterator soft_trigger_iter= _swTriggerOnVec.begin();
	   soft_trigger_iter != _swTriggerOnVec.end();
	   soft_trigger_iter++, sub_det_i++ ) {
	if (*soft_trigger_iter) {
	  std::cout << sub_det_i << " ";
	}
      }
    }
    else {
      std::cout << " off ";
    }
    std::cout << std::endl;
  }

  updateTriggerConfBits();

  return _swTriggerOnInd;
}


// Rebuild connection tree for the corresponding sub detector.
// Keep the connection tree of the other sub detectors untouched.

void TriggerHandlerCalice::moduleConnectionChange(EVENT::LCCollection *module_connection_col, unsigned int set_i)
{
#ifdef RECO_DEBUG
  std::cout << "Do module connection collections exist? " << module_connection_col << std::endl;
#endif

  if (!module_connection_col) return;

#ifdef BOUNDARY_CHECK
  assert(set_i < _nFrontEnds.size());

  // paranoia
  //  assert(_calibrationOnVec.size() == _connections.size());
#endif

  _nFrontEnds[set_i]=0;

  // remove all connections for this sub_detector

  // FIXME: empty crates will stay forever in the connection tree. 
  //        But this is probably a case which will never occure

  // DEBUG: std::cout << " before removal:" << std::endl;
  // DEBUG printConnections(); 

  for (CrateList_t::iterator crate_iter=_connections.begin();
       crate_iter != _connections.end();
       crate_iter++ ) {
      
      for (SlotList_t::iterator slot_iter = crate_iter->second.slotList().begin();
	   slot_iter != crate_iter->second.slotList().end();
	   slot_iter++ ) {
	  for (FrontEndList_t::iterator fe_iter = slot_iter->begin();
	       fe_iter != slot_iter->end();
	       fe_iter++) {
	    if (*fe_iter==set_i) {
	      *fe_iter=UINT_MAX; 
	    }
	  }
      }
  }
  
  // DBEUG: std::cout << " after removal:" << std::endl;
  // DEBUG: printConnections(); 

  const unsigned int max_n_slots=22;
  const unsigned int max_n_fe   =8;

  for (UInt_t element_i=0; element_i<static_cast<unsigned int>(module_connection_col->getNumberOfElements()); element_i++) {
    CALICE::ModuleConnection a_connection(module_connection_col->getElementAt(element_i));

    // ignore unexisting slots, front-ends
    if (static_cast<unsigned int>(a_connection.getSlot())>=max_n_slots) continue;
    if (static_cast<unsigned int>(a_connection.getFrontEnd())>=max_n_fe) continue;

    CrateList_t::iterator a_crate_iter=_connections.find(a_connection.getCrate());
    if (a_crate_iter == _connections.end()) {
      pair<CrateList_t::iterator, bool> res=_connections.insert(make_pair(static_cast<unsigned int>(a_connection.getCrate()), Crate_t(set_i)));

      if (!res.second) {
	throw std::runtime_error("TriggerHandlerCalice::moduleConnectionChanged>ERROR failed to insert front-end connection.");
      }

      a_crate_iter = res.first;
    }

    Crate_t &a_crate=a_crate_iter->second;
    a_crate.setSubDetectorID(set_i);

    if (a_crate.slotList().size() <= static_cast<unsigned int>(a_connection.getSlot())) {
      UInt_t old_size=a_crate.slotList().size();
      a_crate.slotList().resize(max_n_slots);
      UInt_t new_size=a_crate.slotList().size();
      for (UInt_t slot_i=old_size; slot_i<new_size; slot_i++) {
	// initialise elements with non-existing set id
	a_crate.slotList()[slot_i].resize(max_n_fe,UINT_MAX);
      }
    }

#ifdef BOUNDARY_CHECK
    assert ( static_cast<unsigned int>(a_connection.getSlot()) < a_crate.slotList().size() );
    assert ( static_cast<unsigned int>(a_connection.getFrontEnd()) < a_crate.slotList()[a_connection.getSlot()].size() );
#endif

    a_crate.slotList()[a_connection.getSlot()][a_connection.getFrontEnd()]=set_i;
    _nFrontEnds[set_i]++;
  }

  printConnections();

  checkForCalibrationOn();
  checkForSoftwareTriggerOn();
}


// debug method to verify that the connection tree is correctly build up
//
void TriggerHandlerCalice::printConnections()
{
  for (CrateList_t::const_iterator crate_iter=_connections.begin();
       crate_iter != _connections.end();
       crate_iter++ ) {
    /* if (crate_iter->second.slotList().size()>0) */ {
      std::cout << "crate = " << std::hex << std::setw(4) << crate_iter->first << std::dec << " : " <<std::endl;
      
      unsigned int  slot_i=0;
      for (SlotList_t::const_iterator slot_iter = crate_iter->second.slotList().begin();
	   slot_iter != crate_iter->second.slotList().end();
	   slot_iter++, slot_i++ ) {
	
	  UInt_t fe_i=0;
	  for (FrontEndList_t::const_iterator fe_iter = slot_iter->begin();
	       fe_iter != slot_iter->end();
	       fe_iter++, fe_i++) {
	    if (*fe_iter != UINT_MAX) break;
	  }
	  if (fe_i==8) continue;
	/* if (slot_iter->size() > 0 )*/ {
	  std::cout << "slot " << std::setw(3) << slot_i  << " : " ;
	  fe_i=0;
	  for (FrontEndList_t::const_iterator fe_iter = slot_iter->begin();
	       fe_iter != slot_iter->end();
	       fe_iter++, fe_i++) {
	    if (*fe_iter != UINT_MAX) {
	      std::cout << std::setw(3) << fe_i << ":" << std::setw(2) << *fe_iter;
	    }
	  }
	  std::cout << std::endl;
	}
      }
      std::cout << std::endl;
    }
  }
}

  void TriggerHandlerCalice::ExtractGenSoftTrigConfiguration(EVENT::LCCollection *trgroConfCol) {
    
#ifdef RECO_DEBUG
    cout << "In ExtractGenSoftTrigConf: " << endl;
#endif
    
    _swTriggerOnGen = false;

    //We will check only for be softtriggers 
    //This might be too naive and the checks performed here need to 
    //be much more complicated
    
    if(trgroConfCol) {
      if(trgroConfCol->getNumberOfElements() != 1) {
	std::cout << "TriggerHandlerCalice -Warning: TrgReadconfiguration collection size > 1" << std::endl;
	std::cout << "Will use first element: " << std::endl;
        } 
  
	CALICE::TrgReadoutConfigurationBlock trgro_conf(trgroConfCol->getElementAt(0));
	//Check for general softtriggers  
#ifdef RECO_DEBUG
	std::cout << "SoftTriggerWord: " <<  trgro_conf.getBeTrgSoftTrg() << std::endl;
#endif
	if ( trgro_conf.getBeTrgSoftTrg() ) _swTriggerOnGen = true; 

    }
  }


void TriggerHandlerCalice::PrepareTriggerEventData(const LCEvent* evt) {

  // this is already done in the method where prepare is called:

  //  // better use a member variable then a local static variable.
  //  //  //A local variable to check whether the the method has already been
  //  //  //called for this event
  //  //  //  static LCTime last_update_time;

  LCTime theTime(evt->getTimeStamp());
  //Check whether the method has already been called for this event
  if(_theTime.timeStamp() == theTime.timeStamp()) return; 
  _theTime=theTime;
  
  //Do the integrity check of the configured trigger bits here
  //(currently lccd leaves no other chance) 

  //TriggerConditionsListener();


  // clear the result of the trigger validation.
  _distanceToPreTrigger.clear();
  //Reset the fifo content
  _theTriggerFifoCont.clear();

    
    // the collection name is unique
  if (_trgEventClass == kFeTrgEvent || _trgEventClass == kUnknown) {
  try {
    const LCCollection* col = evt->getCollection( _colNameTriggerEvent );
    
#ifdef BOUNDARY_CHECK
    assert ( col->getNumberOfElements() == 1 );
#endif

#ifdef RECO_DEBUG
    std::cout << "Event Number: " << evt->getEventNumber() << std::endl;
    std::cout << " Number of Elements"<< col->getNumberOfElements() << std::endl;
#endif

    FeTrgData feTrgData(col->getElementAt(0));
    feTrgData.getFifoWords(_theTriggerFifoCont);

#ifdef RECO_DEBUG
      cout << "Fifo size FeTrg: " <<  _theTriggerFifoCont.size() << endl;
      if (_theTriggerFifoCont.size() > 0) {
	int icount(0); 
       for(IntVec::iterator vec_iter = _theTriggerFifoCont.begin();vec_iter != _theTriggerFifoCont.end(); vec_iter++){
	 std::cout << "Fifo Element at " <<  icount << " is: " << std::hex << *vec_iter << std::dec << std::endl;
	  icount++;
	}
      }
#endif

  }
  catch (  DataNotAvailableException &err ) {
#ifdef RECO_DEBUG
      std::cout << "No FeTrgData in: " << evt->getEventNumber() << std::endl;
#endif
  }
  }

  if(  _theTriggerFifoCont.size() == 0) {

  if (_trgEventClass == kBeTrgEvent || _trgEventClass == kUnknown) {
    try {

      const LCCollection* col = evt->getCollection( COL_TRIGGER_EVENT );
      BeTrgEvent beTrgEvent(col->getElementAt(0));
      _theTriggerFifoCont = beTrgEvent.getFifoWords(_theTriggerFifoCont);

#ifdef RECO_DEBUG
      cout << "Fifo size BeTrg: " <<  _theTriggerFifoCont.size() << endl;
      if (_theTriggerFifoCont.size() > 0) {
	int icount(0); 
	for(IntVec::iterator vec_iter = _theTriggerFifoCont.begin();vec_iter != _theTriggerFifoCont.end(); vec_iter++){
	  std::cout << "Fifo Element at " <<  icount << " is: " << std::hex << *vec_iter << std::dec << std::endl;
	  icount++;
	}
      }
#endif      
      
    } catch (lcio::DataNotAvailableException err) {
#ifdef RECO_DEBUG
      std::cout << "No FeTrgData in: " << evt->getEventNumber() << std::endl;
#endif
      }
  }
  }

  if(  _theTriggerFifoCont.size() == 0) {
    _noTriggerEvents++;
  }
  
  
    //    _last_update_time = _theTime;
    //  }
}

void TriggerHandlerCalice::ExtractTriggerCheckConditions(EVENT::LCCollection *checkCol){


  TriggerCheck trigger_check(checkCol->getElementAt(0)); 
  
  //Extract the words needed to validate the trigger
  _main_word = trigger_check.getMainWord();
  _tolerance = trigger_check.getTolerance();
  _word_up   = abs(trigger_check.getUpSearch());
  _word_down = -abs(trigger_check.getDownSearch());

  assert(_word_down<0);
  assert(_word_up>0);

#ifdef RECO_DEBUG
  std::cout << "TriggerHandlerCalice TriggerCheck main word: " << _main_word << std::endl;
  std::cout << "TriggerHandlerCalice TriggerCheck main word: " << _tolerance << std::endl;
  std::cout << "TriggerHandlerCalice TriggerCheck main word: " << _word_up << std::endl;
  std::cout << "TriggerHandlerCalice TriggerCheck main word: " << _word_down << std::endl;
#endif


}
}

#endif
