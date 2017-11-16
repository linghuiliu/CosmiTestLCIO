#ifndef APPENDINTEGRALOBSERVABLES_hh
#define APPENDINTEGRALOBSERVABLES_hh 1

// LCIO includes
#include "lcio.h"
#include "EVENT/CalorimeterHit.h"

// Marlin includes
#include "marlin/Processor.h"

using namespace lcio ;
using namespace marlin ;

namespace CALICE {

  /**  @brief Processor for appending integral observables like energy sum per event to each event as event parameter
   *
   * @author N. Feege, University of Hamburg
   * @date May 2010
   *
   */
  class AppendIntegralObservables : public Processor {

  public:

    virtual Processor*  newProcessor() { return new AppendIntegralObservables ; }


    AppendIntegralObservables() ;

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

    virtual void check( LCEvent * evt ) ;

    /** Called after data processing for clean up.
     */
    virtual void end() ;


  protected:
    std::string _colNameIn;

    int _nRun ;
    int _nEvt ;

  } ;

}

#endif
