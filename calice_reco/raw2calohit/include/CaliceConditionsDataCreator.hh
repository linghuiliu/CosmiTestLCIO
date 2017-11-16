#ifndef _CaliceConditionsDataCreator_hh_
#define _CaliceConditionsDataCreator_hh_

#ifndef DEBUG_CONDDB_WRITER
//#define DEBUG_CONDDB_WRITER
#endif

#include <RtypesSubSet.h>
#include <string>
#include <vector>

#include <marlin/Processor.h>
#include <lccd/DBInterface.hh>
#include <lccd/IConditionsChangeListener.hh>
#include <UTIL/LCTime.h>

namespace EVENT {
  class LCCollection;
}

#include "ConditionsDataWriteHandler.hh"

namespace marlin {

  typedef EVENT::LCCollection* (*CreateCollFunc_t)(UTIL::LCTime &, UTIL::LCTime &, const std::string &);

  /** Marlin processr which creates calice conditions data and makes them via a simplefile handler available.
   * The processor creates:
   * <ol>
   * <li> ModuleConnection
   * <li> ModuleLocation
   * <li> ModuleDescription
   * <li> DriftChamberParameter
   * <li> ExperimentalSetup
   * <li> TriggerAssignament
   * </ol>
   * collections. The creation of each collection can be individually steered.
   */
  class CaliceConditionsDataCreator : public marlin::Processor
  {
    friend class CALICE::ConditionsDataWriteHandler;
  public:
    Processor*  newProcessor() { return new CaliceConditionsDataCreator ; };

    CaliceConditionsDataCreator();
    
    ~CaliceConditionsDataCreator();
    

    /** Install conditions change listener.
     * The listeners are installed for the conditions data specified by the processor 
     * parameters.
     */
    void init();

    void processRunHeader( LCRunHeader* run) {}

    /** Update conditions data (not yet).
     */
    void processEvent( LCEvent * evtP );

    void end();

  protected:
    enum {kAhcModuleLocation, kAhcModuleConnection, kAhcModuleDescription, 
	  kEmcModuleLocation, kEmcModuleConnection, kEmcModuleDescription, 
	  kDriftChamberParameter, kTriggerAssignment, kTriggerCheck,kExperimentalSetup, kNCollections};
    std::string _colName[kNCollections];
    std::vector<std::string> _inputFileName[kNCollections];
    std::vector<std::string> _sinceTillTime[kNCollections];
    std::vector<UTIL::LCTime> _since[kNCollections];
    std::vector<UTIL::LCTime> _till[kNCollections];

    std::vector<std::string> _beamType;            /**< the nominal beam type.*/
    std::vector<float>       _beamEnergy;          /**< nominal beam energy.*/
    std::vector<float>       _configurationAngle;  /**< angle between the detector normal and the beam axis.*/

    std::vector<int>         _triggerMainWord;     /**< position of the trigger main word in th fifo.*/
    std::vector<int>         _triggerMainWordTolerance;     /**< the tolerance on the trigger main word position */
    std::vector<int>         _triggerSearchRange;  /**< search range around the trigger main word which is search for overlapping triggers.*/

    lccd::IConditionsHandler         *_conddbHandler[kNCollections];
    std::vector<EVENT::LCCollection *> _conddbCol[kNCollections];

    static const char *__beamTypeNames[11];
    static const char *__collectionNames[kNCollections];
    static CreateCollFunc_t __createFunc[kNCollections];

    UTIL::LCTime _validSince;
    UTIL::LCTime _validTill;

  };
}
#endif
