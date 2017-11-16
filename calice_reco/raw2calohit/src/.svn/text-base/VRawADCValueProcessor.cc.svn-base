#include "VRawADCValueProcessor.hh"
#include <marlin/ConditionsProcessor.h>
#include <collection_names.hh>
#include <iostream>
#include <EVENT/LCParameters.h>
#include <EVENT/LCCollection.h>
#include <time.h>
#include <cstdlib>

namespace CALICE {
  VRawADCValueProcessor::VRawADCValueProcessor(const std::string &processor_name)
    : Processor(processor_name),
      _moduleTypeChange(this,&VRawADCValueProcessor::moduleTypeChanged),
      _moduleLocationChange(this,&VRawADCValueProcessor::moduleLocationChanged),
      _moduleConnectionChange(this,&VRawADCValueProcessor::moduleConnectionChanged),
      _stagePositionChange(this,&VRawADCValueProcessor::stagePositionChanged)
  {
    _adcColName=COL_ADC;
    registerProcessorParameter( "ADCCollectionName" , 
			       "The name of the adc collection (input) to be used" ,
			       _adcColName ,
			       _adcColName);


    registerProcessorParameter( "ModuleConnectionCollectionName" , 
    				"Name of the conditions data collection which describes the connection between modules and the DAQ front-ends (folder /CaliceEcal/module_connection)"  ,
				_colNameModuleConnection ,
    				std::string("ModuleConnection") ) ;

    registerProcessorParameter( "ModuleLocationCollectionName" , 
    				"Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_location)"  ,
				_colNameModuleLocation ,
    				std::string("ModuleLocation") ) ;

    registerProcessorParameter( "ModuleDescriptionCollectionName" , 
    				"Name of the conditions data collection which contains the description of the module location (folder /CaliceEcal/module_description)"  ,
				_colNameModuleDescription ,
    				std::string("ModuleDescription") ) ;


    registerOptionalParameter( "StageCollectionName" , 
    				"Name of the conditions data collection which contains stage position data"  ,
				_colNameStageCollection ,
    				std::string("StageCollectionNotDefined") ) ;


  }
  
  void VRawADCValueProcessor::init() 
  {
    // --- register conditions data handler
    
    std::stringstream message;
    message << "VRawADCValueProcessor::init> no conditions data handler for the collections:";
    bool error=false;
    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleLocationChange ,_colNameModuleLocation)) {
      message << " " << _colNameModuleLocation;
      error=true;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleConnectionChange , _colNameModuleConnection)) {
      message << " " << _colNameModuleConnection;
      error=true;
    }

    if (!marlin::ConditionsProcessor::registerChangeListener( &_moduleTypeChange ,_colNameModuleDescription) ) {
      message << " " << _colNameModuleDescription;
      error=true;
    }

    if(_colNameStageCollection != "StageCollectionNotDefined") {
      if (!marlin::ConditionsProcessor::registerChangeListener( &_stagePositionChange ,_colNameStageCollection) ) {
	message << " " << _colNameStageCollection;
	error=true;
      }
    }


    if (error) { 
      message <<  ".";
      throw ErrorMissingConditionsDataHandler(message.str());
    }
       
    _mapping.init();

    // correct the wrong order of the cells in the A-Hcal description data.
    struct tm wrong_order_till;
    wrong_order_till.tm_min=0;
    wrong_order_till.tm_sec=0;
    wrong_order_till.tm_hour=0;
    wrong_order_till.tm_wday=0;
    wrong_order_till.tm_yday=0;
    wrong_order_till.tm_isdst=0;
    wrong_order_till.tm_mon=10;
    wrong_order_till.tm_mday=26;
    wrong_order_till.tm_year=2006-1900;

    _wrongOrderTill=mktime(&wrong_order_till);
  }

  class CellPar_t 
  {
  public:
    CellPar_t() : _index(0),_posX(0),_posY(0) {}
    CellPar_t(float x, float y, unsigned int index) : _index(index),_posX(x),_posY(y) {}
    unsigned int index() const {return _index;}
    const float &posX() const {return _posX;}
    const float &posY() const {return _posY;}
    
    unsigned int _index;
    float _posX;
    float _posY;
  };

  void VRawADCValueProcessor::correctModuleDescriptionOrder(LCCollection *colP)
  {
    assert(colP);
    std::vector<std::string> insertion_time;
    colP->getParameters().getStringVals("DBInsertionTime",insertion_time);
    assert(insertion_time.size()>0);
    if (insertion_time[0].size()>9) {
      insertion_time[0].erase(insertion_time[0].size()-9);
    }
    unsigned int timestamp=atoi(insertion_time[0].c_str());
    double a=timestamp;
    double b=_wrongOrderTill;
    std::string map_key("MappingCorrected");
    std::cout << "time since order was corrected : " << (a-b)/(24*3600.)  << std::endl;
    if (timestamp<_wrongOrderTill && colP->getParameters().getIntVal(map_key)!=1) {
      for (unsigned int elm_i=0; elm_i<static_cast<unsigned int>(colP->getNumberOfElements()); elm_i++) {
	CALICE::ModuleDescription description(colP->getElementAt(elm_i));
        //CRP Taken out since we're still using old ModuleDescription 
        //Might be re-enabled later on
	//if (description.hasCellDimensionsPerCell()) continue;
	if (description.getModuleType()>=4 && description.getModuleType()<=7) {
	  std::string folder_name;
	  folder_name = colP->getParameters().getStringVal("DBFolder");
	  std::cout << "flip mapping : " << folder_name << std::endl;
	  std::vector<CellPar_t> cell_buffer;
	  cell_buffer.resize(description.getNCells());
	  UInt_t n_lines=(description.getNCells()>=216  ? 12 : 6);
	  for (unsigned int cell_i=0; cell_i<description.getNCells(); cell_i++) {
	    unsigned int new_cell_i=(cell_i%18)*n_lines+cell_i/18;
	    assert(new_cell_i < description.getNCells());
	    cell_buffer[new_cell_i]=CellPar_t(description.getCellXPos(cell_i),
					      description.getCellYPos(cell_i),
					      description.getGeometricalCellIndex(cell_i));
	  }
	  for (unsigned int new_cell_i=0; new_cell_i<description.getNCells(); new_cell_i++) {
	    description.setCellXPos(new_cell_i,cell_buffer[new_cell_i].posX());
	    description.setCellYPos(new_cell_i,cell_buffer[new_cell_i].posY());
	    description.setGeometricalCellIndex(new_cell_i,cell_buffer[new_cell_i].index());
	  }
	}
      }
      colP->parameters().setValue(map_key,static_cast<int>(1));

    }
    
  }


  void VRawADCValueProcessor::showCollectionParameters(LCCollection *colP)
  {
    assert(colP);
    std::vector<std::string> insertion_time;
    colP->getParameters().getStringVals("DBInsertionTime",insertion_time);
    std::cout << "InsertionTime : ";
    for (std::vector<std::string>::const_iterator iter=insertion_time.begin();
	 iter!=insertion_time.end();
	 iter++) {
      std::cout << *iter << ", ";
    }
    std::cout << std::endl;

    lcio::StringVec keys;
    colP->getParameters().getIntKeys(keys);
    std::cout << "collection parameters:" << std::endl;
    UInt_t counter=0;
    for (lcio::StringVec::const_iterator key_iter=keys.begin();
	 key_iter!=keys.end();
	 key_iter++) {
      std::cout << "int " << counter++ << ":"
		<< *key_iter << " (" << colP->getParameters().getNInt(*key_iter) << ")" << std::endl;
    }
    keys.clear();
    colP->getParameters().getFloatKeys(keys);
    counter=0;
    for (lcio::StringVec::const_iterator key_iter=keys.begin();
	 key_iter!=keys.end();
	 key_iter++) {
      std::cout << "float " << counter++ << ":"
		<< *key_iter << " (" << colP->getParameters().getNFloat(*key_iter) << ")" << std::endl;
    }
    keys.clear();
    colP->getParameters().getStringKeys(keys);
  counter=0;
    for (lcio::StringVec::const_iterator key_iter=keys.begin();
	 key_iter!=keys.end();
	 key_iter++) {
      std::cout << "string " << counter++ << ":"
		<< *key_iter << " (" << colP->getParameters().getNString(*key_iter) << ")" << std::endl;
    }
  }
}
