#ifndef IntegratedHcalCalibrationProcessor_h
#define IntegratedHcalCalibrationProcessor_h 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// FIXME: no magic numbers please
#define HCAL_N_MOD 39
#define HCAL_N_CELL 216

#include <cstdlib>
#include <map>

#include <lcio.h>
#include <lccd.h>

#include <IntegratedHcalProcessor.hh>
#include <marlin/Processor.h>
#include <marlin/Exceptions.h>
#include <marlin/ConditionsProcessor.h>
#include <ConditionsChangeDelegator.hh>
#include <FastCaliceHit.hh>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCFlagImpl.h>
#include <EVENT/LCCollection.h>
#include <lccd/DBInterface.hh>
#include <VRawADCValueProcessor.hh>
#include <collection_names.hh>
#include <HcalTempModel.hh>
#include <CalibrationSet.hh>
#include <GainConstants.hh>
#include <InterConstants.hh>
#include <MIPConstants.hh>

namespace CALICE {

  /** The Ahcal calibration processor.
   * @author Sebastian Schmidt
   */

class IntegratedHcalCalibrationProcessor : public IntegratedHcalProcessor {
 public:
  IntegratedHcalCalibrationProcessor();

  IntegratedHcalCalibrationProcessor* newProcessor(){return new IntegratedHcalCalibrationProcessor;}

  virtual void init();
  virtual void processEvent(lcio::LCEvent* evt);
  virtual void end();

 protected:
  std::string _inputColName;
  std::string _outputColName;
  int _zeroSuppression;
  float _significanceCut;
  int _skipPedestals;
  int _minPedNumber;
  int _pedCounter;
  int _pedestalSubtraction;
  float _mipCut;

  bool _doSaturationCorrection;

  unsigned long _hitCounter;
  unsigned long _invalidMIPCounter;
  unsigned long _invalidSaturationCorrectionCounter;
  unsigned long _saturationCounter;
  unsigned long _eventCounter;
  std::map<unsigned, unsigned> _invalidMIPCalibrations;
  std::map<unsigned, unsigned> _invalidSaturationCorrections;
  std::map<unsigned, unsigned> _saturations;

  double _pedSum[HCAL_N_MOD+1][HCAL_N_CELL];
  double _pedSumSquare[HCAL_N_MOD+1][HCAL_N_CELL];
  unsigned _pedNum[HCAL_N_MOD+1][HCAL_N_CELL];
  float _ped[HCAL_N_MOD+1][HCAL_N_CELL];
  float _pedWidth[HCAL_N_MOD+1][HCAL_N_CELL];
  float _pedError[HCAL_N_MOD+1][HCAL_N_CELL];
};

}

#endif
