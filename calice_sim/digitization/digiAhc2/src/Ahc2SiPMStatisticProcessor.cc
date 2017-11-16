#include "Ahc2SiPMStatisticProcessor.hh"

#include <iostream>
#include <vector>
#include <TMath.h>

// -- CALICE Header
#include "MappingProcessor.hh"
#include "CellDescriptionProcessor.hh"
#include "CellNeighboursProcessor.hh"
#include "Ahc2Calibrations.hh"
#include "Ahc2CalibrationsProcessor.hh"
#include "CellIterator.hh"
#include "Ahc2CalibrationStatusBits.hh"
#include "SiPMTemperatureProcessor.hh"
#include "Ahc2SatCorr.hh"

// ---- LCIO Headers
#include "EVENT/LCCollection.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/LCFlagImpl.h"
#include "UTIL/LCTypedVector.h"
#include "UTIL/CellIDDecoder.h"
#include "UTIL/CellIDEncoder.h"
#include "UTIL/BitField64.h"

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"

using namespace lcio;
using namespace marlin;
using namespace std;

namespace CALICE
{

  Ahc2SiPMStatisticProcessor::Ahc2SiPMStatisticProcessor() : Processor("Ahc2SiPMStatisticProcessor")
  {

    _description = "AHCAL EPT digitisation";

    // register steering parameters: name, description, class-variable, default value

    registerProcessorParameter("Input_Collection",
    "Name of SimCalorimeterHit input collections",
    _calorimInpCollection,
    std::string("hcal_MIP") );

    registerProcessorParameter("Output_Collection" ,
    "Name of the Calorimeter Hit output collection converted to px with SiPM statistic treatment"  ,
    _calorimOutCollection,
    std::string("hcal_smeared") ) ;

    registerProcessorParameter("NoiseCollectionName",
    "Name of the input noise collection",
    _noiseColName,
    std::string("Ahc2Noise"));

    registerProcessorParameter( "MappingProcessorName" ,
    "Name of the MappingProcessor instance that provides"
    " the geometry of the detector." ,
    _mappingProcessorName,
    std::string("MyMappingProcessor") ) ;

    registerProcessorParameter( "CellNeighboursProcessorName" ,
    "Name of the CellNeighboursProcessor instance that provides the neighbours",
    _cellNeighboursProcessorName,
    std::string("MyNeighboursProcessor") ) ;

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

    registerProcessorParameter("filterDeadCells",
    "Filter dead cells",
    _filterDeadCells,
    (bool) false);

    registerProcessorParameter("filterDefaultCells",
    "Filter cells that use some default value in calibration.",
    _filterDefaultCells,
    (bool) false);

    registerProcessorParameter("doMIPTempCorrection",
    "Do MIP Temperature correction.",
    _doMipTempCorr,
    (bool) false);

    registerProcessorParameter("doGainTempCorrection",
    "Do Gain Temperature correction.",
    _doGainTempCorr,
    (bool) false);

    registerProcessorParameter("DoAddNoise",
    "Add detector noise to pure MC",
    _doAddNoise,
    bool(false));

    registerProcessorParameter("NoiseEnergyMIP",
    "Case energy of noise hits are in MIPs",
    _noiseEnergyMIP,
    bool(false));

    registerProcessorParameter("RandomSeed", "Seed for random number generator",
    _randomSeed, int(0));

    registerProcessorParameter("doSaturation",
    "Saturation of the pixel",
    _doSaturation,
    (bool) true);

    registerProcessorParameter("doBinomialSmearing",
    "Binomial Smearing of the pixel",
    _doBinomialSmearing,
    bool(true));

    registerProcessorParameter("correctDefaultGainTofixedLightYield",
    "Correct light yield for cells with default gain value in calibration.",
    _correctDefaultGainToLY,
    (bool) false);

    registerProcessorParameter( "fixedLightYieldForCorrection" ,
    "Fixed light yield for cells with default gain value in calibration.",
    _fixedLY,
    (float)13. ) ;

    registerProcessorParameter( "ITEP_PhysicsMode" ,
    "Physics Mode for new ITEP.",
    _physicsMode,
    (bool)false ) ;

    registerProcessorParameter("DoOpticalCrossTalk",
    "Apply optical cross talk",
    _doOpticalCrossTalk,
    bool(true));

    StringVec _lightLeakageExample;
    _lightLeakageExample.push_back("Layer");
    _lightLeakageExample.push_back("LightLeakage");

    registerProcessorParameter( "LightLeakageParameters" ,
    "Light leakage (i.e. factor for the tiles cross-talk) for each layer",
    _lightLeakage,
    _lightLeakageExample,
    _lightLeakageExample.size() ) ;

  }

  /************************************************************************************/

  void Ahc2SiPMStatisticProcessor::init()
  {
    std::cout << "init Ahc2SiPMStatistics called" << std::endl ;
    printParameters();

    /*initialize the random generator*/
    _randomGenerator = new TRandom3(_randomSeed);

    _HitContainer = NULL;

    std::stringstream message;
    bool error = false;

    _mapper = dynamic_cast<const Ahc2Mapper*>(MappingProcessor::getMapper(_mappingProcessorName));
    if ( !_mapper )
    {
      message << "MappingProcessor::getMapper("<< _mappingProcessorName
      << ") did not return a valid mapper." << std::endl;
      error = true;
    }

    _calibContainer = Ahc2CalibrationsProcessor::getCalibrations(_calibProcessorName);
    if ( ! _calibContainer )
    {
      message << "init(): Ahc2CalibrationsProcessor::getCalibrations("<< _calibProcessorName
      << ") did not return a valid MappedContainer." <<std::endl;
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

    _cellDescriptions = CellDescriptionProcessor::getCellDescriptions(_cellDescriptionProcessorName);
    if ( ! _cellDescriptions )
    {
      streamlog_out(ERROR) << "Cannot obtain cell descriptions from "<<_cellDescriptionProcessorName
      <<" Maybe, processor is not present" << std::endl;
      error = true;
    }

    if (error)
    {
      streamlog_out(ERROR) << message.str();
      //throw marlin::StopProcessingException(this);
      return;
    }

    _mapperVersion = _mapper->getVersion();

    _nRun = 0 ;
    _nEvt = 0 ;

    //LightLeakage values
    if( parameterSet( "LightLeakageParameters" ) )
    {
      unsigned index = 0 ;
      while( index < _lightLeakage.size() )
      {
        string strK( _lightLeakage[ index++ ] );
        string strValue( _lightLeakage[ index++ ] );

        int K = atoi(strK.c_str());
        float value = atof(strValue.c_str());

        m_leakage.insert(make_pair(K, value));
      }
    }

    std::cout << "Init finished" << std::endl;

    //fOut = new TFile("CheckTimeSiPM.root", "RECREATE");
    //hFillTimeSiPM = new TH1F("hFillTimeSiPM", "hFillTimeSiPM", 400, -200, 200);
  }

  /************************************************************************************/
  void Ahc2SiPMStatisticProcessor::processRunHeader( LCRunHeader* run)
  {
    _nRun++ ;
  }

  void Ahc2SiPMStatisticProcessor::processEvent( LCEvent * evt )
  {
    int evtNumber = evt->getEventNumber();
    streamlog_out(DEBUG) << " \n ---------> Event: " << evtNumber << "!!! <-------------\n" << std::endl;

    LCCollection *inputCalorimCollection = NULL;
    try{
      inputCalorimCollection = evt->getCollection( _calorimInpCollection );
    }
    catch ( EVENT::DataNotAvailableException &e )
    {
      streamlog_out(WARNING) << "Collection " << _calorimInpCollection << " not available, skip this event" <<std::endl;
      throw marlin::SkipEventException(this);
      return; // allow lcio event go to next processors.
    }

    LCCollection *noiseCol = NULL;
    if(_doAddNoise)
    {
      try{
        noiseCol = evt->getCollection( _noiseColName );
      }
      catch ( EVENT::DataNotAvailableException &e )
      {
        streamlog_out(WARNING)<<" Collection "<< _noiseColName <<" not available, skip this event"<<std::endl;
        throw marlin::SkipEventException(this);
        return;
      }
    }

    int nHits = inputCalorimCollection->getNumberOfElements();
    streamlog_out(DEBUG) << "Number of hits: " << nHits << std::endl;

    /*the Mokka encoding string*/
    _mokkaEncodingString = inputCalorimCollection->getParameters().getStringVal(LCIO::CellIDEncoding);

    /*clean up the container at the beginning of the event*/
    delete _HitContainer;

    /*----------------------------------------------------------------------------------
    First step: fill the container with hits E[MIPs]*/
    this->fillContainer(inputCalorimCollection);

    /*---------------------------------------------------------------------------------
    Next step: OPTICAL cross talk, still E[MIPs]
    */
    if(_doOpticalCrossTalk)
    this->simulateOpticalCrossTalk();

    /*---------------------------------------------------------------------------------
    Next step: Simulate SiPM behaviour,
    i.e. convert the energy from MIPs to ADC scale
    */
    this->simulateSiPMBehaviour();

    /*---------------------------------------------------------------------------------
    Next step: Add the noise from data, E[ADC]
    */
    if(_doAddNoise) this->addNoise(noiseCol);

    /*---------------------------------------------------------------------------------
    Next step: Finally, create the output collection, E[ADC]
    */
    createOutputCollection(evt, _calorimOutCollection);

    streamlog_out(DEBUG) << " \n ---------> End Event: " << evtNumber << "!!! <-------------\n" << std::endl;

    _nEvt++;

  }

  /************************************************************************************/

  void Ahc2SiPMStatisticProcessor::check( LCEvent * evt ) {
    // nothing to check here - could be used to fill checkplots in reconstruction processor
  }

  /************************************************************************************/
  void Ahc2SiPMStatisticProcessor::fillContainer(LCCollection *inputCol)
  {
    streamlog_out(DEBUG) << "\n\n Start to fill Hit container " << std::endl;

    /*create a new map container. Note the 'false' flag, which tells the container
    that he is not responsible for the deletion of the contained elements.
    We use this because at the end of the event we have all the elements from
    the container into an LCCollection, and Marlin deletes them at the end of the event.*/

    _HitContainer = new MappedContainer<CalorimeterHitImpl>(_mapper, false);
    _HitContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);

    /*Copy the input collection into the container, which we'll modify afterwards*/
    for (int iHit = 0; iHit < inputCol->getNumberOfElements(); ++iHit)
    {
      CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inputCol->getElementAt(iHit));
      int cellID0 = hit->getCellID0();

      CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
      newHit->setTime(hit->getTime());
      newHit->setEnergy(hit->getEnergy());
      newHit->setCellID0(cellID0);
      newHit->setPosition(hit->getPosition());

      streamlog_out(DEBUG) << "\n---------------" << std::endl;
      streamlog_out(DEBUG) << " currentHit with cell ID " << cellID0 <<", energyInMIPs "<< hit->getEnergy()
      << ", I/J/K " << _HitContainer->getDecoder()->getIFromCellID(cellID0)
      << "/" << _HitContainer->getDecoder()->getJFromCellID(cellID0)
      << "/" << _HitContainer->getDecoder()->getKFromCellID(cellID0)
      << std::endl;

      _HitContainer->fillByCellID(cellID0, newHit);
    }

    streamlog_out(DEBUG)  <<"\n After: HitContainer has "
    <<_HitContainer->getAllElements().size()
    <<" elements"<<std::endl;

  }

  /*********************************************************************************/
  /*                                                                               */
  /*  Note: during cross-talk, 10% of the energy leaks to the neighbouring         */
  /*  tiles, but the leaked energy is NOT subtracted from the energy because       */
  /*  the measured MIP is actually only 90%MIPs. For more details, see page 101 in */
  /*   http://www-library.desy.de/preparch/desy/thesis/desy-thesis-10-006.pdf      */
  /*                                                                               */
  /*********************************************************************************/
  void Ahc2SiPMStatisticProcessor::simulateOpticalCrossTalk()
  {
    streamlog_out(WARNING)<< "\n\n Start to simulate optical cross-talk";

    std::vector<CalorimeterHitImpl*> HitVec = _HitContainer->getAllElements();
    streamlog_out(WARNING)<< " for " << HitVec.size() << " hits" << std::endl;

    /*counter for debug purposes*/
    int countNewCells = 0;
    int countGanged   = 0;

    // Generate a new vector to save the original hits information
    // Loop over the original hits, and add optical xtalk into the
    // neighbours cell.
    // It is only first order optical xtalk has been applied here.
    std::vector<CalorimeterHitImpl> originalHitVec;
    for (std::vector<CalorimeterHitImpl *>::iterator iter = HitVec.begin();
    iter != HitVec.end(); ++iter)
    {
      CalorimeterHitImpl *currentHit = (*iter);
      originalHitVec.push_back((*currentHit));
    }

    /*------------------------------------------------------------------
    Start to loop over original hits
    ------------------------------------------------------------------*/
    for (std::vector<CalorimeterHitImpl>::iterator iter = originalHitVec.begin();
    iter != originalHitVec.end(); ++iter)
    {
      /*set the encoding string back to the Mokka encoding string*/
      _HitContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);

      int cellID = (*iter).getCellID0();
      CalorimeterHitImpl *currentHit = &(*iter);

      _cellDescriptions->getDecoder()->setCellIDEncoding(_mokkaEncodingString);

      CellDescription* currentCellDescription = _cellDescriptions->getByCellID(cellID);
      const float currentTileSize = currentCellDescription->getSizeX();

      streamlog_out(WARNING)<<"------------"<<endl;
      streamlog_out(WARNING)<<"i="<<countGanged<<" hit  I/J/K: "<< _HitContainer->getDecoder()->getIFromCellID(cellID)
      <<"/"<<_HitContainer->getDecoder()->getJFromCellID(cellID)
      <<"/"<<_HitContainer->getDecoder()->getKFromCellID(cellID)
      <<" currentHit: "<<currentHit
      <<" cellID: "<<cellID
      <<" energy[MIPs]: "<<currentHit->getEnergy()
      <<" currentTileSize: "<<currentTileSize
      <<endl;
      countGanged ++;

      //Get the light leakage value for the hit in the layer
      float _lightLeakageValue = 0.0;

      if(m_leakage.find(_HitContainer->getDecoder()->getKFromCellID(cellID)) != m_leakage.end())
      _lightLeakageValue = m_leakage[_HitContainer->getDecoder()->getKFromCellID(cellID)];
      else
      continue;

      /*start loop over neighbours*/
      const std::vector<int> neighboursVec = _cellNeighbours->getByCellID(cellID)->getNeighbours(CellNeighbours::direct, CellNeighbours::module);
      streamlog_out(WARNING)<<"   ->number of neighbours found: "<<neighboursVec.size()<<endl;

      /*before getting the neighbours, set the encoding string to the string used to
      set the neighbours*/
      const std::string neighboursEncodingString = CALICE::CellNeighboursProcessor::getEncodingString(_cellNeighboursProcessorName);
      streamlog_out(WARNING)<<" neighboursEncodingString: "<<neighboursEncodingString<<endl;

      /*---------------------------------------------------------------
      loop over neighbours
      -----------------------------------------------------------------*/
      for (unsigned int i = 0; i < neighboursVec.size(); ++i)
      {
        streamlog_out(WARNING)<<"  \n i="<<i<<endl;
        _cellDescriptions->getDecoder()->setCellIDEncoding(neighboursEncodingString);
        streamlog_out(WARNING)<<"   neighboursEncodingString: "<<neighboursEncodingString<<endl;

        streamlog_out(WARNING)<<"         neighbours: I/J/K="
        <<_cellDescriptions->getDecoder()->getIFromCellID(neighboursVec[i])
        <<"/"<<_cellDescriptions->getDecoder()->getJFromCellID(neighboursVec[i])
        <<"/"<<_cellDescriptions->getDecoder()->getKFromCellID(neighboursVec[i])
        <<" endcoding string: "<<_cellDescriptions->getDecoder()->getCellIDEncoding()
        <<endl;

        CellDescription *neighbourCellDescription = _cellDescriptions->getByCellID(neighboursVec[i]);
        if (neighbourCellDescription == NULL)
        {
          streamlog_out(WARNING)<<"      sorry, no cell description found for neighbour "<<i<<endl;
          continue;
        }

        const float neighbourTileSize = neighbourCellDescription->getSizeX();
        streamlog_out(WARNING)<<"      neighbourTileSize: "<<neighbourTileSize<<endl;

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
        float leakedEnergy = leakageFraction * _lightLeakageValue  * (currentHit->getEnergy()) / (1. - _lightLeakageValue);
        float time = currentHit->getTime();

        streamlog_out(WARNING)<<"      lightLeakage: "<<_lightLeakageValue<<" energy: "<<currentHit->getEnergy()<<std::endl;
        streamlog_out(WARNING)<<"      leakageFraction: "<<leakageFraction<<" leakedEnergy: "<<leakedEnergy<<endl;
        streamlog_out(WARNING)<<"      looking in list for I/J/K="
        <<_cellDescriptions->getDecoder()->getIFromCellID(neighboursVec[i])
        <<"/"<<_cellDescriptions->getDecoder()->getJFromCellID(neighboursVec[i])
        <<"/"<<_cellDescriptions->getDecoder()->getKFromCellID(neighboursVec[i])
        <<endl;

        _HitContainer->getDecoder()->setCellIDEncoding(neighboursEncodingString);
        CalorimeterHitImpl *neighbourHit = _HitContainer->getByCellID(neighboursVec[i]);

        if (neighbourHit == NULL)
        {
          neighbourHit = new CalorimeterHitImpl();
          neighbourHit->setEnergy(leakedEnergy);
          neighbourHit->setTime(time);

          countNewCells++;
          streamlog_out(WARNING)<<"      new hit, leaked energy in the neighbour: "<<neighbourHit->getEnergy()<<std::endl;

          /*need to get the cell ID in the Mokka encoding, to be able to fill the gangedContainer*/
          const int I = _HitContainer->getDecoder()->getIFromCellID(neighboursVec[i]);
          const int J = _HitContainer->getDecoder()->getJFromCellID(neighboursVec[i]);
          const int K = _HitContainer->getDecoder()->getKFromCellID(neighboursVec[i]);

          _HitContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);
          const int cellIDinMokkaEncoding = _HitContainer->getDecoder()->getCellID(I, J, K);
          neighbourHit->setCellID0(cellIDinMokkaEncoding);
          _HitContainer->fillByCellID(cellIDinMokkaEncoding, neighbourHit);
        }
        else
        {
          streamlog_out(WARNING)<<"         Guess what, hit alread exists, energy="<<neighbourHit->getEnergy()<<endl;

          /*add up the leakedEnergy to the original energy of the neighbours*/
          neighbourHit->setEnergy(neighbourHit->getEnergy() + leakedEnergy);
        }

        streamlog_out(WARNING)<<"      final energy in the neighbour: "<<neighbourHit->getEnergy()<<std::endl;
      }/*---------------- end loop over neighbours vector -----------------------*/

    }/*------------------- end loop over ganged iterator ----------------------------*/


    /*************************************************************/
    streamlog_out(WARNING)<<"\n After cross-talk: HitContainer has "
    <<_HitContainer->getAllElements().size()
    <<"elements, from which "<<countNewCells<<" new hits"
    <<endl;

  }


  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/

  void Ahc2SiPMStatisticProcessor::simulateSiPMBehaviour()
  {
    streamlog_out(DEBUG) << " Start to simulate SiPM behaviour " <<std::endl;

    /*now the hit container contains all hits/subhits */
    std::vector<CalorimeterHitImpl*> HitVec = _HitContainer->getAllElements();
    for (std::vector<CalorimeterHitImpl*>::iterator iter = HitVec.begin(); iter != HitVec.end(); ++iter)
    {
      int cellID = (*iter)->getCellID0();
      CalorimeterHitImpl *currentHit = (*iter);
      float simHitMIP = currentHit->getEnergy();

      _HitContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);

      streamlog_out(DEBUG) << "\n---------------" << std::endl;
      streamlog_out(DEBUG) << " currentHit with cell ID " << cellID <<", energyInMIPs "<< simHitMIP
      << ", I/J/K " << _HitContainer->getDecoder()->getIFromCellID(cellID)
      << "/" << _HitContainer->getDecoder()->getJFromCellID(cellID)
      << "/" << _HitContainer->getDecoder()->getKFromCellID(cellID)
      << std::endl;

      streamlog_out(DEBUG) <<" calibrationEncodingString: "<< _calibContainer->getDecoder()->getCellIDEncoding() << std::endl;

      //Get calibrations for the cell
      Ahc2Calibrations *calibration = _calibContainer->getByCellID(cellID);

      if (calibration == NULL)
      {
        streamlog_out(DEBUG)<<" Sorry, no calibration found for cellID " << cellID <<std::endl;
        _HitContainer->deleteByCellID(cellID);
        continue;
      }

      //Get Status of cell (if dead or default values)
      const Ahc2CalibrationStatusBits bits = Ahc2CalibrationStatusBits( calibration->getStatus() );

      streamlog_out(DEBUG) << "  STATUS bits: " << bits << std::endl;

      if ( _filterDeadCells && bits.isDead() )
      {
        streamlog_out(DEBUG) << " SKIP -- channel is marked as dead: " << std::endl;
        _HitContainer->deleteByCellID(cellID);
        continue;
      }

      if ( _filterDefaultCells && bits.hasDefault() )
      {
        streamlog_out(DEBUG) << " SKIP -- channel has default calibration: " << std::endl;
        _HitContainer->deleteByCellID(cellID);
        continue;
      }

      /*-------------------- calibration coefficients --------------*/
      LinearFitCompound *lfcMIP            = calibration->getMIP();
      LinearFitCompound *lfcGain           = calibration->getGain();
      SimpleValue *interCalibContainer     = calibration->getInterCalibration();//IntercalibHGLG !!!
      SaturationParameters *saturationParameters = calibration->getSaturation();
      SimpleValue *ICValuePhys = calibration->getPhysicsCalibIC();//Intercalibration physics mode / calib mode

      /*Saturation correction function */
      Ahc2SatCorr* satFunc = new Ahc2SatCorr(saturationParameters);

      float interCalibrationValuePhys = ICValuePhys->getValue();

      /*---------------------- temperature -------------------------*/
      //SimpleValue *temperatureObject = calibration->getTemperature();
      //const float temperature = temperatureObject->getValue();
      const float temperature = 31.2; /* TO DO */

      /*---------------------- MIP ---------------------------------*/

      /* Get the mip value from the database from the calib mode */
      float mipValue = 0.;
      if (_doMipTempCorr)
      mipValue = lfcMIP->eval(temperature);
      else
      mipValue = lfcMIP->getConstant();

      if (mipValue <= 0)
      {
        _HitContainer->deleteByCellID(cellID);
        continue;
      }

      if (mipValue > 2500) /*is the default value 100000?*/
      mipValue = 0;

      /*---------------------- gain ---------------------------------*/
      float gainValue = 0;

      if (_doGainTempCorr)
      gainValue = lfcGain->eval(temperature);
      else
      gainValue = lfcGain->getConstant();

      if (gainValue <= 0)
      {
        _HitContainer->deleteByCellID(cellID);
        continue;
      }

      //check if gainConstant or IC is default => put gainConst to have fixed LY.
      if ( _correctDefaultGainToLY && (bits.gainConstantIsDefault() || bits.interCalibrationIsDefault()) )
      {
        gainValue = mipValue / _fixedLY;
      }

      /*---------------------- light yield ---------------------------*/
      const float lightYield  = mipValue/gainValue;

      /*---------------------- InterCalibration ----------------------*/
      float interCalibrationValue = interCalibContainer->getValue();

      if (interCalibrationValue <= 0)
      {
        _HitContainer->deleteByCellID(cellID);
        continue;
      }

      streamlog_out(DEBUG) << "- Amplitude: " << simHitMIP << std::endl;
      streamlog_out(DEBUG) << "- MIP: " << mipValue << std::endl;
      streamlog_out(DEBUG) << "- gain: " << gainValue << std::endl;
      streamlog_out(DEBUG) << "- intercalibration: " << interCalibrationValue << std::endl;
      streamlog_out(DEBUG) << "- intercalibration Physics Calib: " << interCalibrationValuePhys << std::endl;
      streamlog_out(DEBUG) << "- lightYield: " << lightYield << std::endl;
      streamlog_out(DEBUG) << "- NeffPix: " << satFunc->getNeffPix() << std::endl;
      streamlog_out(DEBUG) << "- Running mode: " << (_physicsMode ? "Physics Mode" : "Calib Mode") << std::endl;

      /*---------------------- simulate SiPM behaviour ---------------------------*/
      //            binomial smearing
      //            energy mips to pixels
      //            saturate
      //            smear
      //            unsaturate
      //            energy pixels to mip

      float simHitPx = simHitMIP * lightYield; // energy MIP -> pixels
      float simHitADC = 0.;

      if(_doSaturation)
      {
        float satHitPx =  satFunc->saturate( simHitPx ); // saturate

        streamlog_out(DEBUG) << "- simHitPx: " << simHitPx << std::endl;
        streamlog_out(DEBUG) << "- satHitPx (before smearing): " << satHitPx << std::endl;

        if(_doBinomialSmearing)
        {
          float prob = satHitPx / satFunc->getNeffPix(); // calculate p for binomial smearing
          float satSmearedHitPx = _randomGenerator->Binomial(satFunc->getNeffPix(), prob); // binomial smearing

          if(satSmearedHitPx > 0)
          {
            //if physics mode divide the gain in calib mode by Intercalibration Physics/Calib
            if(_physicsMode)
            simHitADC = satSmearedHitPx * gainValue / interCalibrationValuePhys; //energy pixels -> ADC
            else
            simHitADC = satSmearedHitPx * gainValue;
          }
          else
          simHitADC = 0;

          streamlog_out(DEBUG) << "Smearing" << std::endl;
          streamlog_out(DEBUG) << "- prob: " << prob << std::endl;
          streamlog_out(DEBUG) << "- satSmearedHitPx: " << satSmearedHitPx << std::endl;
          //streamlog_out(DEBUG) << "- unsatSmearedHitPx: " << unsatSmearedHitPx << std::endl;
          streamlog_out(DEBUG) << "- simHitADC: " << simHitADC << std::endl;

        }
        else
        {
          if(_physicsMode)
          simHitADC = satHitPx * gainValue / interCalibrationValuePhys;
          else
          simHitADC = satHitPx * gainValue;
        }
      }
      else
      {
        if(_physicsMode)
        simHitADC = simHitPx * gainValue / interCalibrationValuePhys;
        else
        simHitADC = simHitPx * gainValue;
      }

      if(simHitADC > MAXADC)
      simHitADC = MAXADC;

      streamlog_out(DEBUG) << "- simHitADC (after saturation & smearing): " << simHitADC << std::endl;

      /* now we store the ADC channels per channel*/
      currentHit->setEnergy(simHitADC);

      delete satFunc;

    }/*------------------------ end loop over gangedVector -----------------------------*/

    streamlog_out(DEBUG) << "\n After SIPM behaviour: HitContainer has " << _HitContainer->getAllElements().size() << " elements"<< std::endl;


  }

  /******************************************************************************/
  /*                                                                            */
  /*                                                                            */
  /*                                                                            */
  /******************************************************************************/
  void Ahc2SiPMStatisticProcessor::addNoise(LCCollection *noiseCol)
  {

    streamlog_out(DEBUG) <<" \n\n Start to add noise "<<std::endl;

    int nNoiseHits = noiseCol->getNumberOfElements();
    streamlog_out(DEBUG)<<" nNoiseHits="<<nNoiseHits<<endl;

    const std::string noiseEncodingString = noiseCol->getParameters().getStringVal(LCIO::CellIDEncoding);
    DecoderSet *decoder = _HitContainer->getDecoder();
    decoder->setCellIDEncoding(noiseEncodingString);

    for (int iHit = 0; iHit < nNoiseHits; ++iHit)
    {
      CalorimeterHit *noiseHit = dynamic_cast<CalorimeterHit*>(noiseCol->getElementAt(iHit));
      int cellID   = noiseHit->getCellID0();
      float time = noiseHit->getTime();

      decoder->setCellIDEncoding(noiseEncodingString);

      float energy = 0;
      if(_noiseEnergyMIP)
      {
        Ahc2Calibrations *calibration = _calibContainer->getByCellID(cellID);

        if (calibration == NULL)
        {
          streamlog_out(WARNING)<<" Sorry, no calibration found for cellID " << cellID <<std::endl;
          continue;
        }

        LinearFitCompound *lfcMIP = calibration->getMIP();
        energy = noiseHit->getEnergy()*lfcMIP->getConstant();
      }
      else
      energy = noiseHit->getEnergy();

      streamlog_out(DEBUG) << "Noise hit " << iHit << " CellID " << cellID << " Energy " << energy << " Time " << time << endl;

      CalorimeterHitImpl *hit = _HitContainer->getByCellID(cellID);
      /*if that tile is active, i.e. it is present in the Mokka file*/
      if (hit != NULL)
      {
        if(_doSaturation == true && _doAddNoise == true && (energy + hit->getEnergy()) > MAXADC)
        {
          hit->setEnergy(MAXADC);
          hit->setTime(time + hit->getTime());
        }
        else
        {
          hit->setEnergy(energy + hit->getEnergy());
          hit->setTime(time + hit->getTime());
        }
      }
      else
      {
        CalorimeterHitImpl *newHit = new CalorimeterHitImpl();
        newHit->setEnergy(energy);
        newHit->setTime(time);

        unsigned int I = decoder->getIFromCellID(cellID);
        unsigned int J = decoder->getJFromCellID(cellID);
        unsigned int K = decoder->getKFromCellID(cellID);

        decoder->setCellIDEncoding(_mokkaEncodingString);
        int newCellID = decoder->getCellID(I, J, K);
        newHit->setCellID0(newCellID);

        _HitContainer->fillByCellID(newCellID, newHit);
      }

    }/*----------- end loop over noise hits ------------------------------*/

    streamlog_out(DEBUG) <<"\n After noise addition: HitContainer has "
    <<_HitContainer->getAllElements().size()<<" elements"<<std::endl;

  }

  /******************************************************************************/
  void Ahc2SiPMStatisticProcessor::createOutputCollection(LCEvent *evt, const std::string colName)
  {
    streamlog_out(DEBUG) << " \n\n start to create output collection " << std::endl;

    LCCollectionVec *outputCol = new LCCollectionVec(LCIO::CALORIMETERHIT);
    std::vector<CalorimeterHitImpl*> HitVec = _HitContainer->getAllElements();
    _HitContainer->getDecoder()->setCellIDEncoding(_mokkaEncodingString);

    LCFlagImpl hitFlag(outputCol->getFlag());
    hitFlag.setBit(LCIO::RCHBIT_TIME);
    hitFlag.setBit(LCIO::CHBIT_LONG);
    outputCol->setFlag(hitFlag.getFlag());

    for (std::vector<CalorimeterHitImpl*>::iterator iter = HitVec.begin(); iter != HitVec.end(); ++iter)
    {
      CalorimeterHitImpl *currentHit = (*iter);
      //hFillTimeSiPM->Fill(currentHit->getTime());
      outputCol->addElement(currentHit);
    }

    /*------------ end loop over hits --------------------------------------*/

    LCParameters &theParam = outputCol->parameters();
    theParam.setValue(LCIO::CellIDEncoding, _mokkaEncodingString);

    streamlog_out(DEBUG)<<" output collection "<< colName <<" has "<< outputCol->getNumberOfElements()
    <<", with encoding "<<theParam.getStringVal(LCIO::CellIDEncoding)
    <<std::endl;

    if (outputCol->getNumberOfElements() > 0)
    evt->addCollection(outputCol, colName);

    streamlog_out(DEBUG) << "end create output collection " << std::endl;
  }

  /************************************************************************************/
  void Ahc2SiPMStatisticProcessor::end(){

    std::cout << "SiPMStatisticProcessor::end()  " << name()
    << " processed " << _nEvt << " events in " << _nRun << " runs "
    << std::endl ;

    //fOut->cd();
    //hFillTimeSiPM->Write();
    //fOut->Close();
  }

  /************************************************************************************/
  void Ahc2SiPMStatisticProcessor::printParameters(){
    std::cerr << "============= SiPM Statistics Treatment Processor =================" <<std::endl;
    std::cerr << "Converting Simulation Hits from MIP to pixel scale" <<std::endl;
    std::cerr << "Input Collection name : " << _calorimInpCollection << std::endl;
    std::cerr << "Output Collection name : " << _calorimOutCollection << std::endl;
    std::cerr << "Noise Collection name : " << _noiseColName << std::endl;
    std::cerr << "Add noise : " << _doAddNoise << std::endl;
    std::cerr << "Saturation : " << _doSaturation << std::endl;
    std::cerr << "Smearing : " << _doBinomialSmearing << std::endl;
    std::cerr << "Default LY : " << _fixedLY << std::endl;
    std::cerr << "Cross-talk : " << _doOpticalCrossTalk << std::endl;
    std::cerr << "=======================================================" <<std::endl;
    return;
  }

  /***************************************************************************************
  * create instance to make processor known to Marlin
  * should be very last thing to do, to prevent order problems during
  * deletion of static objects.
  ***************************************************************************************/
  Ahc2SiPMStatisticProcessor aAhc2SiPMStatisticProcessor;

}/*end of namespace CALICE*/
