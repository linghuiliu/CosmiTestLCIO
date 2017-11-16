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
#include "RunTimeWhizard.hh"
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
 * @version $Id: getRunTimeInfo.cc,v 1.2 2008-06-16 15:01:35 lima Exp $
 */

int main(int argc, char** argv ) {
  
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;

  // read file name and collection name from command line 
  if( argc < 2) {
    cout << " usage: getRunTimeInfo <runnumber> [ <folder> <tag> <dbInit>]" << endl ;
    exit(1) ;
  }
  
  int runnum(atoi(argv[1]));

  //The folder in which the runtime info is be stored
  std::string folder("/cd_calice/CALDAQ_RunTimeInfo");
  if(argc > 2 )string folder( argv[2]) ;
  std::cout << "Folder is: " << folder << std::endl;
  
  string dbInit("flccaldb02.desy.de:calice:caliceon:Delice.1:3306") ;
  if( argc > 3 ) dbInit = argv[3] ;
  
  std::cout << "dbinitstring is: " << dbInit << std::endl;
  //lccd::DBInterface db( dbInit, folder , true ) ;
  RunTimeWhizard runtimewhizard(folder, dbInit);
  runtimewhizard.print(runnum, std::cout);

}


#endif
