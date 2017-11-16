#ifndef SimpleHcalCalibrationProcessor_h
#define SimpleHcalCalibrationProcessor_h 1

#include <lcio.h>
#include <marlin/Processor.h>

// FIXME: no magic numbers please
#define HCAL_N_MOD 39
#define HCAL_N_CELL 216

namespace CALICE {

/** Processor which applies all calibration constants needed for the reconstruction of the HCAL, 
  * using input from an ASCII file which lists all the calibration constants.
  * @author N. Meyer
  * @date Dec 2006
  **/

class SimpleHcalCalibrationProcessor : public marlin::Processor{
 public:
  SimpleHcalCalibrationProcessor();
  ~SimpleHcalCalibrationProcessor();

  SimpleHcalCalibrationProcessor* newProcessor(){return new SimpleHcalCalibrationProcessor;}

  virtual void init();
  void processEvent(lcio::LCEvent* evt);
  virtual void end();

 protected:
  //  Processor parameter
  std::string _inputFileName;
  std::string _inputColName;
  std::string _outputColName;
  std::string _correction;

  //  global parameters
  float _Npix;
  float _xTalk;
  float _alpha;  

  bool _calcPed;
  float _pedSum[HCAL_N_MOD+1][HCAL_N_CELL];
  float _pedSumSquare[HCAL_N_MOD+1][HCAL_N_CELL];
  int _pedNum[HCAL_N_MOD+1][HCAL_N_CELL];
  int _pixCut;
  int _pedEvents;
  bool _pedExists;

  //  calibration constants
  float _mip[HCAL_N_MOD+1][HCAL_N_CELL];
  float _gain[HCAL_N_MOD+1][HCAL_N_CELL];
  float _ic[HCAL_N_MOD+1][HCAL_N_CELL];
  float _sat[HCAL_N_MOD+1][HCAL_N_CELL];
  float _ped[HCAL_N_MOD+1][HCAL_N_CELL];
  short _stat[HCAL_N_MOD+1][HCAL_N_CELL];

  //  routines
  void resetPedestal();
  void sumupPedestal(LCCollection *);
  void calcPedestal();


};

}

#endif
