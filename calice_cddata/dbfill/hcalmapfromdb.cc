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

#include "CellMappingHcal.hh"

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


using namespace std ;
using namespace lcio;
using namespace lccd ;
using namespace CALICE;

/** Test program that reads the geom. cellid for a given
 * electronic cellid from the data base and writes 
 *  and dumps it on the screen
 * @author R.Pöschl, DESY
 * @based on example programs by F. Gaede, DESY
 */

int main(int argc, char** argv ) {
  
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;

  // read file name and collection name from command line 
  if( argc < 6) {
    cout << " usage: hcalmapfromdb <crate> <slot> <fe> <adc> <mul> [<timestamp> <tag> <db collection name> <dbinit>]" << endl ;
    std::cout << " If no <timestamp> is given the current time is used" << std::endl;
    std::cout << " Give <timestamp> in format YYYY:MM:DD:HH:MM:SS" << std::endl;    
    std::cout << " If no <tag> is given the HEAD is taken" << std::endl;
    std::cout << " If no <db collectioname> is given 'CellMapHcal' used" << std::endl;
    std::cout << " If no <dbinit> is given 'flc32.desy.de:condb_1:consult:consult' used" << std::endl;
    std::cout << " Optional Parameters can be ommitted with an '!'" << std::endl;
    exit(1) ;
  }

  string crateStr( argv[1] ) ;
  int icrate = atoi( crateStr.c_str());
  string slotStr( argv[2] ) ;
  int islot = atoi( slotStr.c_str());
  string feStr( argv[3]);
  int ife = atoi( feStr.c_str());
  string mulStr( argv[5]);
  int imul = atoi( mulStr.c_str());
  string adcStr( argv[4]);
  int iadc = atoi( adcStr.c_str());



  //default timestamp 
  struct timespec cur_time;
  long64 time_cur;
  clock_gettime(CLOCK_REALTIME, &cur_time);
  time_cur = (long64) ( (cur_time.tv_sec)*SECTONS + cur_time.tv_nsec );
  LCTime now(time_cur) ;
  lccd::LCCDTimeStamp timeStamp   = now.timeStamp();
  //user defined timestamp;
  string checkopt;
  if (argc > 6) checkopt = argv[6];
  if (argc > 6 && checkopt != "!"){

    //The usertime
    string usertimeStr(argv[6]);
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
  if (argc > 7) checkopt = argv[7];
  if (argc > 7 && checkopt != "!") tag = argv[7];

  cout << "tag= " << tag << endl;

  
  string colName( "CellMapHcal" ) ;
  if (argc > 8) checkopt= argv[8];
  if ( argc > 8 && checkopt != "!" ) colName = argv[8];

  string dbinit("flc32.desy.de:condb_1:consult:consult");
  if (argc > 9) checkopt = argv[9];
  if(argc == 9 && checkopt != "!" ) {
    dbinit = argv[9];
  }



    // ---- use the DBCondHandler -----------

  string folder("/lccd_calice/CellMapHcal");
  lccd::IConditionsHandler* conData = 
    new lccd::DBCondHandler( dbinit, folder, colName, tag ) ;
  //    new lccd::DBCondHandler( lccd::getDBInitString() , folder, colName, tag ) ;  

  // ------ create a cellmapping map ------------------
  typedef lccd::ConditionsMap<int,CellMappingHcal> HcalMap ;
  HcalMap hcalMap( &CellMappingHcal::getElecChannel )   ;
  conData->registerChangeListener(  &hcalMap )  ;
  conData->update( timeStamp ) ;
  //--------------- end cellmapping map -----------------------



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
   

  //create the electronic channel from the input
  int elec_channel = (icrate << CRATESHIFT) | ( islot << SLOTSHIFT ) |
    ( ife << FESHIFT) | ( imul << MULSHIFT) | (iadc << ADCSHIFT);

  hcalMap.find( elec_channel ).print(cout) ;


  delete conData ;

}
#endif
