#ifdef USE_CONDDB
#include <cstdlib>
#include <map>
//#include "stro.hh"

// -- CondDB headers
#include "ConditionsDB/ICondDBMgr.h"
#include "ConditionsDB/CondDBMySQLMgrFactory.h"
#include "ConditionsDB/CondDBObjFactory.h"
//#include "exampleObject.h"

// -- LCIO headers
#include "lcio.h"
#include "UTIL/LCTime.h"
#include "EVENT/LCParameters.h"
#include "UTIL/LCTime.h"
#include <IMPL/LCCollectionVec.h>
// -- LCCD headers
#include "lccd.h"
#include "lccd/DBInterface.hh"

//calice_userlib header
#include "collection_names.hh"
#include "RunTimeInfo.hh"
// -- C++ headers 
#include <iostream>

using namespace std ;
using namespace lcio;
using namespace CALICE;

typedef std::vector< lcio::LCCollection* > ColVec ;

/** Test program that tags an existing folder in the database.
 * 
 * @author F.Gaede, DESY
 * @version $Id: createRunTimeInfo.cc,v 1.2 2008-06-16 15:01:35 lima Exp $
 */

int main(int argc, char** argv ) {
  
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;

  // read file name and collection name from command line 
  if( argc < 4) {
    cout << " usage: tagdbfolder <folder> <tag> [<dbInit>]" << endl ;
    exit(1) ;
  }
  
  string folder     ( argv[1]  ) ;

  std::cout << "Folder is: " << folder << std::endl;
  std::string tag("!");
  if (argv[2] != "!" ) tag  = argv[2] ;
  std::cout << "Tag is: " << tag << std::endl;
  //  string description( argv[3]  ) ;
  


  string dbInit("flc32.desy.de:condb_1:condb:condb") ;
  if( argc > 3 ) dbInit = argv[3] ;
  
  std::cout << "dbinitstring is: " << dbInit << std::endl;
  lccd::DBInterface db( dbInit, folder , true ) ;
  
  //db.tagFolder( tag, description )  ;
  ColVec colVec;  
  
  std::cout << "Before finding collections" << std::endl;
  db.findCollections(colVec) ;

  //The folder in which the runtime info will be stored
  std::string _colRuntime("/cd_calice/CALDAQ_RunTimeInfo");

  //this is for browing the Runinfo db entries
  for (ColVec::iterator colvec_iter = colVec.begin(); colvec_iter != colVec.end();  colvec_iter++) {
    
    int runnumber =  (*colvec_iter)->getParameters().getIntVal(PAR_RUN_NUMBER);     
    std::string since_string =  (*colvec_iter)->getParameters().getStringVal((lccd::DBSINCE).c_str());     
    LCTime since(static_cast<long64>(strtoll(since_string.c_str(),0,0)));
    std::string till_string =  (*colvec_iter)->getParameters().getStringVal((lccd::DBTILL).c_str());     
    LCTime till(static_cast<long64>(strtoll(till_string.c_str(),0,0)));
    std::cout << "Runnumber: " << runnumber << std::endl;
    //std::cout << "Valid from: " << since.getDateString() << std::endl;
    //std::cout << "Valid to: " << till.getDateString() << std::endl;
    //std::cout << "Valid from: " << since_string << std::endl;
    //std::cout << "Valid to: " << till_string << std::endl;
    std::cout << "Valid from long: " << since.getDateString() << std::endl;
    std::cout << "Valid to long: " << till.getDateString()  << std::endl;
    //Create a RunTime Info object
     //The LCIO collection which will be written to the db
    LCCollectionVec* col_runtimeinfo = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;


    //create a new runtime info object
    if( runnumber > 299999 & runnumber < 301000 ) {
      RunTimeInfo* runtimeinfo = new RunTimeInfo() ;
      runtimeinfo->setRunStartTime(since.timeStamp());
      runtimeinfo->setRunStopTime(till.timeStamp());
      
      
      runtimeinfo->print(std::cout);
      //add it to the collection
      col_runtimeinfo->addElement( runtimeinfo ) ;
      //write the runtime info object into the database
      lccd::DBInterface db(dbInit, _colRuntime , true ) ;
      //db.storeCollection(static_cast<lccd::LCCDTimeStamp>(_runnum),lccd::LCCDTimeStamp>(_runnum+1),runtimeinfo, "");
      std::cout << "Before Storing Collection: " << std::endl;
      db.storeCollection(static_cast<lccd::LCCDTimeStamp>(runnumber),static_cast<lccd::LCCDTimeStamp>(runnumber+1),col_runtimeinfo, "");
      //delete runtimeinfo;
      delete col_runtimeinfo;
    }
  }
}


#endif

