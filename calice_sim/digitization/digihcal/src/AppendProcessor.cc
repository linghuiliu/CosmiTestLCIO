#include "AppendProcessor.hh"
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include "IO/LCReader.h"
#include "UTIL/LCTOOLS.h"
#include "marlin/Exceptions.h"
#include <collection_names.hh>
#include <FastCaliceHit.hh>
#include <TcmtHit.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>

#include <streamlog/streamlog.h>

//#define HCALRECO_DEBUG

namespace CALICE { 

AppendProcessor aAppendProcessor;

AppendProcessor::AppendProcessor() : marlin::Processor("AppendProcessor") {
  
  _description = "Opens a second (chain of) lcio file(s) and appends events.";

  _appendFileNames.push_back("append.slcio");
  registerProcessorParameter("AppendFileNames", 
			     "Name of the lcio input file(s)",
			     _appendFileNames,
 			     _appendFileNames);
			     
  _appendCollectionInputNames.push_back("ToAppend");
  registerProcessorParameter("InputCollections",
                             "Name of the input collection(s) to append, "
                             "RawCalorimeterHits or FastCaliceHits",
			     _appendCollectionInputNames,
			     _appendCollectionInputNames);
			     
  _appendCollectionOutputNames.push_back("Appended");
  registerProcessorParameter("OutputCollections",
                             "Name of the appended output collection(s)",
			     _appendCollectionOutputNames,
			     _appendCollectionOutputNames);

  _appendCollectionFixLevels.push_back(1);
  registerProcessorParameter("TransformToFCH", 
                             "apply (1) or ignore (0) transformation "
                             "RawCalorimeterHits -> FastCaliceHits", 
			     _appendCollectionFixLevels,
			     _appendCollectionFixLevels);

  registerProcessorParameter("RepeatCollections", 
                             "repeat or process collection(s) "
                             "only one-time", 
			     _appendRepeatCollections,
			     true);

  _useTcmtHits.push_back(0);
  registerProcessorParameter("UseTcmtHits",
                             "When converting RawHits, use FastCaliceHit "
                             "(0,default) or TcmtHits (1) for each collection.",
			     _useTcmtHits, 
                             _useTcmtHits );

}


void AppendProcessor::init() { 
  
  if (_appendCollectionInputNames.size() != _appendCollectionOutputNames.size()) {
    throw std::runtime_error("AppendProcessor::Init: "
                             "There must be exactly one entry "
                             "in AppendOutputCollections for every "
                             "entry in AppendInputCollections.");
  }
  if (_appendCollectionInputNames.size() != _appendCollectionFixLevels.size()) {
    throw std::runtime_error("AppendProcessor::Init: There must be exactly "
                             "one entry in AppendFixLevels for every entry "
                             "in AppendInputCollections.");
  }

  printParameters();
  
  _lcReader = lcio::LCFactory::getInstance()->createLCReader();
  _lcReader->open(_appendFileNames); 
}

void AppendProcessor::processEvent(LCEvent *evt) { 
  
  lcio::LCEvent* appendEvent;

#ifdef HCALRECO_DEBUG
    streamlog_out(DEBUG) << "reading append event" << std::endl;
#endif  
    appendEvent = _lcReader->readNextEvent();
    if(!appendEvent) {

      if(!_appendRepeatCollections) {
          throw EVENT::DataNotAvailableException("AppendProcessor: "
                                                 "No event left in "
                                                 "append stream.");
      }
      
      streamlog_out(WARNING) << "Collection is repeated" << std::endl;
      _lcReader->close();
      _lcReader->open(_appendFileNames);
      appendEvent = _lcReader->readNextEvent();
      
    }

    int eventNumber = appendEvent->getEventNumber();

       for (unsigned iCol=0; iCol < _appendCollectionInputNames.size(); iCol++) {
         try {
	   if (_appendCollectionFixLevels[iCol]==0) {
	     // Just copy input collection to current event.  Works fine for any hit type,
	     // including CalorimeterHits (used for TCMT)
#ifdef HCALRECO_DEBUG
             streamlog_out(DEBUG) << "ignoring transformation " 
                                  << "RawCalorimterHits -> FastCaliceHits" 
                                  << std::endl;
#endif
             lcio::LCCollection* _appendCol = 
               appendEvent->takeCollection(_appendCollectionInputNames[iCol]);

             evt->addCollection(_appendCol, _appendCollectionOutputNames[iCol]);

	   } else {
#ifdef HCALRECO_DEBUG
             streamlog_out(DEBUG) << "transforming "
                                  << "RawCalorimterHits -> FastCaliceHits" 
                                  << std::endl;
#endif
	     LCCollection* originalVector = 
               appendEvent->getCollection(_appendCollectionInputNames[iCol]);

             LCCollectionVec* _appendCol = 
               new LCCollectionVec(LCIO::RAWCALORIMETERHIT);

             LCFlagImpl hitFlag(_appendCol->getFlag());
             hitFlag.setBit(LCIO::RCHBIT_TIME);
             hitFlag.setBit(LCIO::CHBIT_ID1);
             _appendCol->setFlag(hitFlag.getFlag());

	     // fixme: do smarter things than copy and paste *g*
	     if ( _useTcmtHits[iCol] != 1 ) // default is to create FastCaliceHits from RawCaloHits
	       {
		 for (unsigned int i = 0; 
                      i < static_cast<unsigned>(
                                        originalVector->getNumberOfElements()
                                        );
                      i++) {

		   RawCalorimeterHit* aRawCalorimeterHit = 
                     dynamic_cast<RawCalorimeterHit*>(originalVector->getElementAt(i));

		   if (aRawCalorimeterHit) {

		     FastCaliceHit* aFastCaliceHit = 
                       new FastCaliceHit(aRawCalorimeterHit);

		     _appendCol->addElement(aFastCaliceHit);

		   } else {
		     std::stringstream message;

		     message << "Collection " 
                             << _appendCollectionInputNames[iCol] 
                             << " doesn't contain RawCalorimeterHits" 
                             << std::endl;

		     streamlog_out(WARNING) << message.str();
		   }
		 }
	       }
	     else // if ( _useTcmtHits[iCol] == 1 ) // create TcmtHits from RawCaloHits
	       {
		 for (unsigned int i = 0; 
                      i < static_cast<unsigned>(
                                         originalVector->getNumberOfElements()
                                         );
                      i++) {

		   RawCalorimeterHit* aRawCalorimeterHit = 
                     dynamic_cast<RawCalorimeterHit*>(originalVector->getElementAt(i));

		   if (aRawCalorimeterHit) {

		     TcmtHit* aTcmtHit = new TcmtHit(aRawCalorimeterHit);
		     _appendCol->addElement(aTcmtHit);

		   } else {
		     std::stringstream message;
		     message << "Collection " 
                             << _appendCollectionInputNames[iCol] 
                             << " doesn't contain RawCalorimeterHits" 
                             << std::endl;
		     streamlog_out(WARNING) << message.str();
		   }
		 }
	       }
	     evt->addCollection(_appendCol, _appendCollectionOutputNames[iCol]);
	   }

	 }
         catch (EVENT::DataNotAvailableException &e) {
           streamlog_out(WARNING) << "collection " 
                                  << _appendCollectionInputNames[iCol] 
                                  << " not found in append event "<<eventNumber << std::endl;
         }	
       }
}
  
}
