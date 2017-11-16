#ifndef _ProgressHandler_hh_
#define _ProgressHandler_hh_

#include <marlin/Processor.h>
#include <signal.h>
#include <ctime>

namespace CALICE {

  /** Show processing progess and catch SIGINT signals and abort processing safely.
   *
   * Every several seconds the current run and event number is printed to the screen.
   * If SIGINT is sent to the process (e.g. CTRL-C) the processEvent will
   * throw a marlin::StopProcessingException. Thus, abort processing.
   * After, that the Processor::end() methods of all active processors will be called.
   */
  class ProgressHandler : public marlin::Processor
  {

  public:
    Processor*  newProcessor() { return new ProgressHandler ; }

    ProgressHandler();

    virtual ~ProgressHandler() {};


    void init();

    void processRunHeader( LCRunHeader* run );

    void processEvent( LCEvent * evtP );

    void end();

    /**
     * This will cause the analysis to abort when done with the current event.
     */
    static void setAborted() {__abortSignalRecieved=true;};

  protected:
    unsigned int _nEvents;             // total number of events
    unsigned int _nRuns;               // total number of runs
    unsigned int _nEventsRun;          // number of events in run
    unsigned int _nEventsAtLastReport; // number of events when last report was given

    int _reportInterval;               // mininmum time between two reports
    time_t _startTime;                 // time when processor was started
    time_t _timeOfLastReport;          // time when the last report was given

    clock_t _clockOfLastEvent;         // clock cycles at time of last event
    clock_t _clockOfLastReport;        // clock cycles at time of last report


    /**
     * check if (control-C) was catched
     *
     * @returns true if control-C was catched
     */
    static bool isAborted() {return __abortSignalRecieved!=0;};

    /**
     * install signal handler
     */
    static void installSignalHandler();

    /**
     * remove signal handler
     */
    static void removeSignalHandler();

    /**
     * the signal handler
     *
     * installed by \ref installSignalHandler
     * @param[in] sig signal for which the handler got called
     */
    static void termSignalHandler(int sig);

    static int __abortSignalRecieved;

    //@{ internally used to install/remvoe the signal handler
    static int __signalHandlerInstalled;
    static struct sigaction __oldSignalHandler;
    static struct sigaction __newSignalHandler;
    //@}

  };

}
#endif
