#ifdef HAVE_CONFIG_H
#  include <config.h> 
#endif 

#include "RunTimeProcessor.hh"
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <vector>

#include <EVENT/LCParameters.h>
#include <marlin/ConditionsProcessor.h>
#include <collection_names.hh>
#include <math.h>

//LCIO Headers
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"
#include <lcio.h>
#include <EVENT/LCRunHeader.h>

// -- LCCD headers
#include "lccd.h"
#include "lccd/DBInterface.hh"

//calice_userlib header
#include "collection_names.hh"
#include "RunTimeInfo.hh"

using namespace lcio;
using namespace CALICE;

namespace marlin {
  RunTimeProcessor a_RunTimeProcessor_instance;

  RunTimeProcessor::RunTimeProcessor()
    :    marlin::Processor("RunTimeProcessor"),
	 _beginOfRunFetched(false)
  {


    registerProcessorParameter( "RunTimeCollectionName" , 
			       "The folder in which we will store the run time info" ,
			       _colRuntime ,
				std::string( "/cd_calice/" + std::string(COL_RUNTIME_INFO) ) );

    registerProcessorParameter( "RunTimeCollectionName" , 
			       "The folder in which we will store the run time info" ,
			       _colRuntime ,
				std::string( COL_RUNTIME_INFO ) );

     registerProcessorParameter( "DBInit" ,
                                "Initialization string for conditions database"  ,
                                _dbinit ,
                                std::string("localhost:CaliceCondDB:user:password")) ;


  }

  /*****************************************************/
  /**/
  /**/
  /**/
  /*****************************************************/

  void RunTimeProcessor::init() {
     printParameters();
     _flag = false;
     _runnum = 0;
  }

  /*****************************************************/
  /**/
  /**/
  /**/
  /*****************************************************/

  void RunTimeProcessor::processRunHeader(LCRunHeader* run) 
  { 

  }

  /*****************************************************/
  /**/
  /**/
  /**/
  /*****************************************************/

  void RunTimeProcessor::processEvent(LCEvent * evtP ) 
  {
    _runnum = evtP->getRunNumber();

    if(!_beginOfRunFetched) _beginofrun =  evtP->getTimeStamp();
    _beginOfRunFetched=true;

    _endofrun = evtP->getTimeStamp();
  }

  /*****************************************************/
  /**/
  /**/
  /**/
  /*****************************************************/

  void RunTimeProcessor::end() 
  {
    //Check if start and stop are different if not add more to stop to avoid start == stop
    if(_beginofrun == _endofrun)
      _flag = true;

    //#ifdef CONV_DEBUG
    LCTime beginofrun( _beginofrun);
    LCTime endofrun( _endofrun);

    if(_flag)
      {
	std::cout << "Same start & end run timestamps -- adding 20 mins to the timestamp" << std::endl;
	endofrun += (long64)(20*60)*1000000000; //add 20 mins to the end run
	_endofrun += (long64)(20*60)*1000000000;
      }

    //#endif
    std::cout << "Run Start of Run " << _runnum << " is: " << beginofrun.getDateString() << std::endl;
    std::cout << "Run End of Run " << _runnum << " is: " << endofrun.getDateString() << std::endl;

     //The LCIO collection which will be written to the db
     LCCollectionVec* col_runtimeinfo = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;


    //create a new runtime info object
    RunTimeInfo* runtimeinfo = new RunTimeInfo() ;
    runtimeinfo->setRunStartTime(_beginofrun);
    runtimeinfo->setRunStopTime(_endofrun);

#ifdef CONV_DEBUG
    runtimeinfo->print(std::cout);
#endif
    //add it to the collection
    col_runtimeinfo->addElement( runtimeinfo ) ;


    //write the runtime info object into the database
    lccd::DBInterface db(_dbinit, _colRuntime , true ) ;
    //db.storeCollection(static_cast<lccd::LCCDTimeStamp>(_runnum),lccd::LCCDTimeStamp>(_runnum+1),runtimeinfo, "");
    //std::cout << "Before Storing Collection: " << std::endl;
    db.storeCollection(static_cast<lccd::LCCDTimeStamp>(_runnum),static_cast<lccd::LCCDTimeStamp>(_runnum+1),col_runtimeinfo, "");
  }



}
