#include "BaseMappingIIProcessor.hh"
#include "EVENT/LCCollection.h"
#include "EVENT/LCEvent.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "marlin/ConditionsProcessor.h"

using namespace std;

namespace CALICE {

//BaseMappingIIProcessor aBaseMappingIIProcessor;

BaseMappingIIProcessor::BaseMappingIIProcessor(const string& typeName)
  : Processor(typeName),
    _moduleTypeChange(this,&BaseMappingIIProcessor::moduleTypeChanged),
    _moduleLocationChange(this,&BaseMappingIIProcessor::moduleLocationChanged),
    _moduleConnectionChange(this,&BaseMappingIIProcessor::moduleConnectionChanged),
    _detectorTransformationChange(this,&BaseMappingIIProcessor::detectorTransformationChanged),
    _referenceTransformationChange(this,&BaseMappingIIProcessor::referenceTransformationChanged)
{
  streamlog_out(DEBUG0)<<"*** BaseMappingII:"<< _processorName <<" constructor called"<< endl;
  _description = "Base processor to handle alignment and mapping information, to be specialized for specific detectors.";

  registerProcessorParameter("InputCollectionName", "Name of the input collection",
			     _inputColName, string("InputHits"));

  registerProcessorParameter("OutputCollectionName", "Name of the output collection",
			     _outputColName, string("CalorimeterHits"));

  registerProcessorParameter("ModuleConnectionCollectionName", 
      			     "Name of the conditions data collection which describes the connection between modules and the DAQ front-ends",
			     _colNameModuleConnection,
    			     string("ModuleConnection"));

  registerProcessorParameter("ModuleLocationCollectionName", 
    			     "Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_location)",
			     _colNameModuleLocation,
    			     string("ModuleLocation"));

  registerProcessorParameter("ModuleDescriptionCollectionName", 
    			     "Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_description)",
			     _colNameModuleDescription,
    			     string("ModuleDescription"));

  registerProcessorParameter("DetectorTransformationCollectionName", 
    			     "Name of the conditions data collection which contains the detetcor transformation",
			     _colNameDetectorTransformation,
    			     string("DetectorTransformation"));

  registerProcessorParameter( "ReferenceTransformationCollectionName" ,
			      "Name of the conditions data collection which describes the position and rotation of the reference coordinate system (folder e.g. /Calice/ECAL/Transformation)",
			      _colNameReferenceTransformation,
			      string("ReferenceTransformation") );

}


void BaseMappingIIProcessor::init() {

  streamlog_out(DEBUG0)<<"*** BaseMappingII: init() called"<< endl;
  printParameters();

  stringstream message;
  message << _processorName <<" "<< name() << ": undefined conditions data: ";
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
      error=true;
    }

  }
  catch (ErrorMissingConditionsDataHandler &conddata_error) {
    // --- catch conditions data handler registration errors
    //   ... and build a combined error message.
    string a(conddata_error.what());
    error = true;
    if (a.size()>0) {
      a.erase(a.size()-1);
      message << a; 
    }
  }
  // FIXME: shouldn't this be moved inside the try block above?
  if(error) {
    message <<  ".";
    throw ErrorMissingConditionsDataHandler(message.str());
  }
  _mapping.init();
  _mapping.setViewConnectionTree(_viewConnectionTree!=0);
  streamlog_out(DEBUG0)<<"*** BaseMappingII: init() done!"<< endl;
}


void BaseMappingIIProcessor::processRunHeader(LCRunHeader* run) {
  streamlog_out(DEBUG0)<<"*** BaseMappingII:processRunHeader() called"<< endl;
}


void BaseMappingIIProcessor::check(LCEvent* evt) {
  streamlog_out(DEBUG0)<<"*** BaseMappingII::check() called"<< endl;
}


void BaseMappingIIProcessor::end() {
  streamlog_out(DEBUG0)<<"*** BaseMappingII::end() called"<< endl;
}


void BaseMappingIIProcessor::updateInverseMap() {
  _inverseModuleMap.clear();
  for (unsigned ilocation=0; ilocation<_mapping.getNModules(); ++ilocation) {
    int modID = _mapping.getModuleID(ilocation);
    _inverseModuleMap[ modID ] = ilocation;
  }
}

}
