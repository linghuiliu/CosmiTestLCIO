//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: CalorimeterHitsProcessor.hpp
//
// Purpose:
//   A Marlin processor to convert RawCalorimeterHits into calibrated
//   CalorimeterHits.
//   This processor is configured by the Marlin steering file.
//
// 20060424 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _CALORIMETERHITSPROCESSOR_HPP_
#define _CALORIMETERHITSPROCESSOR_HPP_ 1

#include "marlin/Processor.h"
#include "RawHitConverter.hpp"
#include <vector>
#include <string>

namespace digisim {

class CalorimeterHitsProcessor : public marlin::Processor {
  
public:
  virtual Processor*  newProcessor() { return new CalorimeterHitsProcessor; }

  // constructor
  CalorimeterHitsProcessor();

  // Destructor
  ~CalorimeterHitsProcessor();

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

  /** Called after data processing for clean up.
   */
  virtual void end();

private:
  EVENT::LCCollection* createOutputCollection( const std::vector<const IMPL::CalorimeterHitImpl*>& newhits );

protected:
  LCEvent* _evt;
  std::string _rawName;
  std::string _outputName;
  std::string _posRefName;
  double _eneFactor;
  double _timeFactor;
  int _nRun ;
  int _nEvt ;
  bool _positionsFromFile;
  bool _deferPositionAssignments;
  std::map<long long, std::vector<double> > _positionsMap;

  RawHitConverter* _converter;
};

} // end namespace digisim
#endif // _CALORIMETERHITSPROCESSOR_HPP_
