#ifndef _TBEcalDigitisation_hh_
#define _TBEcalDigitisation_hh_ 1
#include <list>
#include <vector>

#include "TRandom3.h"

#include "lcio.h"
#include "EVENT/LCCollection.h"

#include "NoOpCalibration.hh"
#include "ModuleIndexReverseLookup.hh"
#include "NoiseParameterArray_t.hh"
#include "MappingAndAlignment.hh"
#include "Calibration.hh"
#include "CaliceEcalCalibration.hh"

#include "marlin/Processor.h"

namespace CALICE {

/** File: TBEcalDigitisation.hpp
 *
 * Purpose:
 * Digitisation step for the ECAL TB Prototype
 * Based on digisim (author G.Lima), see http://nicadd.niu.edu/digisim/
 *
 * @author  A.-M. Magnan UCL
 * @date april 2007, A.-M. Magnan
 */


class TBEcalDigitisation : public marlin::Processor {
  
public:
  virtual Processor*  newProcessor() { return new TBEcalDigitisation; }

  // constructor
  TBEcalDigitisation();
  // Destructor
  ~TBEcalDigitisation();

public:
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;

  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;

  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ;

  /** Called after data processing for clean up.
   */
  virtual void end() ;

  void DecalibrateHits(double& hitenergy, int cellID0, int& cellID1);
  void FillRandomNoise();
  void setNoiseCellIDs(int & cellID0, int & cellID1, int lay, int waf, int ch);
  static CALICE::NoiseParameterArrayofArray_t & getCellParameters();


private:

  int _nRun ;
  int _nEvt ;
  int _debug;
  int _runnum;

  double _mipEnergy;
  std::string _inputColName;
  std::string _outputColName;
  std::string _relColName;
  std::string _cellParameterCollectionName;
  std::string _calibrationObjectName;
  std::string _calibrationConstantColName;
  double _dummyCalib;
  std::string _moduleConnection;
  std::string _moduleDescription;
  std::string _moduleLocation;
  int _inputSeed;

  //calibration and alignment objects to fill with the constants accessed from the database
  Calibration *_calib;
  CALICE::MappingAndAlignment _mapping;
  CALICE::ModuleIndexReverseLookup _convert;

  /** Patch RP: declare cell collection data member here in order to allow for protection of program if it is missing */
  //CRP 25/11/15 Remark no further check for non null pointer needed since LCCD either returns a collection or
  //throws an exception, see e.g. DBCondHandler.cc in LCCD 
  //LCCollection *_cellCol;

  TRandom3 _randnum;
  double _noise[30][9][36];
  double _cohnoise[30];
  double _pedestal[30][9][36];
  double _randNoise[30][9][36];
  unsigned int _maxK;
  unsigned int _maxS;
  unsigned int _maxM;
  unsigned int _maxI;
  unsigned int _maxJ;
  bool _there[30][9][36];

} ;

} // end namespace CALICE

#endif // _TBEcalDigitisation_hh_
