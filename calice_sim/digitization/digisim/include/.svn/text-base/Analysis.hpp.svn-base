//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: Analysis.hpp
//
// Purpose:
//   A Marlin processor to convert RawCalorimeterHits into calibrated
//   CalorimeterHits.
//   This processor is configured by the Marlin steering file.
//
// 20060424 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _ANALYSIS_HPP_
#define _ANALYSIS_HPP_ 1

#include "marlin/Processor.h"
#include <vector>
#include <string>

#include "TFile.h"
#include "TH1F.h"
#include "TProfile.h"

namespace digisim {

class Analysis : public marlin::Processor {
  
public:
  virtual Processor*  newProcessor() { return new Analysis; }

  // constructor
  Analysis();

  // Destructor
  ~Analysis();

public:
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;

  /** Called for every run
   */
//   virtual void processRunHeader( LCRunHeader* run ) ;

  /** Called for every event
   */
  virtual void processEvent( LCEvent* evt ) ;
  virtual void processEcalData( LCEvent* evt, std::string colName ) ;
  virtual void processHcalData( LCEvent* evt, std::string colName ) ;
  virtual void processTcmtData( LCEvent* evt, std::string colName ) ;
  virtual void processHcalRawData( LCEvent* evt, std::string colName ) ;

  /** Called after data processing for clean up.
   */
  virtual void end();

protected:
  static const int NLAYERSECAL = 30;
  static const int NLAYERSHCAL = 30;
  static const int NLAYERSTCMT = 16;

  LCEvent* _evt;
//   std::vector<std::string> _collNames;
  std::string _tcmtCollName, _ecalCollName, _hcalCollName;
  int _nRun ;
  int _nEvt ;

  TH1F *_hTcmtCellEnergy;
  TH1F *_hTcmtCellEnergyMIP;
  TH1F *_hTcmtHitsPerEvent;
  TProfile *_hTcmtLayerEnergy;
  TProfile *_hTcmtHitsPerLayer;

  TH1F *_hEcalCellEnergy;
  TH1F *_hEcalCellEnergyMIP;
  TH1F *_hEcalHitsPerEvent;
  TProfile *_hEcalLayerEnergy;
  TProfile *_hEcalHitsPerLayer;

  TH1F *_hHcalCellEnergy;
  TH1F *_hHcalCellEnergyMIP;
  TH1F *_hHcalLiveEnergyPerLayer;
  TH1F *_hHcalHitsPerEvent;
  TProfile *_hHcalLayerEnergy;
  TProfile *_hHcalHitsPerLayer;

//   TupleStructure _tuple;
  static TFile _rootFile;
};

} // end namespace digisim
#endif // _ANALYSIS_HPP_
