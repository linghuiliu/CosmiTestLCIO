/* Processor which check the event builder*/

#ifndef EVENTCHECKER_h
#define EVENTCHECKER_h 1

#include "marlin/Processor.h"

using namespace marlin ;

namespace CALICE {

/** Class to process Labview raw 
  * @author: Shaojun Lu DESY
  * @date Nov 15 2012
  *
  */


  class EventChecker : public Processor{
    
  public:
    virtual Processor* newProcessor() { return new EventChecker;}
    EventChecker();
    ~EventChecker();
    void init();  
    void processEvent( LCEvent * evt ) ;
    void end();
    
  protected:
    std::string _inputColName;

  private:
  };
}
#endif
