#ifndef __MCRunTimeProcessor__
#define __MCRunTimeProcessor__ 1

#include <string>

// marlin and lcio headers
#include "marlin/Processor.h"
#include "marlin/EventModifier.h"
#include "UTIL/LCTime.h"

// userlib headers
#include "RunLocationWhizard.hh"
#include "RunTimeWhizard.hh"

using namespace CALICE;

namespace marlin {
  class MCRunTimeProcessor : public Processor, public EventModifier {
  
  public:

    MCRunTimeProcessor();
    virtual ~MCRunTimeProcessor();
  
    virtual Processor* newProcessor() { return new MCRunTimeProcessor; }
  
    // Needed for modifyEvent
    virtual const std::string & name() const { return Processor::name(); }

    virtual void modifyEvent( LCEvent* evt );
  
    virtual void init();  
    virtual void processRunHeader( LCRunHeader* run );  
    virtual void check( LCEvent* evt );  
    virtual void end();

  private:

    // String to initialize the database
    std::string _dbInit;

    // The variable holding the name of the folder to look up the run information
    std::string _folderRunTime;

    // The variable holding the name of the folder to look up the run location
    std::string _folderLocation;

    // The object which holds the information on the run location
    RunLocationWhizard* _runlocationwhizard;

    // The object which holds the information on the run time
    RunTimeWhizard* _runtimewhizard;

    // The wished run number
    int _runNumber;

    // Savety margin for conditions data
    int _savetyMargin;

    // The event time created for MC
    UTIL::LCTime _eventTime;
  };

#endif
}
