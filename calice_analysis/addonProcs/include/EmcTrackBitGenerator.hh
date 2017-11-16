#ifndef EMC_TRACK_BIT_GENERATOR_H
#define EMC_TRACK_BIT_GENERATOR_H

#include <iostream>

#include "marlin/Processor.h"

#define EMC_NLAYERS 30
#define EMC_1STLAYER 1

namespace CALICE {

  /** Processor which generates a trigger bit based on ECAL information.
   * The trigger bit can be used by the EventSelector, e.g, to select mip-like tracks in the ECAL.
   * @author N. Feege
   * @date Sep 2009
   */

  class EmcTrackBitGenerator : public marlin::Processor  {
  public:
    Processor*  newProcessor() { return new EmcTrackBitGenerator ; }

    EmcTrackBitGenerator();

    ~EmcTrackBitGenerator() {};

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run);

    /** Called for every event - this is where the actual action is taking place.      */
    void processEvent( LCEvent * evt );

    void end();

  protected:

    std::string  _emcHitColName;
    std::string  _outputParName_trackBit;
    int _maxHits;
    float _maxESum;

  };

}

#endif
