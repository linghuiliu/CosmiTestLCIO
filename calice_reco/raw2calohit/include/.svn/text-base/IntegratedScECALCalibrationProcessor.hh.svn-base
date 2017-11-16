// Version 0.1 Nov-10 2008 S.Uozumi

#ifndef IntegratedScECALCalibrationProcessor_h
#define IntegratedScECALCalibrationProcessor_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <TROOT.h>
#include <TH2F.h>
#include <TFile.h>

#include <iostream>
#include <fstream>
#include <utility>

#include <cstdlib>
#include <map>

#include <lcio.h>
#include <lccd.h>

#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/ConditionsProcessor.h>
#include "lccd/IConditionsChangeListener.hh"
#include <ConditionsChangeDelegator.hh>
#include <FastCaliceHit.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <EVENT/LCCollection.h>
#include <lccd/DBInterface.hh>
#include <VRawADCValueProcessor.hh>
#include <collection_names.hh>
#include <HcalTempModel.hh>
//#include <CalibrationSet.hh>
//#include <GainConstants.hh>
//#include <InterConstants.hh>
//#include <MIPConstants.hh>

#include "ScECALParameters.hh"

namespace CALICE {

  //class IntegratedScECALCalibrationProcessor : public IntegratedScECALProcessor {
class IntegratedScECALCalibrationProcessor : public marlin::Processor, public lccd::IConditionsChangeListener {
 public:

  IntegratedScECALCalibrationProcessor();

  IntegratedScECALCalibrationProcessor* newProcessor(){return new IntegratedScECALCalibrationProcessor;}

  virtual void init();
//110512.0835.coterra
  virtual void processRunHeader(LCRunHeader* run);
  virtual void processEvent(lcio::LCEvent* evt);
  virtual void conditionsChanged(LCCollection *col);
  virtual void end();

 protected:
//110723.2252 To skip used timestamp
  int _listReadCounter;
//110722.2304
 std::map<long64, float> _templist;
//110724.1046
 std::map<int, map<long64, float> > _tempOrder;

//110712.1759.coterra
  int  _dataYear;
  bool _forMIPCalibration;
  bool _tempcorrgainOn;
  bool _tempcorrmipOn;
  std::vector<float> _tempcorr_gain;
  std::vector<float> _tempcorr_mip; 
  std::vector<int> _offsetTimeStamp;
//  std::vector<double> _ondotori2stamp; 
  std::vector<int> _ondotori2stamp; 
//  std::vector<float> _ondotori2stamp; 
//110514.1013.coterra 
  int _nnoisy;
//  enum{ TOTNSTRIP = 2160 };
  int  _noisychanLayer[ScECAL_NLAYERS * ScECAL_NSTRIPS]; //TOTNSTRIP];
  int  _noisychanStrip[ScECAL_NLAYERS * ScECAL_NSTRIPS];
// 
  //for the saturation correction. coterra,
  float _effNpix;
  //
  std::string _inputColName;
  std::string _outputColName;
//no use?110512.coterra  std::string _rootfileName;
  bool _zeroSuppression;
  float _significanceCut;
  bool _skipPedestals;
  int _minPedNumber;
  int _pedCounter;
  bool _pedestalSubtraction;
  float _mipCut;
  unsigned long _hitCounter;
  unsigned long _invalidMIPCounter;
  unsigned long _invalidSaturationCorrectionCounter;
  unsigned long _saturationCounter;
  unsigned long _eventCounter;
//121004.coterra
  std::vector<float> _gainLim; 
  std::vector<float> _gainErrLim; 
  std::vector<float> _uniformGainSlope; 
  
  
//110511.coterra
//  long _offsetTimeStamp;
  std::map<unsigned, unsigned> _invalidMIPCalibrations;
  std::map<unsigned, unsigned> _invalidSaturationCorrections;
  std::map<unsigned, unsigned> _saturations;
  double _pedSum[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _pedSumSquare[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  unsigned _pedNum[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  float _ped[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  float _pedError[ScECAL_NLAYERS][ScECAL_NSTRIPS];

  double _mipcalibconst[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _mipcalibconst_err[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _mipSlope[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _mipSlopeErr[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _mipSetTemp[ScECAL_NLAYERS][ScECAL_NSTRIPS];

  double _gaincalibconst[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _gaincalibconst_err[ScECAL_NLAYERS][ScECAL_NSTRIPS];
//121004.coterra
  double _gainSlope[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _gainSlopeErr[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _gainSetTemp[ScECAL_NLAYERS][ScECAL_NSTRIPS];
//
  double _intercalibconst[ScECAL_NLAYERS][ScECAL_NSTRIPS];
  double _intercalibconst_err[ScECAL_NLAYERS][ScECAL_NSTRIPS];

  //TH1F* _sumHist;
  //Add for 
  std::string _ScECALMIPColName;
  LCCollection *_ScECALMIPCol;
  bool _MIPConstantChanged;
//110508.coterra
  std::string _ScECALIntCalibColName;
  LCCollection *_ScECALIntCalibCol;
  bool _IntCalibConstChanged;
//110511.coterra
  std::string _ScECALGainColName;
  LCCollection *_ScECALGainCol;
  bool _GainConstChanged;
  std::string _ScECALTemperatureColName;
  LCCollection *_ScECALTemperatureCol;
  bool _TemperatureChanged; 
//121003.coterra
  std::string _ScECALNoisyChannelColName;
    LCCollection *_ScECALNoisyChannelCol;
      bool _NoisyChannelChanged;
};

}

#endif
