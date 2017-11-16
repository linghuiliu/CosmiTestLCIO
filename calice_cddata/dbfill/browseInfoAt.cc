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

using namespace std ;
using namespace lcio;
using namespace CALICE;
using namespace lccd;

typedef std::vector< lcio::LCCollection* > ColVec ;

/** Test program that tags an existing folder in the database.
 * 
 * @author F.Gaede, DESY
 * @version $Id: browseInfoAt.cc,v 1.2 2008-06-16 15:01:35 lima Exp $
 */

int main(int argc, char** argv ) {
  
  // enable LCIO exception handling (usually automatically done when Reader/Writer exists)
  HANDLE_LCIO_EXCEPTIONS ;

  // read file name and collection name from command line 
  if( argc < 2) {
    cout << " usage: browseInfoAt <runnumber> [ <folder> <tag> <dbInit>]" << endl ;
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

  std::ofstream file;
  file.open("out.txt");
  file << "Runnumber" << std::setw(25) << "Current Begin of Run[A]" << std::setw(25) << "Current End of Run[A]" << std::setw(25) << "Start of Run" << std::setw(30) << "End of Run " << std::setw(35) << "Cern Timestamp" << std::setw(30) << "Commment" << std::endl;;

  //folder to look for cern beam parameters
  std::string foldername("/cd_calice_v0402_cerncomb/CALDAQ_BmlSroRunDataCern");
  //folder to check for runtyp
  std::string folderruninfo("/cd_calice_v0402_cerncomb/CALDAQ_RunInfo");
  IConditionsHandler* conDataRunInfo(0);
  IConditionsHandler* conData(0);
  LCTime starttime(0);
  LCTime stoptime(0);
  int timecor(0);
  RunTimeWhizard runtimewhizard(folder, dbInit);
  //for (runnum=320550; runnum<321101; runnum++) {
for (runnum=330000; runnum<331693; runnum++) {
  //for (runnum=300000; runnum<300965; runnum++) {
    //if(runnum > 300440 && runnum < 300885) timecor=7200;
    //if(runnum >=300885) timecor=3600;
  //for (runnum=320549; runnum<321111; runnum++) {
  //  if(runnum > 300440 && runnum < 300885) timecor=7200;
  //  if(runnum >=300885) timecor=3600;
    try{
      runtimewhizard.print(runnum, std::cout);
      starttime = runtimewhizard.getRunStartTime(runnum);
      stoptime  = runtimewhizard.getRunStopTime(runnum);
    } catch ( lcio::Exception & aExc ) {
      continue;
    }
    //extract the runtype
    conDataRunInfo= new DBCondHandler(dbInit,folderruninfo, "dummy","") ;
    LCCollection* colR(0);
    try{
      conDataRunInfo->update(starttime.timeStamp()) ;
      colR = conDataRunInfo->currentCollection();
      if(colR->getParameters().getStringVal(PAR_RUN_TYPE) != "beamData") continue;
      else std::cout << "run: " << runnum << " is beamData" << std::endl; 
    } catch ( lcio::Exception & aExc ) {
      std::cout << "Warning - No Runinfo for run: " << runnum << std::endl; 
      continue;
    }

    conData= new DBCondHandler(dbInit,foldername, "dummy","") ;
    
    LCTime till(0);
    LCTime since(0);
    LCTime cernTimeStamp(0);
    std::string till_string("");
    std::string since_string("");
    //std::vector<double> bendCurrents;
    double bendCurBofr;
    double bendCurEofr;
    bool havebofr(false);
    bool haveeofr(false);
    LCCollection* colE(0);
    try {
      conData->update(starttime.timeStamp()) ;
      colE = conData->currentCollection();
      CALICE::BmlSlowRunDataBlock bmlSlowRunDataBlock(colE->getElementAt(0));
      //bmlSlowRunDataBlock.print(std::cout);
      bendCurBofr = bmlSlowRunDataBlock.getBendCurrents().at(8);
      cernTimeStamp = bmlSlowRunDataBlock.getTimeStamp();
      till_string =  colE->getParameters().getStringVal((lccd::DBTILL).c_str());
      till = static_cast<long64>(strtoll(till_string.c_str(),0,0));
      since_string =  colE->getParameters().getStringVal((lccd::DBSINCE).c_str());
      since = static_cast<long64>(strtoll(since_string.c_str(),0,0));
      //std::cout << "till: " << till.getDateString() << std::endl;
      havebofr=true;
    } catch ( lcio::Exception & aExc ) {
      std::cout << "No beam parameter information for run: " << runnum << std::endl;
      continue;
    }

    try {
      conData->update(till.timeStamp()) ;
      colE = conData->currentCollection();
      CALICE::BmlSlowRunDataBlock bmlSlowRunDataBlock(colE->getElementAt(0));
      //bmlSlowRunDataBlock.print(std::cout);
      bendCurEofr = bmlSlowRunDataBlock.getBendCurrents().at(8);
      till_string =  colE->getParameters().getStringVal((lccd::DBTILL).c_str());
      till = static_cast<long64>(strtoll(till_string.c_str(),0,0));
      haveeofr=true;
    } catch ( lcio::Exception & aExc ) {
      std::cout << "No beam parameter information at run End for run: " << runnum << std::endl;
      continue;
    }
    
    if ((havebofr && haveeofr)) {
      std::cout << "Run: " << runnum << " Reading at run begin is: " << bendCurBofr*(1000/4.72) << std::endl;
      std::cout << "Run: " << runnum << " Reading at run end is: " << bendCurEofr*(1000/4.72) << std::endl;
    }
    if( (havebofr && haveeofr) && (bendCurBofr != bendCurEofr || fabs(fabs(since.unixTime()-cernTimeStamp.unixTime())-timecor)> 0  )) {
      //std::cout << "Beg of run reading not equal end of run reading for run: " << runnum << std::endl;
      //std::cout << "Reading at run begin is: " << bendCurBofr << std::endl;
      //std::cout << "Reading at run end is: " << bendCurEofr << std::endl;
      //std::cout << "fabs 1: " << fabs(since.unixTime()-cernTimeStamp.unixTime()) << std::endl;  
      //std::cout << "fsbs: " << fabs(fabs(since.unixTime()-cernTimeStamp.unixTime())-7200) << std::endl;
      //if( fabs((bendCurBofr-bendCurEofr)/bendCurBofr) < 0.05) file << runnum << std::setw(25) << bendCurBofr << std::setw(25) << bendCurEofr << std::endl;
      //else file << runnum << std::setw(25) << bendCurBofr << std::setw(25) << bendCurEofr << std::setw(15) << "Warning" << std::endl;
      if( fabs((bendCurBofr-bendCurEofr)/bendCurBofr) > 0.05) file << runnum << std::setw(15) << bendCurBofr << std::setw(30) << bendCurEofr << std::setw(40) << since.getDateString() << std::setw(35) << till.getDateString() << std::setw(35) << cernTimeStamp.getDateString() << std::setw(25) << "Current Warning" << std::endl;
      if( fabs(fabs(since.unixTime()-cernTimeStamp.unixTime())-timecor) > 0 ) file << runnum << std::setw(15) << bendCurBofr << std::setw(30) << bendCurEofr << std::setw(40) << since.getDateString() << std::setw(35) << till.getDateString() << std::setw(35) << cernTimeStamp.getDateString() << std::setw(25) << "Timestamp Warning" << std::endl;
    }

    if(conData) { delete conData; conData=0; }
    if(conDataRunInfo) { delete conDataRunInfo; conDataRunInfo=0; }
  }
  file.close();
}


#endif
