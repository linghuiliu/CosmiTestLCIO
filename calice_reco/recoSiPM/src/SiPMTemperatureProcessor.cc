#include <list>

#include "SiPMTemperatureProcessor.hh"
#include "SimpleValue.hh"
#include "AhcSimpleTempProvider.hh"
#include "AhcCern2010TempProvider.hh"
#include "AhcMedianFilterTempProvider.hh"
#include "MappingProcessor.hh"

#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"
#include "EVENT/LCParameters.h"
#include "IMPL/LCCollectionVec.h"
#include "lccd/LCConditionsMgr.hh"
#include "lccd/IConditionsHandler.hh"


namespace CALICE
{
  SiPMTemperatureProcessor aSiPMTemperatureProcessor;
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*generate instance of static object*/
  std::map<std::string, AhcTempProvider*> SiPMTemperatureProcessor::_tempProviderMap;

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  AhcTempProvider* SiPMTemperatureProcessor::getTemperatureProvider(const std::string& processorName)
  {
    return _tempProviderMap[processorName];
  }

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  SiPMTemperatureProcessor::SiPMTemperatureProcessor() : Processor("SiPMTemperatureProcessor")
  {

    _description = "Provides the temperature of the SiPMs";

    registerProcessorParameter( "AHC_SRO_ModData" ,
                                "Name of the conditions collection for the AHC Slow r/o data.",
                                _ahcSroModDataColName,
                                std::string("AhcSroModData_fixed") ) ;

    registerProcessorParameter("TemperatureSensorCalibrationCollectionName",
                               "Name of collection of temperature sensors",
                               _temperatureSensorCalibrationColName,
                               std::string("AhcTempSensorCalib"));

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides"
				" the geometry of the detector.",
                                _mappingProcessorName,
                                std::string("AHC") ) ;

    lcio::FloatVec sanityRangeVec;
    sanityRangeVec.push_back(0);
    sanityRangeVec.push_back(45);
    registerProcessorParameter( "TempProviderSanityRange" ,
                                "Sanity range of the temperature provider",
                                _tempProviderSanityRangeVec,
                                sanityRangeVec);

    registerOptionalParameter( "TempProviderModel" ,
                               "Temperature model: if 'simple', use AhcSimpleTempProvider,"
			       " else 'MedianFilter' AhcMedianFilterTempProvider, "
			       " else 'Cern2010' AhcCern2010TempProvider",
                               _tempProviderModelName,
                               std::string("simple"));

    registerProcessorParameter("OutputCollection",
                               "Name of output collection of temperatures",
                               _outputColName,
                               std::string("AhcTemperature"));
 }
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  void SiPMTemperatureProcessor::init()
  {
    _ahcSroModDataCol                = NULL;
    _temperatureSensorCalibrationCol = NULL;

    _ahcSroModDataColChanged                = false;
    _temperatureSensorCalibrationColChanged = false;

    /*-------------------------------------------------------------*/
    std::stringstream message;
    message << "undefined conditions: ";
    bool error = false;

    /*-------------------------------------------------------------*/
    _mapper = dynamic_cast<const AhcMapper*>(MappingProcessor::getMapper(_mappingProcessorName));

    if ( ! _mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName 
		<< ") did not return a valid mapper." << std::endl;
        error = true;
      }

    /*-------------------------------------------------------------*/
    if (_tempProviderModelName == "simple")
      {
        _tempProvider = new AhcSimpleTempProvider();	 
        _tempProvider->setSanityRange(0, 45);
      }
    else if (_tempProviderModelName == "Cern2010")
      {
	_tempProvider = new AhcCern2010TempProvider(_mapper);
      }
    else if (_tempProviderModelName == "MedianFilter")
      {
	_tempProvider = new AhcMedianFilterTempProvider(_mapper);
	_tempProvider->setSanityRange(0, 45);
      }
    else
      {
        message<<" Undefined model "<<_tempProviderModelName<<" for the temperature provider"<<std::endl;
        error = true;
      }
 
    _tempProviderMap[Processor::name()] = _tempProvider;


    /*-------------------------------------------------------------*/
    /* registration of the conditions change listeners has to go here*/
    if (!ConditionsProcessor::registerChangeListener(this, _ahcSroModDataColName))
      {
        message << " undefined conditions: " <<_ahcSroModDataColName << std::endl;
        error = true;
      }
    if (!ConditionsProcessor::registerChangeListener(this, _temperatureSensorCalibrationColName))
      {
        message << " undefined conditions: " <<_temperatureSensorCalibrationColName << std::endl;
        error = true;
      }

    if (error)
      {
        streamlog_out(ERROR) << message.str();
        throw marlin::StopProcessingException(this);
      }

    /*-------------------------------------------------------------*/
   _colTemperature = NULL;

    _conditionsHandler = new CALICE::RunTimeConditionsHandler(_outputColName);
    lccd::LCConditionsMgr::instance()->registerHandler(_outputColName, _conditionsHandler);
  }

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  void SiPMTemperatureProcessor::conditionsChanged(LCCollection *col)
  {
    std::string colName = col->getParameters().getStringVal("CollectionName") ;

    if (colName == _ahcSroModDataColName)
      {
        _ahcSroModDataCol        = col;
        _ahcSroModDataColChanged = true;
      }
    else if (colName == _temperatureSensorCalibrationColName)
      {
        _temperatureSensorCalibrationCol        = col;
        _temperatureSensorCalibrationColChanged = true;
      }
    else
      {
	streamlog_out(ERROR) << "SiPMTemperatureProcessor: Called as conditions listener for collection " << colName 
		     << ", but not responsible." << std::endl;
        throw StopProcessingException(this);
      }

  }

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  void SiPMTemperatureProcessor::processEvent(LCEvent *evt)
  {
    streamlog_out(DEBUG0)<<"\n\n Event "<<evt->getEventNumber()<<std::endl;
    streamlog_out(DEBUG0)<<" _ahcSroModDataColChanged: "<<_ahcSroModDataColChanged
		 <<" (collection "<<_ahcSroModDataColName
		 <<")"<<std::endl;
    streamlog_out(DEBUG0)<<" _temperatureSensorCalibrationColChanged: "
		 <<_temperatureSensorCalibrationColChanged
		 <<" (collection "<<_temperatureSensorCalibrationColName<<")"
		 <<std::endl;


    if (_ahcSroModDataColChanged == true)
      {
        _tempProvider->setAhcSroModBlocks(_ahcSroModDataCol);
      }

    if (_temperatureSensorCalibrationColChanged == true)
      {
        _tempProvider->setCalibrations(_temperatureSensorCalibrationCol);
      }


    streamlog_out(DEBUG)<<"\n maxModule: "<< _mapper->getMaxModule()<<" maxChip: "<<_mapper->getMaxChip()<<" maxChannel: "<<_mapper->getMaxChannel()<<endl;
    /*fill the temperature container if something changed in the temperature provider*/
    if (_ahcSroModDataColChanged == true || _temperatureSensorCalibrationColChanged == true)
      {
	_colTemperature = new LCCollectionVec(LCIO::LCGENERICOBJECT);

	/*yes, the AHCAL modules start from 1, because _mapper->getMaxModule() gives either 31 or 39*/
	for (unsigned int module = 1; module < _mapper->getMaxModule(); ++module)
	  {
	    for (unsigned int chip = 0; chip < _mapper->getMaxChip(); ++chip)
	      {
		for (unsigned int channel = 0; channel < _mapper->getMaxChannel(); ++channel)
		  {
		    const float temperature = _tempProvider->getCellTemp(module, chip, channel);
		    const float temperatureError = _tempProvider->getCellTempError(module, chip, channel);

		    streamlog_out(DEBUG)<<"   mod/chip/chan="<<module<<"/"<<chip<<"/"<<channel<<" temperature="<<temperature<<" +- "<<temperatureError<<endl;
		    try {
		      int moduleID = _mapper->getDecoder()->getModuleID(module, chip, channel);
		      
 		      SimpleValue *val = new SimpleValue(moduleID, temperature, temperatureError, 1);
		      _colTemperature->addElement(val);
		      
		      streamlog_out(DEBUG0)<<" moduleID="<<moduleID<<std::endl;
		    }                        
		    catch(BadDataException& e)
		      {
			streamlog_out(DEBUG0) << " invalid cell id for module " << module <<", chip "<<chip
				      <<" and channel "<<channel<<" \n"<< e.what() << std::endl;
		      }
		
		  }
	      }
	  }/*end loop over module*/

	const std::string encoding = _mapper->getDecoder()->getCellIDEncoding();
	LCParameters &theParam =  _colTemperature->parameters();
	theParam.setValue(LCIO::CellIDEncoding, encoding);
	
	_conditionsHandler->update(_colTemperature);
	
	streamlog_out(DEBUG0)<<" updating colTemperature: "<<_colTemperature<<endl;
      }

    /*----------------------------------------------------------------------*/
    if (_ahcSroModDataColChanged == true)
      _ahcSroModDataColChanged = false;
    if (_temperatureSensorCalibrationColChanged == true)
      _temperatureSensorCalibrationColChanged = false;
    
  }


}/*end of namespace CALICE*/
