#include "ScECALMappingProcessor.hh"
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include "FastCaliceHit.hh"
#include "AdcBlock.hh"
#include "HcalCellIndex.hh"
#include <HcalTileIndex.hh>

//110906.coterra using DB
#include "lccd/IConditionsHandler.hh"
#include "ScECALMapping.hh"
#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/ConditionsProcessor.h>
#include <ConditionsChangeDelegator.hh>   //(+)
#include <collection_names.hh>
#include <lccd/DBInterface.hh>
#include <lcio.h>
#include <lccd.h>
//#include <VRawADCValueProcessor.hh>
#include <CalibrationSet.hh>
#include <GainConstants.hh>
#include <InterConstants.hh>
#include <MIPConstants.hh>
#include <TriggerBits.hh>
#include <set>
#include "EventHeader.hh"

#define fromLocalFile 0

//#define HCALRECO_DEBUG

namespace CALICE { 

  //ScECALMappingProcessor aFastMappingIIProcessor;
  ScECALMappingProcessor aScECALMappingProcessor;

  ScECALMappingProcessor::ScECALMappingProcessor(): Processor("ScECALMappingProcessor")//,
						    //_moduleTypeChange(this,&ScECALMappingProcessor::moduleTypeChanged),
						    //_moduleLocationChange(this,&ScECALMappingProcessor::moduleLocationChanged),
						    //_moduleConnectionChange(this,&ScECALMappingProcessor::moduleConnectionChanged),
						    //_detectorTransformationChange(this,&ScECALMappingProcessor::detectorTransformationChanged),
						    //_referenceTransformationChange(this,&ScECALMappingProcessor::referenceTransformationChanged),
						    //_stagePositionChange(this,&ScECALMappingProcessor::stagePositionChanged)
  {

    _description = "This processor reads CALDAQ_ADCColl and applies the ScECAL mapping.";

    registerProcessorParameter("MappingData",
                               "Temporally 2009 MappingData is default",
                               _mappingData,
                               std::string("/home/coterra/tempo_data/ScECALmapping_MayFNAL.dat") );  
                               //std::string("/scratch/coterra/frmAzusa/scecal_codes/calib_data/ScECALmapping_MayFNAL.dat") );  
                               //std::string("/scratch/coterra/frmAzusa/scecal_codes/calib_data/ScECALmapping_v0.dat") );  // 2008

    registerProcessorParameter("InputCollectionName", "Name of the input collection",
			       _inputColName, std::string("CALDAQ_ADCCol"));
    //_inputColName, std::string("CaliceHits999"));

    registerProcessorParameter("OutputCollectionName", "Name of the output collection",
			       _outputColName, std::string("ScECALHitsLevel0"));

    //110906.coterra
    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "ScECALMapping",
                             "Name of the ScECAL mapping collection",
                             _ScECALMappingColName,
			     std::string( "ScECALMapping" ) );
    _ScECALMappingCol     = NULL;      


    //registerProcessorParameter("ModuleConnectionCollectionName", 
    //"Name of the conditions data collection which describes the connection between modules and the DAQ front-ends (folder /CaliceEcal/module_connection)",
    //_colNameModuleConnection,
    //std::string("ModuleConnection"));

    //registerProcessorParameter("ModuleLocationCollectionName", 
    //"Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_location)",
    //_colNameModuleLocation,
    //std::string("ModuleLocation"));

    //registerProcessorParameter("ModuleDescriptionCollectionName", 
    //"Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_description)",
    //_colNameModuleDescription,
    //std::string("ModuleDescription"));

    //registerProcessorParameter("DetectorTransformationCollectionName", 
    //"Name of the conditions data collection which contains the detetcor transformation",
    //_colNameDetectorTransformation,
    //std::string("DetectorTransformation"));

    //registerProcessorParameter("ReferenceTransformationCollectionName",
    //"Name of the conditions data collection which describes the position and rotation of the reference coordinate system (folder e.g. /Calice/ECAL/Transformation)",
    //_colNameReferenceTransformation,
    //std::string("ReferenceTransformation"));

    //registerOptionalParameter("StageCollectionName",
    //"Name of the conditions data collection which contains stage position data",
    //_colNameStageCollection,
    //std::string("StageCollectionNotDefined"));

    //registerProcessorParameter("ViewMapping", 
    //"View the mapping between channels and modules when ever the module location or module connection conditions data change (set to 0 or !=0)",
    //_viewConnectionTree,
    //0);
  }

void ScECALMappingProcessor::conditionsChanged( LCCollection * col )
{
  std::string colName = col->getParameters().getStringVal("CollectionName") ;

  if (colName == _ScECALMappingColName)  {
      _ScECALMappingCol = col;
      _MappingChanged = true;
  }

};

  void ScECALMappingProcessor::init() {

    std::stringstream message;
    message << "ScECALMappingProcessor: undefined conditionsdata: ";
    //bool error = false;
    //try {
    //if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleLocationChange,_colNameModuleLocation)) {
    //	message << " " << _colNameModuleLocation;
    //}
    //
    //if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleConnectionChange, _colNameModuleConnection)) {
    //	message << " " << _colNameModuleConnection;
    //}
    //
    //if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleTypeChange, _colNameModuleDescription) ) {
    //	message << " " << _colNameModuleDescription;
    //}
    //
    //if (!marlin::ConditionsProcessor::registerChangeListener( &_detectorTransformationChange, _colNameDetectorTransformation) ) {
    //	message << " " << _colNameDetectorTransformation;
    //}
    //if(_colNameStageCollection != "StageCollectionNotDefined") {
    //	if (!marlin::ConditionsProcessor::registerChangeListener( &_stagePositionChange ,_colNameStageCollection) ) {
    //	  message << " " << _colNameStageCollection;
    //	  error=true;
    //	}
    //}  
    //}
    //catch (ErrorMissingConditionsDataHandler &conddata_error) {
    //  // --- catch conditions data handler registration errors
    //  //   ... and build a combined error message.
    //  std::string a(conddata_error.what());
    //  error = true;
    //  if (a.size()>0) {
    //	a.erase(a.size()-1);
    //	message << a; 
    //  }
    //}
      
    // if (error) { 
    //   message <<  ".";
    //   throw ErrorMissingConditionsDataHandler(message.str());
    // }
    //_mapping.init();
    //_mapping.setViewConnectionTree(_viewConnectionTree!=0);
    printParameters();

    _MappingChanged = false;

  bool error = false;

//110906.coterra
  if (!marlin::ConditionsProcessor::registerChangeListener(this, _ScECALMappingColName))  {
      message << " undefined conditions: " << _ScECALMappingColName << std::endl;
      error = true;
  }


#if fromLocalFile
    // Read mapping file
    // For now just read from a text data

    for (int slot=0;slot<ScECAL_NSLOT;slot++)
      for (int fe=0;fe<ScECAL_NFE;fe++)
	for (int chip=0;chip<ScECAL_NCHIP;chip++)
	  for (int chan=0;chan<ScECAL_NCHAN;chan++)
	    _ScECALmap[slot][fe][chip][chan] = std::pair<int,int>(0,0);

    

    ifstream file( _mappingData.c_str());
    if(file==0){
      cout<<"!!!ERROR MAPPING FILE DOES NOT EXIST!!!"<<endl;
    }
    int layer,strip,slot,fe,chip,channel,dac;
    double vo, dacover;
    int islot;

    for(int ii=0;ii<2160;ii++){
      file>>layer>>strip>>slot>>fe>>chip>>channel>>dac>>vo>>dacover;
      islot = slot == 5 ? 0 : 1;
      _ScECALmap[islot][fe][chip][channel].first=layer;
      _ScECALmap[islot][fe][chip][channel].second=strip;  
    }  

#endif  // fromLocalFile

  } // init


  void ScECALMappingProcessor::processRunHeader(LCRunHeader* run) {
  }


void ScECALMappingProcessor::processEvent(LCEvent* evt) {

  int nEvent = evt->getEventNumber();

  if (_MappingChanged ) {
    if ( !_ScECALMappingCol ) {
        streamlog_out(ERROR) << "Cannot update Mapping, ScECAL Mapping collection is not valid." << std::endl;
        //        throw StopProcessingException(this);
    }

    for (int slot=0;slot<ScECAL_NSLOT;slot++) {
      for (int fe=0;fe<ScECAL_NFE;fe++) {
        for (int chip=0;chip<ScECAL_NCHIP;chip++) {
          for (int chan=0;chan<ScECAL_NCHAN;chan++) {
            _ScECALmap[slot][fe][chip][chan] = std::pair<int,int>(0,0);
          }
        }
      }
    }

    int layer,strip,slot,fe,chip,channel,dac;
    double vo, dacover;
    int islot;
    for (int i = 0; i < _ScECALMappingCol->getNumberOfElements(); ++i) {
      ScECALMapping * scecalMapping = new ScECALMapping( _ScECALMappingCol->getElementAt(i) );

      layer = scecalMapping->getID0();
      strip = scecalMapping->getID1();
      slot  = scecalMapping->getSLOT();
      fe    = scecalMapping->getFE();
      chip  = scecalMapping->getCHIP();
      channel  = scecalMapping->getCHANNEL();
      dac  = scecalMapping->getDAC();

      islot = slot == 5 ? 0 : 1;

      _ScECALmap[islot][fe][chip][channel].first=layer;
      _ScECALmap[islot][fe][chip][channel].second=strip;
 
      if ( nEvent%1000 == 0 ){
        cout << "Mapping " << layer << "-" << strip << " slot:" << slot << " fe:" << fe 
             << " chip:" << chip << " channel:" << channel << " dac:" << dac 
             << " islot:" << islot << endl;
      }

      delete scecalMapping;

    }

    _MappingChanged = false;
  }



  try {
      LCCollection* inVector = evt->getCollection(_inputColName);
      LCCollectionVec* _outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
      //EVENT::LCParameters & theParam = _outputCol->parameters();

    long64 timestamp = evt->getTimeStamp();


      //Set the cell decoder which might be useful in event displays
      //suggested by Allister
      // 0 <-> CellID0, 32 <-> CellID1
      //theParam.setValue(LCIO::CellIDEncoding,HcalCellIndex::getEncodingString(0) + "," + HcalTileIndex::getEncodingString(32));
      // write 3d coordinates
    LCFlagImpl hitFlag(_outputCol->getFlag());
    hitFlag.setBit(LCIO::RCHBIT_LONG);
      // CellID1 is only stored if the flag word (bit CHBIT_ID1) of the collection is set.
    hitFlag.setBit(LCIO::CHBIT_ID1);
    _outputCol->setFlag(hitFlag.getFlag());

    pair<int,int> chanID;

    for (int i = 0; i < inVector->getNumberOfElements(); i++) {  // i
      AdcBlock adc_block(inVector->getElementAt(i));

	// Skip if it is not the ScECAL crate (=206)
      if (adc_block.getCrateID()!=206) continue;

      for (int kadc = 0; kadc < 12; kadc++) {

	chanID = this->getScECALChannelID(adc_block.getSlotID(),adc_block.getBoardFrontEnd(),
					    kadc,adc_block.getMultiplexPosition());

	CalorimeterHitImpl* aCalorimeterHit = new CalorimeterHitImpl();

	aCalorimeterHit->setCellID0(chanID.first);
	aCalorimeterHit->setCellID1(chanID.second);
	aCalorimeterHit->setEnergy(adc_block.getAdcVal(kadc));
	aCalorimeterHit->setTime(timestamp);
	aCalorimeterHit->setPosition(this->getChannelPosition(chanID.first,chanID.second).data());
	_outputCol->addElement(aCalorimeterHit);   
      } // adc_block

    } // i

    evt->addCollection(_outputCol, _outputColName);
  }  // try
  catch (DataNotAvailableException &e) {
#ifdef HCALRECO_DEBUG
      std::cout << "ScECALMappingProcessor::processEvent(): data not available exception" << std::endl;
#endif

  }

}
  



  void ScECALMappingProcessor::check(LCEvent* evt) {

  }


  void ScECALMappingProcessor::end() {

  }

  /*
  void ScECALMappingProcessor::updateInverseMap() {
    _inverseModuleMap.clear();
    for (unsigned _moduleIndex = 0;  _moduleIndex < _mapping.getNModules(); _moduleIndex++) {
      _inverseModuleMap[(_mapping.getModuleID(_moduleIndex) << 8) + 
			(unsigned short)_mapping.getModuleType(_moduleIndex)-4] = _moduleIndex;	   
    }
  }
  */

}
