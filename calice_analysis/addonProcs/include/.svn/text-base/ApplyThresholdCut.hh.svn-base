#ifndef APPLY_NOISE_CUT_H
#define APPLY_NOISE_CUT_H

#include <vector>

#include "marlin/Processor.h"

#define AHCAL_NLAYERS 38
#define ARRAY_DIM ( AHCAL_NLAYERS + 1 )

namespace CALICE {

  /** Processor which drops all hits below energy threshold from hit collection.
   * @author N. Feege
   * @date Aug 2009
   */

  class ApplyThresholdCut : public marlin::Processor  {
  public:
    Processor*  newProcessor() { return new ApplyThresholdCut ; }

    ApplyThresholdCut();

    ~ApplyThresholdCut() {};

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

    std::string  _ahcHitColName_noCut;
    std::string  _ahcHitColName_wCut;

    float _mipCut;

  };

}

#endif
