#include "EventListProcessor.hh"

#include <cstdlib>
#include "marlin/Exceptions.h"

using namespace std;
using namespace lcio;

namespace CALICE
{

  // Marlin formalities
  EventListProcessor aEventListProcessor;


  Processor* EventListProcessor::newProcessor()
  { 
    return new EventListProcessor;
  }


  // constructor
  EventListProcessor::EventListProcessor():Processor("EventListProcessor"),_skipAll(false)
  {
    _description="Select events given in text file. Skip the rest";

    registerProcessorParameter( "EventListFilename",
				"Textfiles containing run/event numbers to select",
				_eventListFilename, std::string("eventlist.dat")    );
  }

  void EventListProcessor::init()
  {
    //open event list file
    _eventListFStream.clear();
    _eventListFStream.open( _eventListFilename.c_str(), ios::in );
    
    if ( ! _eventListFStream )
      {
	cout << "EventListProcessor: Cannot open file: ["<< _eventListFilename 
	     <<"]. Exit..." << endl;
	exit(-1);
      }

    // read event list into memory
    int linecounter = 0;
    while( _eventListFStream.good() )
      {
	++linecounter;

	int runNumber;
	int eventNumber;

	_eventListFStream >> runNumber >> eventNumber;

	if ( _eventListFStream.fail() && ! _eventListFStream.eof() )
	{
	  cout << "EventListProcessor: Error reading file ["<< _eventListFilename
	       <<"] in line ["<< linecounter <<"]. Exit..." << endl;
	  exit(-1);
	}
	if ( ! _eventListFStream.eof() )
	{
	  _eventList.push_back( RunEvent(runNumber,eventNumber) );
	}
      }
    
    _nextEvent=_eventList.begin();
    
  }


  void EventListProcessor::processEvent( LCEvent* evt)
  {
    if ( _skipAll == true )
      {
	throw SkipEventException( this );
      }
    else
      {

	int currentRun   = evt->getRunNumber();
	int currentEvent = evt->getEventNumber();
	
	while( currentRun > _nextEvent->run )
	  seekNextRun();
	
	if ( currentRun == _nextEvent->run && currentEvent > _nextEvent->event )
	  seekEventAfter( currentEvent );
	
	if ( currentEvent != _nextEvent->event || currentRun != _nextEvent->run )
	  throw SkipEventException( this );
	else
	  if ( ++_nextEvent == _eventList.end() )
	    _skipAll = true;
      }
  }


  inline void EventListProcessor::seekNextRun()
  {
    int thisRun = _nextEvent->run;

    for( ++_nextEvent; _nextEvent != _eventList.end(); ++_nextEvent )
      {
	if( _nextEvent->run != thisRun )
	  break;
      }
    if ( _nextEvent == _eventList.end() )
      {
	cout << "EventListProcessor: End of Event list. Will skip all events from now." << endl;
	_skipAll = true;
	throw SkipEventException( this );
      }
  }


  inline void EventListProcessor::seekEventAfter( int currentEvent )
  {
    int thisRun   = _nextEvent->run;
    //    int thisEvent = _nextEvent->event;

    cout << "EventListProcessor: WARNING: Some events in event list are missing" << endl;

    for( ++_nextEvent;
	 _nextEvent->run == thisRun &&
	   _nextEvent->event < currentEvent &&
	   _nextEvent != _eventList.end(); 
	 ++_nextEvent                                 )
      {}

    if ( _nextEvent == _eventList.end() )
      {
	cout << "EventListProcessor: End of Event list. Will skip all events from now." << endl;
	_skipAll = true;
	throw SkipEventException( this );
      }
  }


  




}// namespace CALICE
