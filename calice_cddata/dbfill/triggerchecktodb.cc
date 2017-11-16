#include <iostream>

// --- lcio headers 
#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTime.h"
// ------------------

// -- LCCD headers
#include "lccd.h"

#include "lccd/StreamerMgr.hh"
#include "lccd/VCollectionStreamer.hh"
#include "lccd/DBInterface.hh"


#include "TriggerCheck.hh"
#include "collection_names.hh"
#include "time.h"


using namespace std ;
using namespace lcio;
using namespace CALICE;
//using namespace lccd;

/** Test program that writes the trigger check parameters constants to
 *  the db.<br>
 * 
 * @author R.Pöschl, DESY
 */

int main(int argc, char** argv ) {
  

 string dbinit("localhost:condb_1:condb:condb"); 

 if(argc > 1) {
   dbinit = argv[1];
 }



  //Ini database
  string colNameDB(COL_TRIGGER_CHECK);
  string folder( "/cd_calice_cernbeam/" + colNameDB ) ;

  LCCollectionVec* col = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;

  TriggerCheck* triggercheck = new TriggerCheck( 195, 1, 55, 60) ;
  col->addElement( triggercheck ) ;
  lccd::DBInterface db(dbinit, folder , true ) ;
  //lccd::LCCDTimeStamp since  = 75690*1000000LL;
  //LCTime farFuture( 2045,1, 1, 0 , 42 , 0 ) ;
  //lccd::LCCDTimeStamp till   = farFuture.timeStamp();
  //lccd::LCCDTimeStamp till   = 2000000*1000000LL;
  LCTime begin( 2010,7, 1, 0 , 0 , 0 ) ;
  lccd::LCCDTimeStamp since   = begin.timeStamp();
  LCTime farFuture( 2011,12, 31, 23 , 59 , 0 ) ;
  lccd::LCCDTimeStamp till   = farFuture.timeStamp();

  //db.storeCollection(since,till,col, "trigger_check_v00002"); 
    db.storeCollection(0, lccd::LCCDPlusInf,col, "trigger_check_v00002");
}
