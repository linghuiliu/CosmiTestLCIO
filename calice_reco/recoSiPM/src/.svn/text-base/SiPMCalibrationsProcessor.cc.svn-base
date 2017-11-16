#include "SiPMCalibrationsProcessor.hh"

#include <list>

// Marlin includes
#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"
#include "lccd/IConditionsHandler.hh"

// CALICE includes
#include "MappingProcessor.hh"
#include "CellIterator.hh"
#include "CellQuality.hh"
#include "SatCorrItep.hh"
#include "LinearFitSlope.hh"
#include "LinearFitConstant.hh"
#include "LinearFitCompound.hh"
#include "SiPMCalibrationStatusBits.hh"



namespace CALICE {

  // generate instances of static object
  std::map<std::string, MappedContainer<SiPMCalibrations>*> SiPMCalibrationsProcessor::_SiPMCalibrationsContainerMap;

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  MappedContainer<SiPMCalibrations>* SiPMCalibrationsProcessor::getCalibrations(const std::string& processorName)
  {
    return _SiPMCalibrationsContainerMap[processorName];
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  SiPMCalibrationsProcessor::SiPMCalibrationsProcessor() : Processor("SiPMCalibrationsProcessor")
  {

    _description = "Processor that provides a MappedContainer of SiPMCalibrations objects";

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "MIPConstantCollection" ,
                             "Name of the MIP constants collection"  ,
                             _MIPConstantColName ,
                             std::string("MIPConstants") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "MIPSlopeCollection" ,
                             "Name of the MIP slopes collection"  ,
                             _MIPSlopeColName ,
                             std::string("MIPSlopes") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "GainConstantCollection" ,
                             "Name of the gain constants collection"  ,
                             _gainConstantColName ,
                             std::string("gainConstants") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "GainSlopeCollection" ,
                             "Name of the gain slopes collection"  ,
                             _gainSlopeColName ,
                             std::string("gainSlopes") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "InterCalibrationCollection" ,
                             "Name of the interCalibration collection"  ,
                             _interCalibrationColName ,
                             std::string("interCalibration") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "PedestalCollection" ,
                             "Name of the pedestal collection"  ,
                             _pedestalColName ,
                             std::string("pedestal") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "TemperatureCollection" ,
                             "Name of the temperature collection",
                             _temperatureColName ,
                             std::string("AhcTemperature") ) ;

    registerInputCollection(LCIO::LCGENERICOBJECT,
                            "SaturationCollection",
                            "Name of the saturation collection",
                            _saturationColName,
                            std::string("AhcSatCorrItep"));
                            
    registerInputCollection(LCIO::LCGENERICOBJECT,
                            "SaturationParametersCollection",
                            "Name of the saturation curves parameters collection",
                            _saturationParametersColName,
                            std::string("AhcSaturationParameters"));

    registerInputCollection(LCIO::LCGENERICOBJECT,
                            "PixelScaleFactorsCollection",
                            "Name of the pixel scale factors collection",
                            _pixelScaleFactorsColName,
                            std::string("PixelScaleFactors"));

    registerProcessorParameter("UseIndividualScaleFactor",
                               "switch for the value for the pixel scale factor,"
			       "0 - GlobalPixelScaleFactor 0.8 (default),"
			       "1 - PixelScaleFactorsCollection (database)",
                               _useIndividualScaleFactor,
                               (bool) false);

    registerProcessorParameter("GlobalPixelScaleFactor",
                               "Default (global) value for the pixel scale factor",
                               _globalPixelScaleFactor,
                               (float)0.8);

    registerInputCollection(LCIO::LCGENERICOBJECT,
                            "CellQualityCollection",
                            "Collection with quality flags",
                            _cellQualityColName,
                            std::string("AhcCellQuality"));
                            
    registerProcessorParameter("SaturationProcedureType",
                              "Type of saturation correction applied, 0 - interpolation (default), 1 - curve",
                              _saturationProcedureType,
                              (int)0);
                                                          

    /* steering file parameters to allow systematic studies
     */
    registerOptionalParameter( "MIPConstantScaleFactor",
                               "factor to scale all MIP constants",
                               _MIPConstantScaleFactor,
                               (float)1. );

    registerOptionalParameter( "MIPSlopeScaleFactor",
                               "factor to scale all MIP slopes",
                               _MIPSlopeScaleFactor,
                               (float)1. );

    registerOptionalParameter( "GainConstantScaleFactor",
                               "factor to scale all gain constants",
                               _gainConstantScaleFactor,
                               (float)1. );

    registerOptionalParameter( "GainSlopeScaleFactor",
                               "factor to scale all gain slopes",
                               _gainSlopeScaleFactor,
                               (float)1. );

    registerOptionalParameter( "InterCalibrationScaleFactor",
                               "factor to scale all inter-calibration constants",
                               _interCalibrationScaleFactor,
                               (float)1. );

    registerOptionalParameter( "SaturationCorrectionScaleFactor",
                               "factor to scale the saturation correction function",
                               _saturationCorrectionScaleFactor,
                               (float)1. );

    registerProcessorParameter("UseDBDefaultValuesOnlyForAllCells",
                               "switch for using database default values only for a virtual machine,"
			       "true - use database default values for all cell for a virtual machine,"
			       "false - backward compatible, and use database",
                               _useDBDefaultValuesOnly,
                               (bool) false);

    _container = NULL;

    _MIPConstantCol              = NULL;
    _MIPSlopeCol                 = NULL;
    _gainConstantCol             = NULL;
    _gainSlopeCol                = NULL;
    _interCalibrationCol         = NULL;
    _pedestalCol                 = NULL;
    _temperatureCol              = NULL;
    _saturationCol               = NULL;
    _saturationParametersCol     = NULL;
    _pixelScaleFactorsCol        = NULL;
    _cellQualityCol              = NULL;

  }
  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  SiPMCalibrationsProcessor::~SiPMCalibrationsProcessor()
  {
    if (_container) delete _container;
    _SiPMCalibrationsContainerMap.erase(Processor::name());
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::init()
  {
    printParameters();

    _MIPConstantChanged              = false;
    _MIPSlopeChanged                 = false;
    _gainConstantChanged             = false;
    _gainSlopeChanged                = false;
    _interCalibrationChanged         = false;
    _pedestalChanged                 = false;
    _temperatureChanged              = false;
    _saturationChanged               = false;
    _saturationParametersChanged     = false;
    _pixelScaleFactorsChanged        = false;
    _cellQualityChanged              = false;

    _MIPSlopeScaled                  = parameterSet("MIPSlopeScaleFactor");
    _MIPConstantScaled               = parameterSet("MIPConstantScaleFactor");
    _gainSlopeScaled                 = parameterSet("GainSlopeScaleFactor");
    _gainConstantScaled              = parameterSet("GainConstantScaleFactor");
    _interCalibrationScaled          = parameterSet("InterCalibrationScaleFactor");
    _saturationCorrectionScaled      = parameterSet("SaturationCorrectionScaleFactor");

    std::stringstream message;
    bool error = false;

    _mapper = MappingProcessor::getMapper(_mappingProcessorName);

    if ( ! _mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName << ") did not return a valid mapper." << std::endl;
        error = true;
      }
    
    /* registration of the conditions change listeners has to go here
     *
     * should not break immediately but append report to message and set error to true,
     * if registration not possible
     *
     */
    if (!ConditionsProcessor::registerChangeListener(this, _MIPConstantColName))
      {
        message << " undefined conditions: " << _MIPConstantColName << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _MIPSlopeColName))
      {
        message << " undefined conditions: " << _MIPSlopeColName << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _gainConstantColName))
      {
        message << " undefined conditions: " << _gainConstantColName << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _gainSlopeColName))
      {
        message << " undefined conditions: " << _gainSlopeColName << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _interCalibrationColName))
      {
        message << " undefined conditions: " << _interCalibrationColName << std::endl;
        error = true;
      }
    if (parameterSet("PedestalCollection") && !ConditionsProcessor::registerChangeListener(this, _pedestalColName))
      {
        message << " undefined conditions: " << _pedestalColName << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _temperatureColName))
      {
        message << " undefined conditions: " << _temperatureColName << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _saturationColName))
      {
        message << " undefined conditions: " << _saturationColName << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _saturationParametersColName))
      {
        message << " undefined conditions: " << _saturationParametersColName << std::endl;
        error = true;
      }

    if (!ConditionsProcessor::registerChangeListener(this, _cellQualityColName))
      {
        message << " undefined conditions: " << _cellQualityColName << std::endl;
        error = true;
      }

    if (error) {
      streamlog_out(ERROR) << message.str();
      throw marlin::StopProcessingException(this);
    }

    _mapperVersion = _mapper->getVersion();

    /*I don't know if there is any pixel collection in the database already*/

    bool pixelScaleFactorsCollectionFound = false;
    if (ConditionsProcessor::registerChangeListener(this, _pixelScaleFactorsColName))
      pixelScaleFactorsCollectionFound = true;

    /*if none of the pixel collections is found, cry out*/
    if (pixelScaleFactorsCollectionFound == false)
      {
        streamlog_out(WARNING) << " Could not find the pixel collections: " << _pixelScaleFactorsColName
                       << std::endl;
      }

    _container = new MappedContainer<SiPMCalibrations>(_mapper);
    _SiPMCalibrationsContainerMap[Processor::name()] = _container;

  }

  /***************************************************************************************/
  /* function to listen for condition changes in LCCD                                    */
  /*                                                                                     */
  /* should only remember pointer of collection and set flag for the changed conditions; */
  /* processing should wait till process event. this ensures that constants that depend  */
  /* on two different conditions collections have both collections available             */
  /*                                                                                     */
  /* the callbacks from LCCD have no guarantied order                                    */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::conditionsChanged( LCCollection * col ) {

    std::string colName = col->getParameters().getStringVal("CollectionName") ;

    if (colName == _MIPConstantColName)
      {
        _MIPConstantCol = col;
        _MIPConstantChanged = true;
      }
    else if (colName == _MIPSlopeColName)
      {
        _MIPSlopeCol = col;
        _MIPSlopeChanged = true;
      }
    else if (colName == _gainConstantColName)
      {
        _gainConstantCol = col;
        _gainConstantChanged = true;
      }
    else if (colName == _gainSlopeColName)
      {
        _gainSlopeCol = col;
        _gainSlopeChanged = true;
      }
    else if (colName == _interCalibrationColName)
      {
        _interCalibrationCol = col;
        _interCalibrationChanged = true;
      }
    else if (colName == _pedestalColName)
      {
        _pedestalCol = col;
        _pedestalChanged = true;
      }
    else if (colName == _temperatureColName)
      {
        _temperatureCol = col;
        _temperatureChanged = true;
      }
    else if (colName == _saturationColName)
      {
        _saturationCol = col;
        _saturationChanged = true;
      }
    else if (colName == _saturationParametersColName)
      {
        _saturationParametersCol = col;
        _saturationParametersChanged = true;
      }
    else if (colName == _pixelScaleFactorsColName)
      {
        _pixelScaleFactorsCol = col;
        _pixelScaleFactorsChanged = true;
      }
    else if (colName == _cellQualityColName)
      {
        _cellQualityCol = col;
        _cellQualityChanged = true;
      }

    else {
      const std::list<lccd::IConditionsHandler*> theList = this->handlerList();
      std::cout<<"AAA "<<theList.size()<<std::endl;

      for(std::list<lccd::IConditionsHandler*>::const_iterator it=theList.begin();it != theList.end();++it){
	std::cout << "AAA handler:" << (*it)->name() << std::endl;
	if( col == (*it)->defaultCollection()){
	  std::cout << "using default collection" << std::endl;
	}
      }

      streamlog_out(ERROR) << "Called as conditions listener for collection " << colName << ", but not responsible." << std::endl;
      throw StopProcessingException(this);
    }
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /* functions to do the actual update (or initialisation) of the calibrations           */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::updateMIPCalibration()
  {
    if (!_MIPSlopeCol)
      {
        streamlog_out(ERROR) << "Cannot update MIP, MIP slope collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    if (!_MIPConstantCol )
      {
        streamlog_out(ERROR) << "Cannot update MIP, MIP constant collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    float defaultMIPConstant      = _MIPConstantCol->getParameters().getFloatVal("defaultValue");
    float defaultMIPConstantError = _MIPConstantCol->getParameters().getFloatVal("defaultError");

    float defaultMIPConstantReferencePoint      = _MIPConstantCol->getParameters().getFloatVal("defaultReferencePoint");
    float defaultMIPConstantReferencePointError = _MIPConstantCol->getParameters().getFloatVal("defaultReferencePointError");

    float defaultMIPSlope      = _MIPSlopeCol->getParameters().getFloatVal("defaultValue");
    float defaultMIPSlopeError = _MIPSlopeCol->getParameters().getFloatVal("defaultError");

    MappedContainer<LinearFitSlope>    *slopes    = new MappedContainer<LinearFitSlope>(_mapper);
    MappedContainer<LinearFitConstant> *constants = new MappedContainer<LinearFitConstant>(_mapper);

    if ( !_useDBDefaultValuesOnly) {

    for (int i = 0; i < _MIPSlopeCol->getNumberOfElements(); ++i)
      {
        LinearFitSlope *slope = new LinearFitSlope( _MIPSlopeCol->getElementAt(i) );

        try
          {
            slopes->fillByModuleID(slope->getID(),slope);
          }
        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updateMIPCalibration(): invalid module id " << slope->getID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }

    for (int i = 0; i < _MIPConstantCol->getNumberOfElements(); ++i)
      {
        LinearFitConstant *constant = new LinearFitConstant( _MIPConstantCol->getElementAt(i) );
       try
          {
            constants->fillByModuleID(constant->getID(),constant);
          }
        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updateMIPCalibration(): invalid module id " << constant->getID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }
    }

    for ( std::vector<SiPMCalibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        /* remove old MIP calibration*/
        if ( (*iter)->getMIP() ) delete (*iter)->getMIP();

        int ID = (*iter)->getCellID();
        SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( (*iter)->getStatus() );

        LinearFitSlope *slope = slopes->getByCellID(ID);
        LinearFitConstant *constant = constants->getByCellID(ID);

        LinearFitCompound *mip = 0;

        bits.setMIPConstantDefault(false);
        bits.setMIPSlopeDefault(false);
        if ( constant && slope )
          {
	      mip = new LinearFitCompound(constant, slope);
	    }
        else if ( constant && !slope )
          {	    
	      mip = new LinearFitCompound(ID,
					  constant->getConstant(),
					  constant->getConstantError(),
					  constant->getConstantReferencePoint(),
					  constant->getConstantReferencePointError(),
					  defaultMIPSlope,
					  defaultMIPSlopeError
					  );
            bits.setMIPSlopeDefault();
          }
        else if ( !constant && slope )
          {
	      mip = new LinearFitCompound(ID,
					  defaultMIPConstant,
					  defaultMIPConstantError,
					  defaultMIPConstantReferencePoint,
					  defaultMIPConstantReferencePointError,
					  slope->getSlope(),
					  slope->getSlopeError()
					  );
            bits.setMIPConstantDefault();
          }
        else
          {
	      mip = new LinearFitCompound(ID,
					  defaultMIPConstant,
					  defaultMIPConstantError,
					  defaultMIPConstantReferencePoint,
					  defaultMIPConstantReferencePointError,
					  defaultMIPSlope,
					  defaultMIPSlopeError
					  );
            bits.setMIPConstantDefault();
            bits.setMIPSlopeDefault();
          }

        if (_MIPConstantScaled) {
          mip->setConstant( mip->getConstant() * _MIPConstantScaleFactor );
          mip->setConstantError( mip->getConstantError() * _MIPConstantScaleFactor );
          bits.setMIPConstantScaled();
        }
        else {
          bits.setMIPConstantScaled(false);
        }

        if (_MIPSlopeScaled) {
          mip->setSlope( mip->getSlope() * _MIPSlopeScaleFactor );
          mip->setSlopeError( mip->getSlopeError() * _MIPSlopeScaleFactor );
          bits.setMIPSlopeScaled();
        }
        else {
          bits.setMIPSlopeScaled(false);
        }

        (*iter)->setMIP(mip);
        (*iter)->setStatus(bits.getInt());


      }/*------------- end iteration over SiPMCalibrations --------------------*/

    _MIPConstantChanged = false;
    _MIPSlopeChanged    = false;

    delete slopes;
    delete constants;

  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::updateGainCalibration()
  {
    if (!_gainSlopeCol)
      {
        streamlog_out(ERROR) << "Cannot update gain, gain slope collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    if (!_gainConstantCol )
      {
        streamlog_out(ERROR) << "Cannot update gain, gain constant collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    float defaultGainConstant      = _gainConstantCol->getParameters().getFloatVal("defaultValue");
    float defaultGainConstantError = _gainConstantCol->getParameters().getFloatVal("defaultError");

    float defaultGainConstantReferencePoint      = _gainConstantCol->getParameters().getFloatVal("defaultReferencePoint");
    float defaultGainConstantReferencePointError = _gainConstantCol->getParameters().getFloatVal("defaultReferencePointError");

    float defaultGainSlope      = _gainSlopeCol->getParameters().getFloatVal("defaultValue");
    float defaultGainSlopeError = _gainSlopeCol->getParameters().getFloatVal("defaultError");

    MappedContainer<LinearFitSlope>    *slopes    = new MappedContainer<LinearFitSlope>(_mapper);
    MappedContainer<LinearFitConstant> *constants = new MappedContainer<LinearFitConstant>(_mapper);

    if ( !_useDBDefaultValuesOnly) {

    for (int i = 0; i < _gainSlopeCol->getNumberOfElements(); ++i)
      {
        LinearFitSlope *slope = new LinearFitSlope( _gainSlopeCol->getElementAt(i) );
        try
          {
            slopes->fillByModuleID(slope->getID(),slope);
          }
        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updateGainCalibration(): invalid module id " << slope->getID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }

    for (int i = 0; i < _gainConstantCol->getNumberOfElements(); ++i)
      {
        LinearFitConstant *constant = new LinearFitConstant( _gainConstantCol->getElementAt(i) );
        constants->fillByModuleID(constant->getID(), constant);
      }
    }

    for ( std::vector<SiPMCalibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        /* remove old gain calibration*/
        if ( (*iter)->getGain() ) delete (*iter)->getGain();

        int ID = (*iter)->getCellID();
        SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( (*iter)->getStatus() );

        LinearFitSlope *slope = slopes->getByCellID(ID);
        LinearFitConstant *constant = constants->getByCellID(ID);

        LinearFitCompound *gain = 0;

        bits.setGainConstantDefault(false);
        bits.setGainSlopeDefault(false);
        if ( constant && slope )
          {
	      gain = new LinearFitCompound(constant, slope);
	    }
        else if ( constant && !slope )
          {
            gain = new LinearFitCompound(ID,
                                         constant->getConstant(),
                                         constant->getConstantError(),
                                         constant->getConstantReferencePoint(),
                                         constant->getConstantReferencePointError(),
                                         defaultGainSlope,
                                         defaultGainSlopeError
                                         );
            bits.setGainSlopeDefault();
          }
        else if ( !constant && slope )
          {
            gain = new LinearFitCompound(ID,
                                         defaultGainConstant,
                                         defaultGainConstantError,
                                         defaultGainConstantReferencePoint,
                                         defaultGainConstantReferencePointError,
                                         slope->getSlope(),
                                         slope->getSlopeError()
                                         );
            bits.setGainConstantDefault();
          }
        else
          {
            gain = new LinearFitCompound(ID,
                                         defaultGainConstant,
                                         defaultGainConstantError,
                                         defaultGainConstantReferencePoint,
                                         defaultGainConstantReferencePointError,
                                         defaultGainSlope,
                                         defaultGainSlopeError
                                         );
            bits.setGainConstantDefault();
            bits.setGainSlopeDefault();
          }


        if (_gainConstantScaled) {
          gain->setConstant( gain->getConstant() * _gainConstantScaleFactor );
          gain->setConstantError( gain->getConstantError() * _gainConstantScaleFactor );
          bits.setGainConstantScaled();
        }
        else {
          bits.setGainConstantScaled(false);
        }

        if (_gainSlopeScaled) {
          gain->setSlope( gain->getSlope() * _gainSlopeScaleFactor );
          gain->setSlopeError( gain->getSlopeError() * _gainSlopeScaleFactor );
          bits.setGainSlopeScaled();
        }
        else {
          bits.setGainSlopeScaled(false);
        }

        (*iter)->setStatus(bits.getInt());
        (*iter)->setGain(gain);


      }/*------------- end iteration over SiPMCalibrations --------------------*/

    _gainConstantChanged = false;
    _gainSlopeChanged    = false;

    delete slopes;
    delete constants;

  }
  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::updatePedestal()
  {
    if (!_pedestalCol)
      {
        streamlog_out(ERROR) << "Cannot update pedestal, pedestal collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    MappedContainer<SimpleValue> *pedestalContainer = new MappedContainer<SimpleValue>(_mapper);
    for (int i = 0; i < _pedestalCol->getNumberOfElements(); ++i)
      {
        SimpleValue *pedestalVal = new SimpleValue(_pedestalCol->getElementAt(i));
        try
          {
            pedestalContainer->fillByCellID(pedestalVal->getCellID(), pedestalVal);
          }

        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updatePedestal(): invalid module id " << pedestalVal->getCellID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }

    for ( std::vector<SiPMCalibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        /*delete old pedestal values*/
        if ( (*iter)->getPedestal() ) delete (*iter)->getPedestal();

        int ID = (*iter)->getCellID();
        SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( (*iter)->getStatus() );

        /*
          just to be safe, create a temporary simple value
          (we delete the container in the end, and the SimpleValue does not
          belong to us)
        */
        SimpleValue *pedestalTemp = pedestalContainer->getByCellID(ID);
        SimpleValue *pedestal = NULL;
        if (pedestalTemp == NULL)
          {
            pedestal = new SimpleValue(ID,
                                       0., /*value*/
                                       0., /*error*/
                                       1   /*status*/
                                       );
            bits.setNoPedestal();
          }
        else
          {
	      pedestal = new SimpleValue(pedestalTemp->getCellID(),
					 pedestalTemp->getValue(),
					 pedestalTemp->getError(),
					 pedestalTemp->getStatus());
	      bits.setNoPedestal(false);
          }
	
        (*iter)->setStatus(bits.getInt());
        (*iter)->setPedestal(pedestal);
      }

    _pedestalChanged = false;

    delete pedestalContainer;

  }



  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::updateTemperature()
  {
    if (!_temperatureCol)
      {
        streamlog_out(ERROR) << "Cannot update temperature, temperature collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    MappedContainer<SimpleValue> *temperatureContainer = new MappedContainer<SimpleValue>(_mapper);
    for (int i = 0; i < _temperatureCol->getNumberOfElements(); ++i)
      {
        SimpleValue *temperatureVal = new SimpleValue(_temperatureCol->getElementAt(i));
        try
          {
	    //temperatureContainer->fillByCellID(temperatureVal->getCellID(), temperatureVal);
	    temperatureContainer->fillByModuleID(temperatureVal->getCellID(), temperatureVal);
          }

        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updateTemperature(): invalid module id " << temperatureVal->getCellID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }

    for ( std::vector<SiPMCalibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        /*delete old pedestal values*/
        if ( (*iter)->getTemperature() ) delete (*iter)->getTemperature();

        int ID = (*iter)->getCellID();
        SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( (*iter)->getStatus() );

        /*
          just to be safe, create a temporary simple value
          (we delete the container in the end, and the SimpleValue does not
          belong to us)
        */
        SimpleValue *temperatureTemp = temperatureContainer->getByCellID(ID);
        SimpleValue *temperature = NULL;
        if (temperatureTemp == NULL)
          {
            temperature = new SimpleValue(ID,
					  0., /*value*/
					  0., /*error*/
					  1   /*status*/
					  );
            bits.setNoTemperature();
          }
        else
          {
	    temperature = new SimpleValue(temperatureTemp->getCellID(),
					  temperatureTemp->getValue(),
					  temperatureTemp->getError(),
					  temperatureTemp->getStatus());
	      bits.setNoTemperature(false);
          }
	
        (*iter)->setStatus(bits.getInt());
        (*iter)->setTemperature(temperature);
      }

    _temperatureChanged = false;

    delete temperatureContainer;

  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::updateInterCalibration()
  {
    if (!_interCalibrationCol)
      {
        streamlog_out(ERROR) << "Cannot update inter-calibration, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    float defaultInterCalibration      = _interCalibrationCol->getParameters().getFloatVal("defaultValue");
    float defaultInterCalibrationError = _interCalibrationCol->getParameters().getFloatVal("defaultError");

    MappedContainer<SimpleValue> *interCalibContainer = new MappedContainer<SimpleValue>(_mapper);

    if ( !_useDBDefaultValuesOnly) {

    for (int i = 0; i < _interCalibrationCol->getNumberOfElements(); ++i)
      {
        LinearFitConstant interCalib(_interCalibrationCol->getElementAt(i));
        SimpleValue *interCalibValue = new SimpleValue(interCalib.getID(),
					   interCalib.getConstant(),
					   interCalib.getConstantError(),
					   1);/*status*/
        try
          {
            interCalibContainer->fillByModuleID(interCalibValue->getCellID(), interCalibValue);
          }
        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updateInterCalibration(): invalid module id " << interCalibValue->getCellID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }
    }

    for ( std::vector<SiPMCalibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        /*delete old inter-calibration values*/
        if ( (*iter)->getInterCalibration() ) delete (*iter)->getInterCalibration();

        int ID = (*iter)->getCellID();
        SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( (*iter)->getStatus() );

        /*
          just to be safe, create a temporary simple value
          (we delete the container in the end, and the SimpleValue does not
          belong to us)
        */
        SimpleValue *interCalibrationTemp = interCalibContainer->removeByCellID(ID);
        SimpleValue *interCalibration = NULL;

        if (interCalibrationTemp == NULL)
          {
	      interCalibration = new SimpleValue(ID,
						 defaultInterCalibration,
						 defaultInterCalibrationError,
						 1   /*status*/
						 );
            bits.setInterCalibrationDefault();
          }
        else
          {
	      interCalibration = new SimpleValue(interCalibrationTemp->getCellID(),
						 interCalibrationTemp->getValue(),
						 interCalibrationTemp->getError(),
						 interCalibrationTemp->getStatus());
	    bits.setInterCalibrationDefault(false);
          }

        if (_interCalibrationScaled) {
          interCalibration->setValue( interCalibration->getValue() * _interCalibrationScaleFactor );
          interCalibration->setError( interCalibration->getError() * _interCalibrationScaleFactor );
          bits.setInterCalibrationScaled();
        }
        else {
          bits.setInterCalibrationScaled(false);
        }

        (*iter)->setStatus(bits.getInt());
        (*iter)->setInterCalibration(interCalibration);
      }

    _interCalibrationChanged = false;

    delete interCalibContainer;

  }
  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::updateSaturation()
  {
    if (!_saturationCol)
      {
        streamlog_out(ERROR) << "Cannot update saturation, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    bool individualScaling        = parameterSet("PixelScaleFactorsCollection");

    MappedContainer<SatCorrItep> *saturationContainer        = new MappedContainer<SatCorrItep>(_mapper);

   if ( !_useDBDefaultValuesOnly) {

    for (int i = 0; i < _saturationCol->getNumberOfElements(); ++i)
      {
        SatCorrItep *saturationFunc = new SatCorrItep(_saturationCol->getElementAt(i)); // this will make a copy of the data (not link against the data like SimpleValue)
	
        try
          {
            saturationContainer->fillByModuleID(saturationFunc->getIndex(), saturationFunc);
          }

        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " invalid module id " << saturationFunc->getIndex() <<", maybe incomplete installation "
                         <<std::endl<<  e.what() << std::endl;
          }


      }
   }

    MappedContainer<SimpleValue> *saturationScalingContainer = new MappedContainer<SimpleValue>(_mapper);

   if ( !_useDBDefaultValuesOnly) {

    if (_pixelScaleFactorsCol)
      {
        for (int i = 0; i < _pixelScaleFactorsCol->getNumberOfElements(); ++i)
          {
            SimpleValue *scaleFactorObject = new SimpleValue(_pixelScaleFactorsCol->getElementAt(i)) ;

            if ( scaleFactorObject->getStatus() ) continue; // filter all with status != 0

            try
              {
                saturationScalingContainer->fillByModuleID( scaleFactorObject->getCellID(), scaleFactorObject );

              }
            catch(BadDataException& e)
              {
                streamlog_out(DEBUG) << " invalid module id " << scaleFactorObject->getCellID() <<", maybe incomplete installation "
                             << std::endl<<  e.what() << std::endl;
              }
          }
      }
    else
      {
        if ( individualScaling ) {
          streamlog_out(ERROR) << "Cannot update saturation, pixelScaleFactorsCollection collection " << _pixelScaleFactorsColName << " is set but not valid." << std::endl;
          throw StopProcessingException(this);
        }
      }
   }

    //applying the saturation curves parameters

    MappedContainer<SaturationParameters> *saturationParametersContainer = new MappedContainer<SaturationParameters>(_mapper);

    float saturationParametersdefaultEpsilon  = _saturationParametersCol->getParameters().getFloatVal("defaultEpsilon");
    float saturationParametersdefaultFraction = _saturationParametersCol->getParameters().getFloatVal("defaultFraction");
    float saturationParametersdefaultNpix     = _saturationParametersCol->getParameters().getFloatVal("defaultNpix");

   if ( !_useDBDefaultValuesOnly) {

    if (_saturationParametersCol)
      {
        for (int i = 0; i < _saturationParametersCol->getNumberOfElements(); ++i)
          {
            SaturationParameters *sp = new SaturationParameters(_saturationParametersCol->getElementAt(i)) ;

            try
              {
                saturationParametersContainer->fillByModuleID( sp->getCellID(), sp );
              }
            catch(BadDataException& e)
              {
                streamlog_out(DEBUG) << " invalid module id " << sp->getCellID() <<", maybe incomplete installation "
                             << std::endl<<  e.what() << std::endl;
              }
          }
      }
    else
      {
          streamlog_out(ERROR) << "Cannot update saturation, SaturationParameters collection " << _saturationParametersColName << " is set but not valid." << std::endl;
          throw StopProcessingException(this);
      }
   }

    for ( std::vector<SiPMCalibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        /*delete old saturation*/
        if ( (*iter)->getSaturationCorrection() ) delete (*iter)->getSaturationCorrection();

        int ID = (*iter)->getCellID();
        SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( (*iter)->getStatus() );

        // clear default bits
        bits.setPixelScaleDefault(false);
        bits.setSaturationCorrectionDefault(false);


        SatCorrItep *saturation = saturationContainer->removeByCellID(ID); // we can savely use these, as data was copied and not linked
	
        if (saturation == NULL) // if there was nothing, generate a default scaling
          {
            HcalTileIndex hti(ID);
            const int nPoints = 20;
            float pixels[nPoints] = {0};
            float pmts[nPoints] = {0};

            saturation = new SatCorrItep(hti, nPoints, pixels, pmts);
            bits.setSaturationCorrectionDefault();
          }

        SimpleValue *individualScalingFactor = saturationScalingContainer->getByCellID(ID);

        float scalingFactor = 1.;

        if ( individualScalingFactor || _useIndividualScaleFactor ) {
          scalingFactor = individualScalingFactor->getValue();
        }
        else {
          scalingFactor = _globalPixelScaleFactor;
          if ( individualScaling ) bits.setPixelScaleDefault();
        }

        if ( _saturationCorrectionScaled ) {
          scalingFactor = scalingFactor * _saturationCorrectionScaleFactor;
          bits.setPixelScaleScaled();
        }
        else {
          bits.setPixelScaleScaled(false);
        }
        saturation->setScaling(scalingFactor);

	if (!_useDBDefaultValuesOnly){
	  SaturationParameters *saturationPar = saturationParametersContainer->getByCellID(ID);
	  saturation->setFunParameters(saturationPar->getNpix(), saturationPar->getFraction(), saturationPar->getEpsilon());
        }
	else {
	  saturation->setFunParameters(saturationParametersdefaultNpix,saturationParametersdefaultFraction,saturationParametersdefaultEpsilon);
	}
        saturation->setProcedureType(_saturationProcedureType);


        (*iter)->setStatus(bits.getInt());
        (*iter)->setSaturationCorrection(saturation);
      }

    _saturationChanged = false;
    _saturationParametersChanged = false;
    _pixelScaleFactorsChanged = false;

    delete saturationContainer;
    delete saturationScalingContainer;
    delete saturationParametersContainer;
  } 

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::updateCellQuality()
  {
    if (!_cellQualityCol)
      {
        streamlog_out(ERROR) << "Cannot update bad cells, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    MappedContainer<CellQuality> *cellQualityContainer = new MappedContainer<CellQuality>(_mapper);

   if ( !_useDBDefaultValuesOnly) {

    for (int i = 0; i < _cellQualityCol->getNumberOfElements(); ++i)
      {
        CellQuality *cellQuality = new CellQuality(_cellQualityCol->getElementAt(i));

        try
          {
            cellQualityContainer->fillByModuleID(cellQuality->getCellID(), cellQuality);
          }
        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updateCellQuality(): invalid module id " << cellQuality->getCellID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }
   }

    for ( std::vector<SiPMCalibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        int ID = (*iter)->getCellID();
        SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( (*iter)->getStatus() );

        CellQuality *cellQuality = cellQualityContainer->getByCellID(ID);
        if (cellQuality != NULL) bits.setDead();
        else bits.setDead(false);

        (*iter)->setStatus(bits.getInt());
      }

    _cellQualityChanged = false;
    delete cellQualityContainer;

  }


  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::updateMapping()
  {
    /*clear all objects in the container*/
    _container->clear();

    /*
     * initialise the container with SiPMCalibration objects for all valid cellIDs
     */
    for ( CellIterator iter = _mapper->begin(); iter != _mapper->end(); ++iter)
      {
        int cellID = *iter;
        SiPMCalibrations *thisCell = new SiPMCalibrations();
        thisCell->setCellID( cellID );
        thisCell->setCellIDEncoding( _mapper->getDecoder()->getCellIDEncoding() );
        _container->fillByCellID( cellID, thisCell );
      }

    _allCalibrations = _container->getAllElements();
  }


  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::processEvent( LCEvent *evt )
  {
    streamlog_out(DEBUG0)<<"\n\n EVENT: "<<evt->getEventNumber()<<std::endl;

    if (_mapperVersion != _mapper->getVersion()) this->updateMapping();
    _mapperVersion = _mapper->getVersion();

    /* checks if rebuilding of calibration constants is necessary  */
    if (_MIPConstantChanged  || _MIPSlopeChanged)  updateMIPCalibration();
    if (_gainConstantChanged || _gainSlopeChanged) updateGainCalibration();
    if (_pedestalChanged)         updatePedestal();
    if (_temperatureChanged)      updateTemperature();
    if (_interCalibrationChanged) updateInterCalibration();

    if ( _saturationChanged || _saturationParametersChanged
         || _pixelScaleFactorsChanged ) updateSaturation();

    if (_cellQualityChanged)      updateCellQuality();
  }



  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void SiPMCalibrationsProcessor::end()
  {
    delete _SiPMCalibrationsContainerMap[name()];
    _SiPMCalibrationsContainerMap.erase(name());
    _container = NULL;
  }


  /***************************************************************************************
   * create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   ***************************************************************************************/
  SiPMCalibrationsProcessor aSiPMCalibrationsProcessor;

} // end namespace CALICE
