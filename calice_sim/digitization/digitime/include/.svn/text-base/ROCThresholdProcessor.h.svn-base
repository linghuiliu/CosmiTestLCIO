#ifndef ROCThresholdProcessor_h
#define ROCThresholdProcessor_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>


using namespace lcio ;
using namespace marlin ;
using std::vector;

/* 
 * @author M. Ramilli, 2014
 */

class ROCThresholdProcessor : public Processor {
 private:
  //processor parameters:
  float _tileEdge;
  float _deadSpace;
  float _MIPvalue;
  float _MIPThr;
  float _tfast;
  float _tslow;



  
 public:
  
  virtual Processor*  newProcessor() { return new ROCThresholdProcessor ; }  
  
  ROCThresholdProcessor() ;  

  virtual void init() ;  

  virtual void processRunHeader( LCRunHeader* run ) ;  

  virtual void processEvent( LCEvent * evt ) ;   
  
  virtual void check( LCEvent * evt ) ; 
  
  virtual void end() ;
  
  virtual void printParameters();

 protected:
  std::string _calorimInpCollection;
  std::string _calorimOutCollection;

  float energyThreshold;
  float halfdS;

  int _nRun ;
  int _nEvt ;
} ;

#endif



