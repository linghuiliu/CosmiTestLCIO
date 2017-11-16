#ifndef ClusterPatternProcessor_hh
#define ClusterPatternProcessor_hh 1

// Marlin includes
#include "marlin/Processor.h"

// LCIO includes
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"

// c++ includes
#include <string>

// CALICE includes

using namespace marlin;
using namespace lcio;

namespace CALICE {


  /** Processor which analyzes clusters (e.g. provided by the ShowerStartCusterProcessor) in an event to extract maximum energy and hits in a single cluster or the overlap of separate clusters.
   *
   * @author nils.feege@desy.de
   * @date April 2011
   */

  class ClusterPatternProcessor : public Processor {
  public:

    ClusterPatternProcessor();

    virtual Processor*  newProcessor() { return new ClusterPatternProcessor ; }

    /* Called at the begin of the job before anything is read.
     */
    virtual void init() ;

    /* Called for every run.
     */
    virtual void processRunHeader(LCRunHeader *run);

    /* Called for every event.
     */
    virtual void processEvent( LCEvent * evt ) ;

    /* Called after data processing for clean up.
     */
    virtual void end() ;

  private:

    std::string _stringColNames;

    StringVec _explicitColNames;

    int           _minHitsCluster;
    float         _minEnergyCluster;

  };

} // end namespace CALICE


#endif
