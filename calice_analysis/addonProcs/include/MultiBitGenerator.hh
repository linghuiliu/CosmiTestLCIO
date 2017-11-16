#ifndef MultiBitGenerator_hh_
#define MultiBitGenerator_hh_

#include <string>
#include "marlin/Processor.h"

#include "ConditionsChangeDelegator.hh"
#include "lccd.h"

#include "collection_names.hh"

#include "VetoThreshold.hh"

namespace CALICE {

  /** Processor which generates a trigger bit for particle multiplicity (PAR_VETO_BIT) based on
   * the multiplicity counter amplitude given by PAR_MULTI_AMPL. The threshold values can be extracted
   * from the database based on a chosen purity for the single-particle-sample (default) or be defined explicitly
   * in the steering file (set "useFixedThreshold" to "true").
   *
   * @author Benjamin.Lutz@desy.de
   * @author Nils.Feege@desy.de
   * @date Mar 2010
   */

  class MultiBitGenerator : public marlin::Processor, public lccd::IConditionsChangeListener  {

  public:

    Processor*  newProcessor() { return new MultiBitGenerator ; }

    MultiBitGenerator();

    ~MultiBitGenerator() {};

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init();

    /** Called for every run, e.g. overwrite to initialize run dependent
     *  histograms.
     */
    void processRunHeader( LCRunHeader* run) ;

    /** Called for every event - the working horse.      */
    void processEvent( LCEvent * evt );

    void end();

  protected:

    std::string  _parNameMultiAmplitude;
    std::string  _parNameMultiBit;
    std::string  _thresholdColName;
    float _setPurity;
    float _effPurity;
    float _setThreshold;
    float _setThresholdErr;

    bool _fixThreshold;

  private:

    void conditionsChanged(lcio::LCCollection* col);

    bool _thresholdAvailable;
  };


}
#endif
