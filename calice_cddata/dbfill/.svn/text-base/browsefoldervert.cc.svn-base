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

// -- LCCD headers
#include "lccd.h"
#include "lccd/DBInterface.hh"

//calice_userlib header
#include "collection_names.hh"
#include "BeTrgConf.hh"
#include "EmcStageDataBlock.hh"

// -- C++ headers 
#include <iostream>

using namespace std ;
using namespace lcio;


typedef std::vector< lcio::LCCollection* > ColVec ;

/** Test program that tags an existing folder in the database.
 * 
 * @author F.Gaede, DESY
 * @version $Id: browsefoldervert.cc,v 1.2 2008-06-16 15:01:35 lima Exp $
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


  //this is for browing the Runinfo db entries
  /*for (ColVec::iterator colvec_iter = colVec.begin(); colvec_iter != colVec.end();  colvec_iter++) {
    
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
  }
  */   

  /*
  //Browse the BeTrgConf Entries
  //Create a map with the configured triggers
  std::map<int,int> confTriggerMap; 
  //...not before a given Date!!!!
  LCTime not_before( 2007,2, 28, 0 , 0 , 0 ) ;
  for (ColVec::iterator colvec_iter = colVec.begin(); colvec_iter != colVec.end();  colvec_iter++) {
    std::string since_string =  (*colvec_iter)->getParameters().getStringVal((lccd::DBSINCE).c_str());     
    LCTime since(static_cast<long64>(strtoll(since_string.c_str(),0,0)));
    std::string till_string =  (*colvec_iter)->getParameters().getStringVal((lccd::DBTILL).c_str());     
    LCTime till(static_cast<long64>(strtoll(till_string.c_str(),0,0)));

    if(since.unixTime() > not_before.unixTime() ) {
      //std::cout << "************** New Entry: *********************" << std::endl;
      //std::cout << "Valid from long: " << since.getDateString() << std::endl;
      //std::cout << "Valid to long: " << till.getDateString()  << std::endl;
      for (unsigned int iel(0); iel < static_cast<unsigned int>((*colvec_iter)->getNumberOfElements()); iel++ ){
	BeTrgConf beTrgConf( (*colvec_iter)->getElementAt(iel));
	if(beTrgConf.getInputEnableMask() > 0 && beTrgConf.getRecordLabel() == 0 ) confTriggerMap.insert( std::make_pair<int,int>(since.unixTime(), beTrgConf.getInputEnableMask())); 
      }
     }
  }

  //Now loop over the map with the configured triggers 
  for( std::map<int,int>::iterator map_iter = confTriggerMap.begin(); map_iter != confTriggerMap.end(); map_iter++){

    LCTime temp_time(map_iter->first);
    std::cout << "Time: " << temp_time.getDateString() << " Configured Trigger: " << std::hex << map_iter->second << std::dec << std::endl; 
  }
  */

  
  //Browse the EmcStageRunData Entries
  //Create a map with the configured triggers
  //std::map<int,int> confTriggerMap; 
  //...not before a given Date!!!!
  std::cout << "Before Emc: " << std::endl;
  LCTime not_before( 1977,2, 28, 0 , 0 , 0 ) ;
  for (ColVec::iterator colvec_iter = colVec.begin(); colvec_iter != colVec.end();  colvec_iter++) {
    std::string since_string =  (*colvec_iter)->getParameters().getStringVal((lccd::DBSINCE).c_str());     
    LCTime since(static_cast<long64>(strtoll(since_string.c_str(),0,0)));
    std::string till_string =  (*colvec_iter)->getParameters().getStringVal((lccd::DBTILL).c_str());     
    LCTime till(static_cast<long64>(strtoll(till_string.c_str(),0,0)));

    //if(since.unixTime() > not_before.unixTime() ) {
      //std::cout << "************** New Entry: *********************" << std::endl;
      //std::cout << "Valid from long: " << since.getDateString() << std::endl;
      //std::cout << "Valid to long: " << till.getDateString()  << std::endl;
      for (unsigned int iel(0); iel < static_cast<unsigned int>((*colvec_iter)->getNumberOfElements()); iel++ ){
	//BeTrgConf beTrgConf( (*colvec_iter)->getElementAt(iel));
        EmcStageDataBlock emcStgDat((*colvec_iter)->getElementAt(iel));
        LCTime temp_time(since.unixTime());
                  std::cout << "Time: " << temp_time.getDateString()
                  << " xStandPosition/mm: " << (float) emcStgDat.getXStandPosition()*0.1
                  << " yStandPosition/mm: " << (float) emcStgDat.getYStandPosition()*0.1 << std::endl;
	//if(beTrgConf.getInputEnableMask() > 0 && beTrgConf.getRecordLabel() == 0 ) confTriggerMap.insert( std::make_pair<int,int>(since.unixTime(), beTrgConf.getInputEnableMask())); 
      }
      //}
  }

  //Now loop over the map with the configured triggers 
  //for( std::map<int,int>::iterator map_iter = confTriggerMap.begin(); map_iter != confTriggerMap.end(); map_iter++){

  //  LCTime temp_time(map_iter->first);
  //  std::cout << "Time: " << temp_time.getDateString() << " Configured Trigger: " << std::hex << map_iter->second << std::dec << std::endl; 
  //}
  //lcio::LCTime t  ;
  //std::cout << " Tagged folder " << folder << " as [" << tag << "] " 
  //	    << " on " << t.getDateString() 
  //	    << std::endl ; 

}


#endif

