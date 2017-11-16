#include "Ahc2CalibrationsProcessor.hh"

#include <list>
#include <map>
#include <iostream>

// Marlin includes
#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"
#include "lccd/IConditionsHandler.hh"

// CALICE includes
#include "MappingProcessor.hh"
#include "CellIterator.hh"
#include "CellQuality.hh"
#include "SatCorrItep.hh"
#include "SaturationParameters.hh"
#include "LinearFitSlope.hh"
#include "LinearFitConstant.hh"
#include "LinearFitCompound.hh"
#include "Ahc2CalibrationStatusBits.hh"
#include "Ahc2Calibrations.hh"

namespace CALICE {

  // generate instances of static object
  std::map<std::string, MappedContainer<Ahc2Calibrations>*> Ahc2CalibrationsProcessor::_Ahc2CalibrationsContainerMap;

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  MappedContainer<Ahc2Calibrations>* Ahc2CalibrationsProcessor::getCalibrations(const std::string& processorName)
  {
    return _Ahc2CalibrationsContainerMap[processorName];
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  Ahc2CalibrationsProcessor::Ahc2CalibrationsProcessor() : Processor("Ahc2CalibrationsProcessor")
  {

    _description = "Processor that provides a MappedContainer of Ahc2Calibrations objects";

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerInputCollection( LCIO::LCGENERICOBJECT,
                             "PedestalCollection" ,
                             "Name of the pedestal collection"  ,
                             _pedestalColName ,
                             std::string("pedestal") ) ;
    
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
                             "PhysicsCalibICCollection" ,
                             "Name of the interCalibration physics calib collection"  ,
                             _PhysicsCalibICColName ,
                             std::string("PhysicsCalibIntercalibration") ) ;

    registerInputCollection(LCIO::LCGENERICOBJECT,
			    "SaturationParametersCollection",
			    "Name of the saturation parameters collection",
			    _saturationParametersColName,
			    std::string("SaturationParameters"));

    registerInputCollection(LCIO::LCGENERICOBJECT,
			    "TimeSlopesParametersCollection",
			    "Name of the time slopes parameters collection",
			    _timeSlopesParametersColName,
			    std::string("TimeSlopes"));

    registerInputCollection(LCIO::LCGENERICOBJECT,
			    "TimePedestalParametersCollection",
			    "Name of the time pedestal parameters collection",
			    _timePedestalParametersColName,
			    std::string("TimePedestal"));

    /*
      registerInputCollection( LCIO::LCGENERICOBJECT,
      "TemperatureCollection" ,
      "Name of the temperature collection",
      _temperatureColName ,
      std::string("AhcTemperature") ) ;
    */

    registerInputCollection(LCIO::LCGENERICOBJECT,
                            "CellQualityCollection",
                            "Collection with quality flags",
                            _cellQualityColName,
                            std::string("AhcCellQuality"));

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

    registerOptionalParameter( "PhysicsCalibICScaleFactor",
			       "factor to scale all inter-calibration physics/calib constants",
			       _PhysicsCalibICScaleFactor,
			       (float)1. );
    
    _container = NULL;

    _pedestalCol                 = NULL;
    _MIPConstantCol              = NULL;
    _MIPSlopeCol                 = NULL;
    _gainConstantCol             = NULL;
    _gainSlopeCol                = NULL;
    _interCalibrationCol         = NULL;
    _PhysicsCalibICCol           = NULL;
    //_temperatureCol              = NULL;
    _saturationParametersCol     = NULL;
    _timeSlopesParametersCol     = NULL;
    _timePedestalParametersCol   = NULL;
    _cellQualityCol              = NULL;
  }
  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  Ahc2CalibrationsProcessor::~Ahc2CalibrationsProcessor()
  {
    if (_container) delete _container;
    _Ahc2CalibrationsContainerMap.erase(Processor::name());
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::init()
  {
    std::cout << "Start init" << std::endl;

    printParameters();

    _pedestalChanged                 = false;
    _MIPConstantChanged              = false;
    _MIPSlopeChanged                 = false;
    _gainConstantChanged             = false;
    _gainSlopeChanged                = false;
    _interCalibrationChanged         = false;
    _PhysicsCalibICChanged           = false;

    _temperatureChanged              = false;
    _saturationParametersChanged     = false;
    _timeSlopesParametersChanged     = false;
    _timePedestalParametersChanged   = false;

    _cellQualityChanged              = false;

    _MIPSlopeScaled                  = parameterSet("MIPSlopeScaleFactor");
    _MIPConstantScaled               = parameterSet("MIPConstantScaleFactor");
    _gainSlopeScaled                 = parameterSet("GainSlopeScaleFactor");
    _gainConstantScaled              = parameterSet("GainConstantScaleFactor");
    _interCalibrationScaled          = parameterSet("InterCalibrationScaleFactor");
    _PhysicsCalibICScaled            = parameterSet("PhysicsCalibICScaleFactor");

    std::stringstream message;
    bool error = false;

    _mapper = MappingProcessor::getMapper(_mappingProcessorName);

    std::cout << "getMapper done" << std::endl;

    const std::string CellIDEncoding = _mapper->getDecoder()->getCellIDEncoding();
    const std::string ModuleEncoding = _mapper->getDecoder()->getModuleEncoding();

    std::cout << "CellIDEncoding " << CellIDEncoding << std::endl;
    std::cout << "ModuleEncoding " << ModuleEncoding << std::endl;

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
    if (!ConditionsProcessor::registerChangeListener(this, _pedestalColName))
      {
        message << " undefined conditions: " << _pedestalColName << std::endl;
        error = true;
      }

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

    if (!ConditionsProcessor::registerChangeListener(this, _PhysicsCalibICColName))
      {
	message << " undefined conditions: " << _PhysicsCalibICColName << std::endl;
	error = true;
      }
    
    /*
      if (!ConditionsProcessor::registerChangeListener(this, _temperatureColName))
      {
      message << " undefined conditions: " << _temperatureColName << std::endl;
      error = true;
      }
    */

    if (!ConditionsProcessor::registerChangeListener(this, _saturationParametersColName))
      {
	message << " undefined conditions: " << _saturationParametersColName << std::endl;
	error = true;
      }

    if (!ConditionsProcessor::registerChangeListener(this, _timeSlopesParametersColName))
      {
	message << " undefined conditions: " << _timeSlopesParametersColName << std::endl;
	error = true;
      }

    if (!ConditionsProcessor::registerChangeListener(this, _timePedestalParametersColName))
      {
	message << " undefined conditions: " << _timePedestalParametersColName << std::endl;
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

    _container = new MappedContainer<Ahc2Calibrations>(_mapper);
    _Ahc2CalibrationsContainerMap[Processor::name()] = _container;

    std::cout << "End of init" << std::endl;

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
  void Ahc2CalibrationsProcessor::conditionsChanged( LCCollection * col ) {

    std::cout << "ConditionsChanged" << std::endl;

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

    else if (colName == _PhysicsCalibICColName)
      {
	_PhysicsCalibICCol = col;
	_PhysicsCalibICChanged = true;
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
    else if (colName == _saturationParametersColName)
      {
        _saturationParametersCol = col;
        _saturationParametersChanged = true;
      }
    else if (colName == _timeSlopesParametersColName)
      {
        _timeSlopesParametersCol = col;
        _timeSlopesParametersChanged = true;
      }
    else if (colName == _timePedestalParametersColName)
      {
        _timePedestalParametersCol = col;
        _timePedestalParametersChanged = true;
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
  void Ahc2CalibrationsProcessor::updateMIPCalibration()
  {
    std::cout << "updateMIPCalibration" << std::endl;

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

    for (int i = 0; i < _MIPSlopeCol->getNumberOfElements(); ++i)
      {
        LinearFitSlope *slope = new LinearFitSlope( _MIPSlopeCol->getElementAt(i) );

        try
          {
	    streamlog_out(DEBUG) << "fill MIP slopes by ModuleID : index " << i << " ID " << slope->getID() << " slope " << slope << std::endl;
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
	    streamlog_out(DEBUG) << "fill MIP constants by ModuleID : index " << i << " ID " << constant->getID() << " constant " << constant << std::endl;
	    CALICE::HcalTileIndex hti( constant->getID() );
	    streamlog_out(DEBUG) << hti << '\n'
			 << *constant << std::endl;
            constants->fillByModuleID(constant->getID(),constant);
          }
        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updateMIPCalibration(): invalid module id " << constant->getID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }

    streamlog_out(MESSAGE)<<"   ===== checking  mip  ====" << std::endl;

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        // remove old MIP calibration
        if ( (*iter)->getMIP() ) delete (*iter)->getMIP();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

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

	//TODO: change to DEBUG
	streamlog_out(DEBUG)<<"ID: " <<ID 
		      <<"  Cell" << *mip << std::endl;


      }/*------------- end iteration over Ahc2Calibrations --------------------*/

    _MIPConstantChanged = false;
    _MIPSlopeChanged    = false;

    delete slopes;
    delete constants;


    std::cout << "end of updateMIPcalibration" << std::endl;
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::updateGainCalibration()
  {
    std::cout << "updateGaincalibration" << std::endl;

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

    for (int i = 0; i < _gainSlopeCol->getNumberOfElements(); ++i)
      {
        LinearFitSlope *slope = new LinearFitSlope( _gainSlopeCol->getElementAt(i) );
        try
          {

	    streamlog_out(DEBUG) << "fill gain slopes by ModuleID : index " << i << " ID " << slope->getID() << " slope " << slope << std::endl;
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

	streamlog_out(DEBUG) << "fill gain constants by ModuleID : index " << i << " ID " << constant->getID() << " constant " << constant << std::endl;
	CALICE::HcalTileIndex hti( constant->getID() );
	streamlog_out(DEBUG) << hti << '\n'
		     << *constant << std::endl;

        constants->fillByModuleID(constant->getID(), constant);
      }


    std::cout << "done filling" << std::endl;

    //TODO: change to DEBUG
    streamlog_out(MESSAGE)<<"   ===== checking  gain  ====" << std::endl;

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        /* remove old gain calibration*/
        if ( (*iter)->getGain() ) delete (*iter)->getGain();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

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

	//TODO: change to DEBUG
	streamlog_out(DEBUG)<<"ID: " <<ID 
		      <<"  Cell" << *gain << std::endl;

      }/*------------- end iteration over Ahc2Calibrations --------------------*/

    _gainConstantChanged = false;
    _gainSlopeChanged    = false;

    delete slopes;
    delete constants;

    std::cout << "end of updateGaincalibration" << std::endl;
  }
  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::updatePedestal()
  {
    
    streamlog_out(MESSAGE)<<" start  updatePedestal now! "<<std::endl;

    if (!_pedestalCol)
      {
        streamlog_out(ERROR) << "Cannot update pedestal, pedestal collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    MappedContainer<SimpleValue> *pedestalContainer = new MappedContainer<SimpleValue>(_mapper);

    streamlog_out(MESSAGE)<<" NumberOfElements(): "<<  _pedestalCol->getNumberOfElements() <<std::endl;

    for (int i = 0; i < _pedestalCol->getNumberOfElements(); ++i)
      {

        SimpleValue *pedestalVal = new SimpleValue(_pedestalCol->getElementAt(i));

        try
          {
	    //std::cout << "Fill pedestal cellID " << pedestalVal->getCellID() 
	    //	      << " pedestal value " << pedestalVal->getValue() << " error " << pedestalVal->getError() << std::endl;

            pedestalContainer->fillByModuleID(pedestalVal->getCellID(), pedestalVal);
	    //pedestalContainer->fillByCellID(pedestalVal->getCellID(), pedestalVal);
          }

        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updatePedestal(): invalid module id " << pedestalVal->getCellID()
                         <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }


    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
	
        // delete old pedestal values	
	if ( (*iter)->getPedestal() ) delete (*iter)->getPedestal();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

	//std::cout << "From pedestalContainer stock the info cellID " << ID << std::endl;

        SimpleValue *pedestal = pedestalContainer->removeByCellID(ID);

        if (pedestal == NULL)
          {
	    float value = 0.;
	    float error = 0.;
	    int status = 1;

            //To do : pedestal = new SimpleValue(ID, nMemo, value, error, status);
	    pedestal = new SimpleValue(ID, value, error, status);
            bits.setNoPedestal();
	    
          }

        (*iter)->setStatus(bits.getInt());
        (*iter)->setPedestal(pedestal);

	//TODO: change to DEBUG
	
	streamlog_out(DEBUG)<<" ID: " << ID 
		      <<" CellID: " << pedestal->getCellID() << " ";

	streamlog_out(DEBUG) << "value "  << pedestal->getValue() << " "
		       << "error "  << pedestal->getError() << " "
		       << "status " << pedestal->getStatus()<< " ";

	streamlog_out(DEBUG)<<" "<< std::endl;
	

	streamlog_out(MESSAGE)<<" updatePedestal done! "<<std::endl;

      }


    _pedestalChanged = false;

    delete pedestalContainer;

  }



  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::updateTemperature()
  {
    std::cout << "update Temperature" << std::endl;

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

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        /*delete old pedestal values*/
        if ( (*iter)->getTemperature() ) delete (*iter)->getTemperature();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

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

    std::cout << "end update Temperature" << std::endl;

  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::updateInterCalibration()
  {
    std::cout << "update Intercalibration" << std::endl;

    if (!_interCalibrationCol)
      {
        streamlog_out(ERROR) << "Cannot update inter-calibration, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    float defaultInterCalibration      = _interCalibrationCol->getParameters().getFloatVal("defaultValue");
    float defaultInterCalibrationError = _interCalibrationCol->getParameters().getFloatVal("defaultError");

    MappedContainer<SimpleValue> *interCalibContainer = new MappedContainer<SimpleValue>(_mapper);

    for (int i = 0; i < _interCalibrationCol->getNumberOfElements(); ++i)
      {
        LinearFitConstant interCalib(_interCalibrationCol->getElementAt(i));
        SimpleValue *interCalibValue = new SimpleValue(interCalib.getID(),
						       interCalib.getConstant(),
						       interCalib.getConstantError(),
						       1);//status
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

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        // delete old inter-calibration values
        if ( (*iter)->getInterCalibration() ) delete (*iter)->getInterCalibration();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

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
					       1   //status
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

    std::cout << "end update Intercalibration" << std::endl;
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::updatePhysicsCalibIC()
  {
    std::cout << "update PhysicsCalibIC" << std::endl;

    if (!_PhysicsCalibICCol)
      {
        streamlog_out(ERROR) << "Cannot update inter-calibration physics calib, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    float defaultPhysicsCalibIC      = _PhysicsCalibICCol->getParameters().getFloatVal("defaultValue");
    float defaultPhysicsCalibICError = _PhysicsCalibICCol->getParameters().getFloatVal("defaultError");

    MappedContainer<SimpleValue> *PhysicsCalibICContainer = new MappedContainer<SimpleValue>(_mapper);

    for (int i = 0; i < _PhysicsCalibICCol->getNumberOfElements(); ++i)
      {
        LinearFitConstant PhysicsCalibIC(_PhysicsCalibICCol->getElementAt(i));
        SimpleValue *PhysicsCalibICValue = new SimpleValue(PhysicsCalibIC.getID(),
						       PhysicsCalibIC.getConstant(),
						       PhysicsCalibIC.getConstantError(),
						       1);//status
        try
          {
            PhysicsCalibICContainer->fillByModuleID(PhysicsCalibICValue->getCellID(), PhysicsCalibICValue);
          }
        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " updatePhysicsCalibIC(): invalid module id " << PhysicsCalibICValue->getCellID()
				 <<", maybe incomplete installation "<<std::endl<<  e.what() << std::endl;
          }
      }

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        // delete old inter-calibration values
        if ( (*iter)->getPhysicsCalibIC() ) delete (*iter)->getPhysicsCalibIC();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

        /*
          just to be safe, create a temporary simple value
          (we delete the container in the end, and the SimpleValue does not
          belong to us)
        */
        SimpleValue *PhysicsCalibICTemp = PhysicsCalibICContainer->removeByCellID(ID);
        SimpleValue *PhysicsCalibIC = NULL;

        if (PhysicsCalibICTemp == NULL)
          {
	    PhysicsCalibIC = new SimpleValue(ID,
					       defaultPhysicsCalibIC ,
					       defaultPhysicsCalibICError,
					       1   //status
					       );
            bits.setPhysicsCalibICDefault();
          }
        else
          {
	    PhysicsCalibIC = new SimpleValue(PhysicsCalibICTemp->getCellID(),
					       PhysicsCalibICTemp->getValue(),
					       PhysicsCalibICTemp->getError(),
					       PhysicsCalibICTemp->getStatus());
	    bits.setPhysicsCalibICDefault(false);
          }

        if (_PhysicsCalibICScaled) {
          PhysicsCalibIC->setValue( PhysicsCalibIC->getValue() * _PhysicsCalibICScaleFactor );
          PhysicsCalibIC->setError( PhysicsCalibIC->getError() * _PhysicsCalibICScaleFactor );
          bits.setPhysicsCalibICScaled();
        }
        else {
          bits.setPhysicsCalibICScaled(false);
        }

        (*iter)->setStatus(bits.getInt());
        (*iter)->setPhysicsCalibIC(PhysicsCalibIC);
      }

    _PhysicsCalibICChanged = false;

    delete PhysicsCalibICContainer;

    std::cout << "end updatePhysicsCalibIC" << std::endl;
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::updateSaturationParameters()
  {
    std::cout << "update Saturation Parameters" << std::endl;

    if (!_saturationParametersCol)
      {
        streamlog_out(ERROR) << "Cannot update saturation, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    MappedContainer<SaturationParameters> *saturationParametersContainer = new MappedContainer<SaturationParameters>(_mapper);
    for (int i = 0; i < _saturationParametersCol->getNumberOfElements(); ++i)
      {
        SaturationParameters *saturationParam = new SaturationParameters(_saturationParametersCol->getElementAt(i)); // this will make a copy of the data (not link against the data like SimpleValue)
	
        try
          {
            saturationParametersContainer->fillByModuleID(saturationParam->getCellID(), saturationParam);
          }

        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " invalid module id " << saturationParam->getCellID() <<", maybe incomplete installation "
                         <<std::endl<<  e.what() << std::endl;
          }
      }

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        // delete old saturation
        if ( (*iter)->getSaturation() ) delete (*iter)->getSaturation();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

        // clear default bits
        bits.setSaturationParametersDefault(false);

        SaturationParameters *saturation = saturationParametersContainer->removeByCellID(ID); // we can savely use these, as data was copied and not linked
	
        if (saturation == NULL) // if there was nothing, generate a default scaling
          {
            int index = ID;
            const float Neffpx = 1600;
            float fraction = 1.;
            float epsilon = 1.;

            saturation = new SaturationParameters(index, Neffpx, fraction, epsilon);
            bits.setSaturationParametersDefault();
	    bits.setisNewITEP(false);
          }

	if(saturation->getNpix() > 11000)//Check if tile is new ITEP
	  bits.setisNewITEP(true);

        (*iter)->setStatus(bits.getInt());
        (*iter)->setSaturation(saturation);

	//TODO: change to DEBUG
	streamlog_out(DEBUG)<<"ID: " <<ID 
		      <<"  CellID: " << saturation->getCellID()
		      <<" Neffpx: " << saturation->getNpix()
			      <<" fraction: " << saturation->getFraction()
			      <<" epsilon: " << saturation->getEpsilon()
			      << std::endl;
      }

    _saturationParametersChanged = false;

    delete saturationParametersContainer;

    std::cout << "end update Saturation Parameters" << std::endl;
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/

  void Ahc2CalibrationsProcessor::updateTimeSlopesParameters()
  {
    std::cout << "update Time Slopes Parameters" << std::endl;

    if (!_timeSlopesParametersCol)
      {
        streamlog_out(ERROR) << "Cannot update time slopes, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    MappedContainer<SimpleValueVector> *timeSlopesParametersContainer = new MappedContainer<SimpleValueVector>(_mapper);
    for (int i = 0; i < _timeSlopesParametersCol->getNumberOfElements(); ++i)
      {
        SimpleValueVector *timeSlopesParam = new SimpleValueVector(_timeSlopesParametersCol->getElementAt(i)); // this will make a copy of the data (not link against the data like SimpleValue)
	
        try
          {
            timeSlopesParametersContainer->fillByModuleID(timeSlopesParam->getCellID(), timeSlopesParam);
          }

        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " invalid module id " << timeSlopesParam->getCellID() <<", maybe incomplete installation "
				 <<std::endl<<  e.what() << std::endl;
          }
      }

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        // delete old saturation
        if ( (*iter)->getTimeSlopes() ) delete (*iter)->getTimeSlopes();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

        // clear default bits
        bits.setTimeSlopesParametersDefault(false);

        SimpleValueVector *timeslope = timeSlopesParametersContainer->removeByCellID(ID); // we can savely use these, as data was copied and not linked
	
        if (timeslope == NULL) // if there was nothing, generate a default scaling
          {
            int index = ID;
	    std::vector<float> slope;
	    for(int i = 0; i < 2; i++)
	      slope.push_back(1.6);
	    std::vector<float> slope_err;
	    for(int i = 0; i < 2; i++)
	      slope.push_back(0.5);
	    std::vector<int> status;
	    for(int i = 0; i < 2; i++)
	      slope.push_back(1);

            timeslope = new SimpleValueVector(index, 2, slope, slope_err, status);
            bits.setTimeSlopesParametersDefault();
          }

        (*iter)->setStatus(bits.getInt());
        (*iter)->setTimeSlopes(timeslope);

	//TODO: change to DEBUG
	streamlog_out(DEBUG)<<"ID: " <<ID 
			      <<"  CellID: " << timeslope->getCellID() << std::endl;
	
	for(int i = 0; i < timeslope->getSize(); i++)
	  {
	    streamlog_out(DEBUG) <<" slope: " << timeslope->getValue(i)
		      <<" slope_error: " << timeslope->getError(i)
		      <<" status: " << timeslope->getStatus(i);
	  }
	streamlog_out(DEBUG) << std::endl;
      }
    
    _timeSlopesParametersChanged = false;

    delete timeSlopesParametersContainer;

    std::cout << "end update Time Slopes Parameters" << std::endl;
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  
  void Ahc2CalibrationsProcessor::updateTimePedestalParameters()
  {
    std::cout << "update Time Pedestal Parameters" << std::endl;

    if (!_timePedestalParametersCol)
      {
        streamlog_out(ERROR) << "Cannot update time pedestal, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    MappedContainer<SimpleValueVector> *timePedestalParametersContainer = new MappedContainer<SimpleValueVector>(_mapper);
    for (int i = 0; i < _timePedestalParametersCol->getNumberOfElements(); ++i)
      {
        SimpleValueVector *timePedestalParam = new SimpleValueVector(_timePedestalParametersCol->getElementAt(i)); // this will make a copy of the data (not link against the data like SimpleValue)
	
        try
          {
            timePedestalParametersContainer->fillByModuleID(timePedestalParam->getCellID(), timePedestalParam);
          }

        catch(BadDataException& e)
          {
            streamlog_out(DEBUG) << " invalid module id " << timePedestalParam->getCellID() <<", maybe incomplete installation "
				 <<std::endl<<  e.what() << std::endl;
          }
      }

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        // delete old saturation
        if ( (*iter)->getTimePedestal() ) delete (*iter)->getTimePedestal();

        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

        // clear default bits
        bits.setTimePedestalParametersDefault(false);

        SimpleValueVector *timepedestal = timePedestalParametersContainer->removeByCellID(ID); // we can savely use these, as data was copied and not linked
	
        if (timepedestal == NULL) // if there was nothing, generate a default scaling
          {
            int index = ID;
	    std::vector<float> pedestal;
	    for(int i = 0; i < 16; i++)
	      pedestal.push_back(1000.);
	    std::vector<float> pedestal_err;
	    for(int i = 0; i < 16; i++)
	      pedestal_err.push_back(5.5);
	    std::vector<int> status;
	    for(int i = 0; i < 16; i++)
	      status.push_back(1);

            timepedestal = new SimpleValueVector(index, 16, pedestal, pedestal_err, status);
            bits.setTimePedestalParametersDefault();
          }

        (*iter)->setStatus(bits.getInt());
        (*iter)->setTimePedestal(timepedestal);

	//TODO: change to DEBUG
	streamlog_out(DEBUG)<<"ID: " <<ID 
			      <<"  CellID: " << timepedestal->getCellID() << std::endl;
	for(int i = 0; i < timepedestal->getSize(); i++)
	  {
	    streamlog_out(DEBUG) <<" pedestal: " << timepedestal->getValue(i)
		      <<" pedestal_error: " << timepedestal->getError(i)
		      <<" status: " << timepedestal->getStatus(i);
	  }
	streamlog_out(DEBUG) << std::endl;
      }

    _timePedestalParametersChanged = false;

    delete timePedestalParametersContainer;

    std::cout << "end update Time pedestal Parameters" << std::endl;
  }

  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/

  void Ahc2CalibrationsProcessor::updateCellQuality()
  {
    std::cout << "update CellQuality" << std::endl;

    if (!_cellQualityCol)
      {
        streamlog_out(ERROR) << "Cannot update bad cells, collection is not valid." << std::endl;
        throw StopProcessingException(this);
      }

    MappedContainer<CellQuality> *cellQualityContainer = new MappedContainer<CellQuality>(_mapper);
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

    for ( std::vector<Ahc2Calibrations*>::iterator iter = _allCalibrations.begin(); iter != _allCalibrations.end(); ++iter)
      {
        int ID = (*iter)->getCellID();
        Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( (*iter)->getStatus() );

        CellQuality *cellQuality = cellQualityContainer->getByCellID(ID);
        if (cellQuality != NULL) bits.setDead();
        else bits.setDead(false);

        (*iter)->setStatus(bits.getInt());
      }

    _cellQualityChanged = false;
    delete cellQualityContainer;

    std::cout << "end update CellQuality" << std::endl;

  }


  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::updateMapping()
  {
    // clear all objects in the container
    _container->clear();

    
    //initialise the container with SiPMCalibration objects for all valid cellIDs

    for ( CellIterator iter = _mapper->begin(); iter != _mapper->end(); ++iter)
      {
        int cellID = *iter;
        Ahc2Calibrations *thisCell = new Ahc2Calibrations();
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
  void Ahc2CalibrationsProcessor::processEvent( LCEvent *evt )
  {
    streamlog_out(DEBUG0)<<"\n\n EVENT: "<<evt->getEventNumber()<<std::endl;

    if (_mapperVersion != _mapper->getVersion()) this->updateMapping();
    _mapperVersion = _mapper->getVersion();

    // checks if rebuilding of calibration constants is necessary
    if (_MIPConstantChanged  || _MIPSlopeChanged)  updateMIPCalibration();
    if (_gainConstantChanged || _gainSlopeChanged) updateGainCalibration();
    if (_pedestalChanged)         updatePedestal();
    if (_temperatureChanged)      updateTemperature();
    if (_interCalibrationChanged) updateInterCalibration();
    if (_PhysicsCalibICChanged) updatePhysicsCalibIC();
    if (_saturationParametersChanged ) updateSaturationParameters();
    if (_timeSlopesParametersChanged ) updateTimeSlopesParameters();
    if (_timePedestalParametersChanged ) updateTimePedestalParameters();
    if (_cellQualityChanged)      updateCellQuality();

  }



  /***************************************************************************************/
  /*                                                                                     */
  /*                                                                                     */
  /*                                                                                     */
  /***************************************************************************************/
  void Ahc2CalibrationsProcessor::end()
  {
    delete _Ahc2CalibrationsContainerMap[name()];
    _Ahc2CalibrationsContainerMap.erase(name());
    _container = NULL;
  }


  /***************************************************************************************
   * create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   ***************************************************************************************/
  Ahc2CalibrationsProcessor aAhc2CalibrationsProcessor;

} // end namespace CALICE
