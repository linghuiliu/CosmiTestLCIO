#ifndef HCALTMVAHISTOS_HH
#define HCALTMVAHISTOS_HH 1

#include "marlin/Processor.h"
#include "EVENT/LCEvent.h"
#include "lcio.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TPluginManager.h"
#include "TH1F.h"

#include "AhcMapper.hh"


class HcalTMVAProcessor : public marlin::Processor
{
public:
  HcalTMVAProcessor();
  virtual ~HcalTMVAProcessor();
  
  HcalTMVAProcessor* newProcessor(){return new HcalTMVAProcessor();}
  
  virtual void init();
  virtual void processEvent(LCEvent*);
  virtual void end();
  
protected:
  int cellsize( int cellid, bool coarse = false );
  int getMarinaShowerStartHcalLayer(LCEvent *evt, std::string showerStartColName);
  bool tbtrackLoop(LCEvent *evt);

  void fillRootTree(LCEvent* evt);
  void TMVAClassification(std::string TMVAMethodName, std::string signalFileName, std::string backgroundFileName, float signalWeight, float backgroundWeight, std::string argumentForPrepareTrainingAndTestTree, std::string argumentForBookMethod); 
  void TMVAClassificationApplication(std::string TMVAMethodName, std::string signalFileName); 

  std::string _showerStartColName;
  std::string _ptfTrackColName;
  std::string _hcalColName;
  std::string _ecalColName;
  std::string _fexTrackColName;
  std::string _feyTrackColName;
  
  //-TMVA variables------------------
  bool _FillRootTree;
  bool _TMVA_Training;
  bool _TMVA_Analysis;
  float _signalWeight;
  float _backgroundWeight; 
  std::string _TMVA_Method;
  std::string _TMVA_Training_SignalFile;
  std::string _TMVA_Training_BackgroundFile;
  std::string _TMVA_Analysis_SignalFile;
  std::string _argumentForPrepareTrainingAndTestTree;
  std::string _argumentForBookMethod;
  
  //---------------------------------

  float _fxTrkAtHcalFront;
  float _fyTrkAtHcalFront;
  

  std::string _mappingProcessorName;
  const CALICE::AhcMapper *_mapper;
    
  std::string _rootFileName;
  std::string _rootFileMode;
  TFile *_rootFile;
  
  TTree *_rootTree;
  float _energySum;
  float _energy1Momentum;
  float _energy2Momentum;
  float _energy3Momentum;
  float _energyRatio;
  float _R90_hits;
  float _N90ratioN;
  float _hitsMean;
  float _energyDensity;
  int _layerMaxDiffStart;
  int _startLayer;
  int _nLayerMaxEnergy;

  /*
    HCAL histograms 
   */
  TH1F *_hLong;
  TH1F *_hLong_fromShowerStart;
  TH1F *_hEnergySum;
  TH1F *_hNumHits;
  TH1F *_hShowerStart;
  TH1F *_hLayerMaxEnergy;
  TH1F *_hNumHitsPerLayer;
  TH1F *_hHitEnergy;

  TH1F *_hCogX;
  TH1F *_hCogY;

  /*
    TRACKS histograms 
   */
  TH1F *_hTrkXSlope;
  TH1F *_hTrkXOffset;
  TH1F *_hTrkXAtHcalFront;
    
  TH1F *_hTrkYSlope;
  TH1F *_hTrkYOffset;
  TH1F *_hTrkYAtHcalFront;
     
};

#endif
