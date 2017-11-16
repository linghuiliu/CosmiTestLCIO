//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//
// File: DigiSimProcessor.hpp
//
// Purpose:
//   A Marlin processor to drive the digitization simulation process.
//   This processor is configured by the Marlin steering file.
//   See user guide at http://nicadd.niu.edu/digisim/ for more details.
//
// 20041112 - Guilherme Lima - Created
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
#ifndef _DIGISIMPROCESSOR_HPP_
#define _DIGISIMPROCESSOR_HPP_ 1

#include "CalHitModifier.hpp"
#include "marlin/Processor.h"
#include "CalHitMapMgr.hpp"
#include "TempCalHit.hpp"
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include <list>
#include <vector>

namespace digisim {

  class CalHitModifier;

class DigiSimProcessor : public marlin::Processor {
  
public:
  virtual Processor*  newProcessor() { return new DigiSimProcessor; }

  // constructor
  DigiSimProcessor();
  // Destructor
  ~DigiSimProcessor();

  //TempCalHitMap* DigiSimProcessor::createTempHits(const CalHitMap& hits) //const;

   //void createOutputCollections( const TempCalHitMap& hits,
//				 std::vector<EVENT::LCCollection*>& coll ) //const;

  void printRelation( const EVENT::LCCollection* col ) const;

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

protected:
  string diginame;
  LCWriter* _lcWrt ;
  int _nRun ;
  int _nEvt ;
  int _debug;

  // modifiers
  std::list<CalHitModifier*> _modifs;
  std::string _inputColl;
  std::string _outputColl;
  std::string _linksColl;
  std::vector<std::string> _modifNames;

  std::vector<Digitizer> _digitizers;
} ;

} // end namespace digisim
#endif // _DIGISIMPROCESSOR_HPP_
