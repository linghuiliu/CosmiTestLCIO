#ifdef USE_LCCD
#include <iostream>
#include "RunLocationWhizard.hh"
#include "RunLocation.hh"
#include "DBInitString.hh"
#include "UTIL/LCTime.h"

// -- LCCD headers
#include "lccd.h"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"

using namespace lccd;

namespace CALICE {


  RunLocationWhizard::RunLocationWhizard(std::string folder, std::string dbinit) {
    _folder = folder;
    _dbinit = dbinit;
    createConditionsHandler();
   }

  RunLocationWhizard::RunLocationWhizard() : 
  _folder("/expert_cd_calice/CALDAQ_RunLocation"), 
  _dbinit( DBInitString() ) 
  {createConditionsHandler();}


  void RunLocationWhizard::createConditionsHandler() {
    //Give an arbitrary Collection Name, needed by the inter face 
    std::string colName = "RunTimeCol";
    _conData = new lccd::DBCondHandler( _dbinit, _folder, colName, "" ) ;

  }


  string RunLocationWhizard::getRunLocation(int run){
     //Access the folder at the desired time stamp (note here we transform the
     //runnumber into a timestamp, we cheat a bit
     _conData->update( static_cast<lccd::LCCDTimeStamp>(run));
     //Get the collection
     LCCollection* col = _conData->currentCollection();
     //Obtain the time
     RunLocation runlocation(col->getElementAt(0));
     return runlocation.getLocation();
  }


  string RunLocationWhizard::getGenericRunType(int run){
     //Access the folder at the desired time stamp (note here we transform the
     //runnumber into a timestamp, we cheat a bit
     _conData->update( static_cast<lccd::LCCDTimeStamp>(run));
     //Get the collection
     LCCollection* col = _conData->currentCollection();
     //Obtain the time
     RunLocation runlocation(col->getElementAt(0));
     return runlocation.getGenericRunType();
  }


  string RunLocationWhizard::getGenericRunMonth(int run){
     //Access the folder at the desired time stamp (note here we transform the
     //runnumber into a timestamp, we cheat a bit
     _conData->update( static_cast<lccd::LCCDTimeStamp>(run));
     //Get the collection
     LCCollection* col = _conData->currentCollection();
     //Obtain the time
     RunLocation runlocation(col->getElementAt(0));
     return runlocation.getMonthInfo();
  }


  void RunLocationWhizard::print( int run,  std::ostream& os ){

    os << " Runnumber: " << run << std::endl;
    os << " Taken at: " << getRunLocation(run) << std::endl;
    os << " of type: " << getGenericRunType(run) << std::endl; 
    os << " on: " << getGenericRunMonth(run) << std::endl; 
  }
  
}
#endif
