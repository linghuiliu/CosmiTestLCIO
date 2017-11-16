#include "Ahc2CollectionMerger.hh"
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <EVENT/LCCollection.h>
#include <IMPL/LCCollectionVec.h>
#include "marlin/Exceptions.h"
#include <collection_names.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"
#include "UTIL/BitField64.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

#include <streamlog/streamlog.h>

namespace CALICE
{

  Ahc2CollectionMerger aAhc2CollectionMerger;

  Ahc2CollectionMerger::Ahc2CollectionMerger() : marlin::Processor("Ahc2CollectionMerger")
  {

    _description = "Open a slcio file and merge collections in the same event to one new collection";

    _CollectionInputNames.push_back("ToAppend");
    registerProcessorParameter("InputCollections",
    "Name of the input collection(s) to append, "
    "Ahc2CalorimeterHits_SSF",
    _CollectionInputNames,
    _CollectionInputNames);

    registerProcessorParameter("OutputCollection",
    "Name of the appended output collection",
    _CollectionOutputName,
    std::string("Ahc2CalorimeterHits"));
  }

  void Ahc2CollectionMerger::init()
  {

    if (_CollectionInputNames.size() == 0) {
      throw std::runtime_error("Ahc2CollectionMerger::Init: "
      "There must be at least one entry "
      "in AppendInputCollections.");
    }

    printParameters();
  }

  void Ahc2CollectionMerger::processEvent(LCEvent *evt)
  {
    streamlog_out(DEBUG) << "reading event " << evt->getEventNumber() << std::endl;

    LCCollectionVec *OutputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    LCFlagImpl hitFlag(OutputCol->getFlag());
    hitFlag.setBit(LCIO::RCHBIT_TIME);
    hitFlag.setBit(LCIO::CHBIT_LONG);
    OutputCol->setFlag(hitFlag.getFlag());

    for( StringVec::const_iterator it = _CollectionInputNames.begin(); it != _CollectionInputNames.end() ; it++)
    {
      streamlog_out(DEBUG0)<< "Collection : " << *it << std::endl;
      LCCollection *inCol = NULL;

      try{
        inCol = dynamic_cast<LCCollection*>(evt->getCollection(*it));
      }
      catch ( EVENT::DataNotAvailableException &e )
      {
        streamlog_out(DEBUG) << "Collection " << *it << " not available" <<std::endl;
        continue;
      }

      CellIDDecoder<CalorimeterHit> decoder(inCol);
      _encoding = inCol->getParameters().getStringVal("CellIDEncoding");
      CellIDEncoder<CalorimeterHitImpl> encoder(_encoding.c_str(), OutputCol);


      for(int cellCounter = 0; cellCounter < inCol->getNumberOfElements(); cellCounter++)
      {
        CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inCol->getElementAt(cellCounter));

        CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
        newHit->setEnergy(hit->getEnergy());
        newHit->setEnergyError(0.);
        newHit->setTime(hit->getTime());
        newHit->setType(hit->getType());
        newHit->setPosition(hit->getPosition());
        newHit->setCellID0(hit->getCellID0());

        //Create a new hit in the output Collection
        OutputCol->addElement(newHit);
      }
    }

    LCParameters &theParam = OutputCol->parameters();
    theParam.setValue(LCIO::CellIDEncoding, _encoding);

    evt->addCollection(OutputCol, _CollectionOutputName.c_str());
    streamlog_out(DEBUG0)<<"Add collection  "<< _CollectionOutputName << " with " << OutputCol->getNumberOfElements() << " Hits" << std::endl;
  }
}
