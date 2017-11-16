#ifdef USE_CONDDB
// -- C++ headers 
#include <cstdlib>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
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
#include "lccd/DBCondHandler.hh"
// -- LCCD headers
#include "lccd.h"
#include "lccd/DBInterface.hh"

//calice_userlib header
#include "RunTimeWhizard.hh"
#include "collection_names.hh"
#include "RunTimeInfo.hh"
#include "BmlSlowRunDataBlock.hh"
#include "FeConfigurationBlock.hh"

using namespace std ;
using namespace lcio;
using namespace CALICE;
using namespace lccd;

typedef std::vector< lcio::LCCollection* > ColVec ;

/** Test program to browse holdvalues for a given run
 * 
 * @author F.Gaede, DESY
 * @version $Id: browseHoldVals.cc,v 1.2 2008-06-16 15:01:35 lima Exp $
 */

int main(int argc, char** argv ) {
  
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;

  // read file name and collection name from command line 
  if( argc < 2) {
    cout << " usage: browseInfoAt <runnumber> [ <folder> <tag> <dbInit>]" << endl ;
    exit(1) ;
  }
  
  int run(atoi(argv[1]));

  //The folder in which the runtime info is be stored
  std::string folder("/cd_calice/CALDAQ_RunTimeInfo");
  if(argc > 2 )string folder( argv[2]) ;
  std::cout << "Folder is: " << folder << std::endl;
  
  string dbInit("flccaldb02.desy.de:calice:caliceon:Delice.1:3306") ;
  if( argc > 3 ) dbInit = argv[3] ;
  
  std::cout << "dbinitstring is: " << dbInit << std::endl;
  //lccd::DBInterface db( dbInit, folder , true ) ;

  //std::ofstream file;
  //file.open("out.txt");
  //file << "Runnumber" << std::setw(25) << "Current Begin of Run[A]" << std::setw(25) << "Current End of Run[A]" << std::setw(25) << "Start of Run" << std::setw(30) << "End of Run " << std::setw(35) << "Cern Timestamp" << std::setw(30) << "Commment" << std::endl;;

  //folder to look for cern beam parameters
  std::string foldername("/cd_calice_v0402_fnalcomb/CALDAQ_EmcFeConfiguration");
  //folder to check for runtyp
  std::string folderruninfo("/cd_calice_v0402_fnalcomb/CALDAQ_RunInfo");
  IConditionsHandler* conDataRunInfo(0);
  IConditionsHandler* conData(0);
  LCTime starttime(0);
  LCTime stoptime(0);
  LCTime till(0);
  LCTime since(0);
  LCTime testtime(0);
  int holdsequence(0);
  LCCollection* _emcFeConfCol(0);
  std::string till_string("");
  std::string since_string("");
  bool found(false);
  int emcfe_confblocks;
  RunTimeWhizard runtimewhizard(folder, dbInit);

for ( int runnum=run; runnum<run+1; runnum++) {
  //Get start and stoptime of the run
    try{
      runtimewhizard.print(runnum, std::cout);
      starttime = runtimewhizard.getRunStartTime(runnum);
      stoptime  = runtimewhizard.getRunStopTime(runnum);
    } catch ( lcio::Exception & aExc ) {
      continue;
    }
    //extract the runtype
    //conDataRunInfo= new DBCondHandler(dbInit,folderruninfo, "dummy","") ;
    //LCCollection* colR(0);
    //try{
    //  conDataRunInfo->update(starttime.timeStamp()) ;
    //  colR = conDataRunInfo->currentCollection();
    //  if(colR->getParameters().getStringVal(PAR_RUN_TYPE) != "beamData") continue;
    //  else std::cout << "run: " << runnum << " is beamData" << std::endl; 
    //} catch ( lcio::Exception & aExc ) {
    //  std::cout << "Warning - No Runinfo for run: " << runnum << std::endl; 
    //  continue;
    // }

    conData= new DBCondHandler(dbInit,foldername, "dummy","") ;
    
    //Loop in steps of 5 musec until we find the first entry of the FeConf datax
      testtime=starttime;
      while (!found) {
	try {
	  conData->update(testtime.timeStamp()) ;
	  found=true;
	} catch ( lcio::Exception & aExc ) {
	  std::cout << "No emc fe information for run: " << runnum << " and timestamp: " << testtime.getDateString() << std::endl;
          testtime+=10000000LL;
	  continue;
	}
    }

      //Loop over all entries for this run
      while(1) {
	try {
	  _emcFeConfCol = conData->currentCollection();
	  emcfe_confblocks = _emcFeConfCol->getNumberOfElements();
	  std::cout << "Holdsequence: " << holdsequence << std::endl;
	  for (int ife = 0; ife < emcfe_confblocks; ife++) {
	    CALICE:: FeConfigurationBlock(_emcFeConfCol->getElementAt(ife));
	    FeConfigurationBlock feConfBlock(_emcFeConfCol->getElementAt(ife));
	    //bmlSlowRunDataBlock.print(std::cout);
            std::cout << "EMC FeConfiguration: CrateID " << feConfBlock.getCrateID() << std::endl;   
            std::cout << "EMC FeConfiguration: SlotID " << feConfBlock.getSlotID() << std::endl;   
            std::cout << "EMC FeConfiguration: BoardComponentNumber " << feConfBlock.getBoardComponentNumber() << std::endl;   
	    std::cout << "holdStart: " << feConfBlock.getHoldStart() << std::endl;
	    std::cout << "holdWidth: " << feConfBlock.getHoldWidth() << std::endl;
	    std::cout << "holdInvert: " << feConfBlock.isHoldInvert() << std::endl;
	  }
	  till_string =  _emcFeConfCol->getParameters().getStringVal((lccd::DBTILL).c_str());
	  till = static_cast<long64>(strtoll(till_string.c_str(),0,0));
	  //std::cout << "till: " << till.getDateString() << std::endl;          
	  //since_string = _emcFeConfCol->getParameters().getStringVal((lccd::DBSINCE).c_str());
	  //since = static_cast<long64>(strtoll(since_string.c_str(),0,0));
          if(till.unixTime() >= stoptime.unixTime()) break;
	  else conData->update(till.timeStamp());      
          holdsequence++; 
	} catch ( lcio::Exception & aExc ) {
	  std::cout << "No emc fe  information at run End for run: " << runnum << std::endl;
	  continue;
	}
      }

    if(conData) { delete conData; conData=0; }
    if(conDataRunInfo) { delete conDataRunInfo; conDataRunInfo=0; }
}
//file.close();
}


#endif
