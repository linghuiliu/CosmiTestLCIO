/**//////////////////////////////////////
//Created March 2007 - A.-M.Magnan.
/////////////////////////////////////////*/
#ifndef _CalibrateAndApplyThreshold_H_
#define _CalibrateAndApplyThreshold_H_
#include <cassert>

#include "TH1F.h"
#include "TFile.h"
#include "TString.h"

#include <EVENT/LCParameters.h>
#include <EVENT/LCCollection.h>
#include <marlin/Processor.h>
#include <ConditionsChangeDelegator.hh>

#include <NoiseParameterArray_t.hh>
#include <collection_names.hh>
#include <Calibration.hh>
#include <VRawADCValueProcessor.hh>
#include <RunInformation.hh>


namespace CALICE {//namespace CALICE

  /** Marlin Processor to go from RawCalorimeterHit to calibrated 
   * CalorimeterHits.
   * The hit position is calculated using information stored in a 
   * the cellID1. Moreover, the signal is calibrated
   * if a proper calibration object exists.
   * should run on Data as well as on MC after digisim.
   * @todo (Re-)Verify its applicability for MC after modifs for this release!!!!
   */


  class CalibrateAndApplyThreshold : public VRawADCValueProcessor
  {//class
  public:
    /** construct a new Processor (required by Marlin).
     */
    Processor*  newProcessor() { return new CalibrateAndApplyThreshold ; }

    /** default constructor.
     * defines and registers its parameters.
     */
    CalibrateAndApplyThreshold();
    
    /** destructor.
     */
    ~CalibrateAndApplyThreshold();
    

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run (does nothing.)
     */
    void processRunHeader( LCRunHeader* run);

    /** Reconstruct calorimeter hits.
     * The class needs as input the RawCalorimeterHit collection from SimpleHitSearch or digisim
     * and produces a CalorimeterHit collection.
     * <P>
     */
    void processEvent( LCEvent * evtP );

  
    void end();

  protected:


   private:

    Calibration *_calibration;
    LCCollection *_cellCol;
    //array or array containing all the cells noise and pedestals (<vector containing a <vector of those parameters per cells> per module>)
    //inherits from LCFixedObject, so can be filled directly from a LCCollection.
    //NoiseParameterArray_t _cellParameter; 
    std::string _cellParameterCollectionName;

    // -- parameters of the analysis
    IntVec   _adcRange;
    Float_t  _signalThreshold;                  /**< Signals above the signal threshold and ... */
    FloatVec _noiseCutVec;                      /**< above the noise cut are considered to be hits. This vector 
						   contains a cut for the hit search and for the hit rejection during noise 
						   calculation*/  
    Float_t  _noiseCut;                         /**< will be set by the first element of the _noiseCutVec */ 

    std::string _rawhitColName;           /**< Name of the hit collection (INPUT).*/
    std::string _hitColName;           /**< Name of the hit collection (OUTPUT).*/

    bool _linkRecToSim;          /**< kkk flag to disable/enable the creation of an LCRelation to simulated hit**/
    std::string _RecToSimColName;      /**< kkk: Name of the relation collection hit -> simhit (OUTPUT).*/
    std::string _RawToSimColName; /**< Name of the relation collection between Sim and Raw hits (INPUT).*/

    std::string _calibrationObjectName;       /**< Name of the Calibration object to be used (will be created using the CalibrationFactory)*/
    std::string _calibrationConstantColName;  /**< Name of the conditions data collection containing the calibration constants.*/

    Bool_t _calibrationOn;
    UInt_t _lastEvent; /* debug*/
    UInt_t _nEventsWithoutRAWHITS;
    UInt_t _nEventsWithoutSIMHITS;
    UInt_t _nEvt;
    Int_t _saveHistograms;


    //int _isMC;

    RunInformation _runInfo;

    TString _rootname;
    TFile* _hfile;
    TH1F *p_noise[30][324];
    TH1F *p_rawAmpl[30][324];
    TH1F *p_pedCorr[30][324];
    TH1F *p_calVal[30][324];
    TH1F *p_sig[30][324];
    TH1F *p_SoN[30][324];

    // -- conditions data names
    std::string _colNameDetectorTransformation; /**< Name of the conditions data collection which describes the position
						   and the rotation of the detector.*/
    std::string _colNameReferenceTransformation;/**< Name of the conditions data collection which describes the 
						   reference position and rotation.*/

    // -- conditions data handler
    ConditionsChangeDelegator<CalibrateAndApplyThreshold> _detectorTransformationChange;/**< helper class to listen for changes of
									        the detector position or rotation.*/

    ConditionsChangeDelegator<CalibrateAndApplyThreshold> _referenceTransformationChange;/**< helper class to listen for changes of
										 the reference position or rotation.*/

    void detectorTransformationChanged(lcio::LCCollection* col) {
      _mapping.detectorTransformation(col);
    };

    void referenceTransformationChanged(lcio::LCCollection* col) {
      _mapping.referenceTransformation(col);
    };

    //CRP Define a shift for cells which cannot be properly handled since they were present in the simulation but
    //not in the data (e.g. DESY 2005, CERN 2006/2007
    float _fallbackShift; 

  };//class

}//namespace

#endif
