#include "fastMappingMCProcessor.hh"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <marlin/ConditionsProcessor.h>
#include <FastCaliceHit.hh>
#include <HcalCellIndex.hh>
#include <HcalTileIndex.hh>

#include <UTIL/CellIDDecoder.h>
#include <UTIL/CellIDEncoder.h>

//#define HCALRECO_DEBUG

namespace CALICE { 

fastMappingMCProcessor aFastMappingMCProcessor;


fastMappingMCProcessor::fastMappingMCProcessor(): Processor("fastMappingMCProcessor"),
      _moduleTypeChange(this,&fastMappingMCProcessor::moduleTypeChanged),
      _moduleLocationChange(this,&fastMappingMCProcessor::moduleLocationChanged),
      _moduleConnectionChange(this,&fastMappingMCProcessor::moduleConnectionChanged)
 {

  _description = "This processor reads simulated CalorimeterHits and transforms "
    "them into hardware dependent FastCaliceHits.";

  registerProcessorParameter("InputCollectionName", "Name of the input "
                             "collection",
                           _inputColName, std::string("AhcCalorimeterHits"));
			   
  registerProcessorParameter("OutputCollectionName", "Name of the output "
                             "collection",
                           _outputColName, std::string("CaliceHits1"));
			   
  registerProcessorParameter("ModuleConnectionCollectionName", 
      			     "Name of the conditions data collection which "
                             "describes the connection between modules and the "
                             "DAQ front-ends "
                             "(folder /CaliceEcal/module_connection)",
			     _colNameModuleConnection,
    			     std::string("ModuleConnection"));

  registerProcessorParameter("ModuleLocationCollectionName", 
    			     "Name of the conditions data collection which "
                             "contains the description of the module location "
                             "(folder /CaliceEcal/module_location)",
			     _colNameModuleLocation,
    			     std::string("ModuleLocation"));

  registerProcessorParameter("ModuleDescriptionCollectionName", 
    			     "Name of the conditions data collection which "
                             "contains the description of the module location "
                             "(folder /CaliceEcal/module_description)",
			     _colNameModuleDescription,
    			     std::string("ModuleDescription"));

  registerProcessorParameter("ViewMapping", 
    			     "View the mapping between channels and modules "
                             "when ever the module location or module "
                             "connection conditions data change "
                             "(set to 0 or !=0)",
			     _viewConnectionTree,
    			     0);
}


void fastMappingMCProcessor::init() {

  std::stringstream message;
  message << "fastMappingMCProcessor: undefined conditionsdata: ";
  bool error = false;
  try {
    if (!marlin::ConditionsProcessor::registerChangeListener
        ( &_moduleLocationChange,_colNameModuleLocation)) {
      message << " " << _colNameModuleLocation;
      error = true;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener
        ( &_moduleConnectionChange, _colNameModuleConnection)) {
      message << " " << _colNameModuleConnection;
      error = true;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener
        ( &_moduleTypeChange,_colNameModuleDescription) ) {
      message << " " << _colNameModuleDescription;
      error = true;
    }
  }
  catch (ErrorMissingConditionsDataHandler &conddata_error) {
    // --- catch conditions data handler registration errors
    //   ... and build a combined error message.
    std::string a(conddata_error.what());
    error = true;
    if (a.size()>0) {
      a.erase(a.size()-1);
      message << a; 
    }
  }
  if (error) { 
    message <<  ".";
    throw ErrorMissingConditionsDataHandler(message.str());
  }
  _mapping.init();
  _mapping.setViewConnectionTree(_viewConnectionTree!=0);
  printParameters();
}


void fastMappingMCProcessor::processEvent(LCEvent* evt) {

  try {

    LCCollection* inVector = evt->getCollection(_inputColName);
    LCCollectionVec* _outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    LCFlagImpl hitFlag(_outputCol->getFlag());
    hitFlag.setBit(LCIO::RCHBIT_TIME);
    hitFlag.setBit(LCIO::CHBIT_ID1);
    hitFlag.setBit(LCIO::CHBIT_LONG);
    _outputCol->setFlag(hitFlag.getFlag());

    std::string encodingString = HcalCellIndex::getEncodingString(0) + "," 
      + HcalTileIndex::getEncodingString(32);

    lcio::CellIDEncoder<CalorimeterHitImpl> outgoingCellIDEncoder(encodingString, _outputCol);

    lcio::CellIDDecoder<CalorimeterHit> incomingCellIDDecoder(inVector);

    for (unsigned i = 0; i < static_cast<unsigned>(inVector->getNumberOfElements()); i++) {

      CalorimeterHit* oldHit = dynamic_cast<CalorimeterHit*>(inVector->getElementAt(i));

      /* Unfortuately we have to transform the mokka conform HCAL cell
         indices into our own HCAL cell indices because we need to have cell
         indices in all detectors from which you can get the layer number
         without knowing the type of the detector at all otherwise mip
         finding and event display do not work.  The transformation however
         should be moved into the ganging processor */

      unsigned aTileRow =    incomingCellIDDecoder(oldHit)["J"];
      unsigned aTileColumn = incomingCellIDDecoder(oldHit)["I"];
      unsigned aLayer =      incomingCellIDDecoder(oldHit)["K"];

      HcalCellIndex _geomCellIndex(aTileRow, aTileColumn, aLayer);

      const std::pair<unsigned, unsigned> 
        module_and_cell_index = 
        _indexLookup.getModuleAndCellIndex(_mapping, _geomCellIndex);

      const unsigned _moduleIndex = module_and_cell_index.first;

      if (_moduleIndex==0xFF) {
        std::cout << "fastMappingMCProcessor::processEvent(): "
          "skipped unknown module (" 
                  << aTileRow << ", " 
                  << aTileColumn  << ", " 
                  << aLayer 
                  << ")" 
                  << std::endl;
	continue;
      }

      const unsigned short _moduleType = _mapping.getModuleType(_moduleIndex);
      const unsigned _cellIndex = module_and_cell_index.second;

      if (_cellIndex==0xFF) {
        std::cout << "fastMappingMCProcessor::processEvent(): "
          "skipped unknown cell (" 
                  << aTileRow << ", " 
                  << aTileColumn  << ", " 
                  << aLayer << ")" 
                  << std::endl;
	continue;
      }

      const unsigned _chip = 
        _cellIndex / 18 + (((_moduleType == 5) || (_moduleType == 7)) ? 6 : 0);

      const unsigned _channel = _cellIndex % 18;
      const float _energy = oldHit->getEnergy();

      /*
        modulenumber is the module production number, 1-38
       */
      const unsigned int modulenumber = _mapping.getModuleID(_moduleIndex);

      CalorimeterHitImpl* pNewHit = new CalorimeterHitImpl();

      outgoingCellIDEncoder["module"] = modulenumber;
      outgoingCellIDEncoder["chip"] = _chip;
      outgoingCellIDEncoder["channel"] = _channel;

      outgoingCellIDEncoder["I"] = _geomCellIndex.getTileColumn();
      outgoingCellIDEncoder["J"] = _geomCellIndex.getTileRow();
      outgoingCellIDEncoder["K-1"] = _geomCellIndex.getLayerIndex() - 1;

      outgoingCellIDEncoder.setCellID(pNewHit);

      pNewHit->setEnergy(_energy);

      ThreeVector_t myPos = _mapping.getPosition(_moduleIndex, _cellIndex);

      pNewHit->setPosition(myPos.data());
      

#ifdef HCALRECO_DEBUG
      std::cout <<"Hit: "<< i 
                << ": geomID=0x" << std::hex << _geomCellIndex.getCellIndex() 
                << std::dec
		<< " mod=" << _moduleIndex << " " << modulenumber 
		<< " type=" << _moduleType 
                << " cellIndex=0x" << std::hex << _cellIndex
		<< " chip=0x" << _chip 
                << " chan=0x" << _channel 
                << std::dec << std::endl;
#endif

      _outputCol->addElement(pNewHit);

    }
    evt->addCollection(_outputCol, _outputColName);
  }

  catch (DataNotAvailableException &e) {
#ifdef HCALRECO_DEBUG
      std::cout << "fastMappingMCProcessor::processEvent(): "
        "data not available exception" << std::endl;
#endif
  }
}


void fastMappingMCProcessor::moduleTypeChanged(lcio::LCCollection* col) {
  _mapping.moduleTypeChanged(col);
  if (_mapping.isModuleConditionsDataComplete())
    _indexLookup.createIndexReverseLookup(_mapping);
};


void fastMappingMCProcessor::moduleLocationChanged(lcio::LCCollection* col) {
  _mapping.moduleLocationChanged(col);
  if (_mapping.isModuleConditionsDataComplete())
    _indexLookup.createIndexReverseLookup(_mapping);
};


void fastMappingMCProcessor::moduleConnectionChanged(lcio::LCCollection* col) {
  _mapping.moduleConnectionChanged(col);
  if (_mapping.isModuleConditionsDataComplete())
    _indexLookup.createIndexReverseLookup(_mapping);
};

}
