#include <cmath>
#include <SimpleHitSearch.hh>

#include <IMPL/LCEventImpl.h>
#include <IMPL/LCRunHeaderImpl.h>
#include <IMPL/RawCalorimeterHitImpl.h>
#include <IMPL/CalorimeterHitImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <EVENT/LCObject.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <EVENT/LCRunHeader.h>
#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
#include <EVENT/LCParameters.h>
#include <EVENT/LCFloatVec.h>
#endif

#include <ErrorBits.hh>
#include <marlin/ConditionsProcessor.h>

#include <AdcValueAccess.hh>
#include <CalibrationFactory.hh>
#include <NoOpCalibration.hh>
#include <Calibration.hh>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include <Average_t.hh>
#include <AverageSimple_t.hh>
#include <histmgr/HistMgr.hh>

#include "LCPayload.hh"
#include "NoiseParameter.hh"
#include "NoiseParameterArray_t.hh"

// -- LCCD headers
#include "lccd.h"
#include "lccd/DBInterface.hh"

//ROOT needed for checks...
#include "TF1.h"
#include "TH1F.h"

//\todo move to collection_names:
// The converter will count the events since the last DAQ state change: configuration change, slow readout, etc.
#define PAR_EVENTS_SINCE_LAST_DAQ_STATE_CHANGE "CALDAQ_EventsSinceLastChange"

//\todo move to collection_names:
// need ECAL, HCAL, TC version or only ECAL, HCAL+TC ?


#define CORRELATED_NOISE
#define WITH_ALL_CONTROL_HISTOGRAMS
#define CORRECT_SIGNAL_INDUCED_PED_SHIFT

using namespace lcio;


namespace CALICE {
   SimpleHitSearch a;


  SimpleHitSearch::SimpleHitSearch() 
    : VRawADCValueProcessor("SimpleHitSearch"),
      _calibration(new NoOpCalibration),
      _cellParameterCollection(0),
      _hfile(0),
      _detectorTransformationChange(this,&SimpleHitSearch::detectorTransformationChanged),
      _referenceTransformationChange(this,&SimpleHitSearch::referenceTransformationChanged),
      _nPedestalEvents(0),
      _nPedestalEventsTotal(0),
      _nPedestalEventSets(0),
      _nEventsTotal(0),
      _nEventsWithHits(0),
      _nEventsWithHitsTotal(0),
      _uncoveredTriggerCounter(0),
      _nEventsWithoutADC(0),
      _rejectedEvents(0),
      _nEventsWithMissingAdcBlocks(0)
#ifdef WITH_CONTROL_HISTOGRAMS
    ,_histGroupKey(type()),
      _histSignalKey("Signal"),
      _histNoiseKey("Noise"),
#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
      _histSignalInducedPedestalShiftKey("SignalInducedPedestalShift"),
#endif
      _histNoiseCorrelationKey("NoiseCorrelation"),
      _histTriggerEnableKey("TriggerEnabledBits")
      //      ,_histSignalPerLayerKey("SignalPerLayer"),
      //      _histNoiseKey("Noise"),
      //      _histPedestalKey("Pedestal"),
      //      _histADCKey("ADCValue")
#endif
  {
    changeState(kStateSkipEvent);

    // --- set state methods
    // illegal state
    _stateFunc[kStateUnknown]=static_cast<SimpleHitSearch::ProcessFunc_t>(0);

    // do nothing
    _stateFunc[kStateSkipEvent]=&SimpleHitSearch::skipEvents;                            

    // cosmics trigger:
    // slowly update pedestals and search for hits
    _stateFunc[kStateCosmics]=&SimpleHitSearch::processCosmics;   

    // beam trigger:
    // detector is full with hits always at the same locations
    _stateFunc[kStateBeam]=&SimpleHitSearch::processBeamEvents;

    // pedestal trigger:
    // recalculate pedestals
    _stateFunc[kStatePedestals]=&SimpleHitSearch::processPedestalEvents;

    // initialisation
    // calculate pedestals for the first time (hits are 
    // the maximum signal in n-events (adjustable by parameter)
    // rejected by ignoreing
    _stateFunc[kStateInitialPedestals]=&SimpleHitSearch::processFirstPedestalEvents;

    // initialisation
    // calculate pedestals. Hits are rejected by using prior 
    // knowledge of the pedestals and the noise
    // the calibratin chip is on. For the time beeing treat the events
    _stateFunc[kStateRefinePedestals]=&SimpleHitSearch::processPedestalEventsWithHitRejection; 

    // calibration chip is on:
    // like beam events (signals at the same place in all events.)
    _stateFunc[kStateCalibration]=&SimpleHitSearch::processBeamEvents; 

    // ----- pedestal, adc, noise cuts

    _adcRange.clear();
    _adcRange.push_back(-(1<<16));
    _adcRange.push_back( (1<<16-1));
    
    registerProcessorParameter( "ADCRange" , 
			       "The allowed ADC range"  ,
			       _adcRange ,
			       _adcRange ,
			       _adcRange.size() ) ;

    _pedestalRange.clear();
    _pedestalRange.push_back(_adcRange[0]+30);
    _pedestalRange.push_back(_adcRange[1]-1000);
    
    registerProcessorParameter( "PedestalRange" , 
			       "The allowed pedestal range. Cells with pedestals outside this range are declared dead." ,
			       _pedestalRange ,
			       _pedestalRange ,
			       _pedestalRange.size() ) ;

    _pedestalChangeCut=FLT_MAX;
    registerProcessorParameter( "PedestalChangeCut" , 
			       "If the mean value of the last 5 values (ignoring hits) changes by more than this threshold, then it is assumed that the pedestal has changed.." ,
			       _pedestalChangeCut ,
			       _pedestalChangeCut);

    _maxPedestalChange=500;
    registerProcessorParameter( "MaxPedestalChange" , 
				"Maximum allowed pedestal change.",
			       _maxPedestalChange ,
			       _maxPedestalChange);

    
    _noiseRange.clear();
    _noiseRange.push_back(1.5);
    _noiseRange.push_back(30);
    
    registerProcessorParameter( "NoiseRange" , 
			       "The allowed noise range. Cells outside this range are declared dead." ,
			       _noiseRange ,
			       _noiseRange ,
			       _noiseRange.size() ) ;

    _maxHitOccupancy = .3;
    registerProcessorParameter( "MaxHitOccupancy" , 
			       "The maximum tolerated hit occupancy. Events with a larger hit occupancy are rejected." ,
			       _maxHitOccupancy ,
			       _maxHitOccupancy) ;

    _toleratedSaturationFraction = .3;
    registerProcessorParameter( "ToleratedSaturationFraction" , 
			       "The maximum fraction of events in which a cell was out of the allowed ADC range (beyond the cell is declared dead)." ,
			       _toleratedSaturationFraction ,
			       _toleratedSaturationFraction) ;
    _signalThreshold = .5;
    registerProcessorParameter( "SignalThreshold" , 
			       "Cells with a signal (after calibration) larger than this threshold are considered to have a hit. " ,
			       _signalThreshold ,
			       _signalThreshold ) ;

    _noiseCutVec.clear();
    _noiseCutVec.push_back( 5.);
    registerProcessorParameter( "NoiseCut" , 
			       "Cells with a signal-to-noise ratio larger than this value are considered to have a hit. The first element is used for hit search the second for hit rejection during noise calculation." ,
			       _noiseCutVec ,
			       _noiseCutVec) ;


    //    registerProcessorParameter( "NNegativeSignalsPerModule" , 
    //			       "The maximum number of allowed negative signals per detector module ( signal < - |noise* NoiseCut| )." ,
    //			       _nNegativeSignalsPerModule ,
    //			       _nNegativeSignalsPerModule ) ;

    registerProcessorParameter( "SkipFirstNEvents" , 
			       "Skip the first n events." ,
			       _skipFirstNEvents ,
			       20) ;

    registerProcessorParameter( "SkipCalibrationEvents" , 
			       "Skip events for which the calibration chip was switched on (Otherwise they are treated as beam events)" ,
			       _skipCalibrationEventsAlways ,
			       0) ;

    registerProcessorParameter( "SkipEventsAfterConfigurationChanges" , 
				"Skip the given number of events after trigger configuration changes." ,
				_skipNEventsAfterConfChanges ,
				2) ;


    registerProcessorParameter( "SkipEventsAfterSlowConfRecords" , 
				"Skip this numnber of events after DAQ slow readout and configuration records." ,
				_skipNEventsAfterSlowConfRecords ,
				0) ;

    {
      lcio::StringVec example;
      for (unsigned int error_i=0; error_i < CALICE::ErrorBits::kNErrorBits; error_i++ ) {
	example.push_back(CALICE::ErrorBits::getErrorName(error_i) );
      }

      registerOptionalParameter( "MaskedErrors" , 
				 "The name of all errors which should be ignored." ,
				 _maskedErrorNames ,
				 example) ;
      
    }

    // ------------- parameters for pedestal and noise calculation 

    _minNEventsForPedestalNoiseUpdate = 20;
    registerProcessorParameter( "MinNEventsForPedestalNoiseUpdate" , 
			       "If at least this numbers of pedestal/random triggers occurred then the pedestals and the noise are updated " ,
			       _minNEventsForPedestalNoiseUpdate ,
			       _minNEventsForPedestalNoiseUpdate) ;

    _updatePedestalsEveryNEvents = 500;
    registerProcessorParameter( "UpdatePedestalsEveryNEvents" ,
				"During pedestal triggers, update the pedestals every n events.  " ,
				_updatePedestalsEveryNEvents,
				_updatePedestalsEveryNEvents);


    _weightOfOldPedestalNoise = 20;
    registerProcessorParameter( "WeightOfOldPedestalNoise" , 
			       "When adjusting the pedestals and the noise with a new value, the old pedestal and noise gets this weight." ,
			       _weightOfOldPedestalNoise ,
			       _weightOfOldPedestalNoise) ;


    _shiftPedestalFactor = 0;
    registerProcessorParameter( "ShiftpedestalFactor" , 
			       "When processing pedestal events, the average value per Module is calculated and used to shift the old pedestals moderated by this factor." ,
			       _shiftPedestalFactor ,
			       _shiftPedestalFactor) ;


    _removeMaximumInNEvents=UINT_MAX;
    registerProcessorParameter( "RemoveMaximumInNEvents" , 
			       "During the initial pedestal and noise calculation the maximum of this number of events is removed to reject hits." ,
			       _removeMaximumInNEvents ,
			       _removeMaximumInNEvents) ;

    // ----------- collection names

    // ADC collection VRawValueProcessor

    _hitColName=COL_HITS;
    registerProcessorParameter( "HitCollectionName" , 
			       "The name of the calorimeter hit collection (output) to be used" ,
			       _hitColName ,
			       _hitColName);

    _rawhitColName=COL_RAWHITS;
    registerProcessorParameter( "RawHitCollectionName" , 
			       "The name of the Rawcalorimeter hit collection (output) to be used" ,
			       _rawhitColName ,
			       _rawhitColName);

    _writeRawHits = false;
    registerProcessorParameter( "WriteRawHits" ,
				"Save the output RawCalorimeterHit collection in the file" ,
				_writeRawHits ,
				_writeRawHits);

    _saveEcalNoise = true;
    registerProcessorParameter( "SaveEcalNoise" ,
				"Save the noise in the database" ,
				_saveEcalNoise ,
				_saveEcalNoise);

    registerProcessorParameter("DBInit",
				"DB Init string needed to write Noise into db",
			       _dbInit,
			       std::string("localhost:calvin:hobbes:calicedb:password:3306"));

    registerProcessorParameter("EcalNoiseFolder",
				"Folder to which the Ecal noise is stored",
			       _folderNoise,
			       std::string("/cd_calice/Ecal/Noise"));

    _cellParameterCollectionName="EmcCellParameters";
    registerProcessorParameter( "CellParameterCollectionName" , 
			       "The name of the collection which contains the pedestals, the noise, etc. of all the cells." ,
			       _cellParameterCollectionName ,
			       _cellParameterCollectionName);

    // ----------- collection names of conditions data

    // --- geoemtry 
    //VRawValueProcessor

    // --- detector position
    registerOptionalParameter( "DetectorTransformationCollectionName" , 
    				"Name of the conditions data collection which describes the detector position and rotation (folder e.g. /Calice/ECAL/Transformation)"  ,
				_colNameDetectorTransformation ,
    				std::string("DetectorTransformation") ) ;

    registerOptionalParameter( "ReferenceTransformationCollectionName" , 
    				"Name of the conditions data collection which describes the position and rotation of the reference coordinate system (folder e.g. /Calice/ECAL/Transformation)"  ,
				_colNameReferenceTransformation ,
    				std::string("ReferenceTransformation") ) ;
    _colNameReferenceTransformation="";
    _colNameDetectorTransformation="";

    // ---- collection name of calibration constants and name of calibration object
    registerProcessorParameter( "Calibration" , 
    				"Name of object to be used for the calibration"  ,
				_calibrationObjectName ,
    				std::string("NoOpCalibration") ) ;

    registerOptionalParameter( "CalibrationConstants" , 
			       "Name of the conditions data collection which contains the calibration constants"  ,
			       _calibrationConstantColName ,
			       std::string("") );

    // ---- trigger configuration
    registerProcessorParameter( "TriggerConfigurationName" , 
    				"Name of the event parameter name which contains the trigger configuration bits."  ,
				_parNameTriggerConf ,
    				std::string(PAR_TRIGGER_CONF) ) ;

    registerProcessorParameter( "TriggerEventName" , 
    				"Name of the event parameter name which contains the current trigger main word ."  ,
				_parNameTriggerEvent ,
    				std::string(PAR_TRIGGER_EVENT) ) ;

//     registerProcessorParameter( "TriggerPreHistoryName" , 
//     				"Name of the parameter which will contain the trigger pre history: will create parameters: [name]Pos, [name]Bits ."  ,
// 				_parNameTriggerPreHistory ,
//     				std::string(PAR_TRIGGER_PRE_HISTORY) ) ;

//     registerProcessorParameter( "TriggerPostHistoryName" , 
//     				"Name of the parameter which will contain the trigger psat history: will create parameters: [name]Pos, [name]Bits ."  ,
// 				_parNameTriggerPostHistory ,
//     				std::string(PAR_TRIGGER_POST_HISTORY) ) ;



    // ---- other parameters
    

    registerProcessorParameter( "StateParameterName" , 
    				"Name of the event parameter name which will be filled with the reconstruction state."  ,
				_parNameRecoState ,
    				std::string(PAR_RECO_STATE) ) ;

    registerOptionalParameter( "EnergyParameterName" , 
			       "Parameter name for the total energy"  ,
			       _parNameEventEnergy ,
			       std::string("EventEnergy") ) ;
    _parNameEventEnergy="";


    registerOptionalParameter( "ModulePedestalCorrectionParameterName" , 
			       "Parameter name of the per module pedestal correction."  ,
			       _parNamePedestalCorrection ,
			       std::string("ModulePedestalCorrection") ) ;
    _parNamePedestalCorrection="";

    // ---------- view the connection tree
    registerProcessorParameter( "ViewMapping" , 
    				"View the mapping between channels and modules when ever the module location or module connection conditions data cahange (set to 0 or !=0)"  ,
				_viewConnectionTree ,
    				0 ) ;


#ifdef WITH_CONTROL_HISTOGRAMS    

    registerProcessorParameter( "HistogramGroupName" , 
    				"The name of the histogram group under which the control histograms will be registered."  ,
				_histGroupKey.nameStorage() ,
    				_histGroupKey.name() );


    // ------------ parameters of some control histograms
    _signalHistPar.clear();
    _signalHistPar.push_back(200);
    _signalHistPar.push_back(0);
    _signalHistPar.push_back(1000);
    
    registerOptionalParameter( "SignalBinning" , 
			       "The binning for the control histograms of the signal distribution." ,
			       _signalHistPar ,
			       _signalHistPar ,
			       _signalHistPar.size() ) ;

    _totalSignalHistPar.clear();
    _totalSignalHistPar.push_back(1000);
    _totalSignalHistPar.push_back(0);
    _totalSignalHistPar.push_back(10000);
    
    registerProcessorParameter( "TotalSignalBinning" , 
				"The binning of the total signal." ,
				_totalSignalHistPar ,
				_totalSignalHistPar ,
				_totalSignalHistPar.size() ) ;


    _noiseHistPar.clear();
    _noiseHistPar.push_back(200);
    _noiseHistPar.push_back(-100);
    _noiseHistPar.push_back(100);
    
    registerOptionalParameter( "NoiseBinning" , 
			       "The binning for the control histograms of the noise distribution." ,
			       _noiseHistPar ,
			       _noiseHistPar ,
			       _noiseHistPar.size() ) ;

    _adcHistPar.clear();
    _adcHistPar.push_back(500);
    _adcHistPar.push_back(-1000);
    _adcHistPar.push_back(1000);
    
    registerOptionalParameter( "AdcBinning" , 
			       "The binning for the control histograms of the distribution of the adc values." ,
			       _adcHistPar ,
			       _adcHistPar ,
			       _adcHistPar.size() ) ;

    _pedestalChangeHistPar.clear();
    _pedestalChangeHistPar.push_back(1000);
    _pedestalChangeHistPar.push_back(-500);
    _pedestalChangeHistPar.push_back(500);
    
    registerOptionalParameter( "PedestalChangeBinning" , 
			       "The binning for the control histograms of the distribution of the pedestalChange values." ,
			       _pedestalChangeHistPar ,
			       _pedestalChangeHistPar ,
			       _pedestalChangeHistPar.size() ) ;

    //    for (UInt_t hist_i=0; hist_i<kNH1; hist_i++) {
    //      _histCol[hist_i]=0;
    //    }

    //    for (UInt_t hist_i=0; hist_i<kNH1Col2D; hist_i++) {
    //      _histCol2D[hist_i]=0;
    //    }
#endif
  }
    
  SimpleHitSearch::~SimpleHitSearch() {
    delete _calibration;
    delete _cellParameterCollection;
  }

  void SimpleHitSearch::init()
  {
    std::stringstream message;
    bool error=false;
    try {
      VRawADCValueProcessor::init();
    }
    catch (ErrorMissingConditionsDataHandler &conddata_error) {
      // --- catch conditions data handler registration errors
      //   ... and build a combined error message.
      std::string a(conddata_error.what());
      error=true;
      if (a.size()>0) {
	a.erase(a.size()-1);
	message << a;
      }
    }

    // --- register conditions data handler
    
    if (!error) {
      message << "SimpleHitSearch::init> no conditions data handler for the collections:";
    }
    
    if (!_colNameDetectorTransformation.empty()) {
      if (!marlin::ConditionsProcessor::registerChangeListener( &_detectorTransformationChange ,_colNameDetectorTransformation) ) {
	message << " " << _colNameDetectorTransformation;
	error=true;
      }
    }

    if (!_colNameReferenceTransformation.empty()) {
      if (!marlin::ConditionsProcessor::registerChangeListener( &_referenceTransformationChange ,_colNameReferenceTransformation) ) {
	message << " " << _colNameReferenceTransformation;
	error=true;
      }
    }
    
    if (error) { 
      // also throw an exception if the registration of conditions data handlers failed in VRawADCValueProcessor::init
      message <<  ".";
      throw ErrorMissingConditionsDataHandler(message.str());
    }

    _mapping.setViewConnectionTree(_viewConnectionTree!=0);

    _lastEvent=0;

    // --- initialise the analysis
    if (_noiseCutVec.size()<1) {
      _noiseCut=5;
    }
    else {
      _noiseCut=_noiseCutVec[0];
    }
    if (_noiseCutVec.size()>1) {
      _noiseCutForPedestalNoiseCalculation=_noiseCutVec[1];
    }
    else {
      _noiseCutForPedestalNoiseCalculation=5;
    }

    _cellParameter.clear();
    _pedestalNoiseIsKnown=kFALSE;

    _skipNEvents=_skipFirstNEvents;
    changeState(kStateSkipEvent);

    if (_skipCalibrationEventsAlways) {
      _stateFunc[kStateCalibration]=&SimpleHitSearch::skipCalibrationEvents;
    }

    // TODO:check parameters
    fullReset();

    printParameters();

    std::cout << "event energy = " << _parNameEventEnergy << std::endl;

    std::cout << "NoiseCut = " << _noiseCut << std::endl;
    std::cout << "NoiseCutForPedestalNoiseCalculation = " << _noiseCutForPedestalNoiseCalculation << std::endl;
    std::cout << std::endl;

#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
    std::cout << "Doing the signal induced pedestal corrections." << std::endl;
#endif
    assert(_updatePedestalsEveryNEvents > _minNEventsForPedestalNoiseUpdate);

    _rejectedEvents=0;
    _rejectBecauseOfError=0;
    _rejectBecauseTooManyHits=0;
    _rejectBecauseOfMissingADCBlocks=0;

    _uncoveredTriggerCounter=0;
    _nEventsWithoutADC=0;
    _nEventsWithMissingAdcBlocks=0;
    _nEventsTotal=0;
    _nEventsWithHits=0;
    _rejectedEvents=0;
    _nPedestalEvents=0;
    _nEventsSkipped=0;

    _skipDirtyEventsInPedestalCalculation=0;

    // build error mask
    _errorMask = ~CALICE::ErrorBits::getErrorBitMask(_maskedErrorNames);
    

#ifdef WITH_CONTROL_HISTOGRAMS
    // --- verify correctness of histogram parameters
    if (_signalHistPar.size()!=3) throw std::runtime_error("SimpleHitSearch::init> Parameter SignalBinning should contain exactly 3 values.");
    if (_noiseHistPar.size()!=3) throw std::runtime_error("SimpleHitSearch::init> Parameter NoiseBinning should contain exactly 3 values.");
    if (_adcHistPar.size()!=3) throw std::runtime_error("SimpleHitSearch::init> Parameter AdcBinning should contain exactly 3 values.");

    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histogramList->createHistogramGroup(_histGroupKey);
    histogramList->lockGroup(_histGroupKey);
    // create dummy histogram collections otherwise the Output processor will not get informed about the 
    // existence of histogram groups (Better solution?)
    createHistograms(0);

#endif

    // --- Calibration object
    delete _calibration;
    CalibrationFactory::getInstance()->listKits();
    _calibration=CalibrationFactory::getInstance()->createCalibrationObject(_calibrationObjectName, _colNameModuleDescription, _calibrationConstantColName);

    if (!_calibration) {
      std::stringstream message;
      message << "SimpleHitSearch::init> Failed to create calibration object \"" << _calibrationObjectName << "\".";
      throw std::runtime_error(message.str());
    }

  }


  void SimpleHitSearch::processRunHeader( LCRunHeader* run) {
    //name();
    std::cout << "SimpleHitSearch::processRun()  " << name()
	 << " in run " << run->getRunNumber()
	 << std::endl ;

    _runInfo.get(run);
    _runnum = run->getRunNumber();

    std::ostringstream rootname;
    rootname << "histos/Noise_Run" << _runnum << ".root";
    _hfile = TFile::Open(rootname.str().c_str(),"RECREATE");
    if (!_hfile) std::cout << "Warning, couldn't open file " <<  rootname.str() << "to save the noise histograms for MC reco." << std::endl;
    //noise histograms, will help filling the database for the MC reco
    //std::ostringstream sLabel1D[30][9][36][2];
    //std::ostringstream sTitle1D[30][9][36][2];
    for (unsigned int layer = 0; layer < 30; layer++){//loop on PCBs
      for (unsigned int waf = 0; waf < 9; waf++){//loop on wafers
	for (unsigned int cell1 = 0; cell1 < 36; cell1++){//loop on cells
	  _isDead[layer][waf][cell1] = 3;//default=disconnected
	  //sLabel1D[layer][waf][cell1][0]<< "p_Ped_l"<<layer<<"_waf"<<waf<<"_cell"<<cell1 ;
	  //sTitle1D[layer][waf][cell1][0]<< "ADC counts, for cell " << cell1 << "; ADC counts " ;
	  //p_Ped[layer][waf][cell1] = new TH1F(sLabel1D[layer][waf][cell1][0].str().c_str(),sTitle1D[layer][waf][cell1][0].str().c_str(),500,-100,20);
	  std::stringstream label;
	  label << "p_Ped_l"<<layer<<"_waf"<<waf<<"_cell"<<cell1 ;
	  std::stringstream title;
	  title << "ADC counts, for cell " << cell1 << "; ADC counts " ;
	  p_Ped[layer][waf][cell1] = new TH1F(label.str().c_str(),title.str().c_str(),500,-100,20);

	}
      }
      }


  }
  /*void SimpleHitSearch::processRunHeader( LCRunHeader* run) {

    //name();
    std::cout << "SimpleHitSearch::processRun()  " << name()
	 << " in run " << run->getRunNumber()
	 << std::endl ;

    //CAMM: not needed, RunInfoProcessor will do that
    //LCParameters *param = &(run->parameters());
    //param->setValue("isMCflag",0);

    _runInfo.get(run);
    _runnum = run->getRunNumber();
    std::ostringstream rootname;
    rootname << "histos/Noise_Run" << _runnum << ".root";
    _hfile = TFile::Open(rootname.str().c_str(),"RECREATE");
    if (!_hfile) std::cout << "Warning, couldn't open file " <<  rootname.str() << "to save the noise histograms for MC reco." << std::endl;

    //noise histograms, will help filling the database for the MC reco
    std::ostringstream sLabel1D[30][9][36][2],sTitle1D[30][9][36][2];
    for (unsigned int layer = 0; layer < 30; layer++){//loop on PCBs
      for (unsigned int waf = 0; waf < 9; waf++){//loop on wafers
	for (unsigned int cell1 = 0; cell1 < 36; cell1++){//loop on cells
	  _isDead[layer][waf][cell1] = 3;//default=disconnected
	  sLabel1D[layer][waf][cell1][0]<< "p_Ped_l"<<layer<<"_waf"<<waf<<"_cell"<<cell1 ;
	  sTitle1D[layer][waf][cell1][0]<< "ADC counts, for cell " << cell1 << "; ADC counts " ;
	  p_Ped[layer][waf][cell1] = new TH1F(sLabel1D[layer][waf][cell1][0].str().c_str(),sTitle1D[layer][waf][cell1][0].str().c_str(),500,-100,20);
	}
      }
    }

  }*/


#ifdef WITH_CONTROL_HISTOGRAMS
  void SimpleHitSearch::createHistograms(UInt_t n_modules) 
  {
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
 
    Bool_t need_to_recreate_histograms=false;

    if (histogramList->getNEntriesTotal(_histGroupKey)>0) {
      // just check for one histogram whether the number of layers have changed. If that is the case
      // recreate the histograms.

      if (histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1Signal]).n() != n_modules) {
	  need_to_recreate_histograms=true;
      }

#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
      const histmgr::Histogram2DCollection_t an_hist_col=histogramList->getHistogram2DCollection(_histGroupKey,_histSignalInducedPedestalShiftKey);
      if (!an_hist_col.is2D()) {
	need_to_recreate_histograms=true;
      }
      else if (an_hist_col.nMajor()!=n_modules) { 
	need_to_recreate_histograms=true;
      }
      else {
	for  (UInt_t major_i=0; major_i<n_modules; major_i++) 
	  if (an_hist_col.nMinor(major_i++)!=5) {
	    need_to_recreate_histograms=true;
	  }
      }
#endif


#ifdef CORRELATED_NOISE
      const histmgr::Histogram2DCollection_t a_hist_col=histogramList->getHistogram2DCollection(_histGroupKey,_histNoiseCorrelationKey);
      if (!a_hist_col.is2D()) {
	need_to_recreate_histograms=true;
      }
      else if (a_hist_col.nMajor()!=n_modules) { 
	need_to_recreate_histograms=true;
      }
      else {
	for  (UInt_t major_i=0; major_i<n_modules; major_i++) 
	  if (a_hist_col.nMinor(major_i++)!=5) {
	    need_to_recreate_histograms=true;
	  }
      }
#endif
      
      if (need_to_recreate_histograms) {
	histogramList->writeHistograms(_histGroupKey,true);
      }
    }
    else {
      need_to_recreate_histograms=true;
    }

    // (re)create the histograms 
    if (need_to_recreate_histograms) {

      // reset for debugging
      //      for (UInt_t i=0; i<kNH1; i++) {
      //	_histCol[i]=0;
      //      }

      histogramList->createHistograms(_histGroupKey, (_histKey[kH1Signal]=histmgr::Key_t("SignalPerLayer")),n_modules,
				      HistPar((UInt_t) _signalHistPar[0], _signalHistPar[1], _signalHistPar[2]),true);

      histogramList->createHistograms(_histGroupKey,(_histKey[kH1TotalSignal]=histmgr::Key_t("TotalSignal")),1,
							  HistPar((UInt_t) _totalSignalHistPar[0], _totalSignalHistPar[1], _totalSignalHistPar[2]),true);

      //      _histCol[kH1Noise]=histogramList->createHistograms(_histGroupKey,histmgr::Key_t("Noise"),n_modules,
      //							 HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]),true);

    histogramList->createHistograms(_histGroupKey,_histKey[kH1CalcNoise]=histmgr::Key_t("CalculatedNoise"),n_modules,
				    HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]),true);

#ifdef WITH_ALL_CONTROL_HISTOGRAMS
    histogramList->createHistograms(_histGroupKey,_histKey[kH1FinalNoise]=histmgr::Key_t("FinalNoise"),n_modules,
				    HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]),true);
#endif

#ifdef WITH_ALL_CONTROL_HISTOGRAMS
    histogramList->createHistograms(_histGroupKey,(_histKey[kH1ADC]=histmgr::Key_t("ADCValues")),n_modules,
						     HistPar((UInt_t) _adcHistPar[0], _adcHistPar[1], _adcHistPar[2]),true);
#endif
    
#ifdef WITH_ALL_CONTROL_HISTOGRAMS
    histogramList->createHistograms(_histGroupKey,_histKey[kH1Pedestal]=histmgr::Key_t("Pedestal"),n_modules,
							  HistPar((UInt_t) _adcHistPar[0], _adcHistPar[1], _adcHistPar[2]),true);

    histogramList->createHistograms(_histGroupKey,_histKey[kH1FinalPedestal]=histmgr::Key_t("FinalPedestal"),n_modules,
				    HistPar((UInt_t) _adcHistPar[0], _adcHistPar[1], _adcHistPar[2]),true);
#endif

    histogramList->createHistograms(_histGroupKey,_histKey[kH1CalcPedestal]=histmgr::Key_t("CalculatedPedestal"),n_modules,
							      HistPar((UInt_t) _adcHistPar[0], _adcHistPar[1], _adcHistPar[2]),true);
    
    histogramList->createHistograms(_histGroupKey, _histKey[kH1PedestalChange]=histmgr::Key_t("PedestalChanges"), n_modules,
				    HistPar((UInt_t) _pedestalChangeHistPar[0], _pedestalChangeHistPar[1], _pedestalChangeHistPar[2]),true);

    histogramList->createHistograms(_histGroupKey,_histKey[kH1PedestalCorrection]=histmgr::Key_t("PedestalCorrection"),n_modules,
				    HistPar((UInt_t) _pedestalChangeHistPar[0], _pedestalChangeHistPar[1], _pedestalChangeHistPar[2]),true);

    histogramList->createHistograms(_histGroupKey, _histKey[kH1NoiseChange]=histmgr::Key_t("NoiseChanges"), n_modules,
				    HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]),true);
    
#ifdef WITH_ALL_CONTROL_HISTOGRAMS
    histogramList->createHistograms(_histGroupKey, _histKey[kH1NoiseAverage]=histmgr::Key_t("AverageNoise"), n_modules,
				    HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]),true);

    histogramList->createHistograms(_histGroupKey,_histKey[kH1SignalAverage]=histmgr::Key_t("AverageSignal"),n_modules,
				    HistPar((UInt_t) _pedestalChangeHistPar[0], _pedestalChangeHistPar[1], _pedestalChangeHistPar[2]),true);
#endif

    histogramList->createHistograms(_histGroupKey,_histKey[kH1DeadCells]=histmgr::Key_t("DeadCells"),n_modules,
							   HistPar(50,-.5,50-.5),true);

    histogramList->createHistograms(_histGroupKey, (_histKey[kH1NHits]=histmgr::Key_t("NHits")), n_modules,
				    HistPar(80,-.5,80-.5),true);
    
    histogramList->createHistograms(_histGroupKey,_histKey[kH1States]=histmgr::Key_t("States"),2,
							HistPar(kNStates+1,-.5,kNStates+.5),true);

    histogramList->createHistograms(_histGroupKey,_histKey[kH1Errors]=histmgr::Key_t("Errors"),1,
							HistPar(33,-.5,32.5),true);

    histogramList->createHistograms(_histGroupKey,_histKey[kH1NoGlobalPedCorr]=histmgr::Key_t("ADCValueBeforeGlobalPedCorrection") ,216,
    			    HistPar(650,-250,400),true);

    histogramList->createHistograms(_histGroupKey,_histKey[kH1NoPedSh]=histmgr::Key_t("ADCValueAfterGlobalPedCorrection") ,216,
    			    HistPar(650,-250,400),true);

    histogramList->createHistograms(_histGroupKey,_histKey[kH1PedSh]=histmgr::Key_t("ADCValueAfterPedShCorr") ,216,
    			    HistPar(650,-250,400),true);


    // verify code sanity: Are all the histograms created?
    assert(kNH1==23);
    
    lcio::IntVec index;
    lcio::IntVec chip_index;
#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
    lcio::IntVec wafer_index;
    lcio::IntVec pixel_index;
#endif      
    for (UInt_t module_i=0; module_i<n_modules; module_i++) {
      index.push_back(5);
      chip_index.push_back(12);
#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
      wafer_index.push_back(6);
      pixel_index.push_back(216);
#endif      
    }

#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT

    histogramList->create2DHistograms(_histGroupKey,_histSignalInducedPedestalShiftKey,chip_index,
				      HistPar(30,100,30000),
				      HistPar(150,-149.5,149.5),true);

#endif


#ifdef CORRELATED_NOISE
    histogramList->create2DHistograms(_histGroupKey,_histNoiseCorrelationKey,chip_index,
				      HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]),
				      HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]),true);
#endif

    if (_mapping.getNModules()>0) {
      lcio::IntVec cell_arr;
      lcio::StringVec noise_name;
      lcio::StringVec signal_name;
      if (_mapping.isModuleConditionsDataComplete()) {
#ifdef RECO_DEBUG
	std::cout << "SimpleHitSearch Number of defined Modules: "  << _mapping.getNModules() << std::endl;
#endif
	for (UInt_t module_i=0; module_i<_mapping.getNModules(); module_i++) {
#ifdef RECO_DEBUG
	  std::cout << "Cells in module " << module_i << ": " << _mapping.getNCellsPerModule(module_i) << std::endl;    
#endif  
	  cell_arr.push_back(_mapping.getNCellsPerModule(module_i));
	  std::stringstream a_name_ext;
#ifdef RECO_DEBUG
	  std::cout << "Module Number: " << module_i << std::endl; 
	  std::cout << "Module Name: " << _mapping.getModuleName(module_i) << std::endl;
	  std::cout << "Cell Index: " << std::hex << _mapping.getGeometricalCellIndex(module_i,0) << std::dec << std::endl;
#endif
	  CellIndex cell_index(_mapping.getGeometricalCellIndex(module_i,0));
#ifdef RECO_DEBUG
	  std::cout << "Layer Index: " << cell_index.getLayerIndex() << std::endl;
	  a_name_ext << "_" << _mapping.getModuleName(module_i) << "_layer_" << cell_index.getLayerIndex();
#endif


	  
	  noise_name.push_back(std::string("Noise")+a_name_ext.str());
	  signal_name.push_back(std::string("Signal")+a_name_ext.str());
	  
	}
      }
      histogramList->createHistograms(_histGroupKey,_histSignalKey,cell_arr,
				      HistPar((UInt_t) _signalHistPar[0], _signalHistPar[1], _signalHistPar[2]),true);
      histogramList->createHistograms(_histGroupKey,_histNoiseKey,noise_name, cell_arr,
				      HistPar((UInt_t) _noiseHistPar[0], _noiseHistPar[1], _noiseHistPar[2]),true);
    }
    
    // verify code sanity: Are all the histograms created?
    assert(kNH1Col2D==3);

    histogramList->createHistograms(_histGroupKey,_histTriggerEnableKey,1,HistPar(33,-1-.5,32-.5),true);
    }
  }




  void SimpleHitSearch::fillErrorHistogram(unsigned int error_bits) {
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histmgr::HistogramCollection_t & error_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1Errors]);
    histmgr::FloatHistogram1D *error_hist=error_hist_arr.histogram(0);
    unsigned int mask=1;
    for (UInt_t bit_i=0; bit_i<32; bit_i++) {
      if (error_bits & mask) {
	error_hist->fill(bit_i);
      }
      mask<<=1;
    }
  }

#endif


  void SimpleHitSearch::skipEvents(LCEvent *evtP) {
    if (_skipNEvents--==0) {
      if (!_pedestalNoiseIsKnown && (!_triggerConf.isPurePedestalTrigger())) {
	// skip events until the first pedestal trigger arrives
	_skipNEvents++;
      }
      else if (stateTransition()) {
	callStateFunc(evtP);
	return; 
      }
    }
    _nEventsSkipped++;
  }

  void SimpleHitSearch::skipCalibrationEvents(LCEvent *evtP) {
    if (!_triggerConf.isPureCalibTrigger()) {
      if (stateTransition()) {
	callStateFunc(evtP);
	return; 
      }
    }
    _nEventsSkipped++;
  }


  void SimpleHitSearch::processCosmics(LCEvent *evtP) {
    if (!_triggerConf.isPureCosmicsTrigger()) {
      if (stateTransition()) {
	callStateFunc(evtP);
	return;
      }
    }
    if (!_pedestalNoiseIsKnown) {
      throw std::runtime_error("SimpleHitSearch::processCosmics> Pedestal and noise is not yet known. Not ready to process cosmics events.");
    }
    searchHitsAndAdjustPedestalsAndNoise(evtP);
  }

  void SimpleHitSearch::processBeamEvents(LCEvent *evtP) {
    if (!_triggerConf.isBeamTrigger()) {
      if (stateTransition()) {
	callStateFunc(evtP);
	return;
      }
    }
    if (!_pedestalNoiseIsKnown) {
      throw std::runtime_error("SimpleHitSearch::processbeamEvents> Pedestal and noise is not yet known. Not ready to process beam events.");
    }
    if (_triggerEvent.isPurePedestalTrigger()) {
      accumulateEventsForPedestalNoiseCalculationWithHitRejection(evtP);
    }
    else {
      searchHits(evtP);
    }
  }

  void SimpleHitSearch::processPedestalEvents(LCEvent *evtP) {
    // --- Pedestal noise calculation with hit rejection
    //if ( _nPedestalEvents==0) {
    // resetForPedestalNoiseReCalculation();
    //}

    if ( _nPedestalEvents > static_cast<UInt_t>(_updatePedestalsEveryNEvents) ) {
      calculatePedestalNoiseAndDeclareCellsDead();
      resetForPedestalNoiseReCalculation();
      _nPedestalEventSets++;
    }

    CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
    if (!error.dirtyEvent() && (_triggerEvent.isPurePedestalTrigger() || _triggerEvent.isPureCosmicsTrigger())) {
      accumulateEventsForPedestalNoiseCalculationWithHitRejection(evtP);
    }
    else {
      error.setDirtyEvent();
      evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
      _skipDirtyEventsInPedestalCalculation++;
    }
  }

  void SimpleHitSearch::processFirstPedestalEvents(LCEvent *evtP) {
    if (_nPedestalEvents>(UInt_t) _minNEventsForPedestalNoiseUpdate) {
      if (stateTransition()) {
	callStateFunc(evtP);
	return;
      }
    }

    // --- Initial pedestal noise calculation
    //if ( _nPedestalEvents==0) {
    //  resetForInitialPedestalNoiseCalculation();
    //}
    CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
    error.print(std::cout);
    if (!error.dirtyEvent() && (_triggerEvent.isPurePedestalTrigger() || _triggerEvent.isCosmicsTrigger())) {
      accumulateEventsForInitialPedestalNoiseCalculation(evtP);
    }
    else {
      error.setDirtyEvent();
      evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
      _skipDirtyEventsInPedestalCalculation++;
    }

  }

  void SimpleHitSearch::processPedestalEventsWithHitRejection(LCEvent *evtP) {
    if (_nPedestalEvents>(UInt_t) _minNEventsForPedestalNoiseUpdate) {
      if (stateTransition()) {
	callStateFunc(evtP);
	return;
      }
    }

    // --- Initial pedestal noise calculation
    //if ( _nPedestalEvents==0) {
    //  resetForPedestalNoiseReCalculation();
    //}
    CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
    if (!error.dirtyEvent() && (_triggerEvent.isPurePedestalTrigger() || _triggerEvent.isPureCosmicsTrigger())) {
      accumulateEventsForPedestalNoiseCalculationWithHitRejection(evtP);
    }
    else {
      error.setDirtyEvent();
      evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
      _skipDirtyEventsInPedestalCalculation++;
    }
  }

  const char *__state_names[]={"Unknown","skip","cosmics","beam","pedestal","initial pedestal","pedestal refinement","calibration"};

  bool SimpleHitSearch::stateTransition()
  {
    EState was_state = _isState;
    if (_skipNEvents<=0 || _isState != kStateSkipEvent) {
      // If the pedestals and the noise is not well known (states: kStateInitialPedestals, kStateRefinePedestals) 
      // it should always be calculated if the accumalted number of events is considered to be sufficient.
      if (was_state == kStateInitialPedestals || was_state == kStateRefinePedestals) {
	if (_nPedestalEvents>(UInt_t) _minNEventsForPedestalNoiseUpdate) {
	  UInt_t from_n_pedestal_events=_nPedestalEvents;  
	  calculatePedestalNoiseAndDeclareCellsDead();

#ifndef RECO_DEBUG
	  std::cout << " --- " << name() << " Notification :" << std::endl
                    <<  (was_state == kStateInitialPedestals ?
                         "  -  Initial pedestal and noise calculation from "
                         :"  -  Pedestal and noise refinement from ")
                    << from_n_pedestal_events << " events:" << std::endl;
          showNoiseStat();
          std::cout <<std::endl;
#endif	  
	  _nPedestalEventSets++;
	}
      }
    
      // TODO: keep "initial pedestal noise calculation" state if pedestal noise is not known?
      // Probably not.
      //    if (!_pedestalNoiseIsKnown && _isState==kStateInitialPedestal) {
      //      return;
      //    }

      if ( _triggerConf.isPureCalibTrigger() ) {
	// If the calibration chip is on but the pedestals are not known yet
	// we can only skip the events until the calibration chip goes off again.
	if (!_pedestalNoiseIsKnown) {
	  changeState(kStateSkipEvent);
	}
	else {
	  changeState(kStateCalibration);
	}
      }
      else if (_triggerConf.isBeamTrigger()) {
	  changeState(kStateBeam);
      }
      else if (_triggerConf.isPureCosmicsTrigger()) {

	// if the pedestals and the noise are not yet known
	// it has to be calculated first. 
	//
	// This is done in two phases:
	//
	//  1) Assume all signals are noise but remove the maximum signal in x-events
	//  2) Use the knowledge of the pedestals and the noise to reject signals
	
	if (_pedestalNoiseIsKnown) {
	  if (_isState==kStateInitialPedestals) {
	    // phase 2 of pedestals and noise estimation
	    changeState(kStateRefinePedestals);
	    resetForPedestalNoiseReCalculation();
	  }
	  else {
	    // if pedestals and noise are calculated
	    // then search for cosmics
	    changeState(kStateCosmics);
	  }
	}
	else {
	  // phase 1 of pedestals and noise estimation
	  changeState(kStateInitialPedestals);
	  resetForInitialPedestalNoiseCalculation();
	}

      }
      else if (_triggerConf.isPurePedestalTrigger()) {
	
	// calculate pedestals 
	// but cut out signal events. The latter requires knowldedge of the
	// pedestals and the noise which is gained in phase 1.
	// Then go over to phase 2
	// (also see above.)

	if (_pedestalNoiseIsKnown) {

	  // phase 2)
	  if (was_state != kStatePedestals){
	    resetForPedestalNoiseReCalculation();
	  }
	  changeState(kStatePedestals);
	}
	else {

	  // phase 1)
	  changeState(kStateInitialPedestals);
	  resetForInitialPedestalNoiseCalculation();
	}
      }
      else {
	changeState(kStateSkipEvent);
      }
    }

    if (was_state != _isState ) {
      // if the new state does not accumulation pedestal data and does not just skip events
      // then re calculate the pedestals and the noise if enough pedestal data was accumulated.
      if ( _isState != kStatePedestals && _isState != kStateSkipEvent ) {
	if (_nPedestalEvents>(UInt_t) _minNEventsForPedestalNoiseUpdate) {
	  UInt_t from_n_pedestal_events=_nPedestalEvents;
	  calculatePedestalNoiseAndDeclareCellsDead();
	  
	  if (_nPedestalEventSets<=3) {
#ifndef RECO_DEBUG
	    std::cout << " --- " << name() << " Notification :" << std::endl
		      <<  "  -  Pedestal and noise from " << from_n_pedestal_events << " events (set=" << _nPedestalEventSets << "):" << std::endl;
		      showNoiseStat();
	    std::cout <<std::endl;
#endif
	  }

	  _nPedestalEventSets++;
	}
      }
    }

#ifdef WITH_CONTROL_HISTOGRAMS
#ifdef BOUNDARY_CHECK
    assert( kNStates>0 );
#endif
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histmgr::HistogramCollection_t & state_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1States]);
    state_hist_arr.histogram(0)->fill(_isState);
#endif

    return _isState!=was_state;
  }

  void SimpleHitSearch::processEvent( LCEvent * evtP ) {

    TriggerBits old_trigger_conf=_triggerConf;
    _triggerConf = evtP->getParameters().getIntVal(_parNameTriggerConf);
#ifdef RECO_DEBUG
    std::cout << "SimpleHitSearch::ProcessEvent " << _triggerConf.getTriggerBits() << std::endl;
#endif

    _triggerEvent = evtP->getParameters().getIntVal(_parNameTriggerEvent);
    
    if (_isState != kStateSkipEvent) {
      // the last trigger is initialised with kUnknown which is a save state to be in 
      // at some point a transition from kUnknown to something more useful must occur.
      if( _triggerConf != old_trigger_conf ) {
	
#ifdef RECO_DEBUG
	std::cout << "SimpleHitSearch::processEvent - Trigger Conf Change Occured" << std::endl;
	std::cout << "Configured TriggerBits: "<< _triggerConf.getTriggerBits() << std::endl;
	std::cout << "Old TriggerBits: "<< old_trigger_conf.getTriggerBits() << std::endl;
#endif
	
	if (_skipNEvents < _skipNEventsAfterConfChanges) {
	  _skipNEvents = _skipNEventsAfterConfChanges;
	  changeState(SimpleHitSearch::kStateSkipEvent);
	}
	else {
	  stateTransition();
	}
	
      }
      
      // for debugging:
      _lastEvent=evtP->getEventNumber();
      
      // skip n events after slow readout and configuration changes
      UInt_t events_since_last_slow_conf_record=static_cast<UInt_t>(evtP->getParameters().getIntVal(PAR_EVENTS_SINCE_LAST_DAQ_STATE_CHANGE));
      if (events_since_last_slow_conf_record < static_cast<UInt_t>(_skipNEventsAfterSlowConfRecords)) {
	changeState(kStateSkipEvent);
	_skipNEvents=_skipNEventsAfterSlowConfRecords-events_since_last_slow_conf_record;
	std::cout << events_since_last_slow_conf_record << " events since last slow readout or configuration record" << std::endl;
      }

    }

    callStateFunc(evtP);

#ifdef WITH_CONTROL_HISTOGRAMS
#ifdef BOUNDARY_CHECK
    assert( kNStates>0 );
#endif
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histmgr::HistogramCollection_t & state_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1States]);
    state_hist_arr.histogram(1)->fill(_isState);
#endif

    // add the pedestals and noise to the event but keep ownership
    if (_cellParameterCollection) {
      evtP->addCollection(_cellParameterCollection, _cellParameterCollectionName);
      evtP->takeCollection( _cellParameterCollectionName );
    }

#ifdef WITH_CONTROL_HISTOGRAMS
    CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
    fillErrorHistogram(error.getBits());
#endif
    
    evtP->parameters().setValue(_parNameRecoState,static_cast<int>(_isState));
  }

  void SimpleHitSearch::fullReset()
  {
    // DEBUG std::cout << "*** SimpleHitSearch::fullReset ***" << _lastEvent<<std::endl;
    _nPedestalEventSets=0;
    _nEventsTotal=0;
    _nEventsWithHitsTotal=0;

    resetHitCounter();
    resetSaturationCounter();
    resetForInitialPedestalNoiseCalculation();
  }
  
  void SimpleHitSearch::resetForInitialPedestalNoiseCalculation()
  {
    // DEBUG std::cout << "*** SimpleHitSearch::resetForInitialPedestalNoiseCalculation ***" << _lastEvent<<std::endl;
    _nPedestalEventsTotal+=_nPedestalEvents;
    _nPedestalEvents=0;
    _eventCounterForMaximumRejection=_removeMaximumInNEvents;

    for(UInt_t module_i=0; module_i<_cellParameter.size(); module_i++) {
      for(UInt_t cell_i=0; cell_i<_cellParameter[module_i].size(); cell_i++) {
	_cellParameter[module_i][cell_i].resetAll();
	_cellParameter[module_i][cell_i].setMaxValue(INT_MIN);
	_cellParameter[module_i][cell_i].initADCMemory();//TODO 

	_cellParameter[module_i][cell_i].setDead(_mapping.isCellDead(module_i,cell_i) 
						 || !_calibration->isValid(_mapping.getModuleID(module_i),_mapping.getModuleType(module_i),cell_i));
      }
    }
  }
  
  void SimpleHitSearch::resetForPedestalNoiseReCalculation()
  {
    //DEBUG std::cout << "*** SimpleHitSearch::resetForPedestalNoiseReCalculation ***" << _lastEvent<<std::endl;
    _nPedestalEventsTotal+=_nPedestalEvents;
    _nPedestalEvents=0;
    for(UInt_t module_i=0; module_i<_cellParameter.size(); module_i++) {
      for(UInt_t cell_i=0; cell_i<_cellParameter[module_i].size(); cell_i++) {
	_cellParameter[module_i][cell_i].resetSums();
	_cellParameter[module_i][cell_i].initADCMemory();//TODO 

	_cellParameter[module_i][cell_i].setDead(_mapping.isCellDead(module_i,cell_i) 
						 || !_calibration->isValid(_mapping.getModuleID(module_i),_mapping.getModuleType(module_i),cell_i));
      }
    }
  }
  
  void SimpleHitSearch::resetHitCounter()
  {
    for(UInt_t module_i=0; module_i<_cellParameter.size(); module_i++) {
      for(UInt_t cell_i=0; cell_i<_cellParameter[module_i].size(); cell_i++) {
	_cellParameter[module_i][cell_i].resetHitCounter();
      }
    }
    _nEventsWithHitsTotal+=_nEventsWithHits;
    _nEventsWithHits=0;
  }
  
  void SimpleHitSearch::resetSaturationCounter()
  {
    for(UInt_t module_i=0; module_i<_cellParameter.size(); module_i++) {
      for(UInt_t cell_i=0; cell_i<_cellParameter[module_i].size(); cell_i++) {
	_cellParameter[module_i][cell_i].resetSaturationCounter();
      }
    }

    _nEventsTotal+=_nEvents;
    _nEvents=0;

  }

  void SimpleHitSearch::accumulateEventsForInitialPedestalNoiseCalculation(LCEvent *evtP)
  {

    try {
      LCCollection* col_adc = evtP->getCollection( _adcColName ) ;
    
      if (_cellParameter.size()==0) {
	buildCellParameters();
      }
      
      if (col_adc && col_adc->getNumberOfElements()>0) {

	CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
	UInt_t n_cells_found=0;

	--_eventCounterForMaximumRejection;

	AdcValueAccess adc_access(col_adc,&_mapping, _calibration, &_cellParameter);
	if (adc_access.hasConnectedBlocks()) {
	do {
	  do {
	    CellParameter &cell_parameter=adc_access.getParameter();
	    if (!cell_parameter.isDead()) {
	    
	      Int_t raw_adc_value=adc_access.getAdcValue();
	      if(raw_adc_value>=_adcRange[0] && raw_adc_value<=_adcRange[1]) {
	      
		cell_parameter.add(raw_adc_value);
		if (raw_adc_value>cell_parameter.getMaxValue()) {
		  cell_parameter.setMaxValue(raw_adc_value);
		}

		if (_eventCounterForMaximumRejection==0) {
		  cell_parameter.sub(cell_parameter.getMaxValue());
		  _eventCounterForMaximumRejection=_removeMaximumInNEvents;
		}

	      }
	      else {
		cell_parameter.incrementSaturationCounter();
	      }
	    }  

	    n_cells_found++;

	  } while (adc_access.nextValue());
	} while (adc_access.nextBlock());
	}

	if (n_cells_found != _mapping.getNConnectedCells()) {
	  error.setMissingADCBlock();
	  evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
	  _nEventsWithMissingAdcBlocks++;
	}

	if (_eventCounterForMaximumRejection==0) {
	  _eventCounterForMaximumRejection=_removeMaximumInNEvents;
	}
	_nPedestalEvents++;
	_nEvents++;

      }
    }
    catch (  DataNotAvailableException &err ) {
      _nEventsWithoutADC++;
    }
  }

  void SimpleHitSearch::accumulateEventsForPedestalNoiseCalculationWithHitRejection(LCEvent *evtP)
  {
    try {
      LCCollection* col_adc = evtP->getCollection( _adcColName ) ;
    
      if (_cellParameter.size()==0) {
	buildCellParameters();
      }
      
      if (col_adc && col_adc->getNumberOfElements()>0) {

	std::vector< AverageSimple_t > pedestal_change_per_module; 
	pedestal_change_per_module.resize(_mapping.getNModules());

	CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
	UInt_t n_cells_found=0;

	AdcValueAccess adc_access(col_adc,&_mapping, _calibration, &_cellParameter);
	if (adc_access.hasConnectedBlocks()) {
	do {
	  do {
	    CellParameter &cell_parameter=adc_access.getParameter();
	    if (!cell_parameter.isDead()) {
	    
	      Int_t raw_adc_value=adc_access.getAdcValue();
	      if(raw_adc_value>=_adcRange[0] && raw_adc_value<=_adcRange[1]) {
		
		Float_t adc_value=raw_adc_value-cell_parameter.getPedestal();
		Float_t noise=cell_parameter.getNoise();

		if (adc_value<_noiseCutForPedestalNoiseCalculation*noise) {
		  cell_parameter.add(raw_adc_value);
#ifdef BOUNDARY_CHECK
		  assert ( static_cast<unsigned int>(adc_access.getModuleIndex()) < pedestal_change_per_module.size());
#endif
		  pedestal_change_per_module[adc_access.getModuleIndex()].add(raw_adc_value-cell_parameter.getPedestal());
		}
	      }
	      else {
		cell_parameter.incrementSaturationCounter();
	      }
	    }  

	    n_cells_found++;

	  } while (adc_access.nextValue());
	} while (adc_access.nextBlock());
	}

	if (n_cells_found != _mapping.getNConnectedCells()) {
	  error.setMissingADCBlock();
	  evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
	  _nEventsWithMissingAdcBlocks++;
	}

	// shift pedestals by average value measured on the PCB
	if (_shiftPedestalFactor> 0) {
	  UInt_t module_i=0;
	  for (std::vector<AverageSimple_t>::iterator module_iter=pedestal_change_per_module.begin();
	       module_iter!=pedestal_change_per_module.end();
	       module_iter++, module_i++) {
	    
	    module_iter->calculate();
	    for (std::vector<CellParameter>::iterator cell_iter=_cellParameter[module_i].begin();
		 cell_iter!=_cellParameter[module_i].end();
		 cell_iter++) {
	      cell_iter->setPedestal(cell_iter->getPedestal()+_shiftPedestalFactor * module_iter->mean());
	    }
		 
	  }
	}
	_nPedestalEvents++;
	_nEvents++;
      }
    }
    catch (  DataNotAvailableException &err ) {
      _nEventsWithoutADC++;
    }
  }

  void SimpleHitSearch::accumulateEventsForPedestalNoiseCalculationWithHitRejectionAndPedestalShiftDetection(LCEvent *evtP)
  {
    try {
      LCCollection* col_adc = evtP->getCollection( _adcColName ) ;
    
      if (_cellParameter.size()==0) {
	buildCellParameters();
      }
      
      if (col_adc && col_adc->getNumberOfElements()>0) {

	CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
	UInt_t n_cells_found=0;

	AdcValueAccess adc_access(col_adc,&_mapping, _calibration, &_cellParameter);
	if (adc_access.hasConnectedBlocks()) {
	do {
	  do {
	    CellParameter &cell_parameter=adc_access.getParameter();
	    if (!cell_parameter.isDead()) {
	    
	      Int_t raw_adc_value=adc_access.getAdcValue();
	      if(raw_adc_value>=_adcRange[0] && raw_adc_value<=_adcRange[1]) {
		
		Float_t adc_value=raw_adc_value-cell_parameter.getPedestal();
		Float_t noise=cell_parameter.getNoise();

		Float_t pedestal_change=cell_parameter.getMeanADCValue(adc_value);

		if (fabs(pedestal_change)>noise*_pedestalChangeCut) {
		  cell_parameter.shiftPedestal(pedestal_change);
		  Float_t pedestal=cell_parameter.getPedestal();
		  adc_value=raw_adc_value-pedestal;
		}

		if (adc_value<_noiseCutForPedestalNoiseCalculation*noise) {
		  cell_parameter.add(raw_adc_value);
		}
	      }
	      else {
		cell_parameter.incrementSaturationCounter();
	      }
	    }

	    n_cells_found++;

	  } while (adc_access.nextValue());
	} while (adc_access.nextBlock());
	}

	if (n_cells_found != _mapping.getNConnectedCells()) {
	  error.setMissingADCBlock();
	  evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
	  _nEventsWithMissingAdcBlocks++;
	}

	_nPedestalEvents++;
	_nEvents++;

      }
    }
    catch (  DataNotAvailableException &err ) {
      _nEventsWithoutADC++;
    }
  }

  void SimpleHitSearch::calculatePedestalNoiseAndDeclareCellsDead()
  {
    //DEBUG std::cout << "*** SimpleHitSearch:: calculatePedestalNoiseAndDeclareCellsDead ***" << _lastEvent<<std::endl;
    UInt_t tolerated_saturation=(UInt_t) (_nEvents*_toleratedSaturationFraction);
    if (tolerated_saturation<1) tolerated_saturation=1;

    if (_avNoisePerModule.size() != _cellParameter.size()) {
      _avNoisePerModule.resize(_cellParameter.size());
    }

#ifdef WITH_CONTROL_HISTOGRAMS
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histmgr::HistogramCollection_t & pedestal_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1CalcPedestal]);
    histmgr::HistogramCollection_t & noise_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1CalcNoise]);

    histmgr::HistogramCollection_t & pedestal_change_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1PedestalChange]);
    histmgr::HistogramCollection_t & noise_change_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NoiseChange]);
#endif

    for(UInt_t module_i=0; module_i<_cellParameter.size(); module_i++) {

#ifdef WITH_CONTROL_HISTOGRAMS
      UInt_t n_dead=0;
#endif
      AverageSimple_t av_noise;
      
      for(UInt_t cell_i=0; cell_i<_cellParameter[module_i].size(); cell_i++) {
	CellParameter &cell_parameter=_cellParameter[module_i][cell_i];
	if (!cell_parameter.isDead()) {

	  av_noise.add(cell_parameter.getNoise());
	  Float_t old_pedestal=cell_parameter.getPedestal();
#ifdef WITH_CONTROL_HISTOGRAMS
	  Float_t old_noise=cell_parameter.getNoise();
#endif

	  cell_parameter.calculate();

#ifdef WITH_CONTROL_HISTOGRAMS
	  pedestal_hist_arr.histogram(module_i)->fill(cell_parameter.getPedestal());
	  noise_hist_arr.histogram(module_i)->fill(cell_parameter.getNoise());
#endif

	  if (   cell_parameter.getSaturationCounter()>tolerated_saturation
	      || cell_parameter.getNoise()<_noiseRange[0] || cell_parameter.getNoise()>_noiseRange[1]
	      || cell_parameter.getPedestal()<_pedestalRange[0] || cell_parameter.getPedestal()>_pedestalRange[1]
	      || (_pedestalNoiseIsKnown && (cell_parameter.getPedestal()-old_pedestal) > _maxPedestalChange )) {
	    cell_parameter.setDead();
#ifdef WITH_CONTROL_HISTOGRAMS
	    n_dead++;
#endif
	  }
	  else {

#ifdef WITH_CONTROL_HISTOGRAMS
	    if (_pedestalNoiseIsKnown) {
	      pedestal_change_hist_arr.histogram(module_i)->fill(cell_parameter.getPedestal()-old_pedestal);
	      noise_change_hist_arr.histogram(module_i)->fill(cell_parameter.getNoise()-old_noise);
	    }
#endif

	  }
	}
#ifdef WITH_CONTROL_HISTOGRAMS
	else {
	  n_dead++;
	}
#endif

      }
      
      av_noise.calculate();
      _avNoisePerModule[module_i]=av_noise.mean();

#ifdef WITH_CONTROL_HISTOGRAMS
      histmgr::HistogramCollection_t & dead_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1DeadCells]);
      dead_hist_arr.histogram(module_i)->fill(n_dead);
#endif	
    }
    if (_cellParameter.size()>0) {
      _pedestalNoiseIsKnown=kTRUE;
    }

    _nPedestalEventsTotal+=_nPedestalEvents;
    _nPedestalEvents=0;
  }

  void SimpleHitSearch::showNoiseStat() const
  {
    for(UInt_t module_i=0; module_i<_cellParameter.size(); module_i++) {
      Average_t noise;
      Average_t pedestal;
      UInt_t n_dead=0;
      for(UInt_t cell_i=0; cell_i<_cellParameter[module_i].size(); cell_i++) {
	const CellParameter &cell_parameter=_cellParameter[module_i][cell_i];
	if (cell_parameter.isDead()) {
	  n_dead++;
	}
	else {
	  if (cell_parameter.getNValues()>0) {
	    noise.add(cell_parameter.getNoise());
	    pedestal.add(cell_parameter.getPedestal());
	  }
	}
      }
      noise.calculate();
      pedestal.calculate();
      std::cout << module_i << ":" << " dead=" <<std::setw(4) << n_dead << " noise" <<  noise << " pedestal=" <<  pedestal << std::endl;
    }
  }

  void SimpleHitSearch::showCurrentNoiseStat() const
  {
    for(UInt_t module_i=0; module_i<_cellParameter.size(); module_i++) {
      Average_t noise;
      Average_t pedestal;
      UInt_t n_dead=0;
      for(UInt_t cell_i=0; cell_i<_cellParameter[module_i].size(); cell_i++) {
	const CellParameter &cell_parameter=_cellParameter[module_i][cell_i];
	if (cell_parameter.isDead()) {
	  n_dead++;
	}
	else {
	  if (cell_parameter.getNValues()>0) {
	    noise.add(sqrt((cell_parameter.getOldNoise()-cell_parameter.getOldPedestal()*cell_parameter.getOldPedestal()/cell_parameter.getNValues())
			   /(cell_parameter.getNValues()-1)));
	    pedestal.add(cell_parameter.getOldPedestal()/cell_parameter.getNValues());
	  }
	}
      }
      noise.calculate();
      pedestal.calculate();
      std::cout << module_i << ":" << " dead=" <<std::setw(4) << n_dead << " noise" << std::setw(30) << noise << " pedestal=" << std::setw(30) << pedestal << std::endl;
    }
  }

  void SimpleHitSearch::searchHits(LCEvent *evtP) 
  {
    
    try {//try
      // DEBUG     
      Bool_t reject_event=false;
      LCCollection* col_adc = evtP->getCollection( _adcColName ) ;

      // determine number of hits per layer to reject dirty events.
      // FIXME: Good idea ? 

      UInt_t n_modules=_mapping.getNModules();
      vector<UInt_t> n_hits_per_module;
      n_hits_per_module.resize(n_modules,0);
      UInt_t max_n_hits=(UInt_t) (_maxHitOccupancy*_mapping.getNCellsTotal()+.5);

      vector<Float_t> pedestal_correction;
      pedestal_correction.resize(n_modules,0.);


#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
      map<UInt_t, vector<Float_t> > signal_induced_pedestal_correction;
      signal_induced_pedestal_correction.clear();
#endif



#ifdef WITH_CONTROL_HISTOGRAMS
      // histogram the average noise per layer
#  ifdef WITH_ALL_CONTROL_HISTOGRAMS
      std::vector< AverageSimple_t > noise_per_module; 
      noise_per_module.resize(n_modules);
#  endif
      histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
      histmgr::HistogramCollection_t & pedestal_correction_per_layer_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1PedestalCorrection]);
      histmgr::HistogramCollection_t & signal_per_layer_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1Signal]);
      histmgr::HistogramCollection_t & signal_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histSignalKey);
      histmgr::HistogramCollection_t & noise_hist_arr= histogramList->getHistogramCollection(_histGroupKey,_histNoiseKey);
      histmgr::Histogram2DCollection_t & a_hist_col=histogramList->getHistogram2DCollection(_histGroupKey,_histNoiseCorrelationKey);
#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
      histmgr::Histogram2DCollection_t & an_hist_col=histogramList->getHistogram2DCollection(_histGroupKey,_histSignalInducedPedestalShiftKey);
#endif

#  ifdef WITH_ALL_CONTROL_HISTOGRAMS
      histmgr::HistogramCollection_t & adc_per_layer_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1ADC]);
#  endif
#endif

      if (_cellParameter.size()==0) {
	buildCellParameters();
      }
      //      std::cout << evtP->getEventNumber() << std::endl;
      LCCollectionVec* col_rawhit=new LCCollectionVec( LCIO::RAWCALORIMETERHIT );
#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
      LCFloatVec signal_over_noise_ratio;
#endif

      //parameters will be saved in in vector of objects containing just noise and pedestal. This vector will be transmitted to the next processor thanks to the singleton getCellParameters();
      //if the flag _writeRawHits is set, will also save the parameters in a collection
      LCCollectionVec *ParamsColToSave=0;
      if (_writeRawHits) ParamsColToSave = new LCCollectionVec( LCIO::LCGENERICOBJECT);

      NoiseParameterArray_t & noiseArray = getCellParameters();
      noiseArray.clear();

      //The fact that the array has to be of a certain size already at this stage
      //is to save time in the execution, as the vector will ultimately have this size
      if (noiseArray.size() != n_modules) noiseArray.resize(n_modules);


      LCFlagImpl hitFlag(col_rawhit->getFlag()) ;
      hitFlag.setBit( LCIO::RCHBIT_ID1) ;
      col_rawhit->setFlag( hitFlag.getFlag()  ) ;
      col_rawhit->setTransient(!(_writeRawHits));
      if (_writeRawHits) ParamsColToSave->setTransient(!(_writeRawHits));

      if (col_adc && col_adc->getNumberOfElements()>0) {//if elements in input col

	//	// for finding events with negative spikes
	//	std::vector<unsigned int> negative_signal;
	//	negative_signal.resize(n_modules,0);

#ifdef CORRELATED_NOISE
#ifdef WITH_CONTROL_HISTOGRAMS
	std::vector<std::vector<std::pair<float,unsigned int> > > module_signal;
	module_signal.resize(n_modules);
	for (UInt_t module_i=0; module_i<n_modules;module_i++) {
	  module_signal[module_i].resize(12*2);
	  for (UInt_t chip_i=0; chip_i<2*12; chip_i++) {
	    module_signal[module_i][chip_i].first=0.;
	    module_signal[module_i][chip_i].second=0;
	  }
	}
#endif
#endif

	Int_t minAdcSignalThreshold = _calibration->getMiniumADCForMipThreshold(_signalThreshold);

	CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
	UInt_t n_cells_found=0;
	Double_t total_signal=0.;
#ifdef RECO_DEBUG
        //CRP test variables for debugging
        UInt_t n_cells_failed(0); 
        UInt_t ncellstotal(0); 
#endif

        //The Adc Value Access Class reorganizes the ADC Blocks in terms of FE    
	AdcValueAccess adc_access(col_adc,&_mapping, _calibration, &_cellParameter);
        //Hence the following is actually a loop over the front ends
	while (adc_access.hasConnectedBlocks()) {//while connected blocks
	  //  -- 1 --  copy the pedestal subtracted adc values of one module into a linear array - the modulebuffer
	  UInt_t last_module_index[2]={UINT_MAX,UINT_MAX};
	  UInt_t cells_per_module[2]={0,0};

	  //This for loop with the help of the AdcValueAccess Class steps through the FEs
          //It breaks when the cells connected to a FE have been processed in the do loop
          //i.e. assigned to the modulebuffer which is then used for the further processing.
          //The counter in the ADC Value Access class is then set to the next connected FE in the
	  for(;;) {//infinite for
#ifdef BOUNDARY_CHECK
	    assert( _moduleBuffer.size() >= _mapping.getNCellsPerModule(adc_access.getModuleIndex()) );
#endif
    
	    //Prepare th splitting of the modulebuffer into a part related to the left and to the right
            //side of a FE if necessary i.e. for the bottom slabs/layers
	    UInt_t side;
	    UInt_t buffer_side_offset;
	    if (adc_access.isRightSide()) {
	      side=1;
	      buffer_side_offset=_moduleBuffer.size()/2;
	    }
	    else {
	      side=0;
	      buffer_side_offset=0;
	    }
            //A new FE has been encountered (counter already set to next FE in AdcAccessValue class)
	    if (static_cast<unsigned int>(adc_access.getModuleIndex()) != last_module_index[side] && last_module_index[side] < n_modules) break;
            
	    last_module_index[side]=adc_access.getModuleIndex();

            //Loop over the cells connected to a frontend and fill of the modulebuffer
  	    do {//do loop
	      CellParameter &cell_parameter=adc_access.getParameter();
              unsigned int cell_index=adc_access.getCellIndexOnModule()+buffer_side_offset;
#ifdef Boundary_Check
	      assert ( cell_index < _moduleBuffer.size());
#endif
	      if (!cell_parameter.isDead()) {
		Int_t raw_adc_value=adc_access.getAdcValue();
		if(raw_adc_value>=_adcRange[0] && raw_adc_value<=_adcRange[1]) {
#ifdef WITH_CONTROL_HISTOGRAMS
#ifdef WITH_ALL_CONTROL_HISTOGRAMS
		  // TODO: histogram also values outside the valid range ?
		  adc_per_layer_hist_arr.histogram(adc_access.getModuleIndex())->fill(raw_adc_value);
#endif
#endif
		  _moduleBuffer[cell_index]=raw_adc_value-cell_parameter.getPedestal();
		}
		else {
		  cell_parameter.incrementSaturationCounter();
		  _moduleBuffer[cell_index]=+FLT_MAX;
		}
	      }
	      else {
		_moduleBuffer[cell_index]=+FLT_MAX;
	      }
	      cells_per_module[side]++;
	    } while (adc_access.nextValue());
	    if (!adc_access.nextBlock()) break;
	  }//infinite for



	  // -- 2 --  correct pedestals and search hits.
          //Loop over the two sides of the FE The Mapping decides whether a side is connected or not
	  for (UInt_t side_i=0; side_i<2; side_i++) {//loop on type
            //Get the index of the module assigned to side_i
     	    UInt_t module_index=last_module_index[side_i];
	    
            //This handles the cases where nothing has been plugged to the right side of a FE
            //i.e. in case of central slabs/layers
	    if (module_index>= n_modules) continue; 
            //Shouldn't this be 
	    if (cells_per_module[side_i] != _mapping.getNCellsPerModule( module_index )) break; 

            n_cells_found+=cells_per_module[side_i];

	    Float_t *a_module_buffer=(side_i==0 ? &(_moduleBuffer[0]) : &(_moduleBuffer[_moduleBuffer.size()/2]));
	    
	    pedestal_correction[module_index]=calculatePedestalCorrection( module_index, a_module_buffer);
	    if (pedestal_correction[module_index]>=FLT_MAX) {
	      pedestal_correction[module_index]=0;
	      error.setDirtyEvent();
	    }
	    else if (fabs(pedestal_correction[module_index])>_maxPedestalChange) {
	      error.setCorruptEventRecord();

	      // dump event
	      if (adc_access.hasConnectedBlocks()) {
		while (adc_access.nextBlock());
	      }
	      break;
	    }
		     

#ifdef WITH_CONTROL_HISTOGRAMS
	    pedestal_correction_per_layer_hist_arr.histogram(module_index)->fill(pedestal_correction[module_index]);
#endif
	    UInt_t n_chips_per_module = (cells_per_module[side_i]==216 ? 12 : 6);
#ifdef WITH_CONTROL_HISTOGRAMS
	    histmgr::HistogramCollection_t &my_hist_col=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NoGlobalPedCorr]);
	    
	    if(module_index==11) {
	      for (UInt_t cell_i=0; cell_i<(cells_per_module[side_i]); cell_i++) {
		my_hist_col.histogram(cell_i)->fill(a_module_buffer[cell_i]);
	      }
	    }
	    histmgr::HistogramCollection_t &my2nd_hist_col=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NoPedSh]);
#endif

	    // Apply global pedestal correction
	    for (UInt_t cell_i=0; cell_i< cells_per_module[side_i] ; cell_i++) {//loop on cells
#ifdef RECO_DEBUG
            ncellstotal++;
#endif
	      if (a_module_buffer[cell_i]<FLT_MAX) {
		CellParameter &cell_parameter=_cellParameter[module_index][cell_i];

		//optim. float pedestal = cell_parameter.getPedestal();
                //We do this since we might encounter a situation in which
		//more modules are included in the simulation than were actually
                //present in the data
		//CAMM: needed ? SimpleHitSearch will not run on MC...
                //if(module_index+1 > noiseArray.size()) noiseArray.resize(module_index+1); 
		NoiseParameter noisePar(module_index,cell_i);
		noiseArray[module_index].push_back(noisePar);
		noisePar.setPedestalBeforeGC(cell_parameter.getPedestal());
		noisePar.setNoiseBeforeGC(cell_parameter.getNoise());

		// change pedestals
		cell_parameter.setOldPedestal( cell_parameter.getPedestal());
		cell_parameter.setPedestal( cell_parameter.getPedestal() + pedestal_correction[module_index]);

		// Float_t pedestal=cell_parameter.getPedestal();
		a_module_buffer[cell_i] -= pedestal_correction[module_index];
#ifdef WITH_CONTROL_HISTOGRAMS	
		if(module_index==11)
		  my2nd_hist_col.histogram(cell_i)->fill(a_module_buffer[cell_i]);
#endif
	      }
	      else {
		NoiseParameter noisePar(module_index,cell_i);
		noisePar.setDead(2);
		CellIndex ind(_mapping.getGeometricalCellIndex(module_index,cell_i));
		int lay = ind.getLayerIndex()-1;
		int waf = 3*(ind.getWaferColumn()-1) + ind.getWaferRow()-1 ;
		int chan = 6*(ind.getPadColumn()-1) + ind.getPadRow()-1 ;

		_isDead[lay][waf][chan] = 2;
                //We do this since we might encounter a situation in which
		//more modules are included in the simulation than were actually
                //present in the data
		//CAMM: I think we don't need that...
                //if(module_index+1 > noiseArray.size()) { 
                  //Have to thing carefully whether we need that
                //  numods = module_index+1;
                //  noiseArray.resize(module_index+1);
                //} 
		noiseArray[module_index].push_back(noisePar);
	      }
	    }//loop on cells
	    
#ifdef RECO_DEBUG
	    assert (noiseArray[module_index].size() == cells_per_module[side_i]);
#endif

	     
#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT	    
	    // Apply global pedestal correction
	    vector<Float_t> sips_correction_per_wafer;
	    sips_correction_per_wafer.clear();
	    sips_correction_per_wafer=calculateSignalInducedPedestalCorrection(module_index, a_module_buffer);
	    signal_induced_pedestal_correction.insert(make_pair(module_index,sips_correction_per_wafer));
	    
#ifdef WITH_CONTROL_HISTOGRAMS
	    histmgr::HistogramCollection_t &my3rd_hist_col=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1PedSh]);
	    vector<Float_t> total_signal_per_wafer;
	    total_signal_per_wafer.resize(n_chips_per_module/2,0);
	    for (UInt_t cell_i=0; cell_i< cells_per_module[side_i] ; cell_i++) {
	      UInt_t of_chip = cell_i%n_chips_per_module;
	      UInt_t on_wafer=of_chip/2;
	      if (a_module_buffer[cell_i]<FLT_MAX) {
		total_signal_per_wafer[on_wafer]+=a_module_buffer[cell_i]- signal_induced_pedestal_correction[module_index][on_wafer];
	      }
	    }
	    //std::cout << "Before hist2D 1: " << std::endl;
	    //histmgr::Histogram2DCollection_t an_hist_col=histogramList->getHistogram2DCollection(_histGroupKey,_histSignalInducedPedestalShiftKey);
	    for (UInt_t wafer_i=0; wafer_i<(n_chips_per_module/2); wafer_i++) {
	       an_hist_col.histogram(module_index,wafer_i)->fill(total_signal_per_wafer[wafer_i],signal_induced_pedestal_correction[module_index][wafer_i]);     
	    }
#endif
#endif

	    for (UInt_t cell_i=0; cell_i< cells_per_module[side_i] ; cell_i++) {//loop on cells
#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
	      UInt_t of_chip = cell_i%n_chips_per_module;
	      UInt_t on_wafer=of_chip/2;
#endif
	      if (a_module_buffer[cell_i]<FLT_MAX) {
		CellParameter &cell_parameter=_cellParameter[module_index][cell_i];
		NoiseParameter &noisePar = noiseArray[module_index][cell_i];
		noisePar.setPedestalBeforeSIC(cell_parameter.getPedestal());
		noisePar.setNoiseBeforeSIC(cell_parameter.getNoise());
#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT	    
		// change pedestals for signal induced shifts
		a_module_buffer[cell_i] -= signal_induced_pedestal_correction[module_index][on_wafer];  
#ifdef WITH_CONTROL_HISTOGRAMS	
		if(module_index==11)
		  my3rd_hist_col.histogram(cell_i)->fill(a_module_buffer[cell_i]);
#endif
#endif
		Float_t adc_value=a_module_buffer[cell_i];
		Float_t noise=cell_parameter.getNoise();
		Bool_t isDead = cell_parameter.isDead();

		noisePar.setPedestal(cell_parameter.getPedestal());
		noisePar.setNoise(cell_parameter.getNoise());
		noisePar.setDead(isDead);

		// buffer the calibrated value, to avoid a recalculation
		Float_t calibrated_value = getCalibratedValue(module_index, cell_i, adc_value);

		// signals are considered to be hits if 
		//  1. the signal passes the noise cut
		//  2. the uncalibrated signal passes the minimum threshold in ADC which is for all pads smaller or equal then the
		//     signal threshold 
		//  3. the calibrated signal is above the signal threshold

		//the following if-else is just to fill control histograms,
		//and to reject events with errors.

		if (   adc_value>minAdcSignalThreshold
		       && adc_value>noise*_noiseCut 
		       && calibrated_value>_signalThreshold ) {//signal
		  if (static_cast<UInt_t>(col_rawhit->getNumberOfElements()) >max_n_hits) {
		    reject_event=true;
		    _rejectBecauseTooManyHits++;
		  }
		  else {
		    cell_parameter.incrementNHits();
		    // UInt_t n_hits=cell_parameter.getNHits();
		    
#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
		    signal_over_noise_ratio.push_back(adc_value/noise);
#endif
		    // count the hits per module to reject events with too many hits
#ifdef BOUNDARY_CHECK
		    if (module_index>=n_hits_per_module.size()) {
		      throw std::logic_error("SimpleHitSearch::SearchHits>Module index out of bounds.");
		    }
#endif
		    n_hits_per_module[module_index]++;
		    
#ifdef WITH_CONTROL_HISTOGRAMS
		    // \todo in principle the signal histograms per module can be calculated by
		    //       summing all the individual signal histograms.
		    //       * this would speed up the processing, and
		    //       * reduce memory consumption slightly
		    //       * but: would require to implement a method in the histogram display to automatically
		    //         sum all the histograms of the same major index.
		    
		    // for online monitoring:
		    signal_per_layer_hist_arr.histogram(module_index)->fill(calibrated_value);
		    
		    // for checking individual pads:
		    signal_hist_arr.histogram(module_index,cell_i)->fill(calibrated_value);
#endif
		  }
		}//signal
		else {
		  // -- noise
#ifdef WITH_CONTROL_HISTOGRAMS
		  
#  ifdef WITH_ALL_CONTROL_HISTOGRAMS
		  noise_per_module[module_index].add(adc_value);
#  endif
#  ifdef CORRELATED_NOISE
#    ifdef BOUNDARY_CHECK
		  assert(module_index <module_signal.size());
#    endif
		  unsigned int chip_i=cell_i%12;
		  unsigned int channel_i=cell_i/12;
		  module_signal[module_index][chip_i*2+channel_i%2].first+=adc_value;
		  module_signal[module_index][chip_i*2+channel_i%2].second++;
#  endif
		  // TODO: only histogram the uncalibrated value, is that a good idea ?
		  //		  dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1Noise]->getElementAt(adc_access.getModuleIndex()))->fill(adc_value);
		  noise_hist_arr.histogram(module_index,cell_i)->fill(adc_value);
#endif

		  //for the noise in MC, fill histograms that will be fitted in the end() method, in order to get the values for the MC reco.
		  CellIndex ind(_mapping.getGeometricalCellIndex(module_index,cell_i));
		  int lay = ind.getLayerIndex()-1;
		  int waf = 3*(ind.getWaferColumn()-1) + ind.getWaferRow()-1 ;
		  int chan = 6*(ind.getPadColumn()-1) + ind.getPadRow()-1 ;

		  //if that is not verified, the program will crash because histogram will not exist...
		  if (lay > 29 || waf > 8 || chan > 35){
		    std::stringstream message;
		    message << "SimpleHitSearch::SearchHit> Something is wrong with the layer, wafer or cell indices in module_index " 
			    << module_index << ": K=" << ind.getLayerIndex() << ", S=" << ind.getWaferColumn() << ", M= " << ind.getWaferRow() << ", I=" << ind.getPadRow() << ", and J=" << ind.getPadColumn() << std::endl;
		    throw runtime_error(message.str());
		  }

		  if (adc_value<minAdcSignalThreshold && adc_value<noise*_noiseCut) p_Ped[lay][waf][chan]->Fill(adc_value);
		  _isDead[lay][waf][chan] = cell_parameter.isDead();

		}//noise		  
		// -- save everybody in the RawHits collection
		IMPL::RawCalorimeterHitImpl *hit=new IMPL::RawCalorimeterHitImpl;
		// FIXME: put MOKKA cell id 
		hit->setCellID0(_mapping.getGeometricalCellIndex(module_index,cell_i));
		
		noisePar.setGeomCellIndex(hit->getCellID0());
		UInt_t module_type = _mapping.getModuleType(module_index);
		UInt_t module_id   = _mapping.getModuleID(module_index);
		CellIndex dec(hit->getCellID0(),module_index,module_type,module_id,cell_i,isDead);
		
		hit->setCellID1(dec.getSecondIndex());
#ifdef BOUNDARY_CHECK
		if ( !(dec.getLayerIndex() >= 1 && dec.getLayerIndex() <= 30) ||
		     !(dec.getWaferRow() >= 1 && dec.getWaferRow() <= 3) ||
		     !(dec.getWaferColumn() >= 1 && dec.getWaferColumn() <= 3)||
		     !(dec.getPadRow() >= 1 && dec.getPadRow() <= 6)||
		     !(dec.getPadColumn() >= 1 && dec.getPadColumn() <= 6))
		  {
		    std::cout << " WRONG decoding of indices ! " << std::endl <<
		      " Layer = " << dec.getLayerIndex() << std::endl <<
		      " Wafer Row (M) = " << dec.getWaferRow() << std::endl <<
		      " Wafer Column (S) = " << dec.getWaferColumn() << std::endl <<
		      " Pad Row (I) = " << dec.getPadRow() << std::endl <<
		      " Pad Column (J) = " << dec.getPadColumn() <<  std::endl <<
		      " ...........Exiting........." << std::endl;
		    exit(0);
		  }
#endif
		
		hit->setTimeStamp(evtP->getTimeStamp());
		
		total_signal += adc_value;
		assert ((fabs(adc_value)*10000+0.5) < FLT_MAX);
		int rawAmpl = 0;
		if (adc_value >= 0.0) rawAmpl = static_cast<int>(adc_value*10000+0.5);
		else rawAmpl = static_cast<int>(adc_value*10000-0.5);
		hit->setAmplitude(rawAmpl);
		col_rawhit->addElement(hit);
#ifdef RECO_DEBUG
	      } else {
		std::cout << "Cell failed:" << std::endl;
		std::cout << "ModuleName " << _mapping.getModuleName(module_index) << std::endl; 
		std::cout << "cell " << cell_i << std::endl; 
		n_cells_failed++;
#endif
	      }//protec
	    }//loop on cells
	  }//loop on type  
	}//while connected blocks
	
	//CAMM: fill a collection with the noise parameters of all connected cells.
	if (_writeRawHits){
#ifdef RECO_DEBUG
	  assert (noiseArray.size() == n_modules);
#endif
	  for (UInt_t mod = 0; mod < n_modules; mod++){//loop on modules
	    for (UInt_t cell = 0; cell < _mapping.getNCellsPerModule(mod); cell++){
	      LCGenericObjectImpl *obj = new LCGenericObjectImpl(4,0,7);
	      LCPayload<NoiseParameter> cte(noiseArray[mod][cell],*obj);
	      ParamsColToSave->addElement(obj) ;
	    }
	  }//loop on modules
	}
	
#ifdef CORRELATED_NOISE
#ifdef WITH_CONTROL_HISTOGRAMS
        //std::cout << "Before hist2D 2: " << std::endl;
	//histmgr::Histogram2DCollection_t a_hist_col=histogramList->getHistogram2DCollection(_histGroupKey,_histNoiseCorrelationKey);
#  ifdef BOUNDARY_CHECK
	assert(a_hist_col.n() >= module_signal.size());
#  endif
	for (UInt_t module_i=0; module_i<module_signal.size(); module_i++) {
	  for (UInt_t chip_i=0; chip_i<12; chip_i++) {
	    if (module_signal[module_i][chip_i+0].second>0 && module_signal[module_i][chip_i+1].second>0) {
	      a_hist_col.histogram(module_i,chip_i)->fill(module_signal[module_i][chip_i+0].first/module_signal[module_i][chip_i+0].second,
							  module_signal[module_i][chip_i+1].first/module_signal[module_i][chip_i+1].second);
	    }
	  }
	}
#endif
#endif
	
	//currently empty by default...
	if (!_parNamePedestalCorrection.empty() && !pedestal_correction.empty()) {
	  evtP->parameters().setValues(_parNamePedestalCorrection,pedestal_correction);
	}
	
	
	if (n_cells_found != _mapping.getNConnectedCells()) {
	  error.setMissingADCBlock();
	  _nEventsWithMissingAdcBlocks++;
	}
	else {
	  if (!_parNameEventEnergy.empty()) {
	    evtP->parameters().setValue(_parNameEventEnergy,static_cast<Float_t>(total_signal));
	  }
#ifdef WITH_CONTROL_HISTOGRAMS
	  histmgr::HistogramCollection_t & total_signal_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1TotalSignal]);
	  total_signal_hist_arr.histogram(0)->fill(total_signal);
#endif
	}
	
#ifdef WITH_CONTROL_HISTOGRAMS
#  ifdef WITH_ALL_CONTROL_HISTOGRAMS
	{
	  UInt_t module_i=0;
	  histmgr::HistogramCollection_t & nhits_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NHits]);
	  histmgr::HistogramCollection_t & noise_average_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NoiseAverage]);
	  histmgr::HistogramCollection_t & pedestal_average_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1SignalAverage]);
	  for (vector<AverageSimple_t>::iterator iter=noise_per_module.begin();
	       iter!=noise_per_module.end();
	       iter++,module_i++) {
	    iter->calculate();
	    
	    noise_average_hist_arr.histogram(module_i)->fill(iter->sigma());
	    pedestal_average_hist_arr.histogram(module_i)->fill(iter->mean());
	    nhits_hist_arr.histogram(module_i)->fill(n_hits_per_module[module_i]);
	  }
	}
#  else
	histmgr::HistogramCollection_t & nhits_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NHits]);
	for(UInt_t module_i=0; module_i<n_modules; module_i) {
	  nhits_hist_arr.histogram(module_i)->fill(n_hits_per_module[module_i]);
	}
#  endif
#endif

#ifdef RECO_DEBUG
	std::cout << "hits=" << col_rawhit->getNumberOfElements() << std::endl;
	std::cout << "NCells not in collection=" << n_cells_failed << std::endl;
	std::cout << "Total cells present=" << ncellstotal << std::endl;
#endif
	if (col_rawhit->getNumberOfElements()>0) {
#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
	  if (!signal_over_noise_ratio.empty()) {
	    col_rawhit->parameters().setValues(std::string("SN"),signal_over_noise_ratio);
	  }
#endif
	  
	  if ( (error.getBits() & _errorMask ) ) {
	    std::cerr << error << std::endl;
	    if (error.missingADCBlock()) {
	      _rejectBecauseOfMissingADCBlocks++;
	    }
	    reject_event=true;
	    _rejectBecauseOfError++;
	  }
	  else {
	    for (UInt_t module_i=0; module_i<n_hits_per_module.size(); module_i++) {
	      if (n_hits_per_module[module_i]>_maxHitOccupancy * _mapping.getNCellsPerModule(module_i)) {
		reject_event=true;
		_rejectBecauseTooManyHits++;
	      }
	    }
	  }
	  if (!reject_event) {
	    _nEventsWithHits++;
	    //if (!(col_rawhit.isTransient())) 
	    evtP->addCollection(col_rawhit,_rawhitColName);
	    if (_writeRawHits) evtP->addCollection(ParamsColToSave,"EmcCellParametersSaved");
	  }
	  else {
	    delete col_rawhit;
	    if (_writeRawHits) delete ParamsColToSave;
	    _rejectedEvents++;
	  }
	  }
	  else {
	    delete col_rawhit;
	    if (_writeRawHits) delete ParamsColToSave;
	  }
	  if (error) {
	    evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
	  }
	  _nEvents++;
	}
	
      }
    catch (  DataNotAvailableException &err ) {
      _nEventsWithoutADC++;
    }
  }
    
  void SimpleHitSearch::searchHitsAndAdjustPedestalsAndNoise(LCEvent *evtP) {
      try {
	Bool_t reject_event=false;
	LCCollection* col_adc = evtP->getCollection( _adcColName ) ;
	
	// determine number of hits per layer to reject dirty events.
	// FIXME: Good idea ? 
	std::vector<UInt_t> n_hits_per_module;
	UInt_t n_modules=_mapping.getNModules();
	n_hits_per_module.resize(n_modules);
	UInt_t max_n_hits=(UInt_t) (_maxHitOccupancy*_mapping.getNCellsTotal()+.5);
	for (vector<UInt_t>::iterator iter=n_hits_per_module.begin(); iter != n_hits_per_module.end(); iter++) {
	  *iter=0;
	}

#ifdef WITH_CONTROL_HISTOGRAMS
      // histogram the average noise per layer
	std::vector< AverageSimple_t > noise_per_module; 
	noise_per_module.resize(n_modules);
	
	histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
	histmgr::HistogramCollection_t & adc_per_layer_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1ADC]);
	histmgr::HistogramCollection_t & pedestal_per_layer_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1Pedestal]);
	histmgr::HistogramCollection_t & signal_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histSignalKey);
	histmgr::HistogramCollection_t & signal_per_layer_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1Signal]);
	histmgr::HistogramCollection_t & noise_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histNoiseKey);
	histmgr::HistogramCollection_t & noise_change_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NoiseChange]);
	histmgr::HistogramCollection_t & pedestal_change_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1PedestalChange]);
#endif
	
	if (_cellParameter.size()==0) {
	  buildCellParameters();
	}
	//      std::cout << evtP->getEventNumber() << std::endl;
      LCCollection* col_hit=new LCCollectionVec( LCIO::CALORIMETERHIT );
#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
      LCFloatVec signal_over_noise_ratio;
#endif
      // write 3d coordinates
      LCFlagImpl hitFlag(col_hit->getFlag()) ;
      hitFlag.setBit( LCIO::RCHBIT_LONG) ;
      col_hit->setFlag( hitFlag.getFlag()  ) ;
      
      
      if (col_adc && col_adc->getNumberOfElements()>0) {
	
	// for finding events with negative spikes
	//	std::vector<unsigned int> negative_signal;
	//	negative_signal.resize(n_modules,0);

	Int_t minAdcSignalThreshold = _calibration->getMiniumADCForMipThreshold(_signalThreshold);
	
	CALICE::ErrorBits error(evtP->getParameters().getIntVal(PAR_ERROR_BITS));
	UInt_t n_cells_found=0;

	AdcValueAccess adc_access(col_adc,&_mapping, _calibration, &_cellParameter);
	if (adc_access.hasConnectedBlocks()) {
	do {
	  do {
	    // pedestals and noise are adjusted if not a hit
	    CellParameter &cell_parameter=adc_access.getParameter();
	    //#ifdef WITH_CELL_PAR_INDEX
	    //	    cell_parameter.setCellIndex(adc_access.getCellIndexOnModule());
	    //	    cell_parameter.setModuleIndex(adc_access.getModuleIndex());
	    //#endif
	    if (!cell_parameter.isDead()) {
	    
	      Int_t raw_adc_value=adc_access.getAdcValue();
	      if(raw_adc_value>=_adcRange[0] && raw_adc_value<=_adcRange[1]) {
	      
		Float_t pedestal=cell_parameter.getPedestal();
		Float_t adc_value=((Float_t) raw_adc_value) - pedestal;

#ifdef WITH_CONTROL_HISTOGRAMS
#  ifdef WITH_ALL_CONTROL_HISTOGRAMS
		adc_per_layer_hist_arr.histogram(adc_access.getModuleIndex())->fill(raw_adc_value);
		pedestal_per_layer_hist_arr.histogram(adc_access.getModuleIndex())->fill(pedestal);
#  endif
#endif
	      
		Float_t noise=cell_parameter.getNoise();
		// buffer the calibrated value, to avoid a recalculation
		Float_t calibrated_value;

		if (   adc_value>minAdcSignalThreshold
		    && adc_value>noise*_noiseCut
		    && (calibrated_value=adc_access.getCalibratedValue(pedestal))>_signalThreshold )
		  {
		    // -- signal
		    cell_parameter.incrementNHits();

		    if (static_cast<UInt_t>(col_hit->getNumberOfElements())>max_n_hits) {
		      reject_event=true;
		      _rejectBecauseTooManyHits++;
		    }
		    else {
		      IMPL::CalorimeterHitImpl *hit=new IMPL::CalorimeterHitImpl;
		      hit->setCellID0(adc_access.getGeometricalCellIndex());

		      //DEBUG:  hit->setCellID1(adc_access.getCellIndexOnModule() & 0xffff+((adc_access.getModuleIndex() & 0x7fff)<<16));
		      hit->setTime(evtP->getTimeStamp());

		      hit->setEnergy( calibrated_value );
		      hit->setPosition(adc_access.getPosition().data());
		      
		      col_hit->addElement(hit);

#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
		      signal_over_noise_ratio.push_back(adc_value/noise);
#endif

		      // count the hits per module to reject events with too many hits
#ifdef BOUNDARY_CHECK
		      if (static_cast<UInt_t>(adc_access.getModuleIndex())>=n_hits_per_module.size()) {
			throw std::logic_error("SimpleHitSearch::SearchHits>Module index out of bounds.");
		      }
#endif
		      n_hits_per_module[adc_access.getModuleIndex()]++;

#ifdef WITH_CONTROL_HISTOGRAMS
		      signal_per_layer_hist_arr.histogram(adc_access.getModuleIndex())->fill(calibrated_value);
		      signal_hist_arr.histogram(adc_access.getModuleIndex(),adc_access.getCellIndexOnModule())->fill(calibrated_value);

#endif

		    }
		  }
		else {
		  // -- noise
		  cell_parameter.adjustPedestalNoise(raw_adc_value, _weightOfOldPedestalNoise );

#ifdef WITH_CONTROL_HISTOGRAMS
		  noise_per_module[adc_access.getModuleIndex()].add(adc_value);

		  pedestal_change_hist_arr.histogram(adc_access.getModuleIndex())
		    ->fill(cell_parameter.getPedestal()-cell_parameter.getOldPedestal());

		  noise_change_hist_arr.histogram(adc_access.getModuleIndex())
		    ->fill(cell_parameter.getNoise()-cell_parameter.getOldNoise());

		  // TODO: only histogram the uncalibrated value, is that a good idea ?
		  //dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1Noise]->getElementAt(adc_access.getModuleIndex()))->fill(adc_value);
		  noise_hist_arr.histogram(adc_access.getModuleIndex(),adc_access.getCellIndexOnModule())->fill(adc_value);
#endif

		  // identify negative spikes
		  // if ( adc_value < -noise*_noiseCutForPedestalNoiseCalculation ) {
		  //  negative_signal[adc_access.getModuleIndex()]++;
		  //}

		}
	      }
	      else {
		cell_parameter.incrementSaturationCounter();
	      }
	    }  

	    n_cells_found++;

	  } while (adc_access.nextValue());
	} while (adc_access.nextBlock());

	//	for (std::vector<unsigned int>::iterator module_iter=negative_signal.begin(); 
	//	     module_iter!=negative_signal.end();
	//	     module_iter++) {
	//	  if ( *module_iter > static_cast<unsigned int>(_nNegativeSignalsPerModule) ) {
	//	    error.setLargeNegativeSignal();
	//	  }
	//	}

	}
	//	std::cout << "hits=" << col_hit->getNumberOfElements() << std::endl;

	if (n_cells_found != _mapping.getNConnectedCells()) {
	  error.setMissingADCBlock();
	}

#ifdef WITH_CONTROL_HISTOGRAMS
	histmgr::HistogramCollection_t & av_signal_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1SignalAverage]);
	histmgr::HistogramCollection_t & av_noise_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NoiseAverage]);
	histmgr::HistogramCollection_t & n_hits_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1NHits]);

	UInt_t module_i=0;
	for (vector<AverageSimple_t>::iterator iter=noise_per_module.begin();
	     iter!=noise_per_module.end();
	     iter++,module_i++) {
	  iter->calculate();
	  av_noise_hist_arr.histogram(module_i)->fill(iter->sigma());
	  av_signal_hist_arr.histogram(module_i)->fill(iter->mean());
	  n_hits_hist_arr.histogram(module_i)->fill(n_hits_per_module[module_i]);
	}
#endif

	if (col_hit->getNumberOfElements()>0 ) {
#ifdef EXPORT_SIGNAL_TO_NOISE_RATIO
	  if (!signal_over_noise_ratio.empty()) {
	    col_hit->parameters().setValues(std::string("SN"),signal_over_noise_ratio);
	  }
#endif
	  
	  if ( (error.getBits() & _errorMask ) ) {
	    reject_event=true;
	    if (error.missingADCBlock()) {
	      _rejectBecauseOfMissingADCBlocks++;
	    }
	    _rejectBecauseOfError++;
	  }
	  else {
	    for (UInt_t module_i=0; module_i<n_hits_per_module.size(); module_i++) {
	      if (n_hits_per_module[module_i]>_maxHitOccupancy * _mapping.getNCellsPerModule(module_i)) {
		reject_event=true;
		_rejectBecauseTooManyHits++;
	      }
	    }
	  }
	  if (!reject_event) {
	    _nEventsWithHits++;
	    evtP->addCollection(col_hit,_hitColName);
	  }
	  else {
	    delete col_hit;
	    _rejectedEvents++;
	  }
	}
	else {
	  delete col_hit;
	}

	if (error) {
	  evtP->parameters().setValue(PAR_ERROR_BITS,error.getBits());
	}
	_nEvents++;
      }

    }
    catch (  DataNotAvailableException &err ) {
      _nEventsWithoutADC++;
    }

  }

  void SimpleHitSearch::buildCellParameters() 
  {
    const UInt_t n_modules=_mapping.getNModules();

    // verify that the calibration object is sane
    for (UInt_t module_i=0; module_i<n_modules; module_i++) {
      if (_mapping.isModuleConnected(module_i)) {
	if (!_calibration->checkForCalibrationConstantsOfModule(_mapping.getModuleID(module_i),
								_mapping.getModuleType(module_i),
								_mapping.getNCellsPerModule(module_i))) {
	  std::stringstream message;
	  message << "SimpleHitSearch::buildCellParameters> Calibration invalide for " 
		  << _mapping.getModuleName(module_i)
		  << " (cells="  << _mapping.getNCellsPerModule(module_i) <<")"
		  << ".";
	  throw runtime_error(message.str());
	}
	
      }
    }
    
    // build the array which stores the pedestals, the noise and the cell status
    // and flag cells dead if the module is not connected or if the calibration 
    // constants are outside the valid range .
    delete _cellParameterCollection;
    _cellParameterCollection=new LCCollectionVec( LCIO::LCGENERICOBJECT );
    lcio::IntVec index_array;
    if (_cellParameter.size()!=n_modules) {
      _cellParameter.resize(n_modules);
      for (UInt_t module_i=0; module_i<n_modules; module_i++) {
	UInt_t cells_per_module=_mapping.getNCellsPerModule(module_i);
	//	_cellParameter[module_i].resize(cells_per_module);
	index_array.push_back(_cellParameterCollection->getNumberOfElements());
	for (UInt_t cell_i=0; cell_i<cells_per_module; cell_i++) {
	  LCGenericObject *an_obj=new LCGenericObjectImpl(kNCellParameterInts,kNCellParameterFloats,0);
	  _cellParameterCollection->addElement(an_obj);
	  _cellParameter[module_i].push_back(CellParameter(an_obj));
	  if (_mapping.isCellDead(module_i,cell_i) || !_calibration->isValid(_mapping.getModuleID(module_i),_mapping.getModuleType(module_i),cell_i)) {
	    _cellParameter[module_i].back().setDead();
	  }
	}
      }
      index_array.push_back(_cellParameterCollection->getNumberOfElements());
      if (!index_array.empty()) {
	_cellParameterCollection->parameters().setValues(std::string("ModuleStartIndex"),index_array);
      }

      //      UInt_t module_i=0;
      //      for (CellParameterArray_t::iterator module_iter=_cellParameter.begin();
      //	   module_iter!=_cellParameter.end();
      //	   module_iter++,module_i++) {
      //	const UInt_t cells_per_module=_mapping.getNCellsPerModule(module_i);
      //	_cellParameter[module_i].resize(cells_per_module);
      //	UInt_t cell_i=0;
      //	for (CellParameterPerModuleArray_t::iterator cell_iter=module_iter->begin();
      //	     cell_iter!=module_iter->end();
      //	     cell_iter++,cell_i++) {
      //	  if (_mapping.isCellDead(module_i,cell_i)) {
      //	    _cellParameter[module_i][cell_i].setDead();
      //	  }
      //	}
      //      }
    }
    
  }

  void SimpleHitSearch::end()
  {//end method

    if ( !_pedestalNoiseIsKnown) {
      if (_nPedestalEvents>(UInt_t) _minNEventsForPedestalNoiseUpdate) {
	//	  UInt_t used_pedestal_events=_nPedestalEvents;
	calculatePedestalNoiseAndDeclareCellsDead();
      }
    }

#ifdef WITH_CONTROL_HISTOGRAMS
    histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();
    histmgr::HistogramCollection_t & pedestal_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1FinalPedestal]);
    histmgr::HistogramCollection_t & noise_hist_arr=histogramList->getHistogramCollection(_histGroupKey,_histKey[kH1FinalNoise]);
#endif

    for(UInt_t module_i=0; module_i<_cellParameter.size(); module_i++) {
      for(UInt_t cell_i=0; cell_i<_cellParameter[module_i].size(); cell_i++) {
	CellParameter &cell_parameter=_cellParameter[module_i][cell_i];
	if (!cell_parameter.isDead()) {

#ifdef WITH_CONTROL_HISTOGRAMS
	  pedestal_hist_arr.histogram(module_i)->fill(cell_parameter.getPedestal());
	  noise_hist_arr.histogram(module_i)->fill(cell_parameter.getNoise());
#endif
	}

      }
    }

#ifdef WITH_CONTROL_HISTOGRAMS
    histogramList->unlockGroup(_histGroupKey);
#endif

    _nEventsTotal         += _nEvents;
    _nEventsWithHitsTotal += _nEventsWithHits;
    _nPedestalEventsTotal += _nPedestalEvents;

    _nEvents=0;
    _nEventsWithHits=0;
    _nPedestalEvents=0;

    std::cout << "--- " << name() << " Report :" << std::endl;
    std::cout << std::setw(8) << _nEventsSkipped              << " events skipped." << std::endl;
    if (_skipDirtyEventsInPedestalCalculation > 0) {
      std::cout << std::setw(8) << _skipDirtyEventsInPedestalCalculation << " dirty events skipped during pedestal calculation." << std::endl;
    }
    std::cout << std::setw(8) << _nEventsTotal                << " events processed." << std::endl
	      << std::setw(8) << _nEventsWithHitsTotal        << " events with hits (fraction " 
	      << (_nEventsTotal>0 ? (Float_t) _nEventsWithHitsTotal/_nEventsTotal : 0.) << ")." << std::endl
	      << std::setw(8) << _rejectedEvents              << " rejected events (fraction " 
	      << (_nEventsTotal>0 ? (Float_t) _rejectedEvents/_nEventsTotal : 0.) << ")." << std::endl;

    if (_rejectedEvents>0) {
      std::cout << std::setw(10) <<  _rejectBecauseTooManyHits  << " rejected because too many hits." << std::endl
		<< std::setw(10) <<  _rejectBecauseOfError          << " rejected because of an error" << std::endl
		<< std::setw(10) <<   _rejectBecauseOfMissingADCBlocks << " rejected because of missing ADC blocks." << std::endl;
    }
    
    std::cout << std::setw(8) <<   _nPedestalEventsTotal        << " events used for pedestal noise calculation in " << std::endl
	      << std::setw(8)  << _nPedestalEventSets << " sets (random trigger sets)." << std::endl
	      << std::setw(8) << _uncoveredTriggerCounter     << " triggers not handled." << std::endl
	      << std::setw(8) << _nEventsWithoutADC           << " events without ADC information." << " " << std::endl
	      << std::setw(8) << _nEventsWithMissingAdcBlocks << " events with missing ADC blocks." << " " << std::endl
	      << std::endl
	      << " -  Noise statistics :" << std::endl;
    showNoiseStat();
    std::cout << std::endl;

    _cellParameter.clear();
    delete _cellParameterCollection;
    _cellParameterCollection=0;

    //now fit the histograms with the pedestals in noise selection in signal events, to extract the noise per channel for MC reco
    //save that in the database
    //LCWriter* wrt = LCFactory::getInstance()->createLCWriter() ;
    //std::ostringstream filename;
    //filename << "cellPars_" << _runnum << ".slcio";
    //wrt->open(filename.str(), LCIO::WRITE_NEW )  ;
    // create and add a run header to the file 
    // this isn't needed by LCCD but allows to use the 'dumpevent' tool
    //LCRunHeader* rHdr = new LCRunHeaderImpl ;
    //wrt->writeRunHeader( rHdr ) ;

    //Since there might be a lot of others files created in the course
    //of the reconstruction it is recommended to link explicitly to
    //the file to be filled
    if ( _hfile ) _hfile->cd();
    //--------- create event with one collection --------------
    //LCEvent* evt = new LCEventImpl ;
    LCCollectionVec* newcol = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;
    TH1F *hchi2 = new TH1F("hchi2","Chi2;#chi^{2}",2000,0,2000);
    TH1F *hchi2ndf = new TH1F("hchi2ndf","Chi2/ndf;#chi^{2}/ndf",200,0,100);
    TF1 *gauss = new TF1("gauss","gaus",-500,500);

    for (int k = 0; k < 30; k++){
      for (int s = 0; s < 3; s++){
	for (int m = 0; m < 3; m++){
	  for (int j = 0; j < 6; j++){
	    for (int i = 0; i < 6; i++){
	      //float fillnoise = 6.0;
	      //if (NUM[k][3*s+m][6*j+i] != 0) _noise[k][3*s+m][6*j+i]/NUM[k][3*s+m][6*j+i];
	      CellIndex dec(m+1,s+1,i+1,j+1,k+1);
	      NoiseParameter parVal(dec.getCellIndex(),0,0,6.0,0.0);
	      int waf = 3*s+m;
	      int chan = 6*j+i;
	      if (p_Ped[k][waf][chan]->GetEntries() != 0){//histo with entries
		parVal.setDead(_isDead[k][waf][chan]);

		gauss->SetParameters(1000,0,6);
		p_Ped[k][waf][chan]->Fit("gauss","RQ0","",p_Ped[k][waf][chan]->GetMean()-5*fabs(p_Ped[k][waf][chan]->GetRMS()),p_Ped[k][waf][chan]->GetMean()+1*fabs(p_Ped[k][waf][chan]->GetRMS()));
		if (_hfile) p_Ped[k][waf][chan]->Write();

		if (gauss->GetNDF() != 0){//valid chi2/ndf
		  if ((fabs(gauss->GetParameter(1)) < 6 && fabs(gauss->GetParameter(2)) < 20 && fabs(gauss->GetParameter(2)) > 4.5) || gauss->GetChisquare()/gauss->GetNDF() < 3) {//valid fit

		    parVal.setNoise(gauss->GetParameter(2));
		    parVal.setPedestal(gauss->GetParameter(1));
		    //NoiseParameter parVal(dec.getCellIndex(),0,0,dataNoise[k][3*s+m][6*j+i],_mean[k][3*s+m][6*j+i]);
		    hchi2->Fill(gauss->GetChisquare());
		    hchi2ndf->Fill(gauss->GetChisquare()/gauss->GetNDF());
		  }
		  else {//save histos

		    hchi2->Fill(gauss->GetChisquare());
		    hchi2ndf->Fill(gauss->GetChisquare()/gauss->GetNDF());
		    //p_Ped[k][waf][chan]->Write();

		    //parVal.setNoise(gauss->GetParameter(2));
		    //parVal.setPedestal(gauss->GetParameter(1));
		    //NoiseParameter parVal(dec.getCellIndex(),0,0,dataNoise[k][3*s+m][6*j+i],_mean[k][3*s+m][6*j+i]);

		  }//save histos
		}//if valid chi2/ndf
	      }//histo with entries
	      else {
		parVal.setDead(_isDead[k][waf][chan]);
	      }
	      LCPayload<NoiseParameter> cte(parVal);
	      LCGenericObject* obj = cte.output();
	      newcol->addElement(obj);
	    }
	  }
	}
      }
    }//loop on k

    if (_saveEcalNoise){
      lccd::DBInterface db( _dbInit , _folderNoise , true ) ;
      std::cout << " ------> Database has been accessed, now writting the new collection." << std::endl;
      //..and finally store the collection
      std::string description = "NoiseParameters";

      db.storeCollection(lccd::LCCDTimeStamp((_runInfo.runStart()).timeStamp()),lccd::LCCDTimeStamp((_runInfo.runEnd()).timeStamp()),newcol,description);
      std::cout << " ------> Collection : " << _folderNoise << " has been successfully stored between : " << (_runInfo.runStart()).getDateString() << " and " << (_runInfo.runEnd()).getDateString() << std::endl;
    }

    //evt->addCollection(newcol,"NoiseParameters");
    //evt->parameters().setValue("RunNum", _runnum ) ;
    //wrt->writeEvent( evt ) ; 
    // --- clean up ---
    //delete evt ;
    //wrt->close() ;

    if ( _hfile ){
      hchi2->Write();
      hchi2ndf->Write();
      _hfile->Write();
      _hfile->Close();
      delete _hfile;
      _hfile = 0;
    } else {
      std::cout << " *** ERROR ***   SimpleHitSearch::end() - " 
		<< "Could not open output file for histograms"
		<< std::endl;
    }

  }//end method



  NoiseParameterArray_t & SimpleHitSearch::getCellParameters() {
    static NoiseParameterArray_t params;
    return params;
  }

  void SimpleHitSearch::moduleTypeChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleTypeChanged(col);
    // maybe the number of cells per module changed
    if (_cellParameter.size()>0) {
      _cellParameter.clear();
      delete _cellParameterCollection;
      _cellParameterCollection=0;
    }
    
    UInt_t n_cells=_mapping.getMaxCellsPerFrontEnd();

    if (n_cells!= _moduleBuffer.size()) {
      _moduleBuffer.resize(n_cells);
    }

#ifdef WITH_CONTROL_HISTOGRAMS
    // Histograms are created per module. Thus, the histogram arrays should be 
    // recreated if the number of modules changes.
    if(_mapping.isModuleConditionsDataComplete()) {
      createHistograms(_mapping.getNModules());
    }
#endif
  }

  void SimpleHitSearch::moduleLocationChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleLocationChanged(col);
#ifdef WITH_CONTROL_HISTOGRAMS
    // Histograms are created per module. Thus, the histogram arrays should be 
    // recreated if the number of modules changes.
    if(_mapping.isModuleConditionsDataComplete()) {
      createHistograms(_mapping.getNModules());
    }
#endif

    UInt_t n_cells=_mapping.getMaxCellsPerFrontEnd();

    if (n_cells!= _moduleBuffer.size()) {
      _moduleBuffer.resize(n_cells);
    }

    // maybe the number of modules changed
    if (_cellParameter.size()>0) {
      _cellParameter.clear();
      delete _cellParameterCollection;
      _cellParameterCollection=0;
    }
#ifdef WITH_CONTROL_HISTOGRAMS
    // Histograms are created per module. Thus, the histogram arrays should be 
    // recreated if the number of modules changes.
    if(_mapping.isModuleConditionsDataComplete()) {
      createHistograms(_mapping.getNModules());
    }
#endif
  }


  //#define DEBUG_RECO
  Float_t SimpleHitSearch::calculatePedestalCorrection(UInt_t module_i, Float_t *adc_values_per_module) const {

#ifdef BOUNDARY_CHECK
    // paranoia checks
    assert( module_i < _mapping.getNModules() );
    assert( module_i < _avNoisePerModule.size() );
#endif


#ifdef DEBUG_RECO    
    bool verbose=(_lastEvent>8710 &&module_i==2);
#endif
    // histmgr::HistMgr *histogramList=histmgr::HistMgr::getInstance();

      //#ifdef DEBUG_RECO    
      //    for (UInt_t redo=0; redo<2; redo++) {
      //      bool verbose=(redo>0 || module_i==1);
      //#endif
    Float_t noise=_avNoisePerModule[module_i];
    UInt_t n_cells=_mapping.getNCellsPerModule(module_i);

    AverageSimple_t pedestal;
    UInt_t cell_i=0;

    //start with the first "valid" cell: cell_i.
    while (adc_values_per_module[cell_i]>=FLT_MAX && cell_i<n_cells) cell_i++;
    if (cell_i>=n_cells) return FLT_MAX;

    // Initialisation done in next loop over chips
    //    Float_t min_val[3]={adc_values_per_module[cell_i],adc_values_per_module[cell_i],adc_values_per_module[cell_i]};
    
    bool ignore_chip[12];
    Float_t min_val_chip[12];
    Float_t max_val_chip[12];
    Float_t min_val[12];
    for (UInt_t i=0; i<12; i++) {
      ignore_chip[i]=false;
    //CRP To prevent from accessing the min_val array beyond its boundaries    
      min_val[i]= adc_values_per_module[cell_i];
      min_val_chip[i]=FLT_MAX;
      max_val_chip[i]=-FLT_MAX;
    }

    UInt_t n_chips=(n_cells == 216 ? 12 : 6 );

    //fill an array "pedestal" with the values from all the valid cell of the PCB
    //find min and max values per chip
    //CAMM why discarding the first cell ????
    //CAMM for (UInt_t pad_i=1; pad_i<n_cells; pad_i++) {
    for (UInt_t pad_i=cell_i; pad_i<n_cells; pad_i++) {
      Float_t adc_val=adc_values_per_module[pad_i];
      if (adc_val<FLT_MAX) {
	pedestal.add(adc_val);
	UInt_t chip_i=pad_i%n_chips;
	if (adc_val < min_val_chip[chip_i]) min_val_chip[chip_i]=adc_val;
	if (adc_val > max_val_chip[chip_i]) max_val_chip[chip_i]=adc_val;
      }
    }

    Float_t mean_min_val=0;
    UInt_t n_min_vals=0;
    
#ifdef DEBUG_RECO
    if (verbose && module_i == 2) {
      std::cout << " module = " << std::setw(3) << module_i << " Chip ADC range " << std::endl;
    }
#endif

    //loop on chips to fill the mean value of the minimum value per chip, discarding whole wafer if one of the chips has a signal hit (defined as max-min > 3000).
    for (UInt_t chip_i=0; chip_i<n_chips; chip_i++) {//loop on chips
#ifdef DEBUG_RECO
      if (verbose && module_i == 2) {
      	std::cout << std::setw(2) << chip_i << " : " << std::setw(12) << min_val_chip[chip_i]  << " -  " << std::setw(12) << max_val_chip[chip_i] 
      		  << " : ";
      	for (UInt_t pad_i=chip_i; pad_i<n_cells; pad_i+=n_chips) {
      	  if (static_cast<int>(adc_values_per_module[pad_i])>=1000) {
      	    std::cout << std::setw(4) << "+++";
      	  }
      	  else if (static_cast<int>(adc_values_per_module[pad_i])<=-1000) {
      	    std::cout << std::setw(4) << "---";
      	  }
      	  else {
      	    std::cout << std::setw(4) << static_cast<int>(adc_values_per_module[pad_i]);
      	  }
      	}
      	std::cout << std::endl;
      
            }
#endif
      //do per wafer : 2chips by 2 chips, so only even chip number.
      if ((chip_i%2)!=0)  continue;

      if (max_val_chip[chip_i] - min_val_chip[chip_i] > 3000 || max_val_chip[chip_i+1] - min_val_chip[chip_i+1] > 3000 ) {
	// ignore the whole wafer
	ignore_chip[chip_i  ]=true;
	ignore_chip[chip_i+1]=true;
      }
      else {
	//CAMM : why not calculating the mean also with chip+1 ????
	mean_min_val+=min_val[chip_i];
	n_min_vals++;
      }
    }//loop on chips

    if (n_min_vals<=1) return FLT_MAX;
    mean_min_val/=n_min_vals;

#ifdef DEBUG_RECO
    if (verbose) {
      std::cout << "pedestal = " << pedestal << "  -- Min val: <" << std::setw(12) << mean_min_val << ">"; 
      for (UInt_t i=0; i<3;i++) {
	std::cout << std::setw(12) << min_val[i];
      }
      std::cout << std::endl;
    }
#endif

    Float_t mean=mean_min_val + noise * 5;
    pedestal.calculate();
    if (mean>pedestal.mean()) {
      mean=pedestal.mean();
    }
    Float_t rms=10*noise;
    Float_t cut=10;

    for(UInt_t pass=0;;pass++) {
      //    if (pass>10) verbose=true;
      if (pass>20) break;
      pedestal=AverageSimple_t();
#ifdef DEBUG_RECO
      if (verbose) {
	std::cout << " pass = " << pass << "  pedestal = " << mean << " cut = " << cut << " * " << noise << std::endl;
      }
#endif
      //      dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1ModuleEventNoise]->getElementAt(module_i))->reset();
      for (UInt_t pad_i=0; pad_i<n_cells; pad_i++) {
	UInt_t chip_i=pad_i%n_chips;
	if (ignore_chip[chip_i]) continue;
 
	Float_t adc_val=adc_values_per_module[pad_i];
	if ( adc_val - mean < cut * noise) {
	  pedestal.add(adc_val);
	  //	  dynamic_cast<histmgr::FloatHistogram1D*>(_histCol[kH1ModuleEventNoise]->getElementAt(module_i))->fill(adc_val-mean);

#ifdef DEBUG_RECO
	  if (verbose) {
	    std::cout << std::setw(2) << pass << " :: " << std::setw(3) << pad_i << " :  " << std::setw(14) << adc_val << std::endl;
	  }
#endif
	}
#ifdef DEBUG_RECO
	else if (verbose) {
	  std::cout << std::setw(2) << pass << " :: " << std::setw(3) << pad_i << " : ( " << std::setw(14) << adc_val << ")" << std::endl;
	}
#endif
      }
      pedestal.calculate();

#ifdef DEBUG_RECO
      if (verbose) {
	std::cout << std::setw(2) << pass << " :: pedestal=" << std::setw(14) << pedestal << std::endl;
      }
#endif
      
      Average_t test;
      UInt_t n=0;
      for (UInt_t pad_i=0; pad_i<n_cells; pad_i++) {
	UInt_t chip_i=pad_i%n_chips;
	if (ignore_chip[chip_i]) continue;

	Float_t adc_val=adc_values_per_module[pad_i];
	if ( adc_val-pedestal.mean() < 0) {
	  test.add(adc_val-pedestal.mean());
	  test.add(-(adc_val-pedestal.mean()));
#ifdef DEBUG_RECO
	  if (verbose) {
	    std::cout << std::setw(2) << pass << " :: " << std::setw(3) << pad_i << " : ( " << std::setw(14) << adc_val << ")" << std::endl;
	  }
#endif
	  n+=2;
	}
      }
      test.calculate();
#ifdef DEBUG_RECO
      if (verbose) {
	std::cout << pass << " :: test = " << test << std::endl;
      }
#endif

      Float_t old_mean=mean;
      if (n<4) {
	if (pedestal.n()<4) {
	  mean += cut * noise;
	}
	else {
	  mean-=(pedestal.mean()-mean)*.2;
	  if (fabs(old_mean-mean)<noise*.05) break;
	}
#ifdef DEBUG_RECO
	if (verbose) {
	  cout << "n<4: mean=" << old_mean << " -> " << pedestal.mean() << " -> " << mean << " (" << n << ")" << "  noise= " << noise << endl;
	}
#endif
	continue;
      }
      if (test.sigma() < noise) {
#ifdef DEBUG_RECO
	if (verbose) {
	  cout << "test.sigma() < noise :: mean=" << old_mean << " -> " << pedestal.mean() 
	       << " diff = " << pedestal.mean() - old_mean << " (~<? " << noise/sqrt(static_cast<Float_t>(pedestal.n())) << ") "
	       << "; rms=" << test.sigma() << " (" << n << ")" << "cut=" << cut << "  noise= " << noise << endl;
	}
#endif
	break;
      }
      mean=pedestal.mean()-sqrt((sqr(test.sigma())-sqr(noise))/n);
#ifdef DEBUG_RECO
      if (verbose) {
	cout << "mean=" << old_mean << " -> " << pedestal.mean() << " -> " << mean  << "; rms=" << test.sigma() << " (" << n << ")"  << "  noise= " << noise << endl;
      }
#endif

      if (fabs(test.sigma()-rms)<.05) {
	if ( test.sigma() < noise * 1.2) break;
	mean -= test.sigma() * .5;
      }

      if (pass>2 && fabs(old_mean-mean)<3*noise/sqrt(static_cast<Float_t>(pedestal.n()))) break;


      rms=test.sigma();
      if (cut/2 >= 3) {
	cut /=2;
      }
    }
    
#ifdef DEBUG_RECO
    //    if (fabs(pedestal.mean())<500) {
    if (verbose) {
      std::cout << " correction = " << pedestal.mean() << std::endl;
    }
#endif

    return pedestal.mean();
    //#ifdef DEBUG_RECO
    //    }
    //    
    //}
    //    return 0;
    //#endif
  }


 // simple iteration
#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
  vector<Float_t> SimpleHitSearch::calculateSignalInducedPedestalCorrection(UInt_t module_i, const Float_t *adc_values_per_module) const {

#ifdef BOUNDARY_CHECK
    // paranoia checks
    assert( module_i < _mapping.getNModules() );
    assert( module_i < _avNoisePerModule.size() );
#endif
    //Float_t noise=_avNoisePerModule[module_i];
    UInt_t n_cells=_mapping.getNCellsPerModule(module_i);

        
    UInt_t cell_i=0;
    while (adc_values_per_module[cell_i]>=FLT_MAX && cell_i<n_cells) cell_i++;
    //if (cell_i>=n_cells) return NULL;

        
    UInt_t n_chips=(n_cells== 216 ? 12 : 6 );
    vector<Float_t> shift_per_wafer;
    shift_per_wafer.resize(n_chips/2,0);
    
    
    vector<Float_t> min_val_wafer;
    min_val_wafer.resize(n_chips/2,FLT_MAX);
    vector<Float_t> max_val_wafer;
    max_val_wafer.resize(n_chips/2,-FLT_MAX);
    //vector<Float_t> min_val;
    //min_val.resize(n_chips/2,adc_values_per_module[cell_i]);
        

    for (UInt_t pad_i=0; pad_i<n_cells; pad_i++) {
      Float_t adc_val=adc_values_per_module[pad_i];
      if (adc_val<FLT_MAX) {
	UInt_t chip_i=pad_i%n_chips;
	UInt_t wafer_i=chip_i/2;
	if (adc_val < min_val_wafer[wafer_i]) min_val_wafer[wafer_i]=adc_val;
	if (adc_val > max_val_wafer[wafer_i]) max_val_wafer[wafer_i]=adc_val;
      }
    }

    //DEBUG
    //cout << "Still " << n_chips/2 << " wafers" << endl;


    for (UInt_t chip_i=0; chip_i<n_chips; chip_i++) {
      
      if ((chip_i%2)!=0)  continue; // process whole wafer at once

      //DEBUG
      Float_t total_signal_on_wafer=0;
      

      //AverageSimple_t pedestal_of_wafer;
      UInt_t wafer_i=chip_i/2;
      if (max_val_wafer[wafer_i] - min_val_wafer[wafer_i] > 200 ) { //assume that wafer has been hit
	
	//DEBUG
	//cout << "wafer "<< wafer_i << " is hit" << endl;
	//cout << "vector holds " << shift_per_wafer.size() << endl;


	// get mean noise of wafer
	Float_t mean_noise_of_wafer=0;
	UInt_t living_cells_on_wafer=0;
	for(UInt_t cell_i=chip_i; cell_i< n_cells ; cell_i+=n_chips) {   // do it for this chip...
	  Float_t adc_val=adc_values_per_module[cell_i];
	  if (adc_val<FLT_MAX) {
	    const CellParameter &cell_parameter=_cellParameter[module_i][cell_i]; 
	    mean_noise_of_wafer+=cell_parameter.getNoise() ;
	    living_cells_on_wafer++;
	  }
	   adc_val=adc_values_per_module[cell_i+1];
	   if (adc_val<FLT_MAX) {
	     const CellParameter &cell_parameter=_cellParameter[module_i][cell_i+1]; 
	     mean_noise_of_wafer+=cell_parameter.getNoise();
	     living_cells_on_wafer++;
	   }
	}
	mean_noise_of_wafer = mean_noise_of_wafer/living_cells_on_wafer;

	// now try to get ped shift
	multimap<Float_t, UInt_t > ordered_adc_values;
	
	for(UInt_t cell_i=chip_i; cell_i< n_cells ; cell_i+=n_chips) {   // do it for this chip...
	  Float_t adc_val=adc_values_per_module[cell_i];
	  if (adc_val<FLT_MAX) {
	    ordered_adc_values.insert(make_pair(adc_val,cell_i));
	    total_signal_on_wafer+=adc_val;
	  }
	  adc_val=adc_values_per_module[cell_i+1];   // ...and for the next one to have the complete wafer
	  if (adc_val<FLT_MAX) {
	    ordered_adc_values.insert(make_pair(adc_val,cell_i+1));
	    total_signal_on_wafer+=adc_val;
	  }
	}

	multimap<Float_t, UInt_t >::iterator cell_it;
	multimap<Float_t, UInt_t >::reverse_iterator rv_cell_it;


	cell_it=ordered_adc_values.begin();
	Float_t starting_adc=0;
	if(ordered_adc_values.size()>2) {
	  starting_adc+=cell_it->first;
	  cell_it++;
	  starting_adc+=cell_it->first;
	  cell_it++;
	  starting_adc+=cell_it->first;
	}
	
	starting_adc=starting_adc/3;

	Float_t ped_mean=0;
	Float_t n_samples=0;
	

	for(cell_it=ordered_adc_values.begin();cell_it!=ordered_adc_values.end(); cell_it++) {
	  if(cell_it->first > starting_adc + 6*mean_noise_of_wafer)  //exclude cells that are most likely to be hit
	    ordered_adc_values.erase(cell_it);
	}


	//if(ordered_adc_values.size()<10)
	//cout << "Size: " << ordered_adc_values.size() << endl;
	UInt_t pass=0;
	for(UInt_t pass_i=0; pass_i<35; pass_i++) {
	  
	  AverageSimple_t pedestal;
	  //Float_t old_mean=ped_mean;
	  for(cell_it=ordered_adc_values.begin();cell_it!=ordered_adc_values.end(); cell_it++) {
	    //if(cell_it->first < starting_adc + 6*mean_noise_of_wafer)
	    pedestal.add(cell_it->first);
	      //if(ordered_adc_values.size()<5)
		//cout << "Size after adding: " <<ordered_adc_values.size() << " pass: " << pass_i <<  endl;
		//cout << cell_it->first <<  endl;
	  }
	  pedestal.calculate();
	  //if( pedestal.n()<3)
	    //cout << pedestal.n() << " after calculating " << " pass: " << pass_i <<  endl;
	  
	  
	  if(pedestal.sigma() < 1.2*mean_noise_of_wafer) {
	    ped_mean=pedestal.mean();
	    n_samples=pedestal.n();
	    //if(n_samples<3){
	    //multimap<Float_t, UInt_t >::iterator cell_it2;
	      //cout << n_samples << " , " << pedestal.mean() << endl;
	      //for(cell_it2=ordered_adc_values.begin();cell_it2!=ordered_adc_values.end(); cell_it2++) {
	    //cout << cell_it2->first << endl;
	    //}
	    //}
	    break;
	  }
	  /*
	  if(fabs(pedestal.mean() - old_mean) < 0.01*mean_noise_of_wafer) {
	    ped_mean=pedestal.mean();
	    n_samples=pedestal.n();
	    //cout << "shift doesnt move any more" << endl;
	    if(n_samples<3){
	      multimap<Float_t, UInt_t >::iterator cell_it2;
	      cout << n_samples << " , " << pedestal.mean() << endl;
	      for(cell_it2=ordered_adc_values.begin();cell_it2!=ordered_adc_values.end(); cell_it2++) {
	      cout << cell_it2->first << endl;
	      }
	    }
	    break;
	    
	  }
	  */
	  rv_cell_it = ordered_adc_values.rbegin();
	  //cout << "Size before removing: " << ordered_adc_values.size() << " pass: " << pass_i <<  endl;
	  if(ordered_adc_values.size()>3)
	    {
	      cell_it=ordered_adc_values.begin();
	      if(fabs((cell_it->first - starting_adc)) > fabs((rv_cell_it->first -starting_adc)) ) {
		ordered_adc_values.erase(cell_it);
		//if(pedestal.n()<5)
		//cout << "removing first one" << endl;
	      }
	      else {
		for(cell_it=ordered_adc_values.begin();cell_it!=ordered_adc_values.end();cell_it++) {
		  if((*cell_it)==(*rv_cell_it)){
		    ordered_adc_values.erase(cell_it);
		    //if(pedestal.n()<5)
		    //cout << "removing last one" << endl;
		      break;

		  }

		}
	      }
	      
	    }
	  ped_mean=pedestal.mean();
	  n_samples=pedestal.n();
	  pass=pass_i;
	  if(pass_i==34) {
	    //cout << "Pass 34!" << endl;
	    if(n_samples==2) {

	      // critere for mean
	      AverageSimple_t two_cells;
	      multimap<Float_t, UInt_t >::iterator cell_it2;
	      for(cell_it2=ordered_adc_values.begin();cell_it2!=ordered_adc_values.end(); cell_it2++) {
		two_cells.add(cell_it2->first);
	      }
	      two_cells.calculate();
	      if(two_cells.sigma()<3*mean_noise_of_wafer) {
		ped_mean=two_cells.mean();
		break;
	      }
	      else {
		ped_mean=ordered_adc_values.begin()->first;
		n_samples=1;
		break;
	      }
	    }
	    else {
	      if(n_samples==3) {
		//cout << n_samples << endl;
		AverageSimple_t two_cells;
		multimap<Float_t, UInt_t >::iterator cell_it2;
		cell_it2=ordered_adc_values.begin();
		two_cells.add(cell_it2->first);
		cell_it2++;
		two_cells.add(cell_it2->first);
		two_cells.calculate();
		AverageSimple_t three_cells;
		for(cell_it2=ordered_adc_values.begin();cell_it2!=ordered_adc_values.end(); cell_it2++) {
		  three_cells.add(cell_it2->first);
		}
		three_cells.calculate();
		if(fabs(two_cells.sigma()-mean_noise_of_wafer) < fabs(three_cells.sigma()-mean_noise_of_wafer)) {
		  ped_mean=two_cells.mean();
		  break;
		}
		else {
		  ped_mean=three_cells.mean();
		  break;
		}
	      }
	      else {
		//cout << n_samples << endl;
		ped_mean=0;
		multimap<Float_t, UInt_t >::iterator cell_it2;
		for(cell_it2=ordered_adc_values.begin();cell_it2!=ordered_adc_values.end(); cell_it2++) {
		  ped_mean+=cell_it2->first;
		  //cout << cell_it2->first << endl;
		}
		ped_mean=ped_mean/ordered_adc_values.size();
		//cout << ped_mean << endl;
		break;
	      }
	    }
	  }
	}
	//if(n_samples<10)
	//cout << n_samples << " pass: " << pass << endl;
	  
	if( ped_mean > 0 || -(ped_mean) < mean_noise_of_wafer)    // sample not big enough to be meaningful, only negative and significant shift
	  ped_mean=0;


	//if(n_samples < 2 )//|| ped_mean() > 0 || -(ped_mean()) < mean_noise_of_wafer)    // sample not big enough to be meaningful, only negative and significant shift
	//  {
	    //cout << "no shift, mean is " << pedestal_of_wafer.mean() << endl;
	//  continue;
	// }
	//DEBUG
	//cout << "shift of " << pedestal_of_wafer.mean() << endl;
	/*
	if(pedestal.mean() > 80)
	  {
	    cout << "Big shift: " << pedestal.mean() << "for wafer " << wafer_i << "on "<< module_i  << ", total Signal: " << total_signal_on_wafer << endl;
	    cout << "calculated with: "<< pedestal.n()	 << " pads " << endl;
	    for(UInt_t cell_i=chip_i; cell_i< n_cells ; cell_i+=n_chips) {   
	      cout << adc_values_per_module[cell_i] << endl;
	      cout << adc_values_per_module[cell_i+1] << endl;
	    }
	  }
	*/
	/*
	if(total_signal_on_wafer<5000 && ped_mean <-70)
	  {
	    cout << ped_mean << " signal with low shift calculated with: "<< n_samples << " pads " << endl;
	    for(UInt_t cell_i=chip_i; cell_i< n_cells ; cell_i+=n_chips) {   
	      cout << adc_values_per_module[cell_i] << endl;
	      cout << adc_values_per_module[cell_i+1] << endl;
	    }

	  }
	*/
	/*
	if(ped_mean < -10 &&ped_mean > -20  && wafer_i==4 && module_i == 10 && total_signal_on_wafer>14000)
	  {
	    cout << "Big shift: " << ped_mean << " for wafer " << wafer_i << " on "<< module_i  << ", total Signal: " << total_signal_on_wafer << endl;
	    cout << "calculated with: "<< n_samples	 << " pads " << endl;
	    for(UInt_t cell_i=chip_i; cell_i< n_cells ; cell_i+=n_chips) {   
	      cout << adc_values_per_module[cell_i] << endl;
	      cout << adc_values_per_module[cell_i+1] << endl;
	    }
	  }
	*/
	  
	shift_per_wafer[wafer_i]=ped_mean;

      }
    }
    
    return shift_per_wafer;

  }
#endif

}
