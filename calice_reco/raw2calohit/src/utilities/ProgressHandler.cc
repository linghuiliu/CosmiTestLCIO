#include "ProgressHandler.hh"

#include <UTIL/LCTime.h>
#include "marlin/Exceptions.h"

#include <iostream>
#include <iomanip>
#include <climits>

namespace CALICE {

  // create instance to make this Processor known to Marlin
  ProgressHandler a_ProgressHandler_instance;

  ProgressHandler::ProgressHandler()
    : Processor("ProgressHandler")
  {
    _description = "Report event/run numbers at fixed time intervals, and handle SIGINT (Ctrl-C) for a graceful exit." ;

    registerProcessorParameter( "ReportInterval" ,
				"The number of seconds after which the number of processed events will be shown." ,
				_reportInterval ,
				(int)10 );

  }

  void ProgressHandler::init() {

    printParameters();

    installSignalHandler();
    time(&_startTime);
    time(&_timeOfLastReport);

    if (_reportInterval<=0) {
      _reportInterval=INT_MAX;
    }

    _nRuns = 0;
    _nEvents = 0;
  }

  void ProgressHandler::processRunHeader( LCRunHeader* runP) {
    if ( runP) {
      ++_nRuns;
      _nEventsRun = 0;
    }
  }

  void ProgressHandler::processEvent( LCEvent * evtP ) {
    if (evtP) {
      if (isAborted()) {
	throw marlin::StopProcessingException(this);
      }
      ++_nEvents;
      ++_nEventsRun;

      time_t now;
      time(&now);

      if ( difftime(now,_timeOfLastReport)> _reportInterval ) {
	unsigned int eventsSinceLastReport = _nEvents - _nEventsAtLastReport;

	double tb = (double)CLOCKS_PER_SEC/1000. ; // timebase in ms

	clock_t nowClock = clock();

	std::cout << "*************************************************" << std::endl
		  << "Current run: " << evtP->getRunNumber() << " (" << _nRuns << " runs so far) "
		  << " processed events: " << _nEventsRun << " (" << _nEvents << " total) "
		  << " processtime: "<< difftime(now,_startTime) << "s" << std::endl
		  << "Current event: " << evtP->getEventNumber() << " time: " << UTIL::LCTime(evtP->getTimeStamp()).getDateString() << std::endl
		  << "Speed: last event took " << std::setw(5) << std::right << ( nowClock - _clockOfLastEvent)/tb << std::setprecision(3) << "ms"
		  << " since last report " << std::setw(5) << std::right  << ( nowClock - _clockOfLastReport)/tb/(double)eventsSinceLastReport << std::setprecision(3) << "ms/event"
		  << " rate: " <<  std::setw(7) << std::right<<   eventsSinceLastReport*CLOCKS_PER_SEC/( nowClock - _clockOfLastReport ) << std::setprecision(5) <<"Hz" << std::endl;
	_timeOfLastReport = now;
	_clockOfLastReport = nowClock;
	_nEventsAtLastReport = _nEvents;
      }
      _clockOfLastEvent = clock();
    }
  }

  void ProgressHandler::end() {
    removeSignalHandler();

    std::cout << std::endl
	      << "--- " << name() <<" Report :" << std::endl
	      << "Processed " << _nEvents << " events in " << _nRuns << " runs " << std::endl
	      << "Average time per event " << std::setw(5) << std::right << (double)difftime(_timeOfLastReport,_startTime)*1000./(double)_nEvents << std::setprecision(3) << "ms/event" << std::endl
	      << "Average rate: " <<  std::setw(7) << std::right << (double)_nEvents/(double)difftime(_timeOfLastReport,_startTime) << std::setprecision(5) << "Hz" << std::endl;
    if (isAborted()) {
	      std::cout << "Processing was aborted manually." << std::endl;
    }
    std::cout << std::endl;

  }

  int ProgressHandler::__abortSignalRecieved=0;
  int ProgressHandler::__signalHandlerInstalled=0;
  struct sigaction ProgressHandler::__oldSignalHandler;
  struct sigaction ProgressHandler::__newSignalHandler;

  void ProgressHandler::installSignalHandler()
  {
    __signalHandlerInstalled++;
    if (__signalHandlerInstalled==1) {

      __newSignalHandler.sa_handler=ProgressHandler::termSignalHandler;
      sigemptyset(&__newSignalHandler.sa_mask);
      __newSignalHandler.sa_flags = 0;
      __abortSignalRecieved=0;

      if (sigaction(SIGINT, &__newSignalHandler, &__oldSignalHandler)) {
	__signalHandlerInstalled--;
      }
    }
  }

  void ProgressHandler::removeSignalHandler()
  {
    if (__signalHandlerInstalled>0) {
      if (__signalHandlerInstalled--==1) {
	sigaction(SIGINT, &__oldSignalHandler,NULL);
	__signalHandlerInstalled=0;
      }
    }
  }

  void ProgressHandler::termSignalHandler(int sig)
  {
    if (sig==SIGINT || sig==SIGTERM)
      __abortSignalRecieved=sig;
  }

}
