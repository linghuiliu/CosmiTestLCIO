#include "AhcDigitisationProcessor.hh"

#include <vector>

#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "marlin/Exceptions.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/LCRelationNavigator.h"
#include "IMPL/LCRelationImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCFlagImpl.h"

#include "MappingProcessor.hh"
#include "CellIterator.hh"
#include "CellNeighboursProcessor.hh"
#include "CellDescriptionProcessor.hh"

#include "SiPMCalibrationsProcessor.hh"
#include "SiPMTemperatureProcessor.hh"
#include "SiPMCalibrationStatusBits.hh"

using namespace marlin;
using namespace lcio;
using namespace std;

namespace CALICE
{  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  AhcDigitisationProcessor::AhcDigitisationProcessor(const std::string processorName) : marlin::Processor(processorName)
  {
    _description = "AHCAL digitisation";
    
    registerProcessorParameter("InputCollectionName",
			       "Name of the input collection",
			       _inputColName,
			       std::string("AhcCalorimeter_Hits_ganged"));
    
    registerProcessorParameter("NoiseCollectionName",
			       "Name of the input noise collection",
			       _noiseColName,
			       std::string("AhcNoise1"));

    registerProcessorParameter("OutputCollectionName",
			       "Name of the output collection",
			       _outputColName,
			       std::string("AhcCalorimeter_Hits"));

    registerProcessorParameter( "MappingProcessorName" ,
                                "Name of the MappingProcessor instance that provides the geometry "
				"of the detector." ,
                                _mappingProcessorName,
                                std::string("MyMappingProcessor") ) ;
    
    registerProcessorParameter( "CellNeighboursProcessorName" ,
                                "Name of the CellNeighboursProcessor instance that provides the neighbours",
				_cellNeighboursProcessorName,
                                std::string("MyNeighboursProcessor") ) ;

    registerProcessorParameter( "CellDescriptionProcessorName" ,
				"name of CellDescriptionProcessor which takes care"
				" of the cell description generation",
				_cellDescriptionProcessorName,
				std::string("MyCellDescriptionProcessor") ) ;

    registerProcessorParameter( "SiPMCalibrationsProcessorName" ,
                                "Name of the SiPMCalibrationsProcessor that provides"
                                " the calibrations of the AHCAL tiles." ,
                                _calibProcessorName,
                                std::string("MySiPMCalibrationsProcessor") ) ;

    registerProcessorParameter( "SiPMTemperatureProcessorName" ,
                                "Name of the SiPMTemperatureProcessor that provides"
                                " the AHCAL temperature container." ,
                                _temperatureProcessorName,
                                std::string("MySiPMTemperatureProcessor") ) ;


    registerProcessorParameter( "MipPerGeVFactor" , "MIP/GeV factor",
				_mipPerGeVFactor, float(0.000846) ) ;

    registerProcessorParameter( "LightLeakage" ,
				"Light leakage (i.e. factor for the tiles cross-talk)",
				_lightLeakage,
				float(0.15) ) ;

    registerProcessorParameter("DoBinomialSmearing", 
			       "Toggle the smearing of the number of firing "
			       "pixel with a binomial statistic",
			       _doBinomialSmearing,
			       bool(true));

    registerProcessorParameter("DoSaturation", 
			       "Apply saturation to the linear Pixel",
			       _doSaturation,
			       bool(true));

    registerProcessorParameter("DoOpticalCrossTalk", 
			       "Apply optical cross talk",
			       _doOpticalCrossTalk,
			       bool(true));

    registerProcessorParameter("DoAddNoise", 
			       "Add detector noise to pure MC",
			       _doAddNoise,
			       bool(true));

    registerProcessorParameter("RandomSeed", "Seed for random number generator",
			       _randomSeed, int(0));
    
    registerProcessorParameter("DoWriteCollectionForDebug", 
			       "Write collections after each step in digitisation",
			       _doWriteCollectionForDebug,
			       bool(false));
    
    registerProcessorParameter("doMipTemperatureCorrection",
			       "Do MIP temperature correction",
			       _doMipTempCorr,
			       (bool) true);

   registerProcessorParameter("doGainTemperatureCorrection",
			       "Do GAIN temperature correction",
			       _doGainTempCorr,
			       (bool) true);

    registerProcessorParameter("correctDefaultGainTofixedLightYield",
                               "Correct light yield for cells with default gain value in calibration.",
                               _correctDefaultGainToLY,
                               (bool) false);
                               
    registerProcessorParameter( "fixedLightYieldForCorrection" ,
                               "Fixed light yield for cells with default gain value in calibration.",
                               _fixedLY,
                               (float)15. ) ;
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  AhcDigitisationProcessor::~AhcDigitisationProcessor()
  {
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcDigitisationProcessor::init()
  {
    /*print the processor parameters and their values*/
    marlin::Processor::printParameters();

    /*initialize the random generator*/
    _randomGenerator = new TRandom3(_randomSeed);

    _gangedContainer = NULL;

    std::stringstream message;
    bool error = false;
    
    _mapper = MappingProcessor::getMapper(_mappingProcessorName);

    if ( ! _mapper )
      {
        message << "MappingProcessor::getMapper("<< _mappingProcessorName 
		<< ") did not return a valid mapper." << std::endl;
        error = true;
      }

    _cellNeighbours = CALICE::CellNeighboursProcessor::getNeighbours(_cellNeighboursProcessorName);
    if ( ! _cellNeighbours ) 
      {
	streamlog_out(ERROR) << "Cannot obtain cell neighbours from CellNeighboursProcessor "
		     <<_cellNeighboursProcessorName<<". Maybe, processor is not present" 
		     << std::endl;
	error = true;
      }

    _cellDescriptions = CALICE::CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if ( ! _cellDescriptions ) 
      {
	streamlog_out(ERROR) << "Cannot obtain cell descriptions from CellDescriptionsProcessor "
		     <<_cellDescriptionProcessorName<<". Maybe, processor is not present" 
		    << std::endl;
	error = true;
      }
    
    _calibContainer = SiPMCalibrationsProcessor::getCalibrations(_calibProcessorName);
    if ( ! _calibContainer )
      {
        message << "init(): SiPMCalibrationsProcessor::getCalibrations("<< _calibProcessorName
                << ") did not return a valid MappedContainer." <<std::endl;
        error = true;
      }

 
    if (error) 
      {
	streamlog_out(ERROR) << message.str();
	throw marlin::StopProcessingException(this);
      }
    
    _mapperVersion = _mapper->getVersion();
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcDigitisationProcessor::processEvent(LCEvent* evt)
  {
    streamlog_out(DEBUG0)<<" \n\n ===================== START event "<<evt->getEventNumber()<<endl;

    LCCollection *inputCol = NULL;
    try{
      inputCol = evt->getCollection( _inputColName );
    }
    catch ( EVENT::DataNotAvailableException &e )
      {
	streamlog_out(DEBUG0)<<" Collection "<<_inputColName<<" not available, skip this event"<<endl;
	//throw marlin::SkipEventException(this);
	return; // allow lcio event go to next processors. 
      }
    
    LCCollection *noiseCol = NULL;
    try{
      noiseCol = evt->getCollection( _noiseColName );
    }
    catch ( EVENT::DataNotAvailableException &e )
      {
	streamlog_out(DEBUG0)<<" Collection "<<_noiseColName<<" not available, skip this event"<<endl;
	throw marlin::SkipEventException(this);
      }
  
    int nHits = inputCol->getNumberOfElements();
    streamlog_out(DEBUG0)<<" number of ganged AHC hits: "<<nHits<<endl;

    /*the Mokka encoding string*/
    _mokkaEncodingString = inputCol->getParameters().getStringVal(LCIO::CellIDEncoding);
    
    /*clean up the container at the beginning of the event*/
    delete _gangedContainer;

    /*----------------------------------------------------------------------------------
      First step: fill the container with ganged hits, and divide the energy of the hits 
     by the MIP to GeV factor ==> E[MIPs]*/
    this->fillGangedContainer(inputCol); 
    if (_doWriteCollectionForDebug) writeCollectionForDebug(evt, "hcalAfterGanging");

    /*---------------------------------------------------------------------------------
     Next step: OPTICAL cross talk, still E[MIPs]
    */
    if(_doOpticalCrossTalk) 
      {
	this->simulateOpticalCrossTalk(); 
	if (_doWriteCollectionForDebug) writeCollectionForDebug(evt, "hcalAfterCrossTalk");
      }
    
    /*---------------------------------------------------------------------------------
     Next step: Simulate SiPM behaviour, 
     i.e. convert the energy from MIPs to ADC counts
    */
    this->simulateSiPMBehaviour(); 
    if (_doWriteCollectionForDebug) writeCollectionForDebug(evt, "hcalAfterSiPMBehaviour");
    
    /*---------------------------------------------------------------------------------
     Next step: Add the noise from data, E[ADC]
    */
    if(_doAddNoise) this->addNoise(noiseCol); 

    /*---------------------------------------------------------------------------------
     Next step: Finally, create the output collection, E[ADC]
    */
   this->createOutputCollection(evt);
  }
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcDigitisationProcessor::fillGangedContainer(LCCollection *inputCol)
  {
    streamlog_out(DEBUG0)<<"\n\n Start to fill ganged container "<<std::endl;
 
    /*create a new map container. Note the 'false' flag, which tells the container
      that he is not responsible for the deletion of the contained elements.
      We use this because at the end of the event we have all the elements from
      the container into an LCCollection, and Marlin deletes them at the end of the event.*/
    _gangedContainer = new MappedContainer<CalorimeterHitImpl>(_mapper, false);
    _gangedContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);
   
   /*Copy the input collection into the container, which we'll modify afterwards*/
     for (int iHit = 0; iHit < inputCol->getNumberOfElements(); ++iHit)
       {
	 CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inputCol->getElementAt(iHit));
	 int trueCellID = hit->getCellID0();
	 
	 CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
	 /*Convert the energy from GeV to MIPs*/
	 newHit->setEnergy(hit->getEnergy()/_mipPerGeVFactor);
	 newHit->setCellID0(trueCellID);
	 _gangedContainer->fillByCellID(trueCellID, newHit);	 
       }
  
     streamlog_out(DEBUG0)<<"\n After ganging: gangedContainer has "
		  <<_gangedContainer->getAllElements().size()
		  <<" elements"<<endl;
     
  }
  

  /*********************************************************************************/
  /*                                                                               */
  /*  Note: during cross-talk, 10% of the energy leaks to the neighbouring         */
  /*  tiles, but the leaked energy is NOT subtracted from the energy because       */
  /*  the measured MIP is actually only 90%MIPs. For more details, see page 101 in */
  /*   http://www-library.desy.de/preparch/desy/thesis/desy-thesis-10-006.pdf      */ 
  /*                                                                               */
  /*********************************************************************************/
  void AhcDigitisationProcessor::simulateOpticalCrossTalk()
  {
    streamlog_out(DEBUG0)<<"\n\n Start to simulate optical cross-talk";

    std::vector<CalorimeterHitImpl*> gangedVec = _gangedContainer->getAllElements();
    streamlog_out(DEBUG0)<<" for "<<gangedVec.size()<<" ganged hits"<<std::endl;

    /*counter for debug purposes*/
    int countNewCells = 0;
    int countGanged   = 0;

    // Generate a new vector to save the original hits information
    // Loop over the original hits, and add optical xtalk into the
    // neighbours cell.
    // It is only first order optical xtalk has been applied here.
    std::vector<CalorimeterHitImpl> originalgangedVec;
    for (std::vector<CalorimeterHitImpl *>::iterator iter = gangedVec.begin();
	 iter != gangedVec.end(); ++iter)
      {
	CalorimeterHitImpl *currentHit = (*iter);
	originalgangedVec.push_back((*currentHit));
      }

    /*------------------------------------------------------------------
      Start to loop over original ganged hits
     ------------------------------------------------------------------*/
    for (std::vector<CalorimeterHitImpl>::iterator iter = originalgangedVec.begin();
	 iter != originalgangedVec.end(); ++iter)
      {
	/*set the encoding string back to the Mokka encoding string*/
	_gangedContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);
 
	int cellID = (*iter).getCellID0();
	CalorimeterHitImpl *currentHit = &(*iter);

	_cellDescriptions->getDecoder()->setCellIDEncoding(_mokkaEncodingString);
	CellDescription* currentCellDescription = _cellDescriptions->getByCellID(cellID);
	const float currentTileSize = currentCellDescription->getSizeX();

	streamlog_out(DEBUG0)<<"------------"<<endl;
	streamlog_out(DEBUG0)<<"i="<<countGanged<<" ganged  I/J/K: "<< _gangedContainer->getDecoder()->getIFromCellID(cellID)
		     <<"/"<<_gangedContainer->getDecoder()->getJFromCellID(cellID)
		     <<"/"<<_gangedContainer->getDecoder()->getKFromCellID(cellID)
		     <<" currentHit: "<<currentHit    
		     <<" cellID: "<<cellID
		     <<" energy[MIPs]: "<<currentHit->getEnergy()
		     <<" currentTileSize: "<<currentTileSize
		     <<endl;
	countGanged ++;

	/*start loop over neighbours*/
	const std::vector<int> neighboursVec = 
	  _cellNeighbours->getByCellID(cellID)->getNeighbours(CALICE::CellNeighbours::direct, 
							      CALICE::CellNeighbours::module);
	streamlog_out(DEBUG0)<<"   ->number of neighbours found: "<<neighboursVec.size()<<endl;

	/*before getting the neighbours, set the encoding string to the string used to 
	  set the neighbours*/
	const std::string neighboursEncodingString = 
	  CALICE::CellNeighboursProcessor::getEncodingString(_cellNeighboursProcessorName);
	streamlog_out(DEBUG0)<<" neighboursEncodingString: "<<neighboursEncodingString<<endl;

	/*---------------------------------------------------------------
	  loop over neighbours
	-----------------------------------------------------------------*/
	for (unsigned int i = 0; i < neighboursVec.size(); ++i) 
	  {
	    streamlog_out(DEBUG0)<<"  \n i="<<i<<endl;
	    _cellDescriptions->getDecoder()->setCellIDEncoding(neighboursEncodingString);
	    streamlog_out(DEBUG0)<<"   neighboursEncodingString: "<<neighboursEncodingString<<endl;

	    streamlog_out(DEBUG0)<<"         neighbours: I/J/K="
			 <<_cellDescriptions->getDecoder()->getIFromCellID(neighboursVec[i])
			 <<"/"<<_cellDescriptions->getDecoder()->getJFromCellID(neighboursVec[i])
			 <<"/"<<_cellDescriptions->getDecoder()->getKFromCellID(neighboursVec[i])
			 <<" endcoding string: "<<_cellDescriptions->getDecoder()->getCellIDEncoding()
			 <<endl;

	    CellDescription *neighbourCellDescription = _cellDescriptions->getByCellID(neighboursVec[i]);
	    if (neighbourCellDescription == NULL) 
	      {
		streamlog_out(DEBUG0)<<"      sorry, no cell description found for neighbour "<<i<<endl;
		continue;
	      }

	    const float neighbourTileSize = neighbourCellDescription->getSizeX();
	    streamlog_out(DEBUG0)<<"      neighbourTileSize: "<<neighbourTileSize<<endl;
	    
	    /*Define fraction of leakage going into neighbouring cells;
	      for example, if the current cell has neighbours of the same size,
	      and if the total leakage is 10%, each neighbour will get
	      an energy fraction of (leakageFraction * totalLeakage) = 0.25 * 0.10 = 0.025*/
	    float leakageFraction = 1.0/4.0;

	    /*if the neighbours are larger or equal to the current tile,
	      they will get a quarter (1/4) of the leaked energy of the current tile;
	      else, the leakageFraction has to consider the smaller size of the neighbours,
	      since for the leakage is equally distributed at each side of the current tile*/
	    if (neighbourTileSize >= currentTileSize)     leakageFraction *= 1;
	    else if (neighbourTileSize < currentTileSize) leakageFraction *= neighbourTileSize/currentTileSize;
	    
	    /*now we can calculate the energy which leaks from the current tile to the neighbours;
	      the factor A=(1. - _lightLeakage)
	      plays the role of an effective MIP/GeV factor. We do this because, for the muon case,
	      if we sum up the energy, we need to get the 1 MIP.
	     */
	    float leakedEnergy = leakageFraction * _lightLeakage  * (currentHit->getEnergy())
	      /(1. - _lightLeakage);
	    
	    streamlog_out(DEBUG0)<<"      lightLeakage: "<<_lightLeakage<<" energy: "<<currentHit->getEnergy()<<std::endl;
	    streamlog_out(DEBUG0)<<"      leakageFraction: "<<leakageFraction<<" leakedEnergy: "<<leakedEnergy<<endl;
	    streamlog_out(DEBUG0)<<"      looking in list for I/J/K="
			 <<_cellDescriptions->getDecoder()->getIFromCellID(neighboursVec[i])
			 <<"/"<<_cellDescriptions->getDecoder()->getJFromCellID(neighboursVec[i])
			 <<"/"<<_cellDescriptions->getDecoder()->getKFromCellID(neighboursVec[i])
			 <<endl;

	    _gangedContainer->getDecoder()->setCellIDEncoding(neighboursEncodingString);
	    CalorimeterHitImpl *neighbourHit = _gangedContainer->getByCellID(neighboursVec[i]);

	    if (neighbourHit == NULL)
	      {
		neighbourHit = new CalorimeterHitImpl();
		neighbourHit->setEnergy(leakedEnergy);

		countNewCells++;
		streamlog_out(DEBUG0)<<"      new hit, leaked energy in the neighbour: "<<neighbourHit->getEnergy()<<std::endl;
		
		/*need to get the cell ID in the Mokka encoding, to be able to fill the gangedContainer*/
		const int I = _gangedContainer->getDecoder()->getIFromCellID(neighboursVec[i]);
		const int J = _gangedContainer->getDecoder()->getJFromCellID(neighboursVec[i]);
		const int K = _gangedContainer->getDecoder()->getKFromCellID(neighboursVec[i]);

		_gangedContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);
		const int cellIDinMokkaEncoding = _gangedContainer->getDecoder()->getCellID(I, J, K);
		neighbourHit->setCellID0(cellIDinMokkaEncoding);
		_gangedContainer->fillByCellID(cellIDinMokkaEncoding, neighbourHit);
	      }
	    else
	      {
		streamlog_out(DEBUG0)<<"         Guess what, hit alread exists, energy="<<neighbourHit->getEnergy()<<endl;

		/*add up the leakedEnergy to the original energy of the neighbours*/
		neighbourHit->setEnergy(neighbourHit->getEnergy() + leakedEnergy);
	      }

	    streamlog_out(DEBUG0)<<"      final energy in the neighbour: "<<neighbourHit->getEnergy()<<std::endl;	    
	  }/*---------------- end loop over neighbours vector -----------------------*/

     }/*------------------- end loop over ganged iterator ----------------------------*/
    

    /*************************************************************/
    streamlog_out(DEBUG0)<<"\n After cross-talk: gangedContainer has "
		 <<_gangedContainer->getAllElements().size()
		 <<"elements, from which "<<countNewCells<<" new hits"
		 <<endl;

  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcDigitisationProcessor::simulateSiPMBehaviour()
  {
    streamlog_out(DEBUG0)<<"\n\n Start to simulate SiPM behaviour "<<endl;

    const std::string neighboursEncodingString = 
      CALICE::CellNeighboursProcessor::getEncodingString(_cellNeighboursProcessorName);

    /*now the ganged container contains ganged cells, with cross-talk simulated*/
    std::vector<CalorimeterHitImpl*> gangedVec = _gangedContainer->getAllElements();
    for (std::vector<CalorimeterHitImpl*>::iterator iter = gangedVec.begin();
	 iter != gangedVec.end(); ++iter)
      {
	int cellID = (*iter)->getCellID0();
	CalorimeterHitImpl *currentHit = (*iter);
	float energyInMIPs = currentHit->getEnergy();

	_gangedContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);

	streamlog_out(DEBUG0)<<"\n---------------"<<endl;
	streamlog_out(DEBUG0)<<" currentHit with cell ID "<<cellID<<", energyInMIPs "<<energyInMIPs
		     <<", I/J/K "<<_gangedContainer->getDecoder()->getIFromCellID(cellID)
		     <<"/"<<_gangedContainer->getDecoder()->getJFromCellID(cellID)
		     <<"/"<<_gangedContainer->getDecoder()->getKFromCellID(cellID)
		     <<endl;

	streamlog_out(DEBUG0)<<" calibrationEncodingString: "<<_calibContainer->getDecoder()->getCellIDEncoding()<<endl;
	SiPMCalibrations *calibration = _calibContainer->getByCellID(cellID);
	if (calibration == NULL) 
	  {
	    streamlog_out(DEBUG0)<<" Sorry, no calibration found for cellID "<<cellID<<endl;
	    continue;
	  }
	
	/*-------------------- calibration coefficients --------------*/
	LinearFitCompound *lfcMIP            = calibration->getMIP();
	LinearFitCompound *lfcGain           = calibration->getGain();
	SimpleValue *interCalibContainer     = calibration->getInterCalibration();
	SatCorrFunction *saturationFunction  = calibration->getSaturationCorrection();
            
	/*---------------------- temperature -------------------------*/
	SimpleValue *temperatureObject = calibration->getTemperature();
        const float temperature = temperatureObject->getValue();
	
	/*---------------------- MIP ---------------------------------*/
	float mipValue = 0;
	if (_doMipTempCorr) mipValue = lfcMIP->eval(temperature);
	else
	  mipValue = lfcMIP->getConstant();

	if (mipValue <= 0)
	  {
	    /*
	      streamlog_out(ERROR0) << "  ERROR - ignoring cell with MIP coefficient "<<mipValue
	      << "at I/J/K "<<_gangedContainer->getDecoder()->getIFromCellID(cellID)
	      <<"/"<<_gangedContainer->getDecoder()->getJFromCellID(cellID)
	      <<"/"<<_gangedContainer->getDecoder()->getKFromCellID(cellID)
	      << endl;
	    */
	    continue;
	  }
	if (mipValue > 2000) /*is the default value 100000?*/
	  {
	    /*
	      streamlog_out(WARNING)<<" Warning, very high MIP value, equal to "<<mipValue
	      <<", setting it to zero..."<<std::endl;
	    */
	    mipValue = 0;
	  }

	/*---------------------- gain ---------------------------------*/
	float gainValue = 0;
	if (_doGainTempCorr) gainValue = lfcGain->eval(temperature);
	else
	  gainValue = lfcGain->getConstant();
	if (gainValue <= 0)
	  {
	    /*
	      streamlog_out(ERROR0) << "  ERROR - ignoring cell with GAIN coefficient "<<gainValue
	      << "at I/J/K "<<_gangedContainer->getDecoder()->getIFromCellID(cellID)
	      <<"/"<<_gangedContainer->getDecoder()->getJFromCellID(cellID)
	      <<"/"<<_gangedContainer->getDecoder()->getKFromCellID(cellID)
	      << endl;
	    */
	    continue;
	  }
	
	/*---------------------- InterCalibration ----------------------*/
	float interCalibrationValue = interCalibContainer->getValue();
	if (interCalibrationValue <= 0)
	  {
	    /*
	      streamlog_out(ERROR0) << "  ERROR - ignoring cell with IC coefficient "<<interCalibrationValue
	      << "at I/J/K "<<_gangedContainer->getDecoder()->getIFromCellID(cellID)
	      <<"/"<<_gangedContainer->getDecoder()->getJFromCellID(cellID)
			  <<"/"<<_gangedContainer->getDecoder()->getKFromCellID(cellID)
			  << endl;
	    */
	    continue;
	  }

	streamlog_out(DEBUG0) << "  - MIP: " << mipValue << endl;
	streamlog_out(DEBUG0) << "  - gain: " << gainValue << endl;
	streamlog_out(DEBUG0) << "  - intercalibration: " << interCalibrationValue << endl;
	
        //check if gainConstant or IC is default => put gainConst to have fixed LY.
        //This prevents an event to have the "strange" saturated amplitudes
        const SiPMCalibrationStatusBits bits = SiPMCalibrationStatusBits( calibration->getStatus() );
        
        if ( _correctDefaultGainToLY && (bits.gainConstantIsDefault() || bits.interCalibrationIsDefault()) ) {
          gainValue = mipValue * interCalibrationValue / _fixedLY;
        }
	
	/*---------------------- light yield ---------------------------*/
	const float lightYield  = mipValue/gainValue * interCalibrationValue;
	const float linearPixel = lightYield * energyInMIPs;

	double saturatedPixel = 0.0;

	if(_doSaturation == true) //saturatedPixel true as default
	  {
	  saturatedPixel   = saturationFunction->saturate(linearPixel);
	  
	  streamlog_out(DEBUG0)<<"  - lightYield: "<<lightYield<<endl;
	  streamlog_out(DEBUG0)<<"  - linearPixel: "<<linearPixel<<endl;
	  streamlog_out(DEBUG0)<<"  - saturatedPixel (before smearing): "<<saturatedPixel<<endl;
	  

	  /* smear the number of pixel with a binomial statistic*/
	  if( _doBinomialSmearing == true ) 
	    {
	      int nTot = static_cast<int>(saturationFunction->saturate(10000));
	      double probability = saturatedPixel / nTot;
	      
	      /*ROOT returns 0 if probability is bigger than 1*/
	      if(probability > 1) probability = 1;
	      
	      streamlog_out(DEBUG0)<<"  - nTot: "<<nTot<<" probability: "<<probability<<endl;
	      saturatedPixel = _randomGenerator->Binomial(nTot, probability);
	      
	    }/*---------- end of binomial smearing ------------------------*/
	  streamlog_out(DEBUG0)<<"  - saturatedPixel: "<<saturatedPixel<<endl;

	  }
	else // linearPixel
	  {
	    saturatedPixel = linearPixel;
	  }	

	float ADCcounts = 0;
	if(lightYield > 0) ADCcounts = ( saturatedPixel / lightYield ) * mipValue;
	streamlog_out(DEBUG0) <<"  - ADC counts: "<<ADCcounts<<endl;
    
	if(_doSaturation == true && _doAddNoise != true && ADCcounts > MAXADC) 
	  {
	    /*
	    streamlog_out(WARNING) << "Maximum number of ADC reached: " << ADCcounts 
	    << '\n'
	    << "Limiting to: " << MAXADC << endl;
	    */

	    ADCcounts = MAXADC;
	  }

	streamlog_out(DEBUG0) <<"  - final ADC counts: "<<ADCcounts<<endl;

	/* now we store the ADC channels per channel*/
	currentHit->setEnergy(ADCcounts);
	
      }/*------------------------ end loop over gangedVector -----------------------------*/

    streamlog_out(DEBUG0)<<"\n After SIPM behaviour: gangedContainer has "
		 <<_gangedContainer->getAllElements().size()<<" elements"<<endl;

 
  }
  

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcDigitisationProcessor::addNoise(LCCollection *noiseCol)
  {
    streamlog_out(DEBUG0)<<" \n\n Start to add noise "<<std::endl;
  
    int nNoiseHits = noiseCol->getNumberOfElements();
    streamlog_out(DEBUG0)<<" nNoiseHits="<<nNoiseHits<<endl;

    const std::string noiseEncodingString = noiseCol->getParameters().getStringVal(LCIO::CellIDEncoding);
    DecoderSet *decoder = _gangedContainer->getDecoder();
    decoder->setCellIDEncoding(noiseEncodingString);

    for (int iHit = 0; iHit < nNoiseHits; ++iHit)
       {
	 CalorimeterHit *noiseHit = dynamic_cast<CalorimeterHit*>(noiseCol->getElementAt(iHit));
	 int cellID   = noiseHit->getCellID0();
	 float energy = noiseHit->getEnergy();
	 
 	 decoder->setCellIDEncoding(noiseEncodingString);

	 CalorimeterHitImpl *gangedHit = _gangedContainer->getByCellID(cellID);
	 /*if that tile is active, i.e. it is present in the Mokka file*/
	 if (gangedHit != NULL)
	   {
	     if(_doSaturation == true && _doAddNoise == true 
		&& (energy + gangedHit->getEnergy()) >  MAXADC) 
	       {
		 /*
		   streamlog_out(WARNING) << "Maximum number of ADC reached: " 
		   << (energy + gangedHit->getEnergy()) 
		   << '\n'
		   << "Limiting to: " << MAXADC << endl;
		 */
		 
		 gangedHit->setEnergy(MAXADC);
	       }
	     else
	       {
		 gangedHit->setEnergy(energy + gangedHit->getEnergy());
	       }
	   }
	 else
	   {
	     CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
	     newHit->setEnergy(energy);
	     
	     unsigned int I = decoder->getIFromCellID(cellID);
	     unsigned int J = decoder->getJFromCellID(cellID);
	     unsigned int K = decoder->getKFromCellID(cellID);
	     
	     decoder->setCellIDEncoding(_mokkaEncodingString);
	     int newCellID = decoder->getCellID(I, J, K);
	     newHit->setCellID0(newCellID);
	     
	     _gangedContainer->fillByCellID(newCellID, newHit);
	   }	 

       }/*----------- end loop over noise hits ------------------------------*/
 
    streamlog_out(DEBUG0)<<"\n After noise addition: gangedContainer has "
		 <<_gangedContainer->getAllElements().size()<<" elements"<<endl;

  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcDigitisationProcessor::createOutputCollection(LCEvent *evt)
  {
    streamlog_out(DEBUG0)<<" \n\n start to create output collection "<<endl;
 
    LCCollectionVec *outputColLast = new LCCollectionVec(LCIO::CALORIMETERHIT);

    std::vector<CalorimeterHitImpl*> gangedVec = _gangedContainer->getAllElements();
     _gangedContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);

    for (std::vector<CalorimeterHitImpl*>::iterator iter = gangedVec.begin();
	 iter != gangedVec.end(); ++iter)
      {
	CalorimeterHitImpl *currentHit = (*iter);
	outputColLast->addElement(currentHit);
      }/*------------ end loop over hits --------------------------------------*/
  
    LCParameters &theParam = outputColLast->parameters();
    theParam.setValue(LCIO::CellIDEncoding, _mokkaEncodingString);
  
    streamlog_out(DEBUG0)<<" output collection "<<_outputColName<<" has "<<outputColLast->getNumberOfElements()
    		 <<", with encoding "<<theParam.getStringVal(LCIO::CellIDEncoding)
    		 <<endl;

    /*add output collection to the event*/
    if (outputColLast->getNumberOfElements() > 0)
      {
	evt->addCollection(outputColLast, _outputColName.c_str());
      } 
    else 
      { 
    	delete outputColLast;
      }


  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcDigitisationProcessor::writeCollectionForDebug(LCEvent *evt, const std::string colName)
  {       
    LCCollectionVec *outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    std::vector<CalorimeterHitImpl*> gangedVec = _gangedContainer->getAllElements();
 
   for (std::vector<CalorimeterHitImpl*>::iterator iter = gangedVec.begin();
	 iter != gangedVec.end(); ++iter)
      {
	CalorimeterHitImpl *currentHit = (*iter);
	CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
	newHit->setEnergy(currentHit->getEnergy());
	newHit->setCellID0(currentHit->getCellID0());
	outputCol->addElement(newHit);

      }//------------ end loop over hits --------------------------------------

   LCParameters &theParam = outputCol->parameters();
   theParam.setValue(LCIO::CellIDEncoding, _mokkaEncodingString);

   if (outputCol->getNumberOfElements() > 0)
     evt->addCollection(outputCol, colName);
    
  }
  
  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void AhcDigitisationProcessor::end()
  {
  }
  
  /***************************************************************************************
   * create instance to make processor known to Marlin
   * should be very last thing to do, to prevent order problems during
   * deletion of static objects.
   ***************************************************************************************/
  AhcDigitisationProcessor aAhcDigitisationProcessor;

}/*end of namespace CALICE*/
