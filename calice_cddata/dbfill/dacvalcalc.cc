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

//The interface to the hcal db tables
#include "HcalBoardsConn.hh"
#include "SiPMMappingHcal.hh"
#include "ConnCellMappingHcal.hh"
#include "TilesItep.hh"
#include "HcalCassVsCrc.hh"
#include "SiPMVolCorr.hh" 
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

typedef std::map<int,string> remarkMap_t;

using namespace std ;
using namespace lcio;
using namespace lccd ;
using namespace CALICE;


/** Small Program that calculates DAC values based on 
 *  parameters of Tile Hcal SiPMs that are stored in a 
 *  database.
 *  
 *  
 *  
 * 
 * @author R.Pöschl, DESY
 * @version $Id: dacvalcalc.cc,v 1.2 2005-12-18 15:26:46 poeschl Exp $
 */


int main(int argc, char** argv ) {
    
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;

  // read file name and collection name from command line 
  if( argc < 2) {
    cout << " usage: hcalmapfromdb <cassette> <val1> <val2> [<timestamp> <dbinit>]" << endl ;
    std::cout << " If no <timestamp> is given the current time is used" << std::endl;
    std::cout << " Give <timestamp> in format YYYY:MM:DD:HH:MM:SS" << std::endl;    
    std::cout << " If no <dbinit> is given 'flc32.desy.de:condb_1:consult:consult' used" << std::endl;
    std::cout << " Optional Parameters can be ommitted with an '!'" << std::endl;
    exit(1) ;
  }

  string cassetteStr( argv[1] ) ;
  int icassette = atoi( cassetteStr.c_str());

  string val1Str( argv[2] ) ;
  float val1 = atof( val1Str.c_str());

  string val2Str( argv[3] ) ;
  float val2 = atof( val2Str.c_str());


  //default timestamp 
  struct timespec cur_time;
  long64 time_cur;
  clock_gettime(CLOCK_REALTIME, &cur_time);
  time_cur = (long64) ( (cur_time.tv_sec)*SECTONS + cur_time.tv_nsec );
  LCTime now(time_cur) ;
  lccd::LCCDTimeStamp timeStamp   = now.timeStamp();
  //user defined timestamp;
  string checkopt;
  if (argc > 4) checkopt = argv[4];
  if (argc > 4 && checkopt != "!"){

    //The usertime
    string usertimeStr(argv[4]);
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



  string dbinit("localhost:condb_1:consult:consult");
  if (argc > 5) checkopt = argv[5];
  if(argc == 5 && checkopt != "!" ) {
    dbinit = argv[5];
  }



 //for this application we will only use the HEAD versions
 //others make no sense I think
  string tag("") ;
  // ---- use the DBCondHandler -----------

  //Create a Map for the Hcal Connections
  string folder("/lccd_calice/HcalBoardsConn");
  lccd::IConditionsHandler* hcalbdcnnData = 
    new lccd::DBCondHandler( dbinit, folder, "HcalBoardsConn", tag ) ;
  typedef lccd::ConditionsMap<int,HcalBoardsConn> HcalBoardsConnMap; 
  HcalBoardsConnMap hcalboardsconnMap( &HcalBoardsConn::getConnectorPin )   ;
  hcalbdcnnData->registerChangeListener(  &hcalboardsconnMap )  ;
  hcalbdcnnData->update( timeStamp ) ;
  //--------------- end Hcal Connections map -----------------------


  //Create a Map ConnectorPins to CellID relations
  folder = "/lccd_calice/ConnCellMap";
  lccd::IConditionsHandler* connclmapData = 
    new lccd::DBCondHandler( dbinit, folder, "ConnCellMap", tag ) ;
  typedef lccd::ConditionsMap<int,ConnCellMappingHcal> ConnCellMappingHcalMap; 
  ConnCellMappingHcalMap connclMap( &ConnCellMappingHcal::getConnpin )   ;
  connclmapData->registerChangeListener(  &connclMap )  ;
  connclmapData->update( timeStamp ) ;
  //--------------- end Hcal Connector Pins to CellID -----------------------


  // ------ create a map to relate SiPMs to CellIDs------------------
  folder = "/lccd_calice/SiPMMapHcal";
  lccd::IConditionsHandler* sipmmapData = 
    new lccd::DBCondHandler( dbinit, folder, "SiPMMapHcal", tag ) ;
  typedef lccd::ConditionsMap<int,SiPMMappingHcal> SiPMMap ;
  SiPMMap sipmMap( &SiPMMappingHcal::getExCellID )   ;
  sipmmapData->registerChangeListener(  &sipmMap )  ;
  sipmmapData->update( timeStamp ) ;
  //--------------- end SiPMs to CellIDs map -----------------------
  

  // ------ create the ITEP SiPM list------------------
  folder = "/lccd_calice/SiPMItep";
  lccd::IConditionsHandler* sipmitepData = 
    new lccd::DBCondHandler( dbinit, folder, "SiPMItep", tag ) ;
  typedef lccd::ConditionsMap<int,TilesItep> TilesItepMap ;
  TilesItepMap tilesItepMap( &TilesItep::getSIPMID )   ;
  sipmitepData->registerChangeListener(  &tilesItepMap )  ;
  sipmitepData->update( timeStamp ) ;
  //--------------- end ITEP SiPM list -----------------------

  // ------ create the list for the SiPM Voltage corr.------------------
  folder = "/lccd_calice/SiPMVolCorr";
  lccd::IConditionsHandler* sipmvcrData = 
    new lccd::DBCondHandler( dbinit, folder, "SiPMVolCorr", tag ) ;
  typedef lccd::ConditionsMap<int,SiPMVolCorr> SiPMVolCorrMap ;
  SiPMVolCorrMap sipmvcrMap( &SiPMVolCorr::getSiPMID )   ;
  sipmvcrData->registerChangeListener(  &sipmvcrMap )  ;
  sipmvcrData->update( timeStamp ) ;
  //--------------- end SiPM Voltage Corr. list -----------------------


  // ------ create the list for the Hcal cassette vs Crc relations------------------
  folder = "/lccd_calice/HcalCassVsCrc";
  lccd::IConditionsHandler* hcsvscrcData = 
    new lccd::DBCondHandler( dbinit, folder, "HcalCassVsCrc", tag ) ;
  typedef lccd::ConditionsMap<int,HcalCassVsCrc> HcalCassVsCrcMap ;
  HcalCassVsCrcMap hcsvscrcMap( &HcalCassVsCrc::getCassetteID )   ;
  hcsvscrcData->registerChangeListener(  &hcsvscrcMap )  ;
  hcsvscrcData->update( timeStamp ) ;
  //--------------- end Hcal cassette vs. Crc list -----------------------




  folder = "/lccd_calice/CellMapHcal";
  lccd::IConditionsHandler* conData = 
  new lccd::DBCondHandler( dbinit, folder, "CellMapHcal", tag ) ;
  typedef lccd::ConditionsMap<int,CellMappingHcal> HcalMap ;
  HcalMap hcalMap( &CellMappingHcal::getCellID )   ;
  conData->registerChangeListener(  &hcalMap )  ;
  conData->update( timeStamp ) ;
  //--------------- end cellmapping map -----------------------


  //  HcalBoardsConnMap::MapIter  HcalBoardsConn_iter;
  //  std::map<int,HcalBoardsConn> mymap = hcalboardsconnMap.map(); 
  //Get the iterator
  std::map<int,HcalBoardsConn>::const_iterator HcalBoardsConn_iter;
  //FIXME: This would be nice
  //HcalBoardsConnMap::const_MapIter HcalBoardsConn_iter;

  //FIXME: Here we need another table in the db
  string granularity;
  cout << "icassette: " << icassette << endl;
  if (icassette == 1) granularity = "fine";
  if (icassette == 2) granularity = "coarse";



  //loop over the connector pin
   for ( HcalBoardsConn_iter =  hcalboardsconnMap.map().begin();
	HcalBoardsConn_iter !=  hcalboardsconnMap.map().end();
	++HcalBoardsConn_iter){

     //cout << "In connector loop: " << (*HcalBoardsConn_iter).first<< endl;
     //get the chipID
     int ChipID = hcalboardsconnMap.find((*HcalBoardsConn_iter).first).getHabID(); 
     //find out if left or right hbab
     string hbab_pos = hcalboardsconnMap.find((*HcalBoardsConn_iter).first).getHbabID();

     int gran_cellID = 0;
     //std::cout << "Before pin assignment " << std::endl;
     //std::cout << "Granularity: " << granularity << endl;
       //get the cell id assigned to the connector pin
       if(granularity == "fine") { 
         //cout << "In fine gran. pin: " << (*HcalBoardsConn_iter).first << endl;
	 gran_cellID = connclMap.find((*HcalBoardsConn_iter).first).getCellID_fine();
         //cout << "In fine gran. cell: " << gran_cellID << endl;
        }

       if(granularity == "coarse") gran_cellID = connclMap.find((*HcalBoardsConn_iter).first).getCellID_coarse();
       //std::cout << "After pin assignment: " << gran_cellID << std::endl;
 

       //get the sipm number     
       if ( gran_cellID != 0) {
       //cout << "Before SiPM " << endl;
       int cellID_sipm = (icassette << CASSETTESHIFT ) | gran_cellID;  
       //cout << " cellID_sipm: " << cellID_sipm << endl;
       int sipmID = sipmMap.find(cellID_sipm).getSiPMID();
       int cellID = sipmMap.find(cellID_sipm).getCellID();          
       //cout << "After SiPM " << endl;


       //get the bias voltage of that SiPM
     float bias_voltage = 0;
     try {
       bias_voltage = tilesItepMap.find(sipmID).getVoltage();
       //cout << "After bias voltage " << endl;
     }catch (Exception e){
       cout << "Exception caught" << endl;
     }
       //check whether there is a voltage correction for that SiPM
       //FIXME: need to do this by hand due to missing const_MapIter
       //in lccd
       //in addition lccd throws a general runtime exception
       //if an element is not present
       //Need to revise exception handling
       std::map<int,HcalBoardsConn>::const_iterator HcalBoardsConn_iter;
       std::map<int,HcalBoardsConn> volcorr_map = hcalboardsconnMap.map();
       HcalBoardsConn_iter = volcorr_map.find(sipmID);       
       if (HcalBoardsConn_iter != volcorr_map.end())
       {/* do something What ????? */} 

       //get fe and slot ID
       int slotID =  hcsvscrcMap.find(icassette).getSlotID();
       int feID =  hcsvscrcMap.find(icassette).getFeID();

       //Finally let's do the dac calculation
       float new_dacval = 0;
       if (hbab_pos == "left") new_dacval = (val1 - bias_voltage - 0.23)/0.018516;
       if (hbab_pos == "right") new_dacval = (val2 - bias_voltage - 0.23)/0.018516;
   
       short channel =  hcalMap.find(cellID).getMulID();
       cout << "Slot: " << slotID << " Fe: " << feID << " Chip: " <<
	 ChipID << " Channel: " << channel <<  " New Dacval: " << (int) new_dacval << std::endl;    

       }
    }


    


  /*for ( HcalBoardsConn_iter =  mymap.begin();
	HcalBoardsConn_iter !=  mymap.end();
	++HcalBoardsConn_iter){

    cout << "In connector loop" << endl;
    }*/
 


  /*  lcio::LCTime t0 ( conData->validSince()  ) ;
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
  */ 


  
}
#endif
