#ifndef CELL_ANALYSIS_PROCESSOR_HH
#define CELL_ANALYSIS_PROCESSOR_HH 1

#include <marlin/Processor.h>

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"

/*LCIO*/
#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/ClusterImpl.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCEvent.h"
#include "EVENT/LCIntVec.h"
#include "EVENT/Cluster.h"
#include "marlin/Processor.h"
#include "UTIL/LCTypedVector.h"

/*CALICE*/
#include "AhcMapper.hh"
#include "CellDescription.hh"



/** Processor filling histograms for every cell identified by module, chip, 
    *  channel.
    */

class CellAnalysisProcessor : public marlin::Processor 
{
public:
  CellAnalysisProcessor();
  virtual ~CellAnalysisProcessor();
  
  CellAnalysisProcessor* newProcessor() 
  { 
    return new CellAnalysisProcessor();
  }
  
  virtual void init();
  virtual void processEvent(lcio::LCEvent*);
  virtual void end();
  virtual void openInputCollections(lcio::LCEvent*);
  
protected:

private:  
  void writeHistograms();
    
  std::string _inputMuonHcalColName;
  std::string _inputMuonTcmtColName;
  std::string _rootfilename;
  std::string _treeName;
  std::string _mappingProcessorName;
  
  static const unsigned int MAXNRMODULES  = 38;
  static const unsigned int MAXNRCHIPS    = 12;
  static const unsigned int MAXNRCHANNELS = 18;
  static const unsigned int MAXTCMTLAYERS = 16;
  static const unsigned int MAXTCMTSTRIPS = 20;
  
  LCCollection *_tcmtInputCol;/**<TCMT input collection*/
  LCCollection *_hcalInputCol;/**<HCAL input collection*/
  
  TFile* _rootout;
  TH1F* _tcmtenergyhistos[MAXTCMTLAYERS][MAXTCMTSTRIPS];
  TH1F* _tcmtlayerenergyhistos[MAXTCMTLAYERS];
  TH1F* _hitenergyhistos[MAXNRMODULES][MAXNRCHIPS][MAXNRCHANNELS];
  TH1F* _hitenergyPerModulehistos[MAXNRMODULES];
  TH1F* _hitenergyAll;
  TH1F* _histo_angle;
  TH2F* _histo_angle_length;
  TH1F* _histo_length;
  TH1F* _hMuonHcalESum;

  int _hitenergyhistobins;
  float _hitenergyhistomin;
  float _hitenergyhistomax;
  int _calibrationSpectra;
  
  bool _useTCMT;
  
  const CALICE::AhcMapper *_ahcMapper;
  
};

#endif
