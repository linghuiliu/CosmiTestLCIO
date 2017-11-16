#include <iostream>

// --- lcio headers 
#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "UTIL/LCTime.h"
// ------------------

// -- LCCD headers
#include "lccd.h"
#include "lccd/StreamerMgr.hh"
#include "lccd/VCollectionStreamer.hh"
#include "lccd/DBInterface.hh"


//#include "TriggerAssignment.hh"
//#include "TriggerDefinition.def"
#include "collection_names.hh"
#include "time.h"

//static int NBOARD = 2 ;

using namespace std ;
using namespace lcio;


/** Small program that writes some a trigger map into the database.<br>
 * 
 * @author R.Pöschl, DESY
 * @based on example programs by F. Gaede, DESY
 */

int main(int argc, char** argv ) {
  

   string dbinit("localhost:condb_1:condb:condb"); 

 if(argc > 1) {
   dbinit = argv[1];
 }

  
  //Ini database
  string colNameDB(COL_TRIGGER_ASSIGNMENT);
  string folder( "/cd_calice/" + colNameDB ) ;

  
  //Create vectors of trigger types and their bit positions
  std::vector<int> TriggerPosition;
  std::vector<string> TriggerType;


  //We have 32 bits to assign
  for (int i=0; i<13; i++) {
    TriggerPosition.push_back(i);
    TriggerType.push_back("UNKNOWN");
  }
  
  TriggerPosition.push_back(13);
  TriggerType.push_back("BEAMDATA");
  
  for (int i=14; i<21; i++) {
    TriggerPosition.push_back(i);
    TriggerType.push_back("UNKNOWN");
  }
  
  TriggerPosition.push_back(21);
  TriggerType.push_back("GENERICBIT");
  TriggerPosition.push_back(22);
  TriggerType.push_back("UNKNOWN");
  TriggerPosition.push_back(23);
  TriggerType.push_back("UNKNOWN");
  TriggerPosition.push_back(24);
  TriggerType.push_back("PEDESTAL");
  
  
  
  for (int i=25; i<32; i++) {
    TriggerPosition.push_back(i);
    TriggerType.push_back("UNKNOWN");
  }
  
  
  
  LCCollectionVec* col = new LCCollectionVec( LCIO::LCGENERICOBJECT ) ;
  LCGenericObjectImpl* triggerobj = new LCGenericObjectImpl(0,0,0);
  col->addElement(triggerobj);
  col->parameters().setValues(PAR_TRIGGER_TYPE_NAMES, TriggerType); 
  col->parameters().setValues(PAR_TRIGGER_BITS, TriggerPosition); 
  lccd::DBInterface db(dbinit , folder , true ) ;
  
  //string sinceStr( "0" ) ;
  //string tillStr( "100" ) ;
  //lccd::LCCDTimeStamp since = std::atoll( sinceStr.c_str()  )  ;
  //lccd::LCCDTimeStamp till  = std::atoll( tillStr.c_str()  )  ;

  //lccd::LCCDTimeStamp since = 0*1000000000LL;
  //lccd::LCCDTimeStamp till  = 75690*1000000LL;

  //lccd::LCCDTimeStamp since = 0*1000000000LL;
  //lccd::LCCDTimeStamp till  = 75690*1000000LL;

  //lccd::LCCDTimeStamp since  = 75690*1000000LL;
  LCTime begin( 2005,1, 1, 0 , 0 , 0 ) ;
  lccd::LCCDTimeStamp since   = begin.timeStamp();
  LCTime farFuture( 2045,1, 1, 0 , 42 , 0 ) ;
  lccd::LCCDTimeStamp till   = farFuture.timeStamp();
  //lccd::LCCDTimeStamp till   = 2000000*1000000LL;


  db.storeCollection(since,till,col, " triggerassigment v00000"); 

}
