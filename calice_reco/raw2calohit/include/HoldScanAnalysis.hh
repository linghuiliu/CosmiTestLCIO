#ifndef _HoldScanAnalysis_hh_
#define _HoldScanAnalysis_hh_

#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 
#include <string>
#include <iostream>
#include <marlin/Processor.h>
#include <lccd/IConditionsChangeListener.hh>
#include <VRawADCValueProcessor.hh>
#include <ModuleIndexReverseLookup.hh>
#include <ConditionsChangeDelegator.hh>

#include "histmgr/Key_t.hh"

// FIXME: Should go into another namespace (?)
namespace CALICE {

  /** Create Signal histograms per pad of all hits of all reconstructed clusters.
   * This class derives from VRawADCValueProcessor, since the information about the 
   * detector is needed to setup the signal histograms.
   */
  
class HoldScanAnalysis : public VRawADCValueProcessor
{
public:
  Processor*  newProcessor() { return new HoldScanAnalysis ; }

  HoldScanAnalysis();
    
  ~HoldScanAnalysis();
    

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
  void createHistograms(); /**< create and register histogram collections. */

  /** Handle changes of Fe configuration data.
   */
  void feConfChanged(EVENT::LCCollection *col);
  FloatVec _signalHistPar;          /**< binning of the per hold value histograms.*/
#ifdef WITH_CONTROL_HISTOGRAMS
  FloatVec _clusterSignalHistPar;   /**< binning of the total cluster signal histograms.*/
#endif

  std::string _clusterColName;
  std::string _feConfColName;
  histmgr::Key_t _histGroupKey;            /**< Key for the histogram group.*/


  int  _nHistMax;                /**< Maximum number of hold values for which signal histograms can be created (parameter) */
  int  _minNumberOfHits;         /**< Minimum number of hits of accepted clusters (parameter).*/
  Float_t _minClusterSignal;        /**< Minimum signal of accepted clusters (parameter).*/


  void moduleTypeChanged(lcio::LCCollection* col);
  void moduleLocationChanged(lcio::LCCollection* col);
  void moduleConnectionChanged(lcio::LCCollection* col);

  ConditionsChangeDelegator<HoldScanAnalysis> _feConfChange;

  ModuleIndexReverseLookup _indexLookup;

  LCCollection * _nHitsHistogram;
  LCCollection * _clusterSignalHistogram;
  LCCollection * _holdValueContainer;

  enum EHistType {kHistSignalPerHoldValue,
		  kHistTotalSignalPerHoldValue,
		  kNHist
  };
  histmgr::Key_t _histKey[kNHist];

  UInt_t _nHist;
  std::map<int,UInt_t> _holdStartMap;
  std::vector<UInt_t>  _histogramIndex;

  UInt_t _nClusters;
  UInt_t _nMissingHoldChanges;
  UInt_t _moduleIndexOutOfRange;
  UInt_t _nBeamOrCosmicsEvents;
  UInt_t _currentCommonHoldStart;
};


}
#endif
