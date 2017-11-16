#include <string>
#include <sstream>
#include <stdexcept>

#include "MCRunTimeProcessor.hh"
#include "DBInitString.hh"
#include "IMPL/LCEventImpl.h"

// seconds to nano seconds
#define NPS 1000000000LL

using namespace std;

namespace marlin {

  MCRunTimeProcessor aMCRunTimeProcessor;
  
  MCRunTimeProcessor::MCRunTimeProcessor() : Processor("MCRunTimeProcessor"),
					     _runlocationwhizard( NULL ),
					     _runtimewhizard( NULL ) {
    
    _description = "Sets the event time to the time of the given run number" ;
    
    registerProcessorParameter( "DBInit" ,
				"initialisation of the database" ,
				_dbInit,
                                DBInitString() );
     
    /* Folder to be searched for the runtime information. 
     *
     * Fixme: we need to make sure that the folders we select here correspond
     * to the run we have at hand
     *
     */
    registerProcessorParameter( "RunTimeFolder",
				"Collection from DB which contains the start "
                                "and stop time of a given run" ,
				_folderRunTime,
				string("/cd_calice/CALDAQ_RunTimeInfo"));
        
    /* Folder to be searched for the runlocation information. 
     *
     * Fixme: we need to make sure that the folders we select here correspond to
     * the run we have at hand
     *
     */
    registerProcessorParameter( "RunLocationFolder",
				"Collection from DB which contains information "
                                "about the location of a run" ,
				_folderLocation,
				string("/cd_calice/RunLocation"));
    
    registerProcessorParameter( "RunNumber" , 
                                "Run number of the run for MC files",
                                _runNumber,
                                int(230101)) ;

    registerProcessorParameter( "SavetyMargin", 
                                "Apply a savety margin of n seconds between the "
                                "nominal run start and the first MC event "
                                "time stamp to ensure validity of conditions "
                                "data", 
				_savetyMargin, 
				int(0));
  }


  MCRunTimeProcessor::~MCRunTimeProcessor() {
    if ( _runtimewhizard != NULL ) delete _runtimewhizard;
    if ( _runlocationwhizard != NULL ) delete _runlocationwhizard;
  }

  
  void MCRunTimeProcessor::init() {
    
    printParameters();

    // Initialize the whizards
    _runtimewhizard = new RunTimeWhizard(_folderRunTime, _dbInit);    
    _runlocationwhizard = new RunLocationWhizard(_folderLocation, _dbInit);

    /* Print some information like:
     *
     *   Runnumber: 350118
     *   Taken at: Cern
     *   of type: cernhcal
     *   on: 07/07
     */
    _runlocationwhizard->print(_runNumber, cout);

    // Get the start und stop time according to the run number
    LCTime starttime(_runtimewhizard->getRunStartTime(_runNumber));
    LCTime stoptime(_runtimewhizard->getRunStopTime(_runNumber));
    
    // Print the gathered time information
    streamlog_out(MESSAGE) << "Start time of run " << _runNumber << " is: " 
                           << starttime.getDateString() << endl
                           << "Stop time of run " << _runNumber << " is: " 
                           << stoptime.getDateString() << endl;

    // Check if the given run is shorter than the savety margin
    if (stoptime.timeStamp() - starttime.timeStamp() > _savetyMargin*NPS)
      _eventTime=LCTime(starttime.timeStamp()+_savetyMargin*NPS);
    else {
      stringstream message;
      message << " Run shorter than given savety margin of " 
              << _savetyMargin << " seconds." << endl;
      throw runtime_error(message.str()); 
    }	
  }

  void MCRunTimeProcessor::modifyEvent( LCEvent* evt ) {

    // Increase the next event time by one nano second
    _eventTime = LCTime(_eventTime.timeStamp() + 1);
    
    // Modify the run number and the time stamp of each event
    static_cast<LCEventImpl*>(evt)->setRunNumber(_runNumber);;
    static_cast<LCEventImpl*>(evt)->setTimeStamp(_eventTime.timeStamp());

  }    

  // Not needed
  void MCRunTimeProcessor::processRunHeader( LCRunHeader* run) {
  }
  
  // Not needed
  void MCRunTimeProcessor::check( LCEvent* evt ) {
  }

  // Not needed  
  void MCRunTimeProcessor::end() {
  }
}
