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
#include "LCIOSTLTypes.h"

// -- LCCD headers
#include "lccd.h"
#include "lccd/StreamerMgr.hh"
#include "lccd/VCollectionStreamer.hh"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"

#include "HcalBoardsConn.hh"

// -- C++ headers 
#include <iostream>
#include <algorithm>

// -- To get the current time
#define SECTONS 1000000000LL
#define MUSECTONS 1000LL
#include <time.h>

//to set the desired time
#define TIMEVECSIZE 6
#define YEAR 0
#define MONTH 1
#define DAY 2
#define HOUR 3
#define MINUTE 4
#define SECONDS 5

typedef std::map<int,string> remarkMap_t;

using namespace std ;
using namespace lcio;
using namespace lccd ;
using namespace CALICE;

/** Test program that reads the hcal boards connections from a 
 *  data base and writes . Demonstrates the usage of lccd::DBCondHandler and
 *  lccd::ConditionsMap. <br>
 * 
 * @author F.Gaede, DESY
 * @version $Id: hcalboards_connfromdb.cc,v 1.2 2005-12-18 15:26:46 poeschl Exp $
 */


int main(int argc, char** argv ) {
    
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;

  // read file name and collection name from command line 
  if( argc < 2) {
    cout << " usage: hcalboards_connfromdb <connectorpin> [<timestamp> <tag> <db collection name> <dbinit>]" << endl ;
    std::cout << " If no <timestamp> is given the current time is used" << std::endl;
    std::cout << " Give <timestamp> in format YYYY:MM:DD:HH:MM:SS" << std::endl;    
    std::cout << " If no <tag> is given the HEAD is taken" << std::endl;
    std::cout << " If no <db collectioname> is given 'HcalBoardsConn' used" << std::endl;
    std::cout << " If no <dbinit> is given 'flc32.desy.de:condb_1:consult:consult' used" << std::endl;
    std::cout << " Optional Parameters can be ommitted with an '!'" << std::endl;
    exit(1) ;
  }

  string connectorStr( argv[1] ) ;
  int iconnector = atoi( connectorStr.c_str());

  //default timestamp 
  struct timespec cur_time;
  long64 time_cur;
  clock_gettime(CLOCK_REALTIME, &cur_time);
  time_cur = (long64) ( (cur_time.tv_sec)*SECTONS + cur_time.tv_nsec );
  LCTime now(time_cur) ;
  lccd::LCCDTimeStamp timeStamp   = now.timeStamp();
  //user defined timestamp;
  string checkopt;
  if (argc > 2) checkopt = argv[2];
  if (argc > 2 && checkopt != "!"){

    //The usertime
    string usertimeStr(argv[2]);
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

    //overwrite the pre-defined time stamp
    timeStamp   = usertime.timeStamp();
  }


  //default tag
  string tag("") ;
  //user defined tag
  if (argc > 3) checkopt = argv[3];
  if (argc > 3 && checkopt != "!") tag = argv[3];

  cout << "tag= " << tag << endl;

  
  string colName( "HcalBoardsConn" ) ;
  if (argc > 4) checkopt= argv[4];
  if ( argc > 4 && checkopt != "!" ) colName = argv[4];

  string dbinit("flc32.desy.de:condb_1:consult:consult");
  if (argc > 5) checkopt = argv[5];
  if(argc == 5 && checkopt != "!" ) {
    dbinit = argv[5];
  }



    // ---- use the DBCondHandler -----------

  string folder("/lccd_calice/HcalBoardsConn");
  lccd::IConditionsHandler* conData = 
    new lccd::DBCondHandler( dbinit, folder, colName, tag ) ;
  //    new lccd::DBCondHandler( lccd::getDBInitString() , folder, colName, tag ) ;  

  // ------ create a boardconnections map ------------------
  typedef lccd::ConditionsMap<int,HcalBoardsConn> HcalBoardsConnMap ;
  HcalBoardsConnMap hcalboardsconnMap( &HcalBoardsConn::getConnectorPin )   ;
  conData->registerChangeListener(  &hcalboardsconnMap )  ;
  conData->update( timeStamp ) ;
  //--------------- end boardconnection map -----------------------
  

  //Put the remarks into a seperate map using the col parameters
  //list. This is not very nice
  remarkMap_t remarkMap;
  StringVec remarkVec;
  conData->currentCollection()->getParameters().getStringVals("Remark", remarkVec);

  IntVec connPinVec;
  conData->currentCollection()->getParameters().getIntVals("Connector_Pin", connPinVec);
  StringVec::iterator remark_iter;
  IntVec::iterator connPin_iter;



   for(remark_iter = remarkVec.begin(), connPin_iter = connPinVec.begin(); 
       remark_iter != remarkVec.end(); ++remark_iter, ++connPin_iter){
 

    // Create the map which links the remarks to a connector pin
    pair<remarkMap_t::iterator,bool > update_trmap = 
       remarkMap.insert( make_pair( *connPin_iter , *remark_iter ) );

     //This should never happen but anyway
    if(!update_trmap.second) {
       throw std::runtime_error ( "hcalboards_connfromdb: Creation of remarkMap failed"); 
    	}			    

   }




  lcio::LCTime t0 ( conData->validSince()  ) ;
  lcio::LCTime t1 ( conData->validTill()  ) ;

  cout << endl 
       << " -- mapping data has been read from data base folder" << folder 
       << endl 
       << " ---- valid from: " <<  t0.getDateString()  
       << " [" <<  conData->validSince()  << "] " 
       << endl 
       << " ----       till: " <<  t1.getDateString()  
       << " [" <<  conData->validTill()  << "] "
       << endl 
       << endl ;
   

  remarkMap_t::iterator connpinremark_iter = remarkMap.find(iconnector);
  hcalboardsconnMap.find( iconnector ).print(cout, connpinremark_iter->second);


  delete conData ;

  
}
#endif
