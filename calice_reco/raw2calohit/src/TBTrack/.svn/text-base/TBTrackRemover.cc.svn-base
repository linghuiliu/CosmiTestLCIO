#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

// -- LCIO headers
#include <EVENT/LCCollection.h>
#include "EVENT/LCIO.h"
#include "EVENT/LCRunHeader.h"
#include "EVENT/LCParameters.h"
#include "EVENT/CalorimeterHit.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/LCCollectionVec.h"

// -- lccd headers
#include "lccd/LCConditionsMgr.hh"

// -- userlib headers
#include "collection_names.hh"
#include "BmlSlowRunDataBlock.hh"
#include "DBInitString.hh"

#include "marlin/ConditionsProcessor.h"
#include "marlin/Global.h"

#include "RunInformation.hh"
#include "TBTrackRemover.hh"
#include "RunTimeWhizard.hh"

#include "LCPayload.hh"
#include "AlnConstants.hh"
#include "FitConstants.hh"


using namespace lcio ;
using namespace marlin ;
using namespace std;

namespace marlin {
  TBTrackRemover aTBTrackRemover ;
  
  
  TBTrackRemover::TBTrackRemover() : Processor("TBTrackRemover")
					 //,_experimentalSetupChange(this,&TBTrackRemover::experimentalSetupChanged),
                                         //_bmlSroRunDataChanged(this, &TBTrackRemover::setBmlSroRunDataCol),_bmlSroRunDataCol(0)
  {
    // modify processor description
    _description = "Add Runinfo to Runheader needed for MC and DATA" ;
    
    /*
    registerProcessorParameter( "DBInit" ,
				"initialisation of the database " ,
				_dbInit ,
				DBInitString());
    
    registerOptionalParameter( "RunNumber" , 
				"Run number of the run for MC files",
				_runNumber,
				static_cast<int>(230101) ) ;
    
    //List of folders to be searched for the runtime information
    //Fixme: we need to make sure that the folders we select here correspond to the 
    //Run we have at hand
    registerProcessorParameter( "RunTimeFolder",
				"Collection from DB which contain the stat time of a given run" ,
				_folderRunTime,
				std::string("/cd_calice/CALDAQ_RunTimeInfo"));
    
//     registerProcessorParameter( "BMLSRO_RUN_Data" , 
//                               "Name of the conditions collection for the Bml SlowReadoutRun data (CERN)."  ,
//                               _colBmlSroRunData ,
//                               std::string(COL_BML_RUNDATACERN) ) ;
   
    */

  }
  
  void TBTrackRemover::init() {//init method
    
    printParameters() ;

  }//end init method
  
  void TBTrackRemover::processRunHeader( LCRunHeader* run) {
    
    cout << endl << " ---> TBTrackRemover::Processing Run Number : " << run->getRunNumber() << endl;

    //Count the number of runs 
    //_nRun++ ;
  }
  
  void TBTrackRemover::processEvent( LCEvent * evt )
  {//processEvent method

    bool cern2006(true);

    TBTrack::AlnConstants ac;
    try {
      const EVENT::LCCollection *c(0);
      c=evt->getCollection("TBTrackAlnConstants");      
      LCPayload<TBTrack::AlnConstants> p1(dynamic_cast<LCGenericObject*>(c->getElementAt(0)));
      ac=p1.payload();


      // CERN 2006
      if(cern2006) {
	ac.cTzero(0,0,  0.0);
	ac.cTzero(0,1, 14.0);
	ac.cTzero(0,2, 98.0);
	ac.cTzero(0,3,-28.0);
	
	ac.cTzero(1,0,  0.0);
	ac.cTzero(1,1, 52.0);
	ac.cTzero(1,2,104.0);
	ac.cTzero(1,3,118.0);
	
	ac.vDrift(0,0,0.0);
	ac.vDrift(0,1,0.151);
	//ac.vDrift(0,2,ac.vDrift(0,1)+_deltaVDrift);
	ac.vDrift(0,2,0.151);
	ac.vDrift(0,3,0.151);
	
	ac.vDrift(1,0,0.0);
	ac.vDrift(1,1,0.151);
	//ac.vDrift(1,2,ac.vDrift(1,1)+_deltaVDrift);
	ac.vDrift(1,2,0.143);
	ac.vDrift(1,3,0.151);
      }

      // CERN 2007
      else {
	ac.cTzero(0,0,  0.0);
	ac.cTzero(0,1, 26.0);
	ac.cTzero(0,2, 14.0);
	ac.cTzero(0,3, 10.0);
	
	ac.cTzero(1,0,  0.0);
	ac.cTzero(1,1,-25.0);
	ac.cTzero(1,2,-16.0);
	ac.cTzero(1,3,-10.0);
	
	ac.vDrift(0,0,0.0);
	ac.vDrift(0,1,0.156);
	//ac.vDrift(0,2,ac.vDrift(0,1)+_deltaVDrift);
	ac.vDrift(0,2,0.158);
	ac.vDrift(0,3,0.156);

	ac.vDrift(1,0,0.0);
	ac.vDrift(1,1,0.156);
	//ac.vDrift(1,2,ac.vDrift(1,1)+_deltaVDrift);
	ac.vDrift(1,2,0.165);
	ac.vDrift(1,3,0.156);
      }

      LCPayload<TBTrack::AlnConstants> p2(ac);
      //p1.payload(ac);



      p2.output("AlnConstants.txt");

      //std::cout << "Updating from AlnFile.txt" << std::endl;
      //p2.update("AlnFile.txt");
      p2.payload().print();
      
      LCEventModifier::deleteCollection(evt,"TBTrackAlnConstants");
      
      LCCollectionVec* c1=new LCCollectionVec( LCIO::LCGENERICOBJECT );
      c1->addElement(p2.output()) ;
      evt->addCollection(c1,"TBTrackAlnConstants");

    } catch (EVENT::DataNotAvailableException &e) {
    }
      


    /*

    TBTrack::FitConstants fc;
    try {
      const EVENT::LCCollection *c(0);
      c=evt->getCollection("TBTrackFitConstants");
      LCPayload<TBTrack::FitConstants> p1(dynamic_cast<LCGenericObject*>(c->getElementAt(0)));
      fc=p1.payload();

    // CERN 2006      
    if(cern2006) {
      for(unsigned i(0);i<2;i++) {
	double offset(29.0*(i-0.5));
	fc.zLayer(i,0,0.0);
	fc.zLayer(i,1,-  29.0+offset);
	fc.zLayer(i,2,- 680.0+offset);
	fc.zLayer(i,3,-2528.0+offset);
	
	fc.cError(i,0,0.0);
	fc.cError(i,1,0.45);
	fc.cError(i,2,0.45);
	fc.cError(i,3,0.45);
      }
    }

    // CERN 2007
    else {
      for(unsigned i(0);i<2;i++) {
	double offset(29.0*(i-0.5));
	fc.zLayer(i,0,0.0);
	fc.zLayer(i,1,-29.0+offset);
	//fc.zLayer(i,2,-29.0-646.0+offset);
	fc.zLayer(i,2,-699.0-offset);
	fc.zLayer(i,3,-2572.0+offset);
	
	fc.cError(i,0,0.0);
	fc.cError(i,1,0.5);
	fc.cError(i,2,0.5);
	fc.cError(i,3,0.5);
      }
    }	
    
    LCPayload<TBTrack::FitConstants> p2(fc);
    //p2.payload(fc);
    p2.payload().print();
    
    p2.output("FitConstants.txt");

    LCEventModifier::deleteCollection(evt,"TBTrackFitConstants");
    
    LCCollectionVec* c1=new LCCollectionVec( LCIO::LCGENERICOBJECT );
    c1->addElement(p2.output()) ;
    evt->addCollection(c1,"TBTrackFitConstants");
    
    } catch (DataNotAvailableException &e) {
    }
    
    */

    LCEventModifier::deleteCollection(evt,"TBTrackFEX");
    LCEventModifier::deleteCollection(evt,"TBTrackFEY");
    LCEventModifier::deleteCollection(evt,"TBTrackBEX");
    LCEventModifier::deleteCollection(evt,"TBTrackBEY");

    //Do we need the event counter?
    //CAMM: yes, for the cout at the end....
    //_nEvt ++ ;
    
  }//end process Event method
  
  
  void TBTrackRemover::check( LCEvent * evt )
  {
    // nothing to check here - could be used to fill checkplots in reconstruction processor
  }
  


  void TBTrackRemover::end()
  {
    /*
    cout << endl << "TBTrackRemover::end()  " << name()
	 << " processed " << _nEvt << " events in " << _nRun << " runs "
	 << endl ;
    */
  }

}
