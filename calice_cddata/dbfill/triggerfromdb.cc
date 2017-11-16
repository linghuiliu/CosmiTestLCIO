#ifdef USE_CONDDB

// -- CondDB headers
#include "ConditionsDB/ICondDBMgr.h"
#include "ConditionsDB/CondDBMySQLMgrFactory.h"
#include "ConditionsDB/CondDBObjFactory.h"
//#include "exampleObject.h"

// -- LCIO headers
#include "lcio.h"
#include "lccd/SimpleFileHandler.hh"
#include "IO/LCWriter.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"
//
#include "UTIL/LCTOOLS.h"
#include "UTIL/LCTime.h"


// -- LCCD headers
#include "lccd.h"
#include "lccd/StreamerMgr.hh"
#include "lccd/VCollectionStreamer.hh"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"

#include "collection_names.hh"

//to set the desired time
#define TIMEVECSIZE 6
#define YEAR 0
#define MONTH 1
#define DAY 2
#define HOUR 3
#define MINUTE 4
#define SECONDS 5



// -- C++ headers 
#include <iostream>
#include <algorithm>

using namespace std ;
using namespace lcio;
//using namespace lccd ;


/** Test program that reads the calice trigger definitions from the
 *  calibration from a data base and prints it to the screen 
 *  Although not urgently needed it demonstrates also how to write
 *  the database content to a simple LCIO file 
 *  Demonstrates the usage of lccd::DBCondHandler and
 *  lccd::ConditionsMap. <br>
 *  To simply create a file with conditions data
 *  for a certain point in time use lccd::DBInterface::createSimpleFile() 
 *  see commented out code at the end.
 * 
 * @author F.Gaede, DESY
 * @version $Id: triggerfromdb.cc,v 1.4 2005-12-18 16:56:59 poeschl Exp $
 */

int main(int argc, char** argv ) {
  
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;

  // read file name and collection name from command line 
  if( argc < 3) {
    cout << " usage: triggerfromdb <db collection name> <timestamp> [<dbinit> <tag>]" << endl ;
    exit(1) ;
  }

  string colName( argv[1] ) ;
  string usertimeStr( argv[2] ) ;
  std::vector<int> usertimevec;
  string usertime_stream;
  for (unsigned i = 0; i < usertimeStr.length(); ++i) {
    if ( usertimeStr[i] != ':' ) {
        
      if( !isdigit( usertimeStr[i] ) ) {
	std::cout << "Error in reading <timestamp> value " << std::endl;
	std::cout << "Check format of <timestamp> value " << std::endl;
	std::cout << "Will leave the program now " << std::endl;
	exit(1);
      }
      
      usertime_stream += usertimeStr[i];
    }
    else {
      usertimevec.push_back( atoi ( usertime_stream.c_str() ) );
      usertime_stream = " ";
    }
    
    if (i == usertimeStr.length()-1) {
      usertimevec.push_back( atoi ( usertime_stream.c_str() ) );
    }
    
    
  }
  
  if ( usertimevec.size() < TIMEVECSIZE ) {
    std::cout << "Error in reading <timestamp> value " << std::endl;
    std::cout << "Don't have the desired six time parameters" << std::endl;
    std::cout << "Will leave the program now " << std::endl;
    exit(1);
  }
  
  LCTime usertime( usertimevec.at(YEAR),
		   usertimevec.at(MONTH), 
		   usertimevec.at(DAY), 
		   usertimevec.at(HOUR), 
		   usertimevec.at(MINUTE), 
		   usertimevec.at(SECONDS)) ;
  
  lccd::LCCDTimeStamp timeStamp   = usertime.timeStamp();
  

  //LCTime usertime( 2005,4, 22, 0 , 0 , 0 ) ;
  //lccd::LCCDTimeStamp timeStamp   = now.timeStamp();
  //lccd::LCCDTimeStamp  timeStamp ( atoi( timeStr.c_str() ) )   ;  
  
  string dbinit("localhost:condb_1:condb:condb");
  if( argc > 3 )
    dbinit = argv[3] ;



  string tag("") ;
  if( argc > 4 )
    tag = argv[4] ;
  

  // testing: dump all collections in tag :
  //   lccd::DBInterface db("localhost:lccd_test:calvin:hobbes" , folder , false ) ;
  
  //   lccd::ColVec colVec ;
  //   db.findCollections( colVec , tag ) ;
  
  //   std::cout << " ---- Collections defined for tag: " << tag ;
  //   std::for_each(  colVec.begin() , colVec.end() , LCTOOLS::printLCGenericObjects ) ;
  //-----------  end dump all ----------------------------------


  // ---- use the DBCondHandler -----------

  string folder( "/cd_calice/" + colName ) ;
  lccd::IConditionsHandler* conData = 
    new lccd::DBCondHandler( dbinit, folder, colName, tag ) ;
  //new lccd::DBCondHandler( lccd::getDBInitString() , folder, colName, tag ) ;  

  // ------ testing: create a calibration map ------------------
  //typedef lccd::ConditionsMap<int,CalibrationConstant> CalMap ;
  
  //CalMap calMap( &CalibrationConstant::getCellID )   ;
  
  //conData->registerChangeListener(  &calMap )  ;
  
  conData->update( timeStamp ) ;
  
  //calMap.print( std::cout ) ;
  //--------------- end calibration map -----------------------



  lcio::LCTime t0 ( conData->validSince()  ) ;
  lcio::LCTime t1 ( conData->validTill()  ) ;

  cout << endl 
       << " -- calibration data has been read from data base folder" << folder 
       << endl 
       << " ---- valid from: " <<  t0.getDateString()  
       << " [" <<  conData->validSince()  << "] " 
       << endl 
       << " ----       till: " <<  t1.getDateString()  
       << " [" <<  conData->validTill()  << "] "
       << endl 
       << endl ;
   
  LCCollection* col = conData->currentCollection() ;
  EVENT::IntVec TriggerPosition;
  EVENT::StringVec TriggerType;
  col->parameters().getIntVals(PAR_TRIGGER_BITS,TriggerPosition);
  col->parameters().getStringVals(PAR_TRIGGER_TYPE_NAMES,TriggerType);

  for (int i=0; i<31; i++) {

  cout << TriggerPosition[i] << endl; 
  cout << TriggerType[i] << endl; 

  }
  //------------------ write constants to file -----------------------
  
  //LCWriter* wrt = LCFactory::getInstance()->createLCWriter() ;
  
  //wrt->open( fileName , LCIO::WRITE_NEW )  ;
  
  
  // create and add a run header to the file 
  // this isn't needed by LCCD but allows to use the 'dumpevent' tool
  LCRunHeader* rHdr = new LCRunHeaderImpl ;
  //wrt->writeRunHeader( rHdr ) ;
  
  LCEvent* evt = new LCEventImpl ;
  evt->addCollection(  col , colName  ) ;  

  LCTOOLS::dumpEventDetailed( evt ) ; 
  //wrt->writeEvent( evt ) ;
  
  // this is needed because the event should not own the collection when beeing deleted 
  // FIXME: not very nice, is it ?
  evt->takeCollection(  colName  ) ;  

  //wrt->close() ;
  //----------------------------------------------------------------

  // clean up
  delete evt ; 
  //delete wrt ;
  delete rHdr ;
  delete conData ;


  //---------------------------------------------------------------------------

  // while the above demonstrates and tests the use of DBCondHandler and ConditionsMap
  // we could have created the LCIO file much easier :)

  //lccd::DBInterface db(  folder )  ;
  //db.createSimpleFile( timeStamp , tag , true ) ;

  //---------------------------------------------------------------------------


}
#endif
