//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: SimCalorimeterHitsProcessor.hpp
//
// Purpose:
//   A Marlin processor to convert RawCalorimeterHits into calibrated
//   SimCalorimeterHits.
//   This processor is configured by the Marlin steering file.
//
// 20060424 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _SIMCALORIMETERHITSPROCESSOR_HPP_
#define _SIMCALORIMETERHITSPROCESSOR_HPP_ 1

#include "marlin/Processor.h"
#include "Raw2SimConverter.hpp"
#include "EVENT/RawCalorimeterHit.h"
#include "EVENT/LCRelation.h"
#include <vector>
#include <string>

namespace digisim {

class SimCalorimeterHitsProcessor : public marlin::Processor {
  
public:
  virtual Processor*  newProcessor() { return new SimCalorimeterHitsProcessor; }

  // constructor
  SimCalorimeterHitsProcessor();

  // Destructor
  ~SimCalorimeterHitsProcessor();

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
  EVENT::LCCollection* createOutputCollection( const std::vector<const IMPL::SimCalorimeterHitImpl*>& newhits ) ;


  void clearLinksMap();


protected:
  LCEvent* _evt;
  std::vector<std::string> _rawNames;
  std::vector<std::string> _linkNames;
  std::vector<std::string> _outputNames;
  double _eneFactor;
  double _timeFactor;
  int _nRun ;
  int _nEvt ;

  std::map<const EVENT::RawCalorimeterHit*, std::vector<const EVENT::LCRelation*>*>* _linksMap;
  Raw2SimConverter* _converter;
};

} // end namespace digisim
#endif // _SIMCALORIMETERHITSPROCESSOR_HPP_
