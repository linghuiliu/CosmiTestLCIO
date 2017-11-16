// -- C++ headers 
#include <iostream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>


// -- CondDB headers
#include "ConditionsDB/ICondDBMgr.h"
//#include "exampleObject.h"

// -- LCIO headers
#include "lcio.h"
#include "IO/LCWriter.h"
#include "EVENT/LCGenericObject.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/LCTime.h"
#include "LCIOSTLTypes.h"

// -- LCCD headers
#include "lccd.h"
#include "lccd/DBCondHandler.hh"
#include "lccd/DBInterface.hh"
#include "lccd/ConditionsMap.hh"

//userlib headers
#include "collection_names.hh" 
#include "ExperimentalSetup.hh"
#include "BmlSlowRunDataBlock.hh"

// -- To get the current time
#define SECTONS 1000000000LL
#include <time.h>

#include "RunTimeWhizard.hh"
#include "RunTimeInfo.hh"

#include "LCPayload.hh"
#include "MapConstants.hh"
#include "SimConstants.hh"
#include "AlnConstants.hh"
#include "FitConstants.hh"


int main(int argc, char** argv ) {
  /* 
   if (argc < 2){
     std::cout << " Usage : " << argv[0] << 
       "<location> being CERN1, CERN2, CERN3 or DESY." << std::endl;
     exit(1);
   }
  */

  std::string dbinitRead("flccaldb02.desy.de:calice:caliceon:Delice.1:3306"); 
  CALICE::RunTimeWhizard runtimewhizard("/cd_calice/CALDAQ_RunTimeInfo",dbinitRead);
    
  for(int r(1);r<argc;r++) {
    std::istringstream sin(argv[r]);
    unsigned run;
    sin >> run;
    std::cout << std::endl << "***** Inspecting Db for Run Number " << argv[r] << " = " << run << std::endl;
    
    LCTime starttime(0);
    LCTime stoptime(0);
    LCTime middle(0);
    try {
      starttime = runtimewhizard.getRunStartTime(run);
      stoptime = runtimewhizard.getRunStopTime(run);
      
      std::cout << " ------> Run is valid between " << starttime.getDateString() << " and " << stoptime.getDateString() << std::endl;
      
      middle = static_cast<long64>((starttime.timeStamp()+stoptime.timeStamp())/2.);
      std::cout << " ------> Run middle time: " << middle.getDateString() << std::endl;
      
      //for(unsigned i(0);i<2;i++) {
	//std::string base("/magnan_test/CERN/");
	//if(i==0) base="/magnan_test/DESY/";
	//std::string base("/cd_calice_cernbeam/");
	//if(i==0) base="/cd_calice_beam/";

      std::string base("/cd_calice_cernbeam/");
      if(run<300000 || run>=400000) base="/cd_calice_beam/";
	
	try {
	  DBCondHandler conData0(dbinitRead,base+"TBTrack/SimConstants","dummy") ;
	  conData0.update(static_cast<lccd::LCCDTimeStamp>(middle.timeStamp())) ;
	  LCCollection* col0 = conData0.currentCollection();
	  //assert (col0->getNumberOfElements()==1);
	  LCPayload<TBTrack::SimConstants> p0(dynamic_cast<LCGenericObject*>(col0->getElementAt(0)));
	  p0.payload().print();
	} catch(CondDBException &cExc) {
	} catch ( lcio::Exception & aExc) {
	}
	
	try {
	  DBCondHandler conData1(dbinitRead,base+"TBTrack/AlnConstants","dummy") ;
	  conData1.update(static_cast<lccd::LCCDTimeStamp>(middle.timeStamp())) ;
	  LCCollection* col1 = conData1.currentCollection();
	  assert (col1->getNumberOfElements()==1);
	  LCPayload<TBTrack::AlnConstants> p1(dynamic_cast<LCGenericObject*>(col1->getElementAt(0)));
	  p1.payload().print();
	} catch(CondDBException &cExc) {
	} catch ( lcio::Exception & aExc) {
	}
	
	try {
	  DBCondHandler conData2(dbinitRead,base+"TBTrack/FitConstants","dummy") ;
	  conData2.update(static_cast<lccd::LCCDTimeStamp>(middle.timeStamp())) ;
	  LCCollection* col2 = conData2.currentCollection();
	  assert (col2->getNumberOfElements()==1);
	  LCPayload<TBTrack::FitConstants> p2(dynamic_cast<LCGenericObject*>(col2->getElementAt(0)));
	  p2.payload().print();
	} catch(CondDBException &cExc) {
	} catch ( lcio::Exception & aExc) {
	}
	//}
      
    }
    catch ( lcio::Exception & aExc) {
      std::cout << "lcio::Exception caught, run time not in database. Continuing..." << std::endl;
    }
    }
    
    /*


  //double inzplane(0);
  //std::istringstream(argv[1])>>inzplane;

  //read a file
  std::string location(argv[1]);
  //std::string dataType(argv[2]);

  assert (location == "CERN1" || location == "CERN2" || location == "CERN3" || location == "DESY");

  std::string dbinit("localhost:calice:root:Delice.1"); 
  std::string dbinitWrite("flccaldb02.desy.de:calice:calicedb:bh7+4FUw:3306"); 

  std::string folder;
  //if (location == "DESY") folder = "/cd_calice_beam/Tracking/";
  //else if (location == "CERN1" || location == "CERN2" || location == "CERN3") folder = "/cd_calice_cernbeam/Tracking/";

  if (location == "DESY")  folder = "/magnan_test/DESY/TBTrack/";
  else                     folder = "/magnan_test/CERN/TBTrack/";

  //if (dataType == "data") folder += "data/";
  //else if (dataType == "mc") folder += "mc/";
  //else  {
  //  std::cout << " Wrong data type (first argument), should be <data> or <mc>, exiting !!" << std::endl;
  //  exit(0);
  //}

  std::string dbColName[5];
  dbColName[0] = "mc/SimConstants";
  dbColName[1] = "mc/AlnConstants";
  dbColName[2] = "data/AlnConstants";
  dbColName[3] = "mc/FitConstants";
  dbColName[4] = "FitConstants";


  std::string dbFolder[5];
  for (int col = 0; col < 5; col++){
    //loop over collections to create
    dbFolder[col] = folder+dbColName[col];
  }

  //The LCIO collection which will be written to the db
  TBTrack::SimConstants scDesy(0);
  TBTrack::SimConstants scCern(1);
  
  LCCollectionVec* simCol=new LCCollectionVec( LCIO::LCGENERICOBJECT );
  LCPayload<TBTrack::SimConstants> *p;
  if (location == "DESY") p = new LCPayload<TBTrack::SimConstants>(scDesy);
  else p = new LCPayload<TBTrack::SimConstants>(scCern);
  simCol->addElement(dynamic_cast<LCGenericObject*>(p)) ;

  TBTrack::AlnConstants tcData;
  TBTrack::AlnConstants tcMC;
  //desy
  if (location == "DESY"){
    tcData=TBTrack::AlnConstants(0,2);
    tcMC=TBTrack::AlnConstants(0,1);
  }
  else {
    tcData=TBTrack::AlnConstants(1,2);
    tcMC=TBTrack::AlnConstants(1,1);
  }

  LCCollectionVec* alnColMC=new LCCollectionVec( LCIO::LCGENERICOBJECT );
  LCCollectionVec* alnColData=new LCCollectionVec( LCIO::LCGENERICOBJECT );
  LCPayload<TBTrack::AlnConstants> *qData = new LCPayload<TBTrack::AlnConstants>(tcData);
  alnColData->addElement(dynamic_cast<LCGenericObject*>(qData)) ;
  LCPayload<TBTrack::AlnConstants> *qMC = new LCPayload<TBTrack::AlnConstants>(tcMC);
  alnColMC->addElement(dynamic_cast<LCGenericObject*>(qMC)) ;

  //Now start the storage into the db
  std::cout << " Will now call the database to fill it with the new collections." << std::endl;

  //Open the database for writing
  lccd::DBInterface db0( dbinitWrite , dbFolder[0] , true ) ;
  std::cout << " ------> Database has been accessed, now writting the new collection." << std::endl;
  //..and finally store the collection
  std::string description0 = "simConstants";
  //lcio::LCTime t0(lccd::LCCDMinusInf);
  //lcio::LCTime t1(lccd::LCCDPlusInf);
  LCTime begin( 1970,1, 1, 0 , 0 , 0 ) ;
  lccd::LCCDTimeStamp since   = begin.timeStamp();
  LCTime farFuture( 2038,12, 31, 23 , 59 , 59 ) ;
  lccd::LCCDTimeStamp till   = farFuture.timeStamp();
  std::cout << " ------> Trying to store collection : " << dbFolder[0] << " between : " << begin.getDateString() << " and " << farFuture.getDateString() << std::endl;

  db0.storeCollection(since,till,simCol,description0);
  std::cout << " ------> Collection : " << dbFolder[0] << " has been successfully stored between : " << begin.getDateString() << " and " << farFuture.getDateString() << std::endl;

  lccd::DBInterface db1( dbinitWrite , dbFolder[1] , true ) ;
  //..and finally store the collection
  std::string description1 = "alnConstants";

  db1.storeCollection(since,till,alnColMC,description1);
  std::cout << " ------> Collection : " << dbFolder[1] << " has been successfully stored between : " << begin.getDateString() << " and " << farFuture.getDateString() << std::endl;

  lccd::DBInterface db2( dbinitWrite , dbFolder[2] , true ) ;
  db2.storeCollection(since,till,alnColData,description1);
  std::cout << " ------> Collection : " << dbFolder[2] << " has been successfully stored between : " << begin.getDateString() << " and " << farFuture.getDateString() << std::endl;


  //for fit constants, depend on the run. Need to loop over all runs:
  int runStart = 230097;
  int runEnd = 230273;

  if (location == "CERN1") {
    runStart = 300000;
    runEnd = 300966;
  }
  else if (location == "CERN2") {
    runStart = 310000;
    runEnd = 310075;
  }
  else if (location == "CERN3") {
    runStart = 330000;
    runEnd = 332000;
  }
  //cern1: 300000->300966
  //cern ecal: 310000->310075
  //cern07: 330000->333000

  //Now start the storage into the db
  lccd::DBInterface db3( dbinitWrite , dbFolder[4] , true ) ;
  std::cout << " ------> Database has been accessed, now writting the new collection." << std::endl;
  std::string description3 = "FitConstants";

  for (int run = runStart; run<=runEnd; run++){//loop on all run

    bool validEnergy(false);
    bool cerna(true);

    std::cout << "------------ Processing run #" << run << " --------------" << std::endl;
    //get the time stamp of the run
    LCTime starttime(0);
    LCTime stoptime(0);
    LCTime middle(0);
    try {
      starttime = runtimewhizard.getRunStartTime(run);
      stoptime = runtimewhizard.getRunStopTime(run);

      std::cout << " ------> Run is valid between " << starttime.getDateString() << " and " << stoptime.getDateString() << std::endl;

      middle = static_cast<long64>((starttime.timeStamp()+stoptime.timeStamp())/2.);
      std::cout << " ------> Run middle time: " << middle.getDateString() << std::endl;

      LCTime ref(2007,9,1,0,0,0);
      if (starttime.timeStamp() > ref.timeStamp()){
	std::cout << " Start time is after " << ref.getDateString() << ", i.e. period not implemented in this program ! Continuing..." << std::endl;
	continue;
      }

    }
    catch ( lcio::Exception & aExc) {
      std::cout << "lcio::Exception caught, run time not in database. Continuing..." << std::endl;
      continue;
    }

    //check data type = beamData
    //       std::string colName = "CALDAQ_RunInfo";
    //       lccd::IConditionsHandler* conData=0;
    //       if (location == "DESY") conData= new lccd::DBCondHandler(dbinitRead,"/cd_calice_v0402_beam/"+colName, "" ) ;
    //       else {
    // 	if (run < 310000) conData= new lccd::DBCondHandler(dbinitRead,"/cd_calice_v0402_cerncomb/"+colName, "" ) ;
    // 	else if (run < 320000) conData= new lccd::DBCondHandler(dbinitRead,"/cd_calice_v0402_cernecal/"+colName, "" ) ;
    // 	else if (run < 330000) conData= new lccd::DBCondHandler(dbinitRead,"/cd_calice_v0402_cernhcal/"+colName, "" ) ;
    //       }
    //       //Access the folder at the desired time stamp
    //       conData->update( static_cast<lccd::LCCDTimeStamp>(starttime.timeStamp()));
    //       //Get the collection
    //       LCCollection* colT = conData->currentCollection();
    //       //Obtain the value
    //       string runMajorType = colT->getParameters().getStringVal(PAR_RUN_MAJ_TYPE);
    //       string runType = colT->getParameters().getStringVal(PAR_RUN_TYPE);
    //       string runSubType = colT->getParameters().getStringVal(PAR_RUN_SUB_TYPE);
    //       std::cout << "----> PAR_RUN_NUMBER: " << colT->getParameters().getIntVal(PAR_RUN_NUMBER) << std::endl;
    //       std::cout << "----> PAR_RUN_TYPE : " << colT->getParameters().getStringVal(PAR_RUN_TYPE) << std::endl;
    //       std::cout << "----> PAR_RUN_MAJ_TYPE : " << colT->getParameters().getStringVal(PAR_RUN_MAJ_TYPE) << std::endl;
    //       std::cout << "----> PAR_RUN_SUB_TYPE : " << colT->getParameters().getStringVal(PAR_RUN_SUB_TYPE) << std::endl;
    //       delete conData;
    //       if (runType != "beamData") {
    // 	std::cout << " ---> run is not of type beamData, continuing..." << std::endl;
    // 	//continue;
    //       }

      //retrieve energy
      unsigned int energy = 999;
      ////cd_calice_v0402_cernxxxx/CALDAQ_BmlSroRunDataCern
      //where xxxx=ecal,hcal or comb
      if (location == "DESY") {
	IConditionsHandler* conData = new DBCondHandler( dbinitRead,"/cd_calice_beam/ExperimentalSetup", "dummy") ;
	//conData->registerChangeListener( &_experimentalSetupChange )  ;
	
	conData->update(static_cast<lccd::LCCDTimeStamp>(middle.timeStamp())) ;
	LCCollection* colE = conData->currentCollection();
	CALICE::ExperimentalSetup expSetup(colE->getElementAt(0));
	//to treat the 1.5 DESY case: put E to 7.
	if (expSetup.getPeakEnergy() > 1200 && expSetup.getPeakEnergy()< 1800) energy = 7;
	else energy=static_cast<unsigned int>(round(expSetup.getPeakEnergy()/1000.));
	std::cout << "----> Energy = " << expSetup.getPeakEnergy()/1000. << std::endl;
	delete conData;

	validEnergy=(energy>=1 && energy<=7);

      }
      else {
	//extract info from beam magnets
	IConditionsHandler* conData = 0;
	if (run < 310000 || (run >= 330000 && run < 340000)) conData= new DBCondHandler( dbinitRead,"/cd_calice_v0402_cerncomb/CALDAQ_BmlSroRunDataCern", "dummy") ;
	else if (run < 320000 || (run >= 340000 && run < 350000)) conData= new DBCondHandler( dbinitRead,"/cd_calice_v0402_cernecal/CALDAQ_BmlSroRunDataCern", "dummy") ;
	else if (run < 330000 || run >= 350000) conData= new DBCondHandler( dbinitRead,"/cd_calice_v0402_cernhcal/CALDAQ_BmlSroRunDataCern", "dummy") ;
	try {
	  conData->update(static_cast<lccd::LCCDTimeStamp>(middle.timeStamp())) ;
	  LCCollection* colE = conData->currentCollection();
	  std::cout << "Number of elements in BmlSroRunDataCern col: " <<  colE->getNumberOfElements() << std::endl;
	  for (unsigned int ival=0; ival < static_cast<unsigned int>(colE->getNumberOfElements()); ival++) {
	    CALICE::BmlSlowRunDataBlock bmlSlowRunDataBlock(colE->getElementAt(ival)); 
	    std::vector<double> bendCurrents(bmlSlowRunDataBlock.getBendCurrents());
	    assert (bendCurrents.size() > 8);
	    //for (unsigned int bc = 0; bc < bendCurrents.size(); bc++){
	    //  std::cout << "           Current " << bc << " = " << bendCurrents[bc] << std::endl;
	    //}
	    energy = static_cast<unsigned int>(round(fabs(bendCurrents[8])/4.72));
	    std::cout << "----> Energy = " << bendCurrents[8]/4.72 << " (rounded to : " << energy << " GeV)" << std::endl;


	    validEnergy=(energy>=6 && energy<=120);

	    LCTime mid(2006,9,30,0,0,0);
	    if (starttime.timeStamp() > mid.timeStamp()) cerna=false;

	  }
	}
	catch ( lcio::Exception & aExc) {
	  std::cout << "lcio::Exception caught, beam parameters not in database." << std::endl;
	}

	delete conData;
      }

      if(validEnergy) {
	TBTrack::FitConstants fe;
	//if(energy<999){
	//if (location == "DESY") setFitConstants(0,energy,fe);
	//else setFitConstants(1,energy,fe);
	//}
	
	LCPayload<TBTrack::FitConstants>
	  *r=new LCPayload<TBTrack::FitConstants>(fe);

	if (location == "DESY") {
	  if(energy==7) {
	    r->update("FitDesy2006001.5GeV.txt");
	    r->payload().print();
	} else {
	  if(energy>0 && energy<7) {
	    std::ostringstream sout;
	    sout << "FitDesy2006_00" << energy << ".0GeV.txt";
	    r->update(sout.str());
	    r->payload().print();
	  }
	}
	  
	} else {
	  std::ostringstream sout;
	  if (location != "CERN3") {
	    if(cerna) sout << "FitCern2006A_" << std::setfill('0') << std::setw(3) << energy << ".0GeV.txt";
	    else      sout << "FitCern2006B_" << std::setfill('0') << std::setw(3) << energy << ".0GeV.txt";
	  } else {
	    sout << "FitCern2007_" << std::setfill('0') << std::setw(3) << energy << ".0GeV.txt";
	  }
	  r->update(sout.str());
	  r->payload().print();
	}
	
	LCCollectionVec* fitCol=new LCCollectionVec( LCIO::LCGENERICOBJECT );
	fitCol->addElement(dynamic_cast<LCGenericObject*>(r)) ;
	
	db3.storeCollection(lccd::LCCDTimeStamp(starttime.timeStamp()),lccd::LCCDTimeStamp(stoptime.timeStamp()),fitCol,description3);
	std::cout << " ------> Collection : " << dbFolder[4] << " has been successfully stored between : " << starttime.getDateString() << " and " << stoptime.getDateString() << std::endl;
      }
	
  }//loop on all runs

*/
  return 0;

}//main method
