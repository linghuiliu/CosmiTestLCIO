#include "TriggerHandlerCalice.hh"
#include "CaliceTriggerProcessor.hh"
//
#include "lccd/LCConditionsMgr.hh"
#include "marlin/ConditionsProcessor.h"
#include "TriggerHandlerCalice.hh"
#include <TriggerBits.hh>
#include <ErrorBits.hh>
#include <collection_names.hh>
#ifdef WITH_CONTROL_HISTOGRAMS
#  include <histmgr/HistMgr.hh>
#  include <histmgr/FloatHistogram1D.hh>
#endif

#include <iostream>
#include <iomanip>
#include <sstream>


#define TICKS_TO_NS 25

namespace marlin {

CaliceTriggerProcessor aCaliceTriggerProcessor;

  CaliceTriggerProcessor::CaliceTriggerProcessor()
    : TriggerProcessorBase("CaliceTriggerProcessor") 
#ifdef WITH_CONTROL_HISTOGRAMS
    ,_histGroupKey(type()),
      _histTriggerEventKey("TriggerEvent"),
      _histTriggerConfKey("TriggerConf"),
      _histTriggerPostKey("TriggerPostHistory"),
      _histTriggerPreKey("TriggerPreHistory")
#endif
  {

    _description = "Processor to add Trigger information on CALICE data to the event";

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


    _noTriggerActivityRange.clear();
    _noTriggerActivityRange.push_back(400);
    _noTriggerActivityRange.push_back(200);
    registerProcessorParameter( "NoTriggerActivityRanges" , 
    				"Two integers which specify the time in ns before and after the main word during which there should not be any trigger activity."  ,
				_noTriggerActivityRange,
    				_noTriggerActivityRange,
				_noTriggerActivityRange.size()) ;

    {
      _beamTriggerNames.clear();
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kBeamBit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kCosmicsBit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kVetoBit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kCherenkovBit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC1_3x3Bit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC2_3x3Bit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC1_10x10Bit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC2_10x10Bit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC1_100x100Bit));
      _beamTriggerNames.push_back(CALICE::TriggerBits::getName(CALICE::TriggerBits::kSC2_100x100Bit));
    
      std::stringstream description;
      description << "Names of all triggers which are considered to indicate physics events. Events with these triggers not set will be ignored. Only these bits are used for data consistency checks" << std::ends
		  << "Possible names: ";
      for (UInt_t i=0; i<32; i++) {
	if (i>0) {
	  description << ", ";
	}
	description << CALICE::TriggerBits::getName(i);
      }
      registerProcessorParameter( "BeamTriggerNames" , description.str()  , _beamTriggerNames,_beamTriggerNames);
    }

#ifdef WITH_CONTROL_HISTOGRAMS    

    registerProcessorParameter( "HistogramGroupName" , 
                               "The name of the histogram group under which the control histograms will be registered."  ,
                               _histGroupKey.nameStorage() ,
                               _histGroupKey.name() );
#endif

}


  // CaliceTriggerProcessor::~CaliceTriggerProcessor() {}

  void CaliceTriggerProcessor::init() {

    //Some initialization
    CALICE::TriggerProcessorBase::init();
    
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


    // trigger paramters
    assert ( _noTriggerActivityRange.size() == 2 && _noTriggerActivityRange[0]>=0 && _noTriggerActivityRange[1]>=0 );
    _beamTriggerBitMask = 0;
    for (lcio::StringVec::const_iterator trigger_name_iter=_beamTriggerNames.begin(); trigger_name_iter!=_beamTriggerNames.end(); trigger_name_iter++) {
      _beamTriggerBitMask |= (1<<CALICE::TriggerBits::getBit(trigger_name_iter->c_str()));
    }
    _noTriggerActivityRange[0] /= TICKS_TO_NS;
    _noTriggerActivityRange[1] /= TICKS_TO_NS;

    
#ifdef WITH_CONTROL_HISTOGRAMS
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histogramList->createHistogramGroup(_histGroupKey);
    histogramList->lockGroup(_histGroupKey);
    std::vector<std::string> conf_names;
    conf_names.push_back("TriggerInputEnabledRaw");
    conf_names.push_back("TriggerMainWordRaw");
    conf_names.push_back("TriggerInputEnabledMapped");
    conf_names.push_back("TriggerEventMapped");
    histogramList->createHistograms(_histGroupKey,_histTriggerConfKey,conf_names, 4,HistPar((UInt_t) 32+1+2,-.5,32+2.5),true);
    std::vector<std::string> trigger_names;
    for (UInt_t bit_i=0; bit_i<32; bit_i++) {
      trigger_names.push_back(CALICE::TriggerBits::getName(bit_i));
      if (trigger_names.back().empty()) {
	std::stringstream new_name;
	new_name << "TriggerBit" << bit_i;
	trigger_names.back()=new_name.str();
      }
    }
    histogramList->createHistograms(_histGroupKey,_histTriggerPreKey,trigger_names,32,HistPar((UInt_t) 201,-10-.5,190+.5),true);
    histogramList->createHistograms(_histGroupKey,_histTriggerPostKey,trigger_names,32,HistPar((UInt_t) 201,-10-.5,190+.5),true);
    
#endif


}




  void CaliceTriggerProcessor::processEvent(LCEvent* evt){
    try {
      //UInt_t ev_nr=evt->getEventNumber();
    // copy fifo from the event to the trigger handler
    getTriggerHandler()->setTrigger(evt);
    // search around the nominal main word were the enabled trigger fired.
    UInt_t main_word=getTriggerHandler()->searchTriggerMainWord();
    evt->parameters().setValue(_parNameTriggerMainWord, static_cast<int>(main_word));
    
    if (main_word >= getTriggerHandler()->getUsedFifoDepth() ) {
      _noMainWord++;
    }

    // scan the history before and after the main word
    getTriggerHandler()->searchTriggerHistory();

    CALICE::TriggerBits out_of_range_triggers=getTriggerHandler()->getOutOfRangeTriggerBits();
    if ( out_of_range_triggers.getTriggerBits() & getTriggerHandler()->getPrincipleTriggerMask() ) {
      _eventsWithOutOfRangeTriggers++;
    }


    //get the history prior to the main word and copy it to the event header.
    //This history takes only bits into account which were not set in
    //the the main word or after those bits were at least one time
    //off before the main word 
    const std::map<unsigned int, unsigned int > &pre_trigger_history = getTriggerHandler()->getPreTriggerHistory();
    std::vector<int> temp_pos;
    std::vector<int> temp_bits;
    bool dirty_event=false;
    for (std::map<unsigned int, unsigned int>::const_iterator history_iter = pre_trigger_history.begin();
	 history_iter != pre_trigger_history.end();
	 history_iter++ ) {
      temp_pos.push_back(history_iter->first);
      temp_bits.push_back(history_iter->second);
      if (history_iter->first < static_cast<unsigned int>(_noTriggerActivityRange[0]) ) {
	if (history_iter->second  & _beamTriggerBitMask) dirty_event=true;
      }
    }

    //Do not give the vectors to lcio if they are empty
    //otherwise the lcio file gets corrupted !!!!???
    if(temp_pos.size()>0) evt->parameters().setValues(_parNameTriggerPreHistoryPos , temp_pos);
    if(temp_bits.size()>0) evt->parameters().setValues(_parNameTriggerPreHistoryBits , temp_bits);


    // get the history posterior to the main word and copy it to the event header.
    //This history takes only bits into account which were not set in
    //the the main word or after those bits were at least one time
    //off after the main word 
    const std::map<unsigned int, unsigned int > &post_trigger_history = getTriggerHandler()->getPostTriggerHistory();
    temp_pos.clear();
    temp_bits.clear();
    for (std::map<unsigned int, unsigned int>::const_iterator history_iter = post_trigger_history.begin();
	 history_iter != post_trigger_history.end();
	 history_iter++ ) {
      temp_pos.push_back(history_iter->first);
      temp_bits.push_back(history_iter->second);

      if (history_iter->first < static_cast<unsigned int>(_noTriggerActivityRange[1]) ) {
	if (history_iter->second  & _beamTriggerBitMask) dirty_event=true;
      }
    }
    if(temp_pos.size()>0) evt->parameters().setValues(_parNameTriggerPostHistoryPos , temp_pos);
    if(temp_bits.size()>0) evt->parameters().setValues(_parNameTriggerPostHistoryBits , temp_bits);


    CALICE::ErrorBits error(evt->getParameters().getIntVal(PAR_ERROR_BITS));
    if (dirty_event) {
      error.setDirtyEvent();
      evt->parameters().setValue(PAR_ERROR_BITS,error.getBits());
    }
    
  
    /*
#ifdef TRIGGER_HANDLER_IS_SINGLETON
    CALICE::TriggerBits trigger_conf(((LCEvent *) evt)->getParameters().getIntVal(_parNameTriggerConf));
    CALICE::TriggerBits trigger_event(((LCEvent *) evt)->getParameters().getIntVal(_parNameTriggerEvent));
#else
    CALICE::TriggerBits trigger_conf(getTriggerHandler()->getTriggerConfiguration());
    CALICE::TriggerBits trigger_event(getTriggerHandler()->getTriggerEvent());

    evt->parameters().setValue(_parNameTriggerConf , trigger_conf.getTriggerBits());
    evt->parameters().setValue(_parNameTriggerEvent, trigger_event.getTriggerBits());
#endif
    */

    CALICE::TriggerBits trigger_conf(getTriggerHandler()->getTriggerConfiguration());
    CALICE::TriggerBits trigger_andenable(getTriggerHandler()->getAndEnableBits());
    CALICE::TriggerBits trigger_event(getTriggerHandler()->getTriggerEvent());
    
    
    evt->parameters().setValue(_parNameTriggerConf , trigger_conf.getTriggerBits());
    evt->parameters().setValue(_parNameTriggerAndEnable , trigger_andenable.getTriggerBits());
    evt->parameters().setValue(_parNameTriggerEvent, trigger_event.getTriggerBits());


#ifdef WITH_CONTROL_HISTOGRAMS
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histmgr::HistogramCollection_t & trigger_bits_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histTriggerConfKey);
    histmgr::HistogramCollection_t & trigger_post_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histTriggerPostKey);
    histmgr::HistogramCollection_t & trigger_pre_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histTriggerPreKey);

    unsigned int input_enable=getTriggerHandler()->getTrgConfigurationMask();

    UInt_t main_word_content=0;
    if (getTriggerHandler()->isTriggerFifoDepthGood()) {
      main_word_content=getTriggerHandler()->getSmearedTriggerMainWord();
    }
    else {
      // trigger fifo error
    }
    
    unsigned int mask=1;
    for (unsigned int bit_i=0; bit_i<32; bit_i++, mask<<=1)  {
      if (input_enable & mask ) trigger_bits_hist_arr.histogram(0)->fill(bit_i);
      if (trigger_conf.getTriggerBits() & mask ) trigger_bits_hist_arr.histogram(2)->fill(bit_i);
      if (main_word_content & mask ) trigger_bits_hist_arr.histogram(1)->fill(bit_i);
      if (trigger_event.getTriggerBits() & mask ) trigger_bits_hist_arr.histogram(3)->fill(bit_i);

      for (std::map<unsigned int, unsigned int>::const_iterator history_iter = pre_trigger_history.begin();
	   history_iter != pre_trigger_history.end();
	   history_iter++ ) {
	if (history_iter->second & mask ) {
	  trigger_pre_hist_arr.histogram(bit_i)->fill(history_iter->first);
	}
      }

      for (std::map<unsigned int, unsigned int>::const_iterator history_iter = post_trigger_history.begin();
	   history_iter != post_trigger_history.end();
	   history_iter++ ) {
	if (history_iter->second & mask ) {
	  trigger_post_hist_arr.histogram(bit_i)->fill(history_iter->first);
	}
      }


    }
    if (getTriggerHandler()->isSWTriggerOn() ) {
      trigger_bits_hist_arr.histogram(0)->fill(33);
      trigger_bits_hist_arr.histogram(1)->fill(33);
    }
    if (getTriggerHandler()->isCalibrationOn() ) {
      trigger_bits_hist_arr.histogram(0)->fill(34);
      trigger_bits_hist_arr.histogram(1)->fill(34);
    }
#endif
    }
    catch (std::exception &err) {
      std::cerr << err.what()  << std::endl;
      throw err;
    }
    
  }
  
  void CaliceTriggerProcessor::end()
  {
    std::cout << "--- " << name() << " Report :" << std::endl;
    std::cout << std::setw(8) << _noMainWord                    << " events for which no main word was found." << std::endl
              << std::setw(8) << _eventsWithOutOfRangeTriggers  << " events for which principle trigger inputs could have fired outside the fifo range." << std::endl;
  }
  
}
