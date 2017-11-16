#include "fastMappingIIProcessor.hh"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <marlin/ConditionsProcessor.h>
#include "FastCaliceHit.hh"
#include "AdcBlock.hh"
#include "HcalCellIndex.hh"
#include <HcalTileIndex.hh>

//#define HCALRECO_DEBUG

namespace CALICE { 

fastMappingIIProcessor aFastMappingIIProcessor;


fastMappingIIProcessor::fastMappingIIProcessor(): Processor("fastMappingIIProcessor"),
      _moduleTypeChange(this,&fastMappingIIProcessor::moduleTypeChanged),
      _moduleLocationChange(this,&fastMappingIIProcessor::moduleLocationChanged),
      _moduleConnectionChange(this,&fastMappingIIProcessor::moduleConnectionChanged),
      _detectorTransformationChange(this,&fastMappingIIProcessor::detectorTransformationChanged),
      _referenceTransformationChange(this,&fastMappingIIProcessor::referenceTransformationChanged),
      _stagePositionChange(this,&fastMappingIIProcessor::stagePositionChanged)
 {

  _description = "This processor reads calibrated CaliceHits and applies part II of the mapping.";

  registerProcessorParameter("InputCollectionName", "Name of the input collection",
                           _inputColName, std::string("CaliceHits999"));
			   
  registerProcessorParameter("OutputCollectionName", "Name of the output collection",
                           _outputColName, std::string("CalorimeterHits"));
			   
  registerProcessorParameter("ModuleConnectionCollectionName", 
      			     "Name of the conditions data collection which describes the connection between modules and the DAQ front-ends (folder /CaliceEcal/module_connection)",
			     _colNameModuleConnection,
    			     std::string("ModuleConnection"));

  registerProcessorParameter("ModuleLocationCollectionName", 
    			     "Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_location)",
			     _colNameModuleLocation,
    			     std::string("ModuleLocation"));

  registerProcessorParameter("ModuleDescriptionCollectionName", 
    			     "Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_description)",
			     _colNameModuleDescription,
    			     std::string("ModuleDescription"));

  registerProcessorParameter("DetectorTransformationCollectionName", 
    			     "Name of the conditions data collection which contains the detetcor transformation",
			     _colNameDetectorTransformation,
    			     std::string("DetectorTransformation"));

  registerProcessorParameter("ReferenceTransformationCollectionName",
                             "Name of the conditions data collection which describes the position and rotation of the reference coordinate system (folder e.g. /Calice/ECAL/Transformation)",
                             _colNameReferenceTransformation,
                             std::string("ReferenceTransformation"));

  registerOptionalParameter("StageCollectionName",
                            "Name of the conditions data collection which contains stage position data",
                            _colNameStageCollection,
                            std::string("StageCollectionNotDefined"));

  registerProcessorParameter("ViewMapping", 
    			     "View the mapping between channels and modules when ever the module location or module connection conditions data change (set to 0 or !=0)",
			     _viewConnectionTree,
    			     0);
}


void fastMappingIIProcessor::init() {

  std::stringstream message;
  message << "fastMappingIIProcessor: undefined conditionsdata: ";
  bool error = false;
  try {
    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleLocationChange,_colNameModuleLocation)) {
      message << " " << _colNameModuleLocation;
      error=true;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleConnectionChange, _colNameModuleConnection)) {
      message << " " << _colNameModuleConnection;
      error=true;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleTypeChange, _colNameModuleDescription) ) {
      message << " " << _colNameModuleDescription;
      error=true;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener( &_detectorTransformationChange, _colNameDetectorTransformation) ) {
      message << " " << _colNameDetectorTransformation;
      //error=true;
    }
    if(_colNameStageCollection != "StageCollectionNotDefined") {
      if (!marlin::ConditionsProcessor::registerChangeListener( &_stagePositionChange ,_colNameStageCollection) ) {
      message << " " << _colNameStageCollection;
      //error=true;
      }
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


void fastMappingIIProcessor::processRunHeader(LCRunHeader* run) {
}


void fastMappingIIProcessor::processEvent(LCEvent* evt) {

  try {
    LCCollection* inVector = evt->getCollection(_inputColName);
    LCCollectionVec* _outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    EVENT::LCParameters & theParam = _outputCol->parameters();
   
    float avgTemp = inVector->parameters().getFloatVal(string("avgTemp"));
    theParam.setValue( "avgTemp", avgTemp );

    vector<float> dummy;
    theParam.setValues( "avgModTemp_value",
                        inVector->parameters().getFloatVals(string("avgModTemp_value"),dummy)
                     );
    vector<int> intdummy;
    theParam.setValues( "avgModTemp_moduleNumber",
                        inVector->parameters().getIntVals(string("avgModTemp_moduleNumber"),intdummy)
                     );

    //Set the cell decoder which might be useful in event displays
    //suggested by Allister
    // 0 <-> CellID0, 32 <-> CellID1
    theParam.setValue(LCIO::CellIDEncoding,HcalCellIndex::getEncodingString(0) + "," + HcalTileIndex::getEncodingString(32));
    // write 3d coordinates
    LCFlagImpl hitFlag(_outputCol->getFlag());
    hitFlag.setBit(LCIO::RCHBIT_LONG);
    // CellID1 is only stored if the flag word (bit CHBIT_ID1) of the collection is set.
    hitFlag.setBit(LCIO::CHBIT_ID1);
    _outputCol->setFlag(hitFlag.getFlag());

    for (int i = 0; i < inVector->getNumberOfElements(); i++) {
      RawCalorimeterHit* aRawCalorimeterHit = dynamic_cast<RawCalorimeterHit*>(inVector->getElementAt(i));
      
      if (!aRawCalorimeterHit) {
        std::stringstream message;
        message << "Collection " 
                << _inputColName 
                << " doesn't contain RawCalorimeterHits/FastCaliceHits" 
                << std::endl;
        throw runtime_error(message.str());
      }  
      
      FastCaliceHit* oldHit = new FastCaliceHit(aRawCalorimeterHit);

      unsigned _moduleIndex = _inverseModuleMap[oldHit->getModuleID()];
      unsigned _cellIndex = oldHit->getChannel() + oldHit->getChip() * 18;
      if (_cellIndex>107) _cellIndex=_cellIndex-108;  
      ThreeVector_t _myPos = _mapping.getPosition(_moduleIndex, _cellIndex);
      CalorimeterHitImpl* aCalorimeterHit = new CalorimeterHitImpl();
      aCalorimeterHit->setCellID0(_mapping.getGeometricalCellIndex(_moduleIndex, _cellIndex));
      aCalorimeterHit->setCellID1(HcalTileIndex(_mapping.getModuleID(_moduleIndex),oldHit->getChip(),oldHit->getChannel()).getIndex());
      aCalorimeterHit->setEnergy(oldHit->getEnergyValue());
      aCalorimeterHit->setTime(oldHit->getTimeStamp());
      aCalorimeterHit->setPosition(_myPos.data());
#ifdef HCALRECO_DEBUG
      HcalCellIndex ci(aCalorimeterHit->getCellID0());
      std::cout << "Hit: " << std::setw(2) << oldHit->getChip() << " " << 
        std::setw(2) << oldHit->getChannel() << " " << 
        std::setw(3) << _cellIndex << " " <<
        std::hex << std::setw(8) << aCalorimeterHit->getCellID0() << std::dec << " " <<
	std::setw(3) << ci.getLayerIndex() << " " <<
	std::setw(3) << ci.getTileColumn() << " " << 
	std::setw(3) << ci.getTileRow() << " " <<
        std::setw(7) << aCalorimeterHit->getEnergy()  << " " << 
        std::setw(7) << aCalorimeterHit->getPosition()[0] << " " << 
	std::setw(7) << aCalorimeterHit->getPosition()[1] << " " <<
        std::setw(7) << aCalorimeterHit->getPosition()[2] << std::endl;	
#endif
      _outputCol->addElement(aCalorimeterHit);  

      delete oldHit;
      
    }
    evt->addCollection(_outputCol, _outputColName);
  }
  catch (DataNotAvailableException &e) {
#ifdef HCALRECO_DEBUG
      std::cout << "mappingIIProcessor::processEvent(): data not available exception" << std::endl;
#endif
  }
}


void fastMappingIIProcessor::check(LCEvent* evt) {

}


void fastMappingIIProcessor::end() {

}


void fastMappingIIProcessor::updateInverseMap() {
  _inverseModuleMap.clear();
  for (unsigned _moduleIndex = 0;  _moduleIndex < _mapping.getNModules(); _moduleIndex++) {
    _inverseModuleMap[(_mapping.getModuleID(_moduleIndex) << 8) + 
                      (unsigned short)_mapping.getModuleType(_moduleIndex)-4] = _moduleIndex;	   
  }
}

}
