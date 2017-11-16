#include "Ahc2AppendProcessor.hh"
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include "IO/LCReader.h"
#include "UTIL/LCTOOLS.h"
#include "marlin/Exceptions.h"
#include <collection_names.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>

#include <streamlog/streamlog.h>

namespace CALICE {

  Ahc2AppendProcessor aAhc2AppendProcessor;

  Ahc2AppendProcessor::Ahc2AppendProcessor() : marlin::Processor("Ahc2AppendProcessor")
  {

    _description = "Opens a lcio file and appends events. For Noise overlay in digitization";

    _appendFileNames.push_back("append.slcio");
    registerProcessorParameter("AppendFileNames",
    "Name of the lcio input file(s)",
    _appendFileNames,
    _appendFileNames);

    _appendCollectionInputNames.push_back("ToAppend");
    registerProcessorParameter("InputCollections",
    "Name of the input collection(s) to append, "
    "Ahc2Noise",
    _appendCollectionInputNames,
    _appendCollectionInputNames);

    _appendCollectionOutputNames.push_back("Appended");
    registerProcessorParameter("OutputCollections",
    "Name of the appended output collection(s)",
    _appendCollectionOutputNames,
    _appendCollectionOutputNames);

    registerProcessorParameter("RepeatCollections",
    "repeat or process collection(s) "
    "only one-time",
    _appendRepeatCollections,
    true);
  }

  void Ahc2AppendProcessor::init()
  {

    if (_appendCollectionInputNames.size() != _appendCollectionOutputNames.size()) {
      throw std::runtime_error("Ahc2AppendProcessor::Init: "
      "There must be exactly one entry "
      "in AppendOutputCollections for every "
      "entry in AppendInputCollections.");
    }

    printParameters();

    _lcReader = lcio::LCFactory::getInstance()->createLCReader();
    _lcReader->open(_appendFileNames);
  }

  void Ahc2AppendProcessor::processEvent(LCEvent *evt)
  {

    lcio::LCEvent* appendEvent;

    streamlog_out(DEBUG) << "reading append event" << std::endl;

    appendEvent = _lcReader->readNextEvent();
    if(!appendEvent) {

      if(!_appendRepeatCollections) {
        throw EVENT::DataNotAvailableException("Ahc2AppendProcessor: "
        "No event left in "
        "append stream.");
      }

      streamlog_out(WARNING) << "Collection is repeated" << std::endl;
      _lcReader->close();
      _lcReader->open(_appendFileNames);
      appendEvent = _lcReader->readNextEvent();
    }

    int eventNumber = appendEvent->getEventNumber();

    for (unsigned iCol=0; iCol < _appendCollectionInputNames.size(); iCol++)
    {
      try {
        lcio::LCCollection* _appendCol =
        appendEvent->takeCollection(_appendCollectionInputNames[iCol]);

        evt->addCollection(_appendCol, _appendCollectionOutputNames[iCol]);
      }
      catch (EVENT::DataNotAvailableException &e)
      {
        streamlog_out(WARNING) << "collection "
        << _appendCollectionInputNames[iCol]
        << " not found in append event "<<eventNumber << std::endl;
      }
    }
  }
}
