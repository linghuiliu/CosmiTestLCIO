#ifndef _SimpleHitSearch_H_
#define _SimpleHitSearch_H_

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 
//#define WITH_CONTROL_HISTOGRAMS

#include "TFile.h"
#include <TH1F.h>
#include <RtypesSubSet.h>

#include <marlin/Processor.h>
#include <EVENT/LCParameters.h>
#include <collection_names.hh>
#include <CellParameterArray_t.hh>
#include <NoiseParameterArray_t.hh>
#include <ConditionsChangeDelegator.hh>
#include <TriggerBits.hh>
#include <VRawADCValueProcessor.hh>
#include <Calibration.hh>
#include <RunInformation.hh>

#ifdef WITH_CONTROL_HISTOGRAMS
#  include "histmgr/Key_t.hh"
#endif

#ifdef BOUNDARY_CHECK
#  include <cassert>
#endif

#define CORRECT_SIGNAL_INDUCED_PED_SHIFT

namespace CALICE {

  /** Marlin Processor to search and reconstruct colorimeter hits.
   * The hits are selected using a signal-over-noise threshold or 
   * a signal threshold (can be chosen at compile time by defining the 
   * environment variable (FIXED_THRESHOLD -> signal threshold).
   * The class takes care of the pedestal and noise calculation.
   * The hit position is calculated using information stored in a 
   * LCCD conditions database. Moreover, the signal is calibrated
   * if a proper calibration object exists. Currently, there is only
   * the NoOpCalibration which does nothing.
   */
class SimpleHitSearch : public VRawADCValueProcessor
{
public:
  /** construct a new Processor (required by Marlin.
   */
  Processor*  newProcessor() { return new SimpleHitSearch ; }

  /** default constructor.
   * defines and registers its parameters.
   */
  SimpleHitSearch();
    
  /** destructor.
   */
  ~SimpleHitSearch();
    

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
  void init();

  /** Called for every run (does nothing.)
  */
  void processRunHeader( LCRunHeader* run); 

    /** Calculate Pedestals and Noise and search and reconstruct calorimeter hits.
     * The class needs as input the ADC value collection and produces a CalorimeterHit collection.
     * <P>
     * Depending on the trigger configuration, the pedestal and noise are either
     * calculated from random triggers (test beam data) or it is assumed that the 
     * hit distribution is sparse and randomly distributed over the detector (cosmics data).
     * In the latter case the pedestals and noise are calculated initially from
     * the first events assuming the events do not contain any hits. Then, the initial guess 
     * is slowly adjusted. 
     * Otherwise the pedestals and the noise are calculted from pedestal events.
     *
     * The event processing method depends on the state (@ref EState) of the analysis. 
     * The state transition is trigger by trigger configuration changes, changes of
     * the calibration chip configuration, knowledge about the pedestals and noise.
     * @sa  searchHit @sa ssearchHitsAndAdjustPedestalsAndNoise, stateTransition
     */
  void processEvent( LCEvent * evtP );

  
  void end();

  static NoiseParameterArray_t & getCellParameters();

protected:
  typedef void (SimpleHitSearch::* ProcessFunc_t)(EVENT::LCEvent * evtP);
  enum EState {kStateUnknown,          /**< 0: unknown state.*/
	       kStateSkipEvent,        /**< 1: events are skipped unprocessed.*/
	       kStateCosmics,          /**< 2: events are treated as cosmics (low, signals at random locations).*/
	       kStateBeam,             /**< 3: events are treated as beam events (high, localised signals).*/
	       kStatePedestals,        /**< 4: events are treated as pedestal/noise events (no signals).*/
	       kStateInitialPedestals, /**< 5: pedestals and noise are estimated assuming little signal at random locations.*/
	       kStateRefinePedestals,  /**< 6: pedestal and noise estimates are improved assuming little signal at random locations.*/
	       kStateCalibration,      /**< 7: There are  permanent (calibration) signals at specific locations .*/
	       kNStates};              /**< 8: The number of considered states.*/

  ProcessFunc_t _stateFunc[kNStates];
  ProcessFunc_t _theStateFunc;
  EState _isState;

  /** Change the state to the one specified.
   */
  void changeState(EState new_state) {
#ifdef BOUNDARY_CHECK
    assert ( new_state < kNStates );
#endif
    _isState=new_state;
    _theStateFunc=_stateFunc[_isState];
  }

  /** Change the state.
   * return true if the state changed.
   */
  bool stateTransition();

  /** Call the event processing functions of the current state (state function).
   * @sa skipEvents, processCosmics, processBeamEvents, processPedestalEvents, processFirstPedestalEvents
   * @sa processPedestalEventsWithHitRejection, skipCalibrationEvents, 
   */
  void callStateFunc(LCEvent *evtP) {
    (this->*_theStateFunc)(evtP);
  };

  /** Skip events.
   * State function called for the first n events (configurable)
   * and for n events after configuration changes.
   */
  void skipEvents(LCEvent *evtP);

  /** Process cosmics events.
   * State function called if _isState = kStateCosmics.
   */
  void processCosmics(LCEvent *evtP);

  /** Process beam events.
   * State function called if _isState = kStateBeam or if kStateCalibration (configurable).
   */
  void processBeamEvents(LCEvent *evtP);

  /** Process pedestal events.
   * State function called if _isState = kStatePedestals
   */
  void processPedestalEvents(LCEvent *evtP);

  /** Process events to get the first idea of the pedestals.
   * State function called if the pedestal and noise is not yet known and if the 
   * events are either pedestal or cosmics events.
   */
  void processFirstPedestalEvents(LCEvent *evtP);

  /** Refine the initial guess of the pedestals.
   * State function called  subsequent to @ref processFirstPedestalEvents and
   * if the events are either pedestal or cosmics events.
   */
  void processPedestalEventsWithHitRejection(LCEvent *evtP);

  /** Skip events for which the calibration chip is on.
   * State function which skip events until the calibration chip is switched off (configurable).
   */
  void skipCalibrationEvents(LCEvent *evtP);

  /** Reset the arrays which contain the pedestals and the noise, hit counter etc.
   */
  void fullReset();

  /** Initialise the pedestal/noise arrays for the initial pedestal/noise estimate.
   */
  void resetForInitialPedestalNoiseCalculation();

  /** Initialise the pedestal/noise arrays for the  pedestal/noise estimate (An initial pedestal/noise estimate is assumed).
   */
  void resetForPedestalNoiseReCalculation();

  void resetHitCounter();
  
  void resetSaturationCounter();

  //  void accumulateEventsForPedestalNoiseCalculation(LCEvent *evt);

  /** Accumulate data to calculate pedestals and the noise.
   * It is assumed that the event sample does not contain any hits. Otherwise
   * The noise and the pedestals will be biased to larger values.
   */
  void accumulateEventsForInitialPedestalNoiseCalculation(LCEvent *evtP);


  /** Accumulate data to calculate pedestals and the noise with hit rejection.
   * Values which are above the threshold _signalCutForPedestalNoiseCalculation
   * are not considered in the pedestal and noise calculation.
   * If the threshold is too low the pedestals and the noise will be biased to lower values. 
   * If the threshold is too high and there are many hits the pedestals and the noise will be biased 
   * to higher values. 
   * This method is suitable for events with little amount of hits which are clearly separated from
   * the pedestals.
   */
  void accumulateEventsForPedestalNoiseCalculationWithHitRejection(LCEvent *evtP);

  /** Accumulate data to calculate pedestals and the noise.
   * If the pedestal shifts by more than the threshold _pedestalChangeCut than the old pedestal is shifted.
   * As a consequence the precision on the pedestal is lousy, the noise may be biased to lower values. 
   * This method is suitable for noise events which were taken over a long periode which suffers large
   * and discrete shifts of the pedestals.
   * @todo useful?
   * @sa calculatePedestalNoiseAndDeclareCellsDead
   */
  void accumulateEventsForPedestalNoiseCalculationWithHitRejectionAndPedestalShiftDetection(LCEvent *evtP);

  /** Calculate pedestals and noise using the gathered information of the accumalted events.
   * @sa accumulateEventsForInitialPedestalNoiseCalculation,
   *  accumulateEventsForPedestalNoiseCalculationWithHitRejection,
   *  accumulateEventsForPedestalNoiseCalculationWithHitRejectionAndPedestalShiftDetection 
   */
  void calculatePedestalNoiseAndDeclareCellsDead();

  /** Search hits without adjusting noise and pedestals (suitable for test beam data).
   * This method is only useful in cases in which cells rarely contain hits (like cosmics data).
   * Test beam data usually has hits always at the same place. 
   * @param evt pointer to a event which contains the ADC value collection.
   */
  void searchHits(LCEvent *evt);

  /** Search hits and adjust the pedestal and noise of cells with a signal below the cut _signalCut (suitable for cosmics data).
   * This method is only useful in cases in which cells rarely contain hits (like cosmics data).
   * Test beam data usually has hits always at the same place. 
   * @param evtP pointer to a event which contains the ADC value collection.
   */
  void searchHitsAndAdjustPedestalsAndNoise(LCEvent *evtP);

  /** Create and initialise the array which will contain for all cells the pedestals and the noise and further status information.
   * @sa CellParameter
   */
  void buildCellParameters();

  /** Show per layer statistics about the noise and the pedestals.
   */
  void showNoiseStat() const;

  /** Show per layer statistics about the noise and the pedestals (during pedestal noise accumulation).
   * This method calculates the pedestals and the noise itself from the accumulated sums.
   * Will show incorrect results after the pedestals and the noise have been calculated.
   */
  void showCurrentNoiseStat() const;

  /** calculculate an average pedestal correction for one module.
   * @param module_i the module index
   * @param adc_values_per_module an array which has for each cell of the module one adc value or -FLT_MAX for dead cells.
   * @return an additive correction which should be applied to all pedestals of the module.
   */
  Float_t calculatePedestalCorrection(UInt_t module_i, Float_t *adc_values_per_module) const;

#ifdef CORRECT_SIGNAL_INDUCED_PED_SHIFT
  vector<Float_t> calculateSignalInducedPedestalCorrection(UInt_t module_i, const Float_t *adc_values_per_module) const;
#endif

  Float_t getCalibratedValue(UInt_t module_index, UInt_t cell_index, Float_t pedestal_subtracted_adc_value) const {
#ifdef BOUNDARY_CHECK
    assert( _calibration );
#endif
    return _calibration->getCalibratedValue(_mapping.getModuleID(module_index),
					    _mapping.getModuleType(module_index),
					    cell_index,
					    pedestal_subtracted_adc_value);
  }


private:
  bool _pedestalNoiseIsKnown;
  Calibration *_calibration;

  
  TriggerBits _triggerConf;      /**< Variable which contains the current trigger configuration.*/
  TriggerBits _lastTriggerConf;  /**< Variable which contains the trigger configuration of the previous event.*/

  TriggerBits _triggerEvent;     /**< Variable which contains the current trigger main word.*/

  //  enum ETriggerConf {kUnconfiguredTrigger, kBeamTrigger, kCosmicsTrigger, kPedestalTrigger} _lastTriggerConf;

  LCCollection *_cellParameterCollection;
  std::string _cellParameterCollectionName;
  CellParameterArray_t _cellParameter;

  RunInformation _runInfo;
  //needed to save some histograms for noise studies, in the end method
  int _runnum;
  // -- histograms for the noise for MC reco

  TFile *_hfile;
  TH1F *p_Ped[30][9][36];
  bool _isDead[30][9][36];
  // -- parameters of the analysis
  IntVec   _adcRange;
  FloatVec _noiseRange;
  FloatVec _pedestalRange;
  Float_t  _pedestalChangeCut;
  Float_t  _maxPedestalChange;                /**< Maximum allowed pedestal change.*/

  Int_t   _removeMaximumInNEvents;      

  Float_t _maxHitOccupancy;                   /**< Maximum number of tolerated hits. Events with more  hits are 
						   rejected (parameter).*/

  Int_t   _minNEventsForPedestalNoiseUpdate;  /**< Minimum number of events to be accumulated before the 
						   pedestals and noise are calculated (parameter).*/

  Int_t   _updatePedestalsEveryNEvents;        /**< Update pedestals every n events, during pedestal triggers.*/

  Int_t   _weightOfOldPedestalNoise;         /**< Weight given to old pedestals and noise when using a new value to 
						   adjust the pedestals and the noise (parameter). */

  Float_t _shiftPedestalFactor;               /**< When processing pedestal events, shift pedestals by average values measured on one 
						 module times this factor.*/

  Int_t   _skipFirstNEvents;                  /**< Skip the first n events (parameter).*/
  Int_t   _skipNEventsAfterConfChanges;       /**< Skip n events after configuration changes (parameter).*/
  Int_t   _skipNEvents;                       /**< Skip the next n events (not a parameter).*/

  Int_t   _skipCalibrationEventsAlways;       /**< Skip events in which the calibration chip was turned on.*/      
  Int_t   _skipNEventsAfterSlowConfRecords;   /**< Events skipped after slow or configuration records.*/
  Int_t   _skipDirtyEventsInPedestalCalculation; /**< Events skipped during pedestal calculation because they are considered to be dirty.*/

  Float_t _toleratedSaturationFraction;

  Float_t  _signalThreshold;                  /**< Signals above the signal threshold and ... */
  FloatVec _noiseCutVec;                      /**< above the noise cut are considered to be hits. This vector 
						   contains a cut for the hit search and for the hit rejection during noise 
						   calculation*/  
  Float_t  _noiseCut;                         /**< will be set by the first element of the _noiseCutVec */
  Float_t  _noiseCutForPedestalNoiseCalculation; /**< will be set by the second element of the _noiseCutVec (if available).*/

  int      _nNegativeSignalsPerModule;        /**< maximum number of negative signals per module.*/

  std::string _hitColName;           /**< Name of the hit collection (OUTPUT).*/
  std::string _rawhitColName;           /**< Name of the raw hit collection (optional OUTPUT).*/
  bool _writeRawHits;              /**< Save the output collection containing RawCalorimeterHits.*/
  bool _saveEcalNoise;            /**< Save the Noise value in the database.*/
  /** DBInit string */ 
  std::string _dbInit;
  /** Variable containing folder name for Ecal Noise */
  std::string _folderNoise;
  // -- conditions data names
  std::string _colNameDetectorTransformation; /**< Name of the conditions data collection which describes the position
						   and the rotation of the detector.*/
  std::string _colNameReferenceTransformation;/**< Name of the conditions data collection which describes the 
						   reference position and rotation.*/

  std::string _calibrationObjectName;       /**< Name of the Calibration object to be used (will be created using the CalibrationFactory)*/
  std::string _calibrationConstantColName;  /**< Name of the conditions data collection containing the calibration constants.*/

  std::string _parNameTriggerConf;          /**< Name of the event parameter which contains the current trigger configuration.*/
  std::string _parNameTriggerEvent;         /**< Name of the event parameter which contains the current trigger main word.*/

  std::string _parNameTriggerPreHistory;
  std::string _parNameTriggerPostHistory;

  std::string _parNameRecoState;            /**< Name of the event parameter which will be filled with the current state of the reconstruction.*/

  std::string _parNameEventEnergy;          /**< Parameter name of the event energy. If left blank nothing will be added.*/

  std::string _parNamePedestalCorrection;   /**< Parameter name of the event energy. If left blank nothing will be added.*/


  // -- conditions data handler
  ConditionsChangeDelegator<SimpleHitSearch> _detectorTransformationChange;/**< helper class to listen for changes of
									        the detector position or rotation.*/

  ConditionsChangeDelegator<SimpleHitSearch> _referenceTransformationChange;/**< helper class to listen for changes of
										 the reference position or rotation.*/

  void moduleTypeChanged(lcio::LCCollection* col);

  void moduleLocationChanged(lcio::LCCollection* col);

  void detectorTransformationChanged(lcio::LCCollection* col) {
    _mapping.detectorTransformation(col);
  };

  void referenceTransformationChanged(lcio::LCCollection* col) {
    _mapping.referenceTransformation(col);
  };
  
  // -- analysis results
  UInt_t _nPedestalEvents;
  UInt_t _nPedestalEventsTotal;
  UInt_t _nPedestalEventSets;
  
  UInt_t _nEvents;
  UInt_t _nEventsTotal;
  UInt_t _nEventsWithHits;
  UInt_t _nEventsWithHitsTotal;

  UInt_t _nEventsSkipped;

  UInt_t _eventCounterForMaximumRejection;

  UInt_t _uncoveredTriggerCounter;
  UInt_t _nEventsWithoutADC;
  UInt_t _rejectedEvents;
  UInt_t _rejectBecauseOfError;
  UInt_t _rejectBecauseTooManyHits;
  UInt_t _rejectBecauseOfMissingADCBlocks;
  UInt_t _nEventsWithMissingAdcBlocks;

  Int_t _viewConnectionTree;

  vector<Float_t> _moduleBuffer;
  vector<Float_t> _avNoisePerModule;

#ifdef WITH_CONTROL_HISTOGRAMS
  
  //  std::string _histogramGroupName;
  histmgr::Key_t       _histGroupKey;            /**< Key for the histogram group.*/
  histmgr::Key_t       _histSignalKey;           /**< Key for the signal histograms.*/
  histmgr::Key_t       _histNoiseKey;            /**< Key for the noise histograms.*/
  histmgr::Key_t       _histSignalInducedPedestalShiftKey;   /**< Key for the signal induced pedestal shift histograms.*/  
  histmgr::Key_t       _histNoiseCorrelationKey; /**< Key for the noise correlation histograms.*/
  histmgr::Key_t       _histTriggerEnableKey;    /**< Key for a histogram which shows enabled trigger bits.*/
  //  histmgr::Key_t       _histNoiseKey;

  void createHistograms(UInt_t n_modules);
  void fillErrorHistogram(unsigned int);
  // -- histogram collections

  // histogram parameters
  lcio::FloatVec _signalHistPar;
  lcio::FloatVec _noiseHistPar;
  lcio::FloatVec _adcHistPar;
  lcio::FloatVec _pedestalChangeHistPar;
  lcio::FloatVec _totalSignalHistPar;

  // 1d collections of 1D histograms
  enum EH1Type {kH1Signal,kH1TotalSignal,kH1ADC,
		kH1CalcNoise,    kH1FinalNoise,    kH1NoiseChange,   kH1NoiseAverage,
		kH1Pedestal,kH1CalcPedestal, kH1FinalPedestal, kH1PedestalChange, kH1PedestalCorrection,kH1SignalAverage,
		kH1ModuleEvent,kH1ModuleEventCorrection, kH1ModuleEventNoise, 
		kH1DeadCells,
		kH1NHits,kH1States,kH1Errors, 
		kH1PedSh,kH1NoPedSh,kH1NoGlobalPedCorr,
		kNH1};
  // LCCollection *_histCol[kNH1];

  histmgr::Key_t _histKey[kNH1];

  // 2d collections of 1D histograms
  enum EH1Col2DType {kH1Col2DNoiseCorrelation,kH1Col2DSignal,kH1Col2DNoise,kNH1Col2D};
  // LCCollection *_histCol2D[kNH1Col2D];
#endif
  lcio::StringVec _maskedErrorNames;          /**< Names of alle masked errors (proc. parameter)*/
  unsigned int    _errorMask;                 /**< bit mask of all errors to be ignored.*/


  Bool_t _calibrationOn;
  UInt_t _lastEvent; /* debug*/
};
}
#endif
