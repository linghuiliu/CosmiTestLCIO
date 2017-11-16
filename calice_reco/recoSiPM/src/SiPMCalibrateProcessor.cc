#include "SiPMCalibrateProcessor.hh"
#include "AdcBlock.hh"
#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "SiPMCalibrations.hh"
#include "CellIterator.hh"
#include "SiPMTemperatureProcessor.hh"
#include "SiPMCalibrationStatusBits.hh"

#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTOOLS.h"
#include "EVENT/SimCalorimeterHit.h"

#include <cstdlib>

using std::endl;

namespace CALICE
{
  SiPMCalibrateProcessor aSiPMCalibrateProcessor;

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  SiPMCalibrateProcessor::SiPMCalibrateProcessor():Processor("SiPMCalibrateProcessor")
  {
    _description = "Does calibration of AHCal hits";

    registerProcessorParameter("InputCollectionName",
                               "Name of the input collection",
                               _inputColName,
                               std::string("CALDAQ_ADCCol"));

    registerProcessorParameter("OutputAhcHitCollectionName",
                               "Name of the output AHCal hit collection, of type CalorimeterHit",
                               _ahcHitOutputColName,
                               std::string("AhcCalorimeter_Hits"));

    registerProcessorParameter("OutputAhcAmplCollectionName",
                               "Name of the output AHCal amplitude collection, of type LCGenericObject",
                               _ahcAmplOutputColName,
                               std::string("AhcAmplitude"));

    registerProcessorParameter("HitAmplRelationName",
                               "Name of the output LCRelation between CalorimeterHit and AhcAmplitude",
                               _hitAmplRelationColName,
                               std::string("AhcHitAmplitudeRelation"));

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides"
                                " the geometry of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
                                "Name of the CellDescriptionProcessor instance that provides"
				" the corrected position of the cells." ,
                                _cellDescriptionProcessorName,
                                std::string("MyCellDescriptionProcessor") ) ;

    registerProcessorParameter( "SiPMCalibrationsProcessorName" ,
                                "Name of the SiPMCalibrationsProcessor that provides"
                                " the calibrations of the AHCal tiles." ,
                                _calibProcessorName,
                                std::string("MySiPMCalibrationsProcessor") ) ;

    registerProcessorParameter( "SiPMTemperatureProcessorName" ,
                                "Name of the SiPMTemperatureProcessor that provides"
                                " the AHCal temperature provider." ,
                                _temperatureProcessorName,
                                std::string("MySiPMTemperatureProcessor") ) ;
   
    registerProcessorParameter( "SimRecRelationName" ,
                                "Name of the relation between the Mokka SimCalorimeterHits"
				" and reconstructed CalorimeterHits",
                                _simRecRelationColName,
                                std::string("AhcSimRecRelation") ) ;
 
    registerProcessorParameter( "SimHitName" ,
                                "Name of the Mokka SimCalorimeterHits collection",
                                _simHitColName,
                                std::string("hcalSD") ) ;
 
    registerProcessorParameter("PedestalSubtraction",
                               "apply (1) or ignore (0) the pedestal subtraction",
                               _pedestalSubtraction,
                               (bool) true);

    registerProcessorParameter("ZeroSuppression",
                               "apply (1) or ignore (0) the MIP threshold cut",
                               _zeroSuppression,
                               (bool) true);

    registerProcessorParameter("MipToGeVFactor", "AHCal conversion factor from MIP to GeV",
                               _mipToGeVFactor,
                               (float) 0.0255);

    registerProcessorParameter("MipCut", "Minimal energy deposition in "
                               "units of MIP to keep hit, applied only if ZeroSuppression==1.",
                               _mipCut,
                               (float) 0.4);

    registerProcessorParameter("doMipTemperatureCorrection",
                               "Do MIP temperature correction",
                               _doMipTempCorr,
                               (bool) true);

    registerProcessorParameter("doGainTemperatureCorrection",
                               "Do gain temperature correction",
                               _doGainTempCorr,
                               (bool) true);

    registerProcessorParameter("doSaturationCorrection",
                               "Do saturation correction",
                               _doSaturationCorr,
                               (bool) true);

    registerProcessorParameter("doErrorCalculation",
                               "Do error calculation",
                               _doErrorCalculation,
                               (bool) false);

    registerProcessorParameter("doWriteOnlyRawAmplitude",
                               "Write only the raw amplitude to the output collection"
			       " (i.e. don't do any calibration, used for creation of noise files)",
                               _doWriteOnlyRawAmplitude,
                               (bool) false);

    registerProcessorParameter("filterDeadCells",
                               "Filter dead cells",
                               _filterDeadCells,
                               (bool) true);

    registerProcessorParameter("filterDefaultCells",
                               "Filter cells that use some default value in calibration.",
                               _filterDefaultCells,
                               (bool) false);

    registerOptionalParameter( "ScaleEnergy" ,
                               "scale factor for the energy",
                               _energyScaleFactor,
                               (float)1. ) ;
                               
    registerProcessorParameter("correctDefaultGainTofixedLightYield",
                               "Correct light yield for cells with default gain value in calibration.",
                               _correctDefaultGainToLY,
                               (bool) false);
                               
    registerProcessorParameter( "fixedLightYieldForCorrection" ,
                               "Fixed light yield for cells with default gain value in calibration.",
                               _fixedLY,
                               (float)15. ) ;
                               
  }
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  void SiPMCalibrateProcessor::init()
  {
    printParameters();

    std::stringstream message;
    bool error = false;

    _isFirstEvent = true;
    _isDATA = false;
    _isMC = false;

    _mapper = dynamic_cast<const Mapper*>(MappingProcessor::getMapper(_mappingProcessorName));
    if ( !_mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName
                << ") did not return a valid mapper." << endl;
        error = true;
      }

    _calibContainer = SiPMCalibrationsProcessor::getCalibrations(_calibProcessorName);
    if ( ! _calibContainer )
      {
        message << "init(): SiPMCalibrationsProcessor::getCalibrations("<< _calibProcessorName
                << ") did not return a valid MappedContainer." <<endl;
        error = true;
      }

    _cellDescriptions = CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if ( ! _cellDescriptions ) 
      {
	streamlog_out(ERROR) << "Cannot obtain cell descriptions from "<<_cellDescriptionProcessorName
		     <<" Maybe, processor is not present" << endl;
	error = true;
      }
    
    if (error)
      {
        streamlog_out(ERROR) << message.str();
        throw marlin::StopProcessingException(this);
      }

    _scaleEnergy = parameterSet("ScaleEnergy");
    _createSimRecRelation = (parameterSet("SimRecRelationName") && parameterSet("SimHitName"));
  }

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  void SiPMCalibrateProcessor::processEvent(LCEvent *evt)
  {
    std::stringstream message;
    bool error=false;

    if (error)
      {
        streamlog_out(ERROR) << message.str();
        throw marlin::StopProcessingException(this);
      }

    LCCollection *inputCol = NULL;
    try
      {
        inputCol = evt->getCollection(_inputColName);
      }
    catch (EVENT::DataNotAvailableException &e)
      {
 	streamlog_out(WARNING)<< "missing collection "
		      <<_inputColName<<endl<<e.what()<<endl;
        return;
      }

    /*----------------------------------------------------------------------*/
    /*                                                                      */
    /*       start loop over input collection                               */
    /*                                                                      */
    /*----------------------------------------------------------------------*/
    int noElem = inputCol->getNumberOfElements();
    if (noElem <= 0) return;

    streamlog_out(DEBUG0)<<" \n\n=========================Start to process event "<<evt->getEventNumber()<<endl;
    streamlog_out(DEBUG0)<<"inputcol "<<_inputColName<<" has "<<noElem<<" hits"<<endl;

    LCCollection *ahcHitOutputCol  = new LCCollectionVec(LCIO::CALORIMETERHIT);
    /*we want to save the position, and this can only be done if the CHBIT_LONG bit is set*/
    ahcHitOutputCol->setFlag(ahcHitOutputCol->getFlag() | 1 << LCIO::CHBIT_LONG );
    LCCollection *ahcAmplOutputCol = new LCCollectionVec(LCIO::LCGENERICOBJECT);
    LCRelationNavigator relationNavigator(LCIO::CALORIMETERHIT, LCIO::LCGENERICOBJECT);

    if (_createSimRecRelation == true)
      _recoContainer = new MappedContainer<CalorimeterHitImpl>(_mapper, false);


    /*get the collection type and use it to distinguish between data and Monte Carlo;
     if collectionType = LCGENERICOBJECT => AdcBlock => data
     else if collectionType = CALORIMETERHIT => Monte Carlo*/
    const std::string collectionType = inputCol->getTypeName();
    std::string outputEncodingString = "";


    DecoderSet *decoder = _calibContainer->getDecoder();
    decoder->setCellIDEncoding(inputCol->getParameters().getStringVal(LCIO::CellIDEncoding));

    if( _isFirstEvent == true && inputCol != 0)
      {
	if(collectionType == "LCGenericObject"){_isDATA = true;}
	else if(collectionType == "CalorimeterHit"){_isMC = true;}
	_isFirstEvent = false;
      }

 
    if (_isDATA)/*if data*/
      {
	for (unsigned int i = 0; i < (unsigned int)noElem; ++i)
	  {
	    const AdcBlock adcBlock(inputCol->getElementAt(i));
	    	    
	    for (short chip = 0; chip < 12; ++chip)
	      {
		const int daqChannelID = decoder->getDAQID(adcBlock.getCrateID(),//crate 
							   adcBlock.getSlotID(), //slot, 
							   adcBlock.getBoardFrontEnd(), //fe, 
							   chip, 
							   adcBlock.getMultiplexPosition()//channel
							   );
		
		SiPMCalibrations *calibration = _calibContainer->getByDAQID(daqChannelID);
		if (calibration == NULL) 
		  {
		    continue;
		  }

		streamlog_out(DEBUG0) << "\n\nStart calibrating hit with crate/slot/fe/chip/chan "
			      << adcBlock.getCrateID() << "/" << adcBlock.getSlotID() 
			      << "/" << adcBlock.getBoardFrontEnd() << "/" << chip 
			      << "/" << adcBlock.getMultiplexPosition() << endl;
		
		outputEncodingString = calibration->getCellIDEncoding();
		const float rawEnergy = (float)adcBlock.getAdcVal(chip);
		
		/*do the energy calibration*/
		streamlog_out(DEBUG0)<<"------------------------------------"<<endl;
		streamlog_out(DEBUG0)<<"\n before calibrateEnergyAndFillOutputCollections"<<endl;

		/*---------------------------------------------------------------------*/
		/*in case of writing the noise files, we only need the raw amplitude to 
		 be saved in the output collection*/
		if (_doWriteOnlyRawAmplitude)
		  {
		    SimpleValue *pedestalContainer = calibration->getPedestal();
		    if (pedestalContainer == NULL) 
		      {
			streamlog_out(DEBUG0) << " SKIP -- no pedestal available: " << endl;
			return;
		      }
		    float pedestal = pedestalContainer->getValue();

		    int cellID = calibration->getCellID();

		    CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
		    newHit->setEnergy(rawEnergy - pedestal);
		    newHit->setCellID0(cellID);
		    
		    /*add collection of AHCal raw hits to the event*/
		    ahcHitOutputCol->addElement(newHit);
		    
		    const string encodingString = calibration->getCellIDEncoding();
		    LCParameters &param = ahcHitOutputCol->parameters();
		    param.setValue(LCIO::CellIDEncoding, encodingString);
		    
		    streamlog_out(DEBUG0)<<" \n output collection "<<_ahcHitOutputColName<<" has "
				 <<ahcHitOutputCol->getNumberOfElements()<<" elements"<<endl;
		    streamlog_out(DEBUG0)<<"=====================\n\n" << endl;
		    }
		
		else 
		  {
		    this->calibrateEnergyAndFillOutputCollections(calibration, rawEnergy,
								  ahcHitOutputCol, ahcAmplOutputCol, 
								  relationNavigator);
		  }
		
	      }/*--------------- end loop over chips-------------------------*/
	  }/*--------------end loop over input collection elements ----------------------*/
      }/*------------------- end if data --------------------------------*/

    else if (_isMC)/*Monte Carlo*/
      {
	for (unsigned int i = 0; i < (unsigned int)noElem; ++i)
	  {
	    CalorimeterHit *digiHit = dynamic_cast<CalorimeterHit*>(inputCol->getElementAt(i));
	    int cellID = digiHit->getCellID0();


	    //decoder->setCellIDEncoding(encodingString);
	    //decoder->setCellIDEncoding(inputCol->getParameters().getStringVal(LCIO::CellIDEncoding));

	    streamlog_out(DEBUG0)<<"\n\n---------------------------------------------------------"<<endl;
	    streamlog_out(DEBUG0)<<"encodingString: "<<inputCol->getParameters().getStringVal(LCIO::CellIDEncoding)
			 <<" of collection "<<_inputColName
			 <<" with "<<inputCol->getNumberOfElements()<<" elements"
			 <<endl;
	    streamlog_out(DEBUG0)<<" \nencoding string of mapper: "<< _mapper->getDecoder()->getCellIDEncoding()<<endl;


	    _mapper->getDecoder()->setCellIDEncoding(inputCol->getParameters().getStringVal(LCIO::CellIDEncoding));
	    streamlog_out(DEBUG0)<<"i="<<i<<" digiHit: I/J/K="<<decoder->getIFromCellID(cellID)
			 <<"/"<<decoder->getJFromCellID(cellID)
			 <<"/"<<decoder->getKFromCellID(cellID)
			 <<", module="<<_mapper->getModuleFromCellID(cellID)
			 <<endl;
	    

	    SiPMCalibrations *calibration = _calibContainer->getByCellID(cellID);
	    if (calibration == NULL)
	      {
		streamlog_out(DEBUG0)<<"  no calibration found for hit I/J/K="
			     <<decoder->getIFromCellID(cellID)
			     <<"/"<<decoder->getJFromCellID(cellID)
			     <<"/"<<decoder->getKFromCellID(cellID)
			     <<endl;
		continue;
	      }

	    streamlog_out(DEBUG0)<<" encoding string of SiPMCalibrations: "<<calibration->getCellIDEncoding()<<endl;
	    streamlog_out(DEBUG0)<<"cellID from digi hit:    "<<cellID<<endl;
	    streamlog_out(DEBUG0)<<"cellID from calibration: "<<calibration->getCellID()<<endl;
	    streamlog_out(DEBUG0)<<"calibration: "<<calibration<<" for I/J/K "<<decoder->getIFromCellID(cellID)
			 <<"/"<<decoder->getJFromCellID(cellID)
			 <<"/"<<decoder->getKFromCellID(cellID)
			 <<endl;

	    outputEncodingString = calibration->getCellIDEncoding();
	    const float rawEnergy  = digiHit->getEnergy();

	    /*do the energy calibration*/
	    streamlog_out(DEBUG0)<<"\n before calibrateEnergyAndFillOutputCollections"<<endl;

	    this->calibrateEnergyAndFillOutputCollections(calibration, rawEnergy,
							  ahcHitOutputCol, ahcAmplOutputCol, relationNavigator);
	  }/*---------- end of loop over input collection elements ------------------------*/
      }/*------------------- end if MC --------------------------------*/
    else/*--------- not supported (neither DATA nor MC)----------------*/
      {
	streamlog_out(ERROR)<<" Nothing to be done for collections of type "<<collectionType<<endl;
	return;
      }
	


    /* add AHCal collection to event*/
    //move relationNavigator here
    /*add the LCRelation between CalorimeterHit and AhcAmplitude to the event*/
    if (ahcHitOutputCol->getNumberOfElements() > 0)
      {
	LCParameters &param = ahcHitOutputCol->parameters();
	param.setValue(LCIO::CellIDEncoding, outputEncodingString);

	evt->addCollection(ahcHitOutputCol, _ahcHitOutputColName.c_str());
	evt->addCollection(relationNavigator.createLCCollection(), _hitAmplRelationColName);
	/*checkHitAmplRelation(evt);*/
	streamlog_out(DEBUG0)<<" Collection "<<_ahcHitOutputColName<<" added to the event"<<endl;
      } else {delete ahcHitOutputCol;}

    /* add AhcAmplitude collection to event*/
    if (ahcAmplOutputCol->getNumberOfElements() > 0)
      {
	evt->addCollection(ahcAmplOutputCol, _ahcAmplOutputColName.c_str());
	streamlog_out(DEBUG0)<<" Collection "<<_ahcAmplOutputColName<<" added to the event"<<endl;	
      } else {delete ahcAmplOutputCol;}


    /*----------------------------------------------------------------
      for the relation between CalorimeterHits and SimCalorimeterHits
      ------------------------------------------------------------------*/
    if (_createSimRecRelation == true)
      {
	this->createSimRecRelation(evt);
	delete _recoContainer;
      }
 

  }

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  void SiPMCalibrateProcessor::calibrateEnergyAndFillOutputCollections(SiPMCalibrations* calibration, 
								       const float rawEnergy,
								       LCCollection* ahcHitOutputCol,
								       LCCollection* ahcAmplOutputCol,
								       LCRelationNavigator &relationNavigator)
  {
   const SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( calibration->getStatus() );
    streamlog_out(DEBUG0) << "  STATUS bits: " << bits << endl;
    
    if ( _filterDeadCells && bits.isDead() ) 
      {
	streamlog_out(DEBUG0) << " SKIP -- channel is marked as dead: " << endl;
	return;
      }

    if ( _filterDefaultCells && bits.hasDefault() ) 
      {
	streamlog_out(DEBUG0) << " SKIP -- channel has default calibration: " << endl;
	return;
      }
    
    /*-------------------- calibration coefficients --------------*/
    LinearFitCompound *lfcMIP            = calibration->getMIP();
    LinearFitCompound *lfcGain           = calibration->getGain();
    SimpleValue *interCalibContainer     = calibration->getInterCalibration();
    SatCorrFunction *saturationContainer = calibration->getSaturationCorrection();
    
    /*------------------------------------------------
      get pedestal
      -----------------------------------------------*/
    float pedestal      = 0;
    float pedestalError = 0;
    
    SimpleValue *pedestalContainer = calibration->getPedestal();
    if (_pedestalSubtraction == 1)
      {
	if (pedestalContainer == NULL) {
	  streamlog_out(DEBUG0) << " SKIP -- no pedestal available: " << endl;
	  return;
	}
	
	pedestal = pedestalContainer->getValue();
	
	/*
	  What about the significance cut?
	  need pedestal width !
	*/
      }
    

    /*----------------------------------------------
      get temperature
    -----------------------------------------------*/   
    SimpleValue *temperatureObject = calibration->getTemperature();
    float temperature = 0;
    if (temperatureObject != NULL) temperature = temperatureObject->getValue();
    else
      {
	streamlog_out(ERROR)<<" No temperature found"<<endl;
	exit(1);
      }
    
    int cellID = calibration->getCellID();
    _mapper->getDecoder()->setCellIDEncoding(calibration->getCellIDEncoding());
    
    streamlog_out(DEBUG0) << "  cellID: "<<cellID<<endl;
    streamlog_out(DEBUG0) << "  from module " << _mapper->getModuleFromCellID(cellID) << endl;
    streamlog_out(DEBUG0) << "  with amplitude: "<< rawEnergy << endl
		  << "  @ temperature: " << temperature << " deg. C" << endl;

    /*------------------------------------------------
      get MIP, gain and IC
      -----------------------------------------------*/
    
    /*---------------------- MIP ---------------------------------*/
    float mipValue = 0;
    
    if (_doMipTempCorr) mipValue = lfcMIP->eval(temperature);
    else
      mipValue = lfcMIP->getConstant();
    if (mipValue <= 0)
      {
	/*
	  streamlog_out(ERROR0) << "  ERROR - ignoring cell with negative/"
	  << "zero MIP coefficient at I/J/K "
	  << _mapper->getDecoder()->getIFromCellID(cellID)
	  <<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
	  <<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
	  << endl;
	*/
	return;
      }


    /*---------------------- gain ---------------------------------*/
    float gainValue = 0;
    if (_doGainTempCorr) gainValue = lfcGain->eval(temperature);
    else                 gainValue = lfcGain->getConstant();
    if (gainValue <= 0)
      {
	/*
	  streamlog_out(ERROR0) << "  ERROR - ignoring cell with negative/"
	  << "zero gain coefficient at I/J/K "
	  << _mapper->getDecoder()->getIFromCellID(cellID)
	  <<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
	  <<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
	  << endl;
	*/
	return;
      }

    float interCalibrationValue = interCalibContainer->getValue();
    if (interCalibrationValue <= 0)
      {
	streamlog_out(ERROR0) << "  ERROR - ignoring cell with negative/"
		      << "zero inter calibration at I/J/K "
		      << _mapper->getDecoder()->getIFromCellID(cellID)
		      <<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
		      <<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
		      << endl;
	return;
      }
    
    streamlog_out(DEBUG0) << "  - pedestal: " << pedestal << endl;
    streamlog_out(DEBUG0) << "  - MIP: " << mipValue << endl;
    streamlog_out(DEBUG0) << "  - gain: " << gainValue << endl;
    streamlog_out(DEBUG0) << "  - intercalibration: " << interCalibrationValue << endl;
    
    
    //check if gainConstant or IC is default => put gainConst to have fixed LY.
    //This prevents an event to have the "strange" desaturated amplitudes        
    if ( _correctDefaultGainToLY && (bits.gainConstantIsDefault() || bits.interCalibrationIsDefault()) ) {
      gainValue = mipValue * interCalibrationValue / _fixedLY;
    } 
        
    /*----------------------------------------------
      do calibration
      ---------------------------------------------*/
    float calibratedEnergy      = 0;

    if ( _doSaturationCorr )
      {
	streamlog_out(DEBUG0)<< "  - amplitude [pix]: " << rawEnergy * mipValue/gainValue * interCalibrationValue<<endl;
	streamlog_out(DEBUG0)<< "  - corrected [pix]: " 
		     << saturationContainer->deSaturate( (rawEnergy - pedestal) 
							 * interCalibrationValue / gainValue )
		     <<endl;

	calibratedEnergy = saturationContainer->deSaturate( (rawEnergy - pedestal) * interCalibrationValue / gainValue )
	  * gainValue / interCalibrationValue / mipValue;

	streamlog_out(DEBUG0) << "  - saturationCorrectionFactor: "
		      << saturationContainer->deSaturate( (rawEnergy - pedestal) * interCalibrationValue / gainValue )
	  / ( (rawEnergy - pedestal) * interCalibrationValue / gainValue)
		      << endl;
      }
    else
      {
	calibratedEnergy      = (rawEnergy - pedestal) / mipValue;
      }
    
    
   streamlog_out(DEBUG0)<<"  - calibratedEnergy[MIPs]: "<<calibratedEnergy<<endl;
    /*--------------------------------------
     * if requested, do error calculation
     ---------------------------------------*/
   float calibratedEnergyError = 0;
   if ( _doErrorCalculation ) 
      {
	if (_pedestalSubtraction == 1) pedestalError = pedestalContainer->getError();
	
	const float interCalibrationError = interCalibContainer->getError();;
	const float temperatureError = temperatureObject->getError();
	
	float mipError  = 0;
	float gainError = 0;
	if (_doMipTempCorr) mipError = lfcMIP->evalErr(temperature,temperatureError);
	else                mipError = lfcMIP->getConstantError();
	
	if (_doGainTempCorr) gainError = lfcGain->evalErr(temperature,temperatureError);
	else                 gainError = lfcGain->getConstantError();
	
	streamlog_out(DEBUG0) << "  - MIP error: " << mipError << endl;
	streamlog_out(DEBUG0) << "  - gain error: " << gainError << endl;
	streamlog_out(DEBUG0) << "  - intercalibration error: " << interCalibrationError << endl;
		
	const float relativeErrorPedSubSquare = 
	  ( pow(calibratedEnergyError, 2) + pow(pedestalError, 2) ) / pow( rawEnergy - pedestal, 2);
	
	/*----------------------------------------
	  if requested, do saturation correction 
	  ---------------------------------------*/
	if ( _doSaturationCorr ) 
	  {
	    const float saturatedSignalInPixel = (rawEnergy - pedestal) * interCalibrationValue / gainValue ;
	    const float unsaturatedSignalInPixel = saturationContainer->deSaturate( saturatedSignalInPixel );
	    
	    /* assuming that the intercalibration error cancels in first order*/
	    const float saturatedSignalInPixelError = 
	      sqrt( relativeErrorPedSubSquare + pow(gainError/gainValue,2) ) * saturatedSignalInPixel;
	    
	    calibratedEnergyError =
	      sqrt( pow(saturationContainer->deSaturatedError(saturatedSignalInPixel,
							      saturatedSignalInPixelError)/unsaturatedSignalInPixel,2)
		    + pow(mipError/mipValue,2) ) * calibratedEnergy;
	  }
	else 
	  {
	    calibratedEnergyError =
	      sqrt( relativeErrorPedSubSquare + pow(mipError/mipValue,2) ) * calibratedEnergy;
	  }
      }

    /*-----------------------------------------------
      0.5 MIP cut
      ----------------------------------------------*/
    if ( _zeroSuppression &&  calibratedEnergy < _mipCut )
      {
	streamlog_out(DEBUG0) << " STOP - " << calibratedEnergy 
		      << " - below threshold of: " << _mipCut << endl;
	streamlog_out(DEBUG0)<<"  ------------------------------"<<endl;
	return;
      }

    streamlog_out(DEBUG0)<< " DONE - calibrated amplitude: " << calibratedEnergy;
    if (_doErrorCalculation)  streamlog_out( DEBUG0 ) << " +- " << calibratedEnergyError;
    streamlog_out(DEBUG0)<< endl;
 
    
  
    /*---------------------------------------------
      write output calorimeter hit
      --------------------------------------------*/
    CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
    if (_scaleEnergy) 
      {
	newHit->setEnergy(     calibratedEnergy*_energyScaleFactor );
	newHit->setEnergyError(calibratedEnergyError*_energyScaleFactor );
      }
    else
      {
	newHit->setEnergy(calibratedEnergy);
	newHit->setEnergyError(calibratedEnergyError);
      }
    newHit->setCellID0(cellID);

    _cellDescriptions->getDecoder()->setCellIDEncoding(calibration->getCellIDEncoding());
    CellDescription* cellDescription = _cellDescriptions->getByCellID(cellID);
    float newPosition[3] = {cellDescription->getX(), cellDescription->getY(), cellDescription->getZ()};
    
    streamlog_out(DEBUG0)<<" position: ("<<newPosition[0]<<", "<<newPosition[1]<<", "<<newPosition[2]<<std::endl;
    
    newHit->setPosition(newPosition);
     
    /*add collection of AHCal calibrated hits to the event*/
    ahcHitOutputCol->addElement(newHit);

    streamlog_out(DEBUG0)<<" \n output collection "<<_ahcHitOutputColName<<" has "
	     <<ahcHitOutputCol->getNumberOfElements()<<" elements"<<endl;
    streamlog_out(DEBUG0)<<"=====================\n\n" << endl;

    /*----------------------------------------------------------------
      for the relation between CalorimeterHits and SimCalorimeterHits
      ------------------------------------------------------------------*/
    if (_createSimRecRelation == true)
	    _recoContainer->fillByCellID(newHit->getCellID0(), newHit);
    
    /*--------------------------------------------------
      write output AHCal hit amplitude in different units
      -------------------------------------------------*/
    /*set the amplitude in the different units*/
    AhcAmplitude *ahcAmplitude = new AhcAmplitude(cellID,
						  rawEnergy,
						  rawEnergy - pedestal,
						  (rawEnergy - pedestal)/lfcMIP->eval(temperature),
						  (rawEnergy - pedestal)/lfcMIP->getConstant(),
						  calibratedEnergy * _mipToGeVFactor,
						  temperature);
  
    ahcAmplOutputCol->addElement(ahcAmplitude);

    LCParameters &paramAmpl = ahcAmplOutputCol->parameters();
    paramAmpl.setValue("DataDescription", ahcAmplitude->getDataDescription());

    /* ---------------------------------------------------------
      Set the relation between CalorimeterHit and AhcAmplitude
      -----------------------------------------------------------*/
    relationNavigator.addRelation(newHit, ahcAmplitude);

  }

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*                                                             */
  /*                                                             */
  /*                                                             */
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  void SiPMCalibrateProcessor::checkHitAmplRelation(LCEvent *evt)
  {
    LCCollection *relCol;
    try{
      relCol = evt->getCollection(_hitAmplRelationColName);
    }
    catch ( lcio::DataNotAvailableException err ) 
      {
        streamlog_out( WARNING )<<"Event "<<evt->getEventNumber() << ", no relation named "<<_hitAmplRelationColName<<" found" << endl;
	return ;
     }/*catch*/

    LCRelationNavigator relNav(relCol);

    LCCollection *hitCol = NULL;
    try
      {
        hitCol = evt->getCollection(_ahcHitOutputColName);
      }
    catch (EVENT::DataNotAvailableException &e)
      {
 	streamlog_out(WARNING)<< "missing collection "
		      <<_ahcHitOutputColName<<endl<<e.what()<<endl;
        return;
      }
    
    for (int i = 0; i < hitCol->getNumberOfElements(); ++i)
      {
	CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(hitCol->getElementAt(i));
	const LCObjectVec &amplVec = relNav.getRelatedToObjects(hit);
	if (amplVec.size() > 0) 
	  {	
	    AhcAmplitude *ahcAmpl = dynamic_cast<AhcAmplitude*>(amplVec[0]);
	    float amplGeV = ahcAmpl->getAmplGeV();
	    streamlog_out(DEBUG0)<<" ampl[GeV] = "<<amplGeV<<endl;
	  } 
	else
	  {
	    streamlog_out(DEBUG0)<<"empty AhcAmpl collection"<<endl;
	  }
      }
   
  }



  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void SiPMCalibrateProcessor::createSimRecRelation(LCEvent *evt)
  {
   const LCCollection *simHitCol = evt->getCollection( _simHitColName ) ;
    if (simHitCol == NULL)
      {
	streamlog_out(WARNING)<<" There is no collection named "<<_simHitColName
		      <<" which contains SimCalorimeterHits in this event ("
		      <<evt->getEventNumber()<<")"<<endl;
	return;
      }

    streamlog_out(DEBUG0)<<"\n\n============================="<<endl;
    streamlog_out(DEBUG0)<<"createSimRecRelation in event "<<evt->getEventNumber()<<endl;

    LCRelationNavigator relationNavigator(LCIO::CALORIMETERHIT, LCIO::SIMCALORIMETERHIT);

    int counter = 0;

    /*set the Mokka encoding string*/
    const std::string mokkaEncodingString = simHitCol->getParameters().getStringVal(LCIO::CellIDEncoding);
    DecoderSet *decoder = _mapper->getDecoder();
    decoder->setCellIDEncoding(mokkaEncodingString);

    for (int iHit = 0; iHit < simHitCol->getNumberOfElements(); ++iHit)
       {
	 SimCalorimeterHit *simHit = dynamic_cast<SimCalorimeterHit*>(simHitCol->getElementAt(iHit));
	 int mokkaCellID = simHit->getCellID0();
	 
	 int trueCellID = 0;

	 try
	   {
	     trueCellID = _mapper->getTrueCellID(mokkaCellID);
	   }
	 catch (BadDataException &e)
	   {
	     streamlog_out(DEBUG0)<<" invalid cell id "<<mokkaCellID<<endl<< e.what() << endl;
	     continue;
	   }

	 CalorimeterHitImpl *recoHit = _recoContainer->getByCellID(trueCellID);
	 
	 streamlog_out(DEBUG0)<<"  i: "<<iHit<<" mokkaCellID="<<mokkaCellID<<" I/J/K="
		      <<_mapper->getDecoder()->getIFromCellID(mokkaCellID)
		      <<"/"<<_mapper->getDecoder()->getJFromCellID(mokkaCellID)
		      <<"/"<<_mapper->getDecoder()->getKFromCellID(mokkaCellID)
		      <<", trueCellID="<<trueCellID
		      <<", I/J/K="<<_mapper->getDecoder()->getIFromCellID(trueCellID)
		      <<"/"<<_mapper->getDecoder()->getJFromCellID(trueCellID)
		      <<"/"<<_mapper->getDecoder()->getKFromCellID(trueCellID)
		      <<" recoHit="<<recoHit
		      <<endl;
	 
	 if (recoHit != NULL)
	   {
	     counter++;
	     relationNavigator.addRelation(recoHit, simHit);
	   }

       }/*end loop over SimCalorimeterHits*/

    streamlog_out(DEBUG0)<<"\n event: "<<evt->getEventNumber()<<" no sim hits: "<<simHitCol->getNumberOfElements()
		 <<" no rec hits: "<<counter
		 <<endl;
    
    evt->addCollection(relationNavigator.createLCCollection(), _simRecRelationColName);

  }
  
}/*end of namespace CALICE*/
