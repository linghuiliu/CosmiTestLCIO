#include "CellDescriptionProcessor.hh"

/* Marlin includes*/
#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"

/* CALICE includes*/
#include "Mapper.hh"
#include "MappingProcessor.hh"
#include "CellDescriptionGenerator.hh"

namespace CALICE {

  /* generate instances of static objects*/
  std::map<std::string, MappedContainer<CellDescription>*> CellDescriptionProcessor::_cellDescriptionContainerMap;

  /************************************************************************/
  /*                                                                      */
  /*                                                                      */
  /*                                                                      */
  /************************************************************************/
  MappedContainer<CellDescription>* CellDescriptionProcessor::getCellDescriptions(const std::string& processorName) 
  {
    return _cellDescriptionContainerMap[processorName];
  }

  /************************************************************************/
  /*                                                                      */
  /*                                                                      */
  /*                                                                      */
  /************************************************************************/
  CellDescriptionProcessor::CellDescriptionProcessor() : Processor("CellDescriptionProcessor") 
  {
    _description = "Processor that provides a MappedContainer of CALICE CellDescription objects from conditions data";

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "ModuleConnection" ,
                             "Name of the ModuleConnection collection"  ,
                             _colNameModuleConnection ,
                             std::string("ModuleConnection") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "ModuleDescription" ,
                             "Name of the ModuleDescription collection"  ,
                             _colNameModuleDescription ,
                             std::string("ModuleDescription") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "ModuleLocation" ,
                             "Name of the ModuleLocation collection"  ,
                             _colNameModuleLocation ,
                             std::string("ModuleLocation") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "DetectorTransformation" ,
                             "Name of the DetectorTransformation collection"  ,
                             _colNameDetectorTransformation ,
                             std::string("DetectorTransformation") ) ;

    registerProcessorParameter( "MappingProcessorName" ,
                                "name of MappingProcessor which takes care of the mapping",
                                _mappingProcessorName,
                                std::string("AhcMappingProcessor") ) ;

  }


  /************************************************************************/
  /*                                                                      */
  /*                                                                      */
  /*                                                                      */
  /************************************************************************/
  void CellDescriptionProcessor::init() 
  {
    /* usually a good idea*/
    printParameters();

    _colModuleDescription      = 0;
    _colModuleConnection       = 0;
    _colModuleLocation         = 0;
    _colDetectorTransformation = 0;

    _moduleDescriptionChanged      = false;
    _moduleConnectionChanged       = false;
    _moduleLocationChanged         = false;
    _detectorTransformationChanged = false;

    std::stringstream message;
    bool error=false;

    _mapper = MappingProcessor::getMapper(_mappingProcessorName);

    streamlog_out(MESSAGE)<<" CellIDEncoding used by Mapper: "<< _mapper->getDecoder()->getCellIDEncoding() << std::endl;
    streamlog_out(MESSAGE)<<" ModuleEncoding used by Mapper: "<< _mapper->getDecoder()->getModuleEncoding() << std::endl;

    if ( ! _mapper ) {
      message << "MappingProcessor::getMapper("<< _mappingProcessorName 
	      << ") did not return a valid mapper." << std::endl;
      error = true;
    }

    if (!ConditionsProcessor::registerChangeListener(this, _colNameModuleConnection)) {
      message << " undefined conditions: " << _colNameModuleConnection << std::endl;
      error=true;
    }

    if (!ConditionsProcessor::registerChangeListener(this, _colNameModuleDescription)) {
      message << " undefined conditions: " << _colNameModuleDescription << std::endl;
      error=true;
    }

    if (!ConditionsProcessor::registerChangeListener(this, _colNameModuleLocation)) {
      message << " undefined conditions: " << _colNameModuleLocation << std::endl;
      error=true;
    }

    if (!ConditionsProcessor::registerChangeListener(this, _colNameDetectorTransformation)) {
      message << " undefined conditions: " << _colNameDetectorTransformation << std::endl;
      error=true;
    }

    if (error) {
      streamlog_out(ERROR) << message.str();
      throw marlin::StopProcessingException(this);
    }

    _cellDescriptionContainerMap[name()] = new MappedContainer<CellDescription>(_mapper);
    _mapperVersion = _mapper->getVersion();
  }


  /************************************************************************/
  /*                                                                      */
  /*                                                                      */
  /*                                                                      */
  /************************************************************************/
 void CellDescriptionProcessor::conditionsChanged(  LCCollection *col ) 
  {
    std::string colName = col->getParameters().getStringVal("CollectionName") ;
  
    if (colName == _colNameModuleDescription)
      {
	_colModuleDescription     = col;
	_moduleDescriptionChanged = true;
      }
    else if (colName == _colNameModuleConnection)
      {
	_colModuleConnection     = col;
	_moduleConnectionChanged = true;
      }
     else if (colName == _colNameModuleLocation)
      {
	_colModuleLocation     = col;
	_moduleLocationChanged = true;
      }
    else if (colName == _colNameDetectorTransformation)
      {
	_colDetectorTransformation     = col;
	_detectorTransformationChanged = true;
      }

    
  }


  /************************************************************************/
  /*                                                                      */
  /*                                                                      */
  /*                                                                      */
  /************************************************************************/
  void CellDescriptionProcessor::generateCellDescription() 
  {
    CellDescriptionGenerator generator(_mapper);
    generator.generate(_colModuleDescription, _colModuleConnection, _colModuleLocation, 
		       _colDetectorTransformation, _cellDescriptionContainerMap[name()]);

    _mapperVersion = _mapper->getVersion();
    _moduleConnectionChanged       = false;
    _moduleDescriptionChanged      = false;
    _moduleLocationChanged         = false;
    _detectorTransformationChanged = false;

  }

  /************************************************************************/
  /*                                                                      */
  /*                                                                      */
  /*                                                                      */
  /************************************************************************/
  void CellDescriptionProcessor::processEvent( LCEvent *evt ) 
  {
    if ( (_mapper->getVersion() != _mapperVersion ) || _detectorTransformationChanged 
	 ||_moduleLocationChanged || _moduleDescriptionChanged || _moduleConnectionChanged  )
      generateCellDescription();
  }

  /************************************************************************/
  /*                                                                      */
  /*                                                                      */
  /*                                                                      */
  /************************************************************************/
  void CellDescriptionProcessor::end() 
  {
    delete _cellDescriptionContainerMap[name()];
    _cellDescriptionContainerMap.erase(name());
  }


  /* create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   */
  CellDescriptionProcessor aCellDescriptionProcessor;

} // end namespace CALICE
