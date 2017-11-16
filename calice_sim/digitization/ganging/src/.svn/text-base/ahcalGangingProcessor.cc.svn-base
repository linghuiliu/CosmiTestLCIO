#include "ahcalGangingProcessor.hh"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"
#include "EVENT/SimCalorimeterHit.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCCollectionVec.h"

#include "streamlog/streamlog.h"

#include <iostream>
#include <vector>

using namespace lcio;
using namespace CALICE::AHCAL::Digitization::Ganging;

ahcalGangingProcessor aahcalGangingProcessor;

ahcalGangingProcessor::ahcalGangingProcessor() : marlin::Processor("ahcalGangingProcessor") {

  _description = "The HCAL ganging Processor.";

  registerInputCollection(LCIO::SIMCALORIMETERHIT,
                          "RequiredCollection",
                          "Name of the collection to be ganged",
                          _inputCollectionName,
                          std::string("hcalSD"));

  registerOutputCollection(LCIO::CALORIMETERHIT,
                          "OutputCollection",
                          "Name of the output collection",
                          _outputCollectionName,
                          std::string("AfterGanging"));
                          

  registerProcessorParameter("TileBorderAttenuationFactor",
                             "",
                             _tileBorderAttenuationFactor,
                             float(1));

}

/** Implementation of ganger
 */
class innerFine : public Ganger {

public:
  
  innerFine(ContributionMap* cm, float factor) : Ganger(3,1,1,cm,factor) {}

  bool responsible(GeometricalIndices indices) {

    return indices.I >= 31 && indices.I < 61 &&
           indices.J >= 31 && indices.J < 61;

  }
  
};

/** Implementation of ganger
 */
class ringFine : public Ganger {

public:
  
  ringFine(ContributionMap* cm, float factor) : Ganger(6,1,1,cm,factor) {}

  bool responsible(GeometricalIndices indices) {

    return !(indices.I >= 31 && indices.I < 61 &&
             indices.J >= 31 && indices.J < 61 ) 
      
           &&

           indices.I >= 13 && indices.I < 79 &&
           indices.J >= 13 && indices.J < 79;

  }
  
};

/** Implementation of ganger
 */
class innerCoarse : public Ganger {

public:
  
  innerCoarse(ContributionMap* cm, float factor) : Ganger(6,1,1,cm,factor) {}

  bool responsible(GeometricalIndices indices) {

    return indices.I >= 13 && indices.I < 79 &&
           indices.J >= 13 && indices.J < 79;

  }
  
};

/** Implementation of ganger
 */
class topBoth : public Ganger {

public:
  
  topBoth(ContributionMap* cm, float factor) : Ganger(12,1,7,cm,factor) {}

  bool responsible(GeometricalIndices indices) {

    return indices.I >= 13 && indices.I < 73 &&
           indices.J >= 79 && indices.J < 91;

  }
  
};

/** Implementation of ganger
 */
class leftBoth : public Ganger {

public:
  
  leftBoth(ContributionMap* cm, float factor) : Ganger(12,1,1,cm,factor) {}

  bool responsible(GeometricalIndices indices) {

    return indices.I >= 1  && indices.I < 13 &&
           indices.J >= 13 && indices.J < 73;

  }
  
};

/** Implementation of ganger
 */
class bottomBoth : public Ganger {

public:
  
  bottomBoth(ContributionMap* cm, float factor) : Ganger(12,7,1,cm,factor) {}

  bool responsible(GeometricalIndices indices) {

    return indices.I >= 19 && indices.I < 79 &&
           indices.J >= 1  && indices.J < 13;

  }
  
};

/** Implementation of ganger
 */
class rightBoth : public Ganger {
  
public:
  
  rightBoth(ContributionMap* cm, float factor) : Ganger(12,7,7,cm,factor) {}

  bool responsible(GeometricalIndices indices) {

    return indices.I >= 79 && indices.I < 91 &&
           indices.J >= 19 && indices.J < 79;

  }
  
};

void ahcalGangingProcessor::init() {
  
  printParameters();

  //  _gangMap.clear();

   // all ganger for a fine module are gathered

  _gangersForFineModules.push_back(new innerFine( &(this->_contributionMap),
                                                    this->_tileBorderAttenuationFactor));

  _gangersForFineModules.push_back(new ringFine(  &(this->_contributionMap),
                                                    this->_tileBorderAttenuationFactor));

  _gangersForFineModules.push_back(new topBoth(   &(this->_contributionMap),
                                                    this->_tileBorderAttenuationFactor));

  _gangersForFineModules.push_back(new bottomBoth(&(this->_contributionMap),
                                                    this->_tileBorderAttenuationFactor));

  _gangersForFineModules.push_back(new leftBoth(  &(this->_contributionMap),
                                                    this->_tileBorderAttenuationFactor));

  _gangersForFineModules.push_back(new rightBoth( &(this->_contributionMap),
                                                    this->_tileBorderAttenuationFactor));

  // all ganger for a fine module are gathered

  _gangersForCoarseModules.push_back(new innerCoarse(&(this->_contributionMap),
                                                       this->_tileBorderAttenuationFactor));

  _gangersForCoarseModules.push_back(new topBoth(    &(this->_contributionMap),
                                                       this->_tileBorderAttenuationFactor));

  _gangersForCoarseModules.push_back(new bottomBoth( &(this->_contributionMap),
                                                       this->_tileBorderAttenuationFactor));

  _gangersForCoarseModules.push_back(new leftBoth(   &(this->_contributionMap),
                                                       this->_tileBorderAttenuationFactor));

  _gangersForCoarseModules.push_back(new rightBoth(  &(this->_contributionMap),
                                                       this->_tileBorderAttenuationFactor));

  // assigning the module type to layer map, needs cleanup

  for(unsigned int i = 1; i != 31; ++i) {
    _moduleTypeOrder[i] = fine;
  }

  for(unsigned int i = 31; i != 39; ++i) {
    _moduleTypeOrder[i] = coarse;
  }

}

void ahcalGangingProcessor::processEvent(lcio::LCEvent* evt) {

  try {

  LCCollection* inVector = evt->getCollection(_inputCollectionName);

  CellIDDecoder<SimCalorimeterHit> cid(inVector);

  for(int i = 0; i != inVector->getNumberOfElements(); ++i) {

    SimCalorimeterHit* hit = dynamic_cast<SimCalorimeterHit*>(
                                     inVector->getElementAt(i));

    GeometricalIndices ijk = {cid(hit)["I"],
                              cid(hit)["J"],
                              cid(hit)["K"]};

    //float energy = hit->getEnergy();

    bool gangerFound = false;

    if(_moduleTypeOrder[ijk.K] == fine) {

      for(unsigned int j = 0; j != _gangersForFineModules.size(); ++j) {

        if ( _gangersForFineModules[j]->addHit(ijk,hit) == true) {
          gangerFound = true;
          break;
        }
        
      }
      
    } else if(_moduleTypeOrder[ijk.K] == coarse) {

      gangerFound = false;

      for(unsigned int j = 0; j != _gangersForCoarseModules.size(); ++j) {

        if ( _gangersForCoarseModules[j]->addHit(ijk,hit) == true) {
          gangerFound = true;
          break;
        }
        
      }

    } else {

      streamlog_out(WARNING) << "Unknown module layer..." << std::endl;

    }

    if(gangerFound == false) {
      
      streamlog_out(DEBUG0) << "No ganger found for: " << ijk << std::endl;

    }

  }

  createOutputCollections(evt);

  } catch (lcio::DataNotAvailableException &e) {

  }

  tidyUp();

}

void ahcalGangingProcessor::createOutputCollections(LCEvent* evt) {

  LCCollectionVec* outCol = new LCCollectionVec(LCIO::CALORIMETERHIT);

  CellIDDecoder<SimCalorimeterHit> cid(evt->getCollection(_inputCollectionName));
  CellIDEncoder<CalorimeterHitImpl> cie("I:16:8,J:8:8,K:0:8",outCol);


  for(ContributionMap::iterator it = _contributionMap.begin();
      it != _contributionMap.end();
      ++it) {

    streamlog_out(DEBUG0) << (*it).first << std::endl;

    CalorimeterHitImpl* newHit = new CalorimeterHitImpl();

    cie["I"] = (*it).first.I;
    cie["J"] = (*it).first.J;
    cie["K"] = (*it).first.K;
  
    cie.setCellID( newHit );
  
   // for(unsigned int j = 0; j != (*it).second.size(); ++j) {
   // 
   //   streamlog_out(DEBUG0) << "  I: " << cid( (*it).second[j] )["I"]
   //                          << " J: " <<  cid( (*it).second[j] )["J"] 
   //                          << " K: " <<  cid( (*it).second[j] )["K"] 
   //                          << std::endl;
   //   
   // }

    newHit->setEnergy( (*it).second );
  
    outCol->addElement(newHit);

  }

  evt->addCollection(outCol,_outputCollectionName);

}

void ahcalGangingProcessor::tidyUp() {

  //_gangMap.clear();
  _contributionMap.clear();

}

void ahcalGangingProcessor::end() {

  for(unsigned int j = 0; j != _gangersForFineModules.size(); ++j) {

    delete _gangersForFineModules[j];

  }

  for(unsigned int j = 0; j != _gangersForCoarseModules.size(); ++j) {

    delete _gangersForCoarseModules[j];

  }

}

//float ahcalGangingProcessor::energySum(std::vector<SimCalorimeterHit*> vecSimHits) {
//
//  float esum(0);
//
//  for(unsigned int i=0; i != vecSimHits.size() ; ++i) {
//
//    esum += vecSimHits[i]->getEnergy();
//
//  }
//
//  return esum;
//
//}
