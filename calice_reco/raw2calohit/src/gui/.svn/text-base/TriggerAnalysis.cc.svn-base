#include "TriggerAnalysis.hh"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CALICEGUI
#include <TriggerDisplayData.hh>
#include <TriggerDisplay.hh>
#include <GuiThread.hh>
using namespace GLVIEW;
#endif

#include <marlin/ConditionsProcessor.h>
#include <collection_names.hh>
#include <BeTrgEvent.hh>
#include <BeTrgConf.hh>
#include <TriggerHandlerCalice.hh>

#include <histmgr/HistMgr.hh>
#include <histmgr/HistogramCollection_t.hh>
#include <histmgr/FloatHistogram1D.hh>


#include <cassert>


namespace CALICE {

  TriggerAnalysis a_TriggerAnalysis_instance;

  TriggerAnalysis::TriggerAnalysis() 
    : TriggerProcessorBase("TriggerAnalysis"),
#ifdef HAVE_CALICEGUI
      _display(0),
#endif
      _histGroupKey(type()),
      _histTriggerOnPosisitonKey("TriggerOnPostion"),
      _histTriggerLengthKey("TriggerPulseLength")
    
  {
    
    registerProcessorParameter( "TriggerMainWordName" , 
    				"Name of the parameter which will be set to the trigger main word."  ,
				_parNameTriggerMainWord ,
    				std::string(PAR_TRIGGER_MAIN_WORD) ) ;

#ifdef HAVE_CALICEGUI
    _visualise=0;
    registerProcessorParameter( "Visualisation" , 
				"Visualise the drift chamber hits and the fit" ,
				_visualise ,
				_visualise);
#endif

    registerProcessorParameter( "HistogramGroupName" , 
                               "The name of the histogram group under which the control histograms will be registered."  ,
                               _histGroupKey.nameStorage() ,
                               _histGroupKey.name() );

  }

  TriggerAnalysis::~TriggerAnalysis() 
  {
#ifdef HAVE_CALICEGUI
    if (_display) {
      GuiThread *gui=GuiThread::getInstance();
      Display *display_ptr=static_cast<Display *>(_display);
      gui->removeDisplay(&display_ptr);
    }
#endif
  }


  void TriggerAnalysis::init() {

    // will also print the parameters.
    TriggerProcessorBase::init();
    
    _missingTriggerEventData=0;
    _nEvents=0;

#ifdef HAVE_CALICEGUI
    // these two objects must stay alive until the GUI thread has created the display
    Display *display_ptr=0;
    TriggerDisplayKit display_kit;
    GuiThread *gui=0;
    if (_visualise>0) {
      gui=GuiThread::getInstance();
      gui->registerDisplay(&display_ptr,&display_kit,"Trigger Display");
      gui->waitForDisplay(&display_ptr);
      _display=dynamic_cast<TriggerDisplay *>(display_ptr);
      if (!_display) {
	throw std::runtime_error("EventViewProcessor::Init> ERROR: Event Display creation failed!");
      }
    }
    else {
      _display=0;
    }
#endif

    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histogramList->createHistogramGroup(_histGroupKey);
    histogramList->lockGroup(_histGroupKey);
    std::vector<std::string> trigger_pos_names;
    std::vector<std::string> trigger_length_names;
    for (UInt_t bit_i=0; bit_i<32; bit_i++) {
      std::string a_trigger_name=CALICE::TriggerBits::getName(bit_i);
      
      if (a_trigger_name.empty()) {
	std::stringstream new_name;
	new_name << _histTriggerOnPosisitonKey.name() << bit_i;
	a_trigger_name=new_name.str();
      }

      {
	std::string hist_name(_histTriggerOnPosisitonKey.name());
	hist_name += "_";
	hist_name += a_trigger_name;
	trigger_pos_names.push_back(hist_name);
      }

      {
	std::string hist_name(_histTriggerLengthKey.name());
	hist_name += "_";
	hist_name += a_trigger_name;
	trigger_length_names.push_back(hist_name);
      }
    }
    histogramList->createHistograms(_histGroupKey,_histTriggerOnPosisitonKey,trigger_pos_names,32,HistPar((UInt_t) 401,-200-.5,200+.5),true);
    histogramList->createHistograms(_histGroupKey,_histTriggerLengthKey,trigger_length_names,32,HistPar((UInt_t) 256,0-.5,256-.5),true);
    
  }

  void TriggerAnalysis::processEvent( LCEvent * evtP ) 
  {

    getTriggerHandler()->setTrigger(evtP);
    _nEvents++;
    //    try {
      
      const lcio::IntVec &trigger_event=getTriggerHandler()->getTriggerFifoContent();  
//       unsigned int used_trigger_fifo_size=getTriggerHandler()->getUsedFifoDepth();
      if (!trigger_event.empty()) {
	UInt_t main_word = static_cast<UInt_t>(evtP->getParameters().getIntVal(_parNameTriggerMainWord));
	const TriggerHandlerCalice::TriggerDefinitionMap_t &trigger_def = getTriggerHandler()->getTriggerDefinitionMap();

#ifdef HAVE_CALICEGUI
	DisplayDataPtr<TriggerDisplayData> display_buffer(_display,evtP->getRunNumber(),evtP->getEventNumber());
	if (display_buffer) {
	  display_buffer->setNumberOfBits(32);
	  display_buffer->setPrincipleMask( getTriggerHandler()->getPrincipleTriggerMask() );
	  display_buffer->setInputEnabledMask( getTriggerHandler()->getTrgConfigurationMask() );
	  display_buffer->setMainWord( main_word);
	  for (TriggerHandlerCalice::TriggerDefinitionMap_t::const_iterator def_iter=trigger_def.begin();
	       def_iter!=trigger_def.end();
	       def_iter++) {
	    
	    // only the lower 16 bits are inputs
	    UInt_t mask_i=1;
	    for(UInt_t bit_i=0; bit_i<15; bit_i++) {
	      if (def_iter->first.mask()  & mask_i) {
		display_buffer->setDelay(bit_i,def_iter->first.delay());
	      }
	      mask_i <<= 1;
	    }
	  }
	}
#endif

	histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
	histmgr::HistogramCollection_t &trigger_position_hist=histogramList->getHistogramCollection(_histGroupKey,_histTriggerOnPosisitonKey);
	histmgr::HistogramCollection_t &trigger_length_hist=histogramList->getHistogramCollection(_histGroupKey,_histTriggerLengthKey);
	UInt_t start[32];
	for (UInt_t bit_i=0; bit_i<=32; bit_i++) {
	  start[bit_i]=static_cast<UInt_t>(-1);
	}
	
	for (UInt_t word_i=0; word_i < getTriggerHandler()->getUsedFifoDepth(); word_i++) {
#ifdef HAVE_CALICEGUI
	  if (display_buffer) {
	    display_buffer->add(word_i, trigger_event[word_i]);
	  }
#endif
	  
	  // fill histograms with start positions and puls length
	  if (main_word>0) {
	    if (trigger_event[word_i]) {
	      for (TriggerHandlerCalice::TriggerDefinitionMap_t::const_iterator trdef_iter=trigger_def.begin();
		   trdef_iter!=trigger_def.end();
		   trdef_iter++) {
		
		if (trigger_event[word_i] & trdef_iter->first.mask()) {
		  if (word_i==0 || (trigger_event[word_i-1] & trdef_iter->first.mask())==0) {
		    UInt_t mask_i=1;
		    for (UInt_t bit_i=0; bit_i<32; bit_i++) {
		      if (mask_i & trdef_iter->second.bitMask()) {
			trigger_position_hist.histogram(bit_i)->fill( static_cast<int>(word_i - main_word - trdef_iter->first.delay()));
			start[bit_i]=word_i;
		      }
		      mask_i<<=1;
		    }
		  }
		  UInt_t mask_i=1;
		  for (UInt_t bit_i=0; bit_i<32; bit_i++) {
		    if (mask_i & trdef_iter->second.bitMask()) {
		      if (word_i+1>=getTriggerHandler()->getUsedFifoDepth() || (trigger_event[word_i+1] & trdef_iter->first.mask())==0) {
			if (start[bit_i]< word_i) {
			  trigger_length_hist.histogram(bit_i)->fill(word_i-start[bit_i]);
			  start[bit_i]=static_cast<UInt_t>(-1);
			}
			
		      }
		    }
		    mask_i <<=1;
		  }
		}
	      }
	    }
	  }
	}
#ifdef HAVE_CALICEGUI
	if (display_buffer) {
	  display_buffer->finish();
	}
#endif
      }
      //      else if (trigger_event.size() != _maxSize) {
      //       std::cerr << "TriggerAnalysis::processEvent>" << "Trigger event data contains " << trigger_event.size()
      //                 << " elements instead of the expected one." << std::endl;
      //      }
      else {
	_missingTriggerEventData++;
      }
      //  }
      //  catch (  DataNotAvailableException &err ) {
      //      // FIXME: What should be done if the trigger event data is missing ?
      //      _missingTriggerEventData++;
      //  }
      
  }

  void TriggerAnalysis::end()
  {
    std::cout << "--- " << name() << " Report :" << std::endl
	      << std::setw(8) << _nEvents                                << " events processed." << std::endl
	      << std::setw(8) << _missingTriggerEventData                << " events with missing trigger event data." << std::endl
	      << std::setw(8) << getTriggerHandler()->getNConfigurationChanges() << " trigger configuration changes." << std::endl;

  }

}
