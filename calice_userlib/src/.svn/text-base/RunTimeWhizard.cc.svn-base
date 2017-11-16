#ifdef USE_LCCD
#include <iostream>
#include "RunTimeWhizard.hh"
#include "RunTimeInfo.hh"
#include "DBInitString.hh"
#include "UTIL/LCTime.h"

// -- LCCD headers
#include "lccd.h"
//#include "lccd/StreamerMgr.hh"
//#include "lccd/VCollectionStreamer.hh"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"

using namespace lccd;

namespace CALICE {


  RunTimeWhizard::RunTimeWhizard(std::string folder, std::string dbinit) {
    _folder = folder;
    _dbinit = dbinit;
   }

  RunTimeWhizard::RunTimeWhizard() : 
  _folder("/expert_cd_calice/CALDAQ_RunTimeInfo"), 
  _dbinit( DBInitString() ) 
  {}



  long64 RunTimeWhizard::getRunStartTime(int run){
    //Give an arbitrary Collection Name, needed by the inter face 
    std::string colName = "RunTimeCol";
     lccd::IConditionsHandler* conData = new lccd::DBCondHandler( _dbinit, _folder, colName, "" ) ;
     //Access the folder at the desired time stamp (note here we transform the
     //runnumber into a timestamp, we cheat a bit
     conData->update( static_cast<lccd::LCCDTimeStamp>(run));
     //Get the collection
     LCCollection* col = conData->currentCollection();
     //Obtain the time
     RunTimeInfo runtimeinfo(col->getElementAt(0));
     return runtimeinfo.getRunStartTime();
  }


  long64 RunTimeWhizard::getRunStopTime(int run){
    //Give an arbitrary Collection Name, needed by the inter face 
    std::string colName = "RunTimeCol";
     lccd::IConditionsHandler* conData = new lccd::DBCondHandler( _dbinit, _folder, colName, "" ) ;
     //Access the folder at the desired time stamp (note here we transform the
     //runnumber into a timestamp, we cheat a bit
     conData->update( static_cast<lccd::LCCDTimeStamp>(run) ) ;
     //Get the collection
     LCCollection* col = conData->currentCollection();
     //Obtain the time
     RunTimeInfo runtimeinfo(col->getElementAt(0));
     return runtimeinfo.getRunStopTime();

  }


  void RunTimeWhizard::print( int run,  std::ostream& os ){

  LCTime starttime(getRunStartTime(run));
  LCTime stoptime(getRunStopTime(run));
  os << " Starttime for run " << run << " is: " << starttime.getDateString() << std::endl;
  os << " Stoptime  for run " << run << " is: " << stoptime.getDateString() << std::endl;
  }
  
}
#endif
