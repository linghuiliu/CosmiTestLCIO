#include "Ahc2CalibrateProcessor.hh"

#include "EUDAQBlock2016.hh"
#include "LabviewBlock2.hh"
#include "Mapper.hh"
#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "Ahc2Calibrations.hh"
#include "CellIterator.hh"
#include "SiPMTemperatureProcessor.hh"
#include "Ahc2CalibrationStatusBits.hh"
#include "Ahc2HardwareConnection.hh"
#include "Ahc2SatCorr.hh"

#include "marlin/Exceptions.h"
#include "marlin/ConditionsProcessor.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTOOLS.h"
#include "EVENT/SimCalorimeterHit.h"

#include <cstdlib>

using std::endl;

namespace CALICE
{
	Ahc2CalibrateProcessor aAhc2CalibrateProcessor;

	/***************************************************************************************/
	/*                                                                                     */
	/*                                                                                     */
	/*                                                                                     */
	/***************************************************************************************/
	Ahc2CalibrateProcessor::Ahc2CalibrateProcessor():Processor("Ahc2CalibrateProcessor")
	{
		_description = "Does calibration of AHCal hits";

		registerProcessorParameter("InputCollectionName",
		"Name of the input collection",
		_inputColName,
		std::string("CALDAQ_ADCCol"));

		registerProcessorParameter("OutputAhcHitCollectionName",
		"Name of the output AHCal hit collection, of type CalorimeterHit",
		_ahcHitOutputColName,
		std::string("Ahc2Calorimeter_Hits"));

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

		registerProcessorParameter( "Ahc2CalibrationsProcessorName" ,
		"Name of the Ahc2CalibrationsProcessor that provides"
		" the calibrations of the AHCal tiles." ,
		_calibProcessorName,
		std::string("MyAhc2CalibrationsProcessor") ) ;

		registerProcessorParameter( "HardwareConnectionCollection" ,
		"Name of the Ahc2HardwareConnection Collection",
		_Ahc2HardwareConnectionName,
		std::string("Ahc2HardwareConnection") ) ;

		registerProcessorParameter("NewDataFormat",
		"apply (1) or ignore (0) if it is the new data format in the EUDAQ",
		_newdataformat,
		(bool) false);

		registerProcessorParameter("PedestalSubtraction",
		"apply (1) or ignore (0) the pedestal subtraction",
		_pedestalSubtraction,
		(bool) true);

		registerProcessorParameter("ZeroSuppression",
		"apply (1) or ignore (0) the MIP threshold cut",
		_zeroSuppression,
		(bool) true);

		registerProcessorParameter("PhysicsMode",
		"Change the energy calibration depending on the running mode (Physics or Calib) - specific for the new ITEP boards",
		_isPhysicsMode,
		(bool) false);

		registerProcessorParameter("MipToGeVFactor", "AHCal conversion factor from MIP to GeV",
		_mipToGeVFactor,
		(float) 0.0255);

		registerProcessorParameter("MipCut", "Minimal energy deposition in "
		"units of MIP to keep hit, applied only if ZeroSuppression==1.",
		_mipCut,
		(float) 0.5);

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

		registerOptionalParameter( "doMipConversion",
		"Convert RAW ADC to MIP cell by cell",
		_doMipConversion,
		(bool) false);

		registerOptionalParameter( "doTimeConversion",
		"Convert RAW TDC to ns cell by cell",
		_doTimeConversion,
		(bool) false);

	}


	/***************************************************************************************/
	/*                                                                                     */
	/*                                                                                     */
	/*                                                                                     */
	/***************************************************************************************/
	void Ahc2CalibrateProcessor::init()
	{
		printParameters();

		std::stringstream message;
		bool error = false;

		_isFirstEvent = true;
		_isDATA = false;
		_isMC = false;

		_HitContainer = NULL;

		_HardwareConnnectionContainer.clear();

		_mapper = dynamic_cast<const Mapper*>(MappingProcessor::getMapper(_mappingProcessorName));
		if ( !_mapper )
		{
			message << "MappingProcessor::getMapper("<< _mappingProcessorName
			<< ") did not return a valid mapper." << endl;
			error = true;
		}

		_calibContainer = Ahc2CalibrationsProcessor::getCalibrations(_calibProcessorName);
		if ( ! _calibContainer )
		{
			message << "init(): Ahc2CalibrationsProcessor::getCalibrations("<< _calibProcessorName
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
	}

	void Ahc2CalibrateProcessor::FillContainer(LCEvent *evt)
	{

		LCCollection *_Ahc2HardwareConnectionCol = evt->getCollection( _Ahc2HardwareConnectionName );

		std::cout << "Fill Hardware Connection Container" << std::endl;

		if (!_Ahc2HardwareConnectionCol)
		{
			streamlog_out(ERROR) << "Cannot fill container, collection is not valid." << std::endl;
			throw StopProcessingException(this);
		}

		for (int i = 0; i < _Ahc2HardwareConnectionCol->getNumberOfElements(); ++i)
		{
			Ahc2HardwareConnection *hardwareConnection = new Ahc2HardwareConnection(_Ahc2HardwareConnectionCol->getElementAt(i)); // this will make a copy of the data (not link against the data like SimpleValue)

			int ChipID = hardwareConnection->getChip();
			int ModuleNumber = hardwareConnection->getModuleNumber();
			int ChipNumber = hardwareConnection->getChipNumber();

			_HardwareConnnectionContainer.insert(make_pair(ChipID, make_pair(ModuleNumber, ChipNumber)));
		}

		std::cout << "Container filled" << std::endl;
		std::cout << "Container contains " << _HardwareConnnectionContainer.size() << " elements" << std::endl;
	}

	/***************************************************************************************/
	/*                                                                                     */
	/*                                                                                     */
	/*                                                                                     */
	/***************************************************************************************/
	void Ahc2CalibrateProcessor::processEvent(LCEvent *evt)
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
		//if (noElem <= 0) return;

		streamlog_out(DEBUG0)<<" \n\n=========================Start to process event "<<evt->getEventNumber()<<endl;
		streamlog_out(DEBUG0)<<"inputcol "<<_inputColName<<" has "<<noElem<<" hits"<<endl;

		LCCollection *ahcHitOutputCol  = new LCCollectionVec(LCIO::CALORIMETERHIT);

		/*we want to save the position, and this can only be done if the CHBIT_LONG bit is set*/
		ahcHitOutputCol->setFlag(ahcHitOutputCol->getFlag() | 1 << LCIO::CHBIT_LONG );
		ahcHitOutputCol->setFlag(ahcHitOutputCol->getFlag() | 1 << LCIO::RCHBIT_TIME );

		/*get the collection type and use it to distinguish between data and Monte Carlo;
		if collectionType = LCGENERICOBJECT => AdcBlock => data
		else if collectionType = CALORIMETERHIT => Monte Carlo*/

		const std::string collectionType = inputCol->getTypeName();
		std::string outputEncodingString = "M:3,S-1:3,I:9,J:9,K-1:6";

		DecoderSet *decoder = _calibContainer->getDecoder();
		decoder->setCellIDEncoding(inputCol->getParameters().getStringVal(LCIO::CellIDEncoding));

		//**** Container Mapped for hits to check if double hits exist in a single cell!! ************/
		delete _HitContainer;
		_HitContainer = new MappedContainer<CalorimeterHitImpl>(_mapper, false);


		if( _isFirstEvent == true && inputCol != 0)
		{
			this->FillContainer(evt);//Fill map providing HardwareConnectionInformations

			if(collectionType == "LCGenericObject"){_isDATA = true;}
			else if(collectionType == "CalorimeterHit"){_isMC = true;}
			_isFirstEvent = false;
		}

		if (_isDATA)/*if data*/
		{
			if(!_newdataformat)
			{
				for (unsigned int i = 0; i < (unsigned int)noElem; ++i)
				{
					const LabviewBlock2 ldata(inputCol->getElementAt(i));

					//Keep type of BXID in event parameters (even / odd)
					int BXID = (int)(ldata.GetBunchXID());
					int EvtNr = (int)(ldata.GetEvtNr());

					//Choose hit bit = 1
					if (!ldata.GetHitBit()) continue;

					//####Reject the first event because the trigger does weird thing here
					if (ldata.GetEvtNr()==0) continue;

					//#########
					streamlog_out(DEBUG0)<< "ChipID: " << ldata.GetChipID()<< "  Channel: "<< ldata.GetChannel() << endl;

					int Module = -1;
					int HBU_CHIPID = ldata.GetChipID();
					int ChipID = -1;

					if( HBU_CHIPID < 100 || HBU_CHIPID > 255 )
					continue;

					map<int, pair<int, int> >::iterator found = _HardwareConnnectionContainer.find(HBU_CHIPID);
					if(found != _HardwareConnnectionContainer.end())
					{
						Module = found->second.first;
						ChipID = found->second.second;
					}
					else {
						streamlog_out(ERROR)<< "ERROR: Chip is not in the container... Did you create the HardwareConnection Collection? "<<endl;
						return;
					}
					//########################################################

					if (Module<0) continue;
					if (ChipID<0) continue;
					const int ModuleID = decoder->getModuleID(Module, ChipID, ldata.GetChannel());

					streamlog_out(DEBUG0)<<"ChipID: "<< ChipID <<"  Channel: "<< ldata.GetChannel() <<endl;
					streamlog_out(DEBUG0)<<"ModuleID: "<< ModuleID << endl;
					streamlog_out(DEBUG0)<<"BXID: "<< BXID%2 <<" Evt: "<< EvtNr <<endl;

					Ahc2Calibrations *calibration = _calibContainer->getByModuleID(ModuleID);

					if (calibration == NULL)
					{
						streamlog_out(DEBUG0)<<"Ahc2Calibrations empty for ChipID: "<< ChipID <<" and Channel: "<< ldata.GetChannel() <<endl;
						continue;
					}

					outputEncodingString = calibration->getCellIDEncoding();
					_HitContainer->getDecoder()->setCellIDEncoding(outputEncodingString);

					//Get ADC
					float rawADC = (float)ldata.GetADC();
					float rawEnergy = rawADC;
					float rawTDC = (float)ldata.GetTDC();
					int GainBit = ldata.GetGainBit();

					/*do the energy calibration*/
					streamlog_out(DEBUG0)<<"------------------------------------"<<endl;
					streamlog_out(DEBUG0)<<"\n before calibrateEnergyAndFillOutputCollections"<<endl;

					this->calibrateEnergyAndFillOutputCollections(calibration, GainBit, rawEnergy, rawTDC, BXID%2, EvtNr, ahcHitOutputCol);

				}/*--------------end loop over input collection elements ----------------------*/
			}/*------------------- end if not new format --------------------------------*/
			else
			{
				for (unsigned int i = 0; i < (unsigned int)noElem; ++i)
				{
					const EUDAQBlock2016 lBlock(inputCol->getElementAt(i));

					std::vector<int> TDC = lBlock.GetTDC();
					std::vector<int> ADC = lBlock.GetADC();
					int BXID = lBlock.GetBunchXID();
					int CycleNr = lBlock.GetCycleNr();
					int Chip = lBlock.GetChipID();//ChipID
					int EvtNr = (int)(lBlock.GetEvtNr());

					//####Reject the first event because the trigger does weird thing here
					if (lBlock.GetEvtNr() == 0) continue;

					for(int ichan = 0; ichan < lBlock.GetNChannels();  ichan++)
					{
						//Get HitBit
						int HitBit = (ADC[ichan]& 0x1000)?1:0;

						//Get GainBit
						int GainBit = (ADC[ichan]& 0x2000)?1:0;

						//Choose hit bit = 1
						if (!HitBit) continue;

						//#########
						streamlog_out(DEBUG0)<< "ChipID: " << Chip << "  Channel: "<< ichan << endl;

						int Module = -1;
						int HBU_CHIPID = Chip;
						int ChipID = -1;

						if( HBU_CHIPID < 100 || HBU_CHIPID > 255 )
						continue;

						map<int, pair<int, int> >::iterator found = _HardwareConnnectionContainer.find(HBU_CHIPID);
						if(found != _HardwareConnnectionContainer.end())
						{
							Module = found->second.first;
							ChipID = found->second.second;//ChipNumber
						}
						else {
							streamlog_out(ERROR)<< "ERROR: Chip is not in the container... Did you create the HardwareConnection Collection? "<<endl;
							return;
						}
						//########################################################

						if (Module<0) continue;
						if (ChipID<0) continue;
						const int ModuleID = decoder->getModuleID(Module, ChipID, ichan);

						streamlog_out(DEBUG0)<<"ChipNumber: "<< ChipID <<"  Channel: "<< ichan <<endl;
						streamlog_out(DEBUG0)<<"ModuleID: "<< ModuleID << endl;
						streamlog_out(DEBUG0)<<"BXID: "<< BXID%2 <<" Evt: "<< EvtNr <<endl;

						Ahc2Calibrations *calibration = _calibContainer->getByModuleID(ModuleID);

						if (calibration == NULL)
						{
							streamlog_out(DEBUG0)<<"Ahc2Calibrations empty for ChipID: "<< ChipID <<" and Channel: "<< ichan <<endl;
							continue;
						}

						outputEncodingString = calibration->getCellIDEncoding();
						_HitContainer->getDecoder()->setCellIDEncoding(outputEncodingString);

						//Get ADC
						float rawADC = (float)(ADC[ichan]%4096);
						float rawEnergy = rawADC;
						float rawTDC = (float)(TDC[ichan]%4096);

						/*do the energy calibration*/
						streamlog_out(DEBUG0)<<"------------------------------------"<<endl;
						streamlog_out(DEBUG0)<<"\n before calibrateEnergyAndFillOutputCollections"<<endl;

						this->calibrateEnergyAndFillOutputCollections(calibration, GainBit, rawEnergy, rawTDC, BXID%2, EvtNr, ahcHitOutputCol);
					}
				}//end loop channels
			}
		}/*------------------- end if data --------------------------------*/
		else if(_isMC)
		{
			for (unsigned int i = 0; i < (unsigned int)noElem; ++i)
			{
				CalorimeterHit *digiHit = dynamic_cast<CalorimeterHit*>(inputCol->getElementAt(i));
				int cellID = digiHit->getCellID0();

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


				Ahc2Calibrations *calibration = _calibContainer->getByCellID(cellID);
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
				_HitContainer->getDecoder()->setCellIDEncoding(outputEncodingString);

				const float rawEnergy = digiHit->getEnergy();
				const float rawTime = digiHit->getTime();
				const int GainBit = 3;//MC GainBit flag

				/*do the energy calibration*/
				streamlog_out(DEBUG0)<<"\n before calibrateEnergyAndFillOutputCollections"<<endl;

				//Default for MC
				int BXID = 0;
				int EvtNr = 0;

				this->calibrateEnergyAndFillOutputCollections(calibration, GainBit, rawEnergy, rawTime, BXID, EvtNr, ahcHitOutputCol);
			}
		}
		else/*--------- not supported (neither DATA nor MC)----------------*/
		{
			streamlog_out(ERROR)<<" Nothing to be done for collections of type "<<collectionType<<endl;
			return;
		}

		if (ahcHitOutputCol->getNumberOfElements() >= 0)
		{
			LCParameters &param = ahcHitOutputCol->parameters();
			param.setValue(LCIO::CellIDEncoding, outputEncodingString);

			evt->addCollection(ahcHitOutputCol, _ahcHitOutputColName.c_str());
			streamlog_out(DEBUG0)<<" Collection "<<_ahcHitOutputColName<<" added to the event"<<endl;

		} else {delete ahcHitOutputCol;}

	}

	/***************************************************************************************/
	/*                                                                                     */
	/*                                                                                     */
	/*                                                                                     */
	/***************************************************************************************/
	void Ahc2CalibrateProcessor::calibrateEnergyAndFillOutputCollections(Ahc2Calibrations* calibration, const int GainBit, const float rawEnergy, const float rawTDC, const int BXID, const int Mem, LCCollection* ahcHitOutputCol)
	{
		const Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( calibration->getStatus() );
		streamlog_out(DEBUG0) << "  STATUS bits: " << bits << endl;

		if ( _filterDeadCells && bits.isDead() )
		{
			streamlog_out(WARNING) << " SKIP -- channel is marked as dead: " << endl;
			return;
		}

		if ( _filterDefaultCells && bits.hasDefault() )
		{
			streamlog_out(WARNING) << " SKIP -- channel has default calibration: " << endl;
			return;
		}

		/*-------------------- calibration coefficients --------------*/
		LinearFitCompound *lfcMIP            = calibration->getMIP();
		LinearFitCompound *lfcGain           = calibration->getGain();
		SimpleValue *interCalibContainerHGLG = calibration->getInterCalibration();	//Get HG/LG Intercalibration Value
		SaturationParameters *saturationParameters = calibration->getSaturation();
		SimpleValue *ICValuePhys = calibration->getPhysicsCalibIC();//PhysicsCalib intercalibration value

		/*Saturation correction function */
		Ahc2SatCorr* satFunc = new Ahc2SatCorr(saturationParameters);

		/*Time calibration*/
		SimpleValueVector *timeSlopesParameters  = calibration->getTimeSlopes();
		SimpleValueVector *timePedestalParameters  = calibration->getTimePedestal();

		/*------------------------------------------------
		get pedestal
		-----------------------------------------------*/
		float pedestal      = 0;
		float pedestalError = 0;

		SimpleValue *pedestalContainer = calibration->getPedestal();

		if (_pedestalSubtraction)
		{

			if(bits.noPedestal())
			{
				streamlog_out(WARNING) << " SKIP -- channel has no pedestal: " << endl;
				return;
			}
			else
			pedestal = pedestalContainer->getValue();

			/*
			What about the significance cut?
			need pedestal width !
			*/
		}


		/*----------------------------------------------
		get temperature
		-----------------------------------------------*/
		float temperature = 0;
		int cellID = calibration->getCellID();

		/*  TODO
		SimpleValue *temperatureObject = calibration->getTemperature();
		float temperature = 0;
		if (temperatureObject != NULL) temperature = temperatureObject->getValue();
		else
		{
		streamlog_out(ERROR)<<" No temperature found"<<endl;
		exit(1);
	}
	*/

	_mapper->getDecoder()->setCellIDEncoding(calibration->getCellIDEncoding());

	streamlog_out(DEBUG0) << "  cellID: "<<cellID<<endl;
	streamlog_out(DEBUG0) << "  from module " << _mapper->getModuleFromCellID(cellID) << endl;
	streamlog_out(DEBUG0) << "  with amplitude: "<< rawEnergy << endl;
	streamlog_out(DEBUG0) << "  in running mode: "<< ( _isPhysicsMode ? "Physics Mode" : "Calib Mode" ) << endl;

	//		  << "  @ temperature: " << temperature << " deg. C" << endl;

	/*------------------------------------------------
	get MIP, gain and IC
	-----------------------------------------------*/
	float mipValue = 0;
	if (_doMipTempCorr)
	{
		mipValue = lfcMIP->eval(temperature);
	}
	else                mipValue = lfcMIP->getConstant();
	if (mipValue <= 0)
	{

		streamlog_out(ERROR0) << "  ERROR - ignoring cell with negative/"
		<< "zero MIP coefficient at I/J/K "
		<< _mapper->getDecoder()->getIFromCellID(cellID)
		<<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
		<<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
		<< endl;

		return;
	}

	float gainValue = 0;
	if (_doGainTempCorr) gainValue = lfcGain->eval(temperature);
	else                 gainValue = lfcGain->getConstant();
	if (gainValue <= 0)
	{

		streamlog_out(ERROR0) << "  ERROR - ignoring cell with negative/"
		<< "zero gain coefficient at I/J/K "
		<< _mapper->getDecoder()->getIFromCellID(cellID)
		<<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
		<<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
		<< endl;

		return;
	}



	float interCalibrationHGLG = interCalibContainerHGLG->getValue();

	if (interCalibrationHGLG <= 0)
	{
		streamlog_out(ERROR0) << "  ERROR - ignoring cell with negative/"
		<< "zero inter calibration HG/LG at I/J/K "
		<< _mapper->getDecoder()->getIFromCellID(cellID)
		<<"/"<<_mapper->getDecoder()->getJFromCellID(cellID)
		<<"/"<<_mapper->getDecoder()->getKFromCellID(cellID)
		<< endl;
		return;
	}

	float interCalibrationValuePhys = ICValuePhys->getValue();

	if (interCalibrationValuePhys <= 0)
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
	streamlog_out(DEBUG0) << "  - intercalibration Physics/Calib: " << interCalibrationValuePhys << endl;
	streamlog_out(DEBUG0) << "  - intercalibration HG/LG: " << interCalibrationHGLG << endl;

	/*----------------------------------------------
	do calibration
	---------------------------------------------*/
	float calibratedEnergy      = 0;
	float calibratedADC = 0.;

	/*----------------------------------------------
	HG/LG calibration
	---------------------------------------------*/

	if(GainBit == 0)
	calibratedADC = (rawEnergy - pedestal) * interCalibrationHGLG;
	else
	calibratedADC = (rawEnergy - pedestal);

	//Implementation same as digitization in Ahc2SiPMStatistics.cc using generic saturation function class Ahc2SatCorr satFunc
	//- _effNpix * TMath::Log(1 - satHitPx / _effNpix); // unsaturate pixel

	if(_doMipConversion)
	{
		if ( _doSaturationCorr)
		{
			if(_isPhysicsMode)
			{
				float saturatedAmpl = calibratedADC / (gainValue / interCalibrationValuePhys);

				//desaturate
				float unsaturatedAmpl = satFunc->deSaturate( saturatedAmpl );

				streamlog_out(DEBUG0)<< "  - amplitude [pix]: " << saturatedAmpl << endl;
				streamlog_out(DEBUG0)<< "  - corrected [pix]: " << unsaturatedAmpl << endl;

				streamlog_out(DEBUG0) << "  - saturationCorrectionFactor: "
				<< unsaturatedAmpl / saturatedAmpl
				<< endl;

				calibratedEnergy = unsaturatedAmpl * gainValue / mipValue;
			}
			else
			{
				float saturatedAmpl = calibratedADC / gainValue;

				//desaturate
				float unsaturatedAmpl = satFunc->deSaturate( saturatedAmpl );

				streamlog_out(DEBUG0)<< "  - amplitude [pix]: " << saturatedAmpl << endl;
				streamlog_out(DEBUG0)<< "  - corrected [pix]: " << unsaturatedAmpl << endl;

				streamlog_out(DEBUG0) << "  - saturationCorrectionFactor: "
				<< unsaturatedAmpl / saturatedAmpl
				<< endl;

				calibratedEnergy = unsaturatedAmpl * gainValue / mipValue;
			}
		}
		else
		{
			if(_isPhysicsMode)
			{
				calibratedEnergy      = calibratedADC / (mipValue / interCalibrationValuePhys);
			}
			else
			{
				calibratedEnergy      = calibratedADC / mipValue;
			}
		}
	}
	else
	{
		calibratedEnergy      = calibratedADC;
	}

	delete satFunc;

	streamlog_out(MESSAGE) <<"  - calibratedEnergy[MIPs]: " << calibratedEnergy
	<< " rawEnergy " << rawEnergy
	<< " pedestal " << pedestal << " mipValue " << mipValue <<endl;

	/*--------------------------------------
	* if requested, do error calculation
	---------------------------------------*/
	float calibratedEnergyError = 0;
	if ( _doErrorCalculation )
	{
		// if (_pedestalSubtraction == 1) pedestalError = pedestalContainer->getError();
		//
		// //const float interCalibrationError = interCalibContainer->getError();
		// const float interCalibrationError = 0;
		// const float temperatureError = 0; //temperatureObject->getError(); //SLU: TODO
		//
		// float mipError  = 0;
		// float gainError = 0;
		// if (_doMipTempCorr) mipError = lfcMIP->evalErr(temperature,temperatureError);
		// else                mipError = lfcMIP->getConstantError();
		//
		// if (_doGainTempCorr) gainError = lfcGain->evalErr(temperature,temperatureError);
		// else                 gainError = lfcGain->getConstantError();
		//
		// streamlog_out(DEBUG0) << "  - MIP error: " << mipError << endl;
		// streamlog_out(DEBUG0) << "  - gain error: " << gainError << endl;
		// streamlog_out(DEBUG0) << "  - intercalibration error: " << interCalibrationError << endl;
		//
		// const float relativeErrorPedSubSquare =
		// ( pow(calibratedEnergyError, 2) + pow(pedestalError, 2) ) / pow( rawEnergy - pedestal, 2);

		/*----------------------------------------
		if requested, do saturation correction
		---------------------------------------*/
		// if ( _doSaturationCorr )
		// {
		// 	const float saturatedSignalInPixel = (rawEnergy - pedestal) * interCalibrationValue / gainValue ;
		// 	const float unsaturatedSignalInPixel = saturationContainer->deSaturate( saturatedSignalInPixel );
		//
		// 	// assuming that the intercalibration error cancels in first order
		// 	const float saturatedSignalInPixelError =
		// 	sqrt( relativeErrorPedSubSquare + pow(gainError/gainValue,2) ) * saturatedSignalInPixel;
		//
		// 	calibratedEnergyError =
		// 	sqrt( pow(saturationContainer->deSaturatedError(saturatedSignalInPixel,
		// 		saturatedSignalInPixelError)/unsaturatedSignalInPixel,2)
		// 		+ pow(mipError/mipValue,2) ) * calibratedEnergy;
		// 	}
		// 	else
		// 	{
		// 		calibratedEnergyError =
		// 		sqrt( relativeErrorPedSubSquare + pow(mipError/mipValue,2) ) * calibratedEnergy;
		// 	}
		//
		// 	calibratedEnergyError = sqrt( relativeErrorPedSubSquare ) * calibratedEnergy;
	}

	/*-----------------------------------------------
	0.5 MIP cut
	----------------------------------------------*/

	if ( _zeroSuppression && calibratedEnergy < _mipCut )
	{
		streamlog_out(WARNING) << " STOP - " << calibratedEnergy
		<< " - below threshold of: " << _mipCut << endl;
		streamlog_out(WARNING)<<"  ------------------------------"<<endl;
		return;
	}

	streamlog_out(DEBUG0)<< " DONE - calibrated amplitude: " << calibratedEnergy;
	if (_doErrorCalculation)  streamlog_out( DEBUG0 ) << " +- " << calibratedEnergyError;
	streamlog_out(DEBUG0)<< endl;

	/*-----------------------------------------------
	Time Calibration
	----------------------------------------------*/

	float time_slope = 1.;// in ns/TDC
	float time_pedestal = 0.;//in TDC
	float calibratedTime = 0.;

	if(_doTimeConversion)
	{
		time_slope = timeSlopesParameters->getValue(BXID);
		time_pedestal = timePedestalParameters->getValue(Mem);
		int status_slope = timeSlopesParameters->getStatus(BXID);
		int status_ped = timeSlopesParameters->getStatus(Mem);

		if(status_slope == 0 || status_ped == 0)
		{
			streamlog_out(WARNING) << " SKIP -- channel has no time slope/pedestal: " << endl;
			calibratedTime = rawTDC;
		}

		calibratedTime = (rawTDC - time_pedestal) * time_slope;//simple conversion from TDC to ns (no time Walk correction)
	}
	else
	{
		calibratedTime = rawTDC;
	}

	/*-----------------------------------------------
	Time Error TODO
	----------------------------------------------*/

	/*---------------------------------------------
	write output calorimeter hit
	--------------------------------------------*/
	CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
	if (_scaleEnergy)
	{
		newHit->setEnergy(calibratedEnergy*_energyScaleFactor);
		newHit->setEnergyError(calibratedEnergyError*_energyScaleFactor );
	}
	else
	{
		newHit->setEnergy(calibratedEnergy);
		newHit->setEnergyError(calibratedEnergyError);
	}

	newHit->setTime(calibratedTime);
	newHit->setCellID0(cellID);
	newHit->setType(GainBit*100+Mem);//Set flag HG/LG Hit - 0 = LG, 1 = HG, 3 = MC

	_cellDescriptions->getDecoder()->setCellIDEncoding(calibration->getCellIDEncoding());
	CellDescription* cellDescription = _cellDescriptions->getByCellID(cellID);
	float newPosition[3] = {cellDescription->getX(), cellDescription->getY(), cellDescription->getZ()};

	streamlog_out(DEBUG0)<<" position: ("<<newPosition[0]<<", "<<newPosition[1]<<", "<<newPosition[2] <<")"<<std::endl;

	newHit->setPosition(newPosition);

	/* Check for double hits! same cellID0 */
	if(checkHit(newHit))
	{
		std::cout <<"!!! Double Hit for cellID " << cellID << " I/J/K " << _mapper->getDecoder()->getIFromCellID(cellID) << "/" << _mapper->getDecoder()->getJFromCellID(cellID) << "/" << _mapper->getDecoder()->getKFromCellID(cellID) << std::endl;
		return;
	}

	/*add collection of AHCal calibrated hits to the event*/
	ahcHitOutputCol->addElement(newHit);

	streamlog_out(DEBUG0)<<" \n output collection "<<_ahcHitOutputColName<<" has "
	<<ahcHitOutputCol->getNumberOfElements()<<" elements"<<endl;
	streamlog_out(DEBUG0)<<"=====================\n\n" << endl;

}

bool Ahc2CalibrateProcessor::checkHit(CalorimeterHitImpl *hit)
{
	bool check = false;

	int cellID = hit->getCellID0();

	if(_HitContainer->getByCellID(cellID) == 0)
	_HitContainer->fillByCellID(cellID, hit);
	else
	check = true;

	return check;
}

}/*end of namespace CALICE*/
