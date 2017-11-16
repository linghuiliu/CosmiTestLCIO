#ifndef __EVENTLISTPROCESSOR_HH__
#define __EVENTLISTPROCESSOR_HH__

// author: Jörgen Samson <joergen.samson@desy.de> 
// Version 0.2 (April 2009)


#include "marlin/Processor.h"

#include <iostream>
#include <fstream>
#include <string>


namespace CALICE
{
  using namespace marlin;


  class EventListProcessor: public Processor
  {
  public:
    EventListProcessor();
    virtual Processor* newProcessor();

    virtual void init();
    virtual void processEvent( EVENT::LCEvent* );

  protected:
    std::string  _eventListFilename;
    std::fstream _eventListFStream;

    bool _skipAll;

    struct RunEvent
    {
      RunEvent( int aRun, int aEvent):run(aRun),event(aEvent) {}
      int run;
      int event;
    };

    typedef std::vector<RunEvent> EventList;
    typedef EventList::const_iterator EventListIter;
    
    EventList     _eventList;
    EventListIter _nextEvent;

    void seekNextRun();
    void seekEventAfter( int );

    
  }; 


}//namespace CALICE


#endif //__EVENTLISTPROCESSOR_HH__
