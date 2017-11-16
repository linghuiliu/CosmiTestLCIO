#ifndef ShowerStartClusterProcessor_hh
#define ShowerStartClusterProcessor_hh 1

// Marlin includes
#include "marlin/Processor.h"

// LCIO includes
#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"

// c++ includes
#include <string>

// CALICE includes
#include "Mapper.hh"
#include "MappedContainer.hh"
#include "CellNeighbours.hh"


using namespace marlin;
using namespace lcio;

namespace CALICE {

  class ShowerStartClusterProcessor : public Processor {
  public:

    ShowerStartClusterProcessor();

    virtual Processor*  newProcessor() { return new ShowerStartClusterProcessor ; }

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
    void collectHits( MappedContainer<CalorimeterHit>& allHits, LCCollectionVec* collection, const int cellID );
    void testForShowerStart( LCCollection *collection, std::vector<bool> &alreadyFound, FloatVec& showerStartPosition );

    std::string _colNameAhc;
    std::string _mappingProcessorName;
    std::string _cellNeighboursProcessorName;
    std::string _cellIDEncoding;
    int         _stopAfterFirstMatch;
    float       _seedThreshold;
    int         _minHitsCluster;
    IntVec      _hitThreshold;
    FloatVec    _energyThreshold;
    FloatVec    _maximumAngleDeviation;
    int         _minHitThreshold;
    float       _minEnergyThreshold;
    float       _maxMaximumAngleDeviation;
    FloatVec    _posIP;

    const Mapper* _mapper;
    unsigned int _mapperVersion;

    MappedContainer<CellNeighbours>* _cellNeighbours;

  };

  class SortByZPosition {
  public:
    bool operator() (const CalorimeterHit* hit, const CalorimeterHit* ref);
  private:
  };

} // end namespace CALICE


#endif
