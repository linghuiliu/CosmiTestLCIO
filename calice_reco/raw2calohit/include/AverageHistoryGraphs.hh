#ifndef _AverageHistoryGraphs_hh_
#define _AverageHistoryGraphs_hh_

#include <string>
#include <vector>
#include <iostream>
#include <VRawADCValueProcessor.hh>
#include <ConditionsChangeDelegator.hh>
#include <Average_t.hh>

#include <histmgr/Key_t.hh>


// FIXME: Should go into another namespace (?)
namespace CALICE {

class AverageHistoryGraphs : public VRawADCValueProcessor
{
public:
  Processor*  newProcessor() { return new AverageHistoryGraphs ; }

  AverageHistoryGraphs();
    
  ~AverageHistoryGraphs() {};
    
    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
  void init();

  /** Called for every run, e.g. overwrite to initialize run dependent 
   *  histograms.
   */
  void processRunHeader( LCRunHeader* run) { } 

    /** Called for every event - the working horse.      */
  void processEvent( LCEvent * evtP );

  void end();

protected:
  Int_t _useTimeStamp;
  IntVec  _eventPar;

  //  std::string _outputFileName;
  //  std::string _folderName;

  Int_t _avADCNMax;

  Float_t _signalCut;

  Int_t _skipCalibrationEventsAlways; /*< If not set equal to zero events for which the calibration chip is on are skipped.*/

  std::string _cellParameterCollectionName;


  enum EGraphType {kGraphMean,kGraphRMS,kGraphMin,kGraphMax, kGraphWeight, kNGraphTypes};
  enum EHistoryType {kADCHistory, kNoiseHistory, kPedestalHistory, kHitHistory, kNHistoryGraphs};
  enum EStampType {kHistoryStamp,  kHistoryConfStamp,kHistoryStateStamp,kHistoryStateValue,kNHistoryStamps};

  static const char *__graphTypeNames[kNGraphTypes];
  static const char *__historyGraphNames[kNHistoryGraphs];

  std::vector< std::vector< Average_t > > _avValues[kNHistoryGraphs];
  bool _avADCIsValid;
  UInt_t _avADCn;
  std::vector< std::vector< Average_t > > _avADC;
  std::vector< std::vector< UInt_t > >    _nHits;

  //  std::vector< std::vector< std::vector< Float_t > >  > _history[kNHistoryGraphs][kNGraphs];
  //  std::vector< Double_t >                _historyStamps[kNHistoryStamps];

  StringVec _monitorConf;
  vector< ConditionsChangeDelegator<AverageHistoryGraphs> >  _confChanges;
  bool _confChanged;
  UInt_t _nConfigurationChanges;

  void configurationChanged(EVENT::LCCollection *) {
    _confChanged=true;
    _nConfigurationChanges++;
  };

  void moduleTypeChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleTypeChanged(col);
    resizeArrays();
  };

  void moduleLocationChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleLocationChanged(col);
    resizeArrays();
  };

  void moduleConnectionChanged(lcio::LCCollection* col) {
    VRawADCValueProcessor::moduleConnectionChanged(col);
    resizeArrays();
  };

  void resizeArrays();
  //  void writeGraphs();


  Int_t _wasState;
  UInt_t _nStateChanges;
  
  static UInt_t __nSamplesPerChip;
  static UInt_t __nChips;

  histmgr::Key_t _graphGroupKey;
  std::vector<histmgr::Key_t> _avValueHistoryKey;
  histmgr::Key_t _stateChangeHistoryKey;
  histmgr::Key_t _confChangeHistoryKey;

};


}
#endif
