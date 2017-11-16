#include "fastMappingIProcessor.hh"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <marlin/ConditionsProcessor.h>
#include "FastCaliceHit.hh"
#include "AdcBlock.hh"

// #define HCALRECO_DEBUG
 
namespace CALICE {



fastMappingIProcessor aFastMappingIProcessor;


fastMappingIProcessor::fastMappingIProcessor(): VRawADCValueProcessor("fastMappingIProcessor") {

  _description = "This processor reads rawCalorimeterHits and applies part I of the mapping.";

  registerProcessorParameter("OutputCollectionName", "Name of the output collection",
                           _outputColName, std::string("CaliceHitsLevel1"));
			   
  registerProcessorParameter("ViewMapping", 
    			     "View the mapping between channels and modules when ever the module location or module connection conditions data change (set to 0 or !=0)",
			     _viewConnectionTree,
    			     0);
  registerProcessorParameter("PickModule",
                             "Select only a single module",
			     _pickModule,
			     0);			     
}


void fastMappingIProcessor::init() {

  std::stringstream message;
  message << "undefined conditionsdata: ";
  bool error=false;
  try {
    VRawADCValueProcessor::init();
  }
  catch (ErrorMissingConditionsDataHandler &conddata_error) {
    // --- catch conditions data handler registration errors
    //   ... and build a combined error message.
    std::string a(conddata_error.what());
    error=true;
    if (a.size()>0) {
      a.erase(a.size()-1);
      message << a; 
    }
  }
  if (error) { 
    message <<  ".";
    throw ErrorMissingConditionsDataHandler(message.str());
  }
  _mapping.setViewConnectionTree(_viewConnectionTree!=0);
  printParameters();
}


void fastMappingIProcessor::processRunHeader(LCRunHeader* run) {
}


void fastMappingIProcessor::processEvent(LCEvent* evt) {

  try {
    LCCollection* inCol = evt->getCollection(_adcColName);
    LCCollectionVec* outCol = new LCCollectionVec( LCIO::RAWCALORIMETERHIT );
    LCFlagImpl hitFlag( outCol->getFlag() );
    hitFlag.setBit( LCIO::RCHBIT_TIME );
    hitFlag.setBit( LCIO::CHBIT_ID1 );
    outCol->setFlag( hitFlag.getFlag() );
    EVENT::LCParameters & theParam = outCol->parameters();
    theParam.setValue(LCIO::CellIDEncoding,HcalTileIndex::getEncodingString(0));
    outCol->setFlag(hitFlag.getFlag());

    for (int i = 0; i < inCol->getNumberOfElements(); i++) {
      AdcBlock* adcBlock = static_cast<AdcBlock*>( inCol->getElementAt(i) );
//      AdcBlock* _adcBlock = static_cast<AdcBlock*>(inVector->getElementAt(i));
      short crate = adcBlock->getCrateID();
      short slot = adcBlock->getSlotID();	
      short fe = adcBlock->getBoardFrontEnd();
      short channel = adcBlock->getMultiplexPosition();
      std::pair<UInt_t,UInt_t> moduleIndices = _mapping.getModuleIndex( crate, slot, fe );
#ifdef HCALRECO_DEBUG
      std::cout << "fastMappingIProcessor::processEvent() ";
      std::cout << crate << "/" << slot << "/" << fe << " -> " << moduleIndices.first << ", " << moduleIndices.second << std::endl; 
#endif
	  if ( moduleIndices.first == UINT_MAX) continue;
	  if ( moduleIndices.second == UINT_MAX) continue;

      /*
	unsigned short _moduleIDs[2];
	unsigned short _moduleTypes[2];
	if (_moduleIndices.first!=UINT_MAX) {
        _moduleIDs[0] = (_mapping.getModuleID(_moduleIndices.first) << 8) + 
	(unsigned short)_mapping.getModuleType(_moduleIndices.first)-4;
        _moduleTypes[0] = (unsigned short)_mapping.getModuleType(_moduleIndices.first)-4;
	}		    
	if (_moduleIndices.second!=UINT_MAX) {
        _moduleIDs[1] = (_mapping.getModuleID(_moduleIndices.second) << 8) + 
	(unsigned short)_mapping.getModuleType(_moduleIndices.second)-4;
        _moduleTypes[1] = (unsigned short)_mapping.getModuleType(_moduleIndices.second)-4;
	}	
      */	    
      for (short kadc = 0; kadc < 12; kadc++) {
        try {
          short chip = kadc;
	  UInt_t moduleIndex; // = moduleIndices.first;
	  UInt_t module; //  = _mapping.getModuleID( moduleIndices.second);
	  UInt_t moduleType; // = HcalTileIndex( module, chip, 0 ).getModuleType();

	  if ( chip<=5 ) {
	    moduleIndex = moduleIndices.first;
	    module = (UInt_t)_mapping.getModuleID( moduleIndices.first );
	    moduleType = (UInt_t)_mapping.getModuleType( moduleIndices.first );
	  } else {
	    moduleIndex = moduleIndices.second;
	    module = (UInt_t)_mapping.getModuleID( moduleIndices.second );
	    moduleType = (UInt_t)_mapping.getModuleType( moduleIndices.second );
	  }
 	  /* fixme: make sure that both parts of module are connected to the same FE */
          /* check if module index is in data base, if not, ignore ADCValue */

	  float energy = adcBlock->getAdcVal( kadc );
	  if ( ( moduleIndex != UINT_MAX) && 
	       (!_pickModule || ( moduleIndex == (UInt_t)_pickModule )) &&
	       // quick fix to remove "virtual" cells from coarse modules, which seem to be read out and converted
	       // to be done: introduce validity flag for cells in ModuleDescription or use a map instead of a vector to relate
	       // cells to chip/channel
	       !(( moduleType==6) && ( chip>3)) &&
	       !(( moduleType==7) && ( chip<8)) &&
	       !(( moduleType==7) && ( chip==8) && ( channel<3))) {
	    
	    FastCaliceHit *aFastCaliceHit = new FastCaliceHit( module, 
							       chip, 
							       channel, 
							       energy, 0., 0 );
#ifdef HCALRECO_DEBUG
            std::cout << "mappingIProcessor::processEvent() ";
            aFastCaliceHit->print(std::cout);
	    std::cout << std::endl;
#endif
            if ( aFastCaliceHit ) outCol->addElement( aFastCaliceHit ); 
	  }
        }
        catch(Exception& e) { 
          std::cout << " Exception: " << e.what() << std::endl;
        }
      }
    }
    evt->addCollection( outCol, _outputColName );
  }
  catch(DataNotAvailableException &e) {
  }  
}


void fastMappingIProcessor::check(LCEvent* evt) {

}


void fastMappingIProcessor::end() {

}

}
