#ifndef AHC_VETO_REGION_H
#define AHC_VETO_REGION_H

// c++ includes
#include <iostream>

// Marlin includes
#include "marlin/Processor.h"

// CALICE includes
#include "AhcMapper.hh"
#include "MappedContainer.hh"
#include "CellDescription.hh"

namespace CALICE {

  /** Processor which generates a trigger bit based on AHCAL information.
   * Threshold parameters are vectors, i.e. a series of different veto conditions
   * can be defined for one instance of this processor.
   * The trigger bit can be used by other processors, e.g. the EventSelector.
   * @author N. Feege
   * @date Aug 2009
   */

  class AhcVetoRegion : public marlin::Processor  {
  public:
    Processor*  newProcessor() { return new AhcVetoRegion ; }

    AhcVetoRegion();

    ~AhcVetoRegion() {};

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run);

    /** Called for every event - this is where the real action is taking place. */
    void processEvent( LCEvent * evt );

    /** Called after data processing for clean up.
     */
    void end();

  protected:

    std::string  _ahcHitColName;

    std::string _mappingProcessorName;
    std::string _cellDescriptionProcessorName;

    FloatVec _hitThreshold;

    IntVec _roiCells;

    IntVec _roiFirstLayer;
    IntVec _roiLastLayer;

    FloatVec _roiInnerRadius;
    FloatVec _roiOuterRadius;

    FloatVec _roiInnerSideLength;
    FloatVec _roiOuterSideLength;

    IntVec _minHits;
    FloatVec _minESum;

    float _roiCenter[2];

    const CALICE::AhcMapper *_mapper;
    unsigned int _mapperVersion;

    MappedContainer<CellDescription>* _cellDescriptions;

  };

}

#endif
