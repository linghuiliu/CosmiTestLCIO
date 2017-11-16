//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: CalHitMapProcessor.hpp
//
// Purpose: A Marlin processor wrapper, this class makes the singleton
//          CalHitMapMgr available within the Marlin framework.
//
// CalHitMapMgr is a convenience class, which makes calorimeter hit
// maps readily available to any other C++ class.  Client classes
// don't need to fill the hit maps themselves, and once filled, hit
// maps will be reused, optimizing the access to the calorimeter hits.
//
// This processor is configured by the Marlin steering file.
// Please check the CalHitMapMgr header file for usage instructions.
//
// 20041112 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _CALHITMAPPROCESSOR_HPP_
#define _CALHITMAPPROCESSOR_HPP_ 1

#include "marlin/Processor.h"
#include "CalHitMapMgr.hpp"

// #include "lcio.h"
// using namespace lcio ;

namespace digisim {

class CalHitMapProcessor : public marlin::Processor {
  
public:
  virtual Processor*  newProcessor() { return new CalHitMapProcessor; }
  
  // constructor
  CalHitMapProcessor();
  // Destructor
  ~CalHitMapProcessor();

public:
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() {};

  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run );

  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 


  /** Called after data processing for clean up.
   */
  virtual void end() {};


protected:
  int _nRun ;
  int _nEvt ;
  CalHitMapMgr* _mgr;
};

} // end namespace digisim 
#endif // _CALHITMAPPROCESSOR_HPP_
