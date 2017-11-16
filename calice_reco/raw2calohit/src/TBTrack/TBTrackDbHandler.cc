#include <cassert>
#include <string>
#include <iostream>

#include "lcio.h"
#include "IO/LCWriter.h"
#include "EVENT/LCGenericObject.h"
#include "EVENT/LCIntVec.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "UTIL/LCTOOLS.h"
#include "UTIL/LCTime.h"
#include "LCIOSTLTypes.h"

#include "lccd/LCConditionsMgr.hh"

#include "LCPayload.hh"
#include "TBTrackDbHandler.hh"
#include "MapConstants.hh"
#include "SimConstants.hh"
#include "AlnConstants.hh"
#include "FitConstants.hh"

#include "BeamMetaData.hh"


using namespace lcio ;
using namespace marlin ;

using std::cout;
using std::endl;

TBTrackDbHandler aTBTrackDbHandler ;

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
TBTrackDbHandler::TBTrackDbHandler() : TBTrackBaseProcessor("TBTrackDbHandler") 
{
  _description = "TBTrackDbHandler" ;
}


/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackDbHandler::ProcessRunHeader(LCRunHeader *run) 
{
  // Check there are runs constants
  if(!_runInformationValid) 
    {
      if(printLevel(-3)) 
	{
	  cout << "No valid RunInformation" << endl;
	  assert(false);
	}
    }
  
  // Print the run information
  _runInformation.print();
}

/****************************************************************************************************/
/*                                                                                                  */
/*                                                                                                  */
/*                                                                                                  */
/****************************************************************************************************/
void TBTrackDbHandler::ProcessEvent(LCEvent *evt) 
{
  if(_nEvt == 0) //first event
    {
      streamlog_out(DEBUG) << " Accessing collections in first event" << endl;

      _runInformation.print();
      std::string location(_runInformation.location());
      float nominalBeamEnergy(0);
      
      streamlog_out(DEBUG) << "location = " << location << endl;
      
      if (!_runInformation.isMC())
	{
	  //mapConstants: not yet in the database (will they ever be ???)
	  //but independant of run or E for now on.
	  
	  TBTrack::MapConstants mc(0);
	  
	  cout << "printing MapConstants" << endl;
	  mc.print();
	  
	  LCCollectionVec* c0 = new LCCollectionVec( LCIO::LCGENERICOBJECT );
	  LCPayload<TBTrack::MapConstants> *p=new LCPayload<TBTrack::MapConstants>(mc);
	  
	  c0->addElement(p->output()) ;
	  evt->addCollection(c0,"TBTrackMapConstants");
	  
	  delete p;
	  if(printLevel(1)) mc.print();
	  
	  LCCollection *col1 = (lccd::LCConditionsMgr::instance()->getHandler(location+"TBTrackAlnConstants"))->currentCollection();
	  
	  assert (col1->getNumberOfElements()==1);
	  LCCollectionVec* c1=new LCCollectionVec( LCIO::LCGENERICOBJECT );
	  
	  LCPayload<TBTrack::AlnConstants> p1(dynamic_cast<LCGenericObject*>(col1->getElementAt(0)));
	  c1->addElement(p1.output()) ;
	  
	  evt->addCollection(c1,"TBTrackAlnConstants");
	  
	  if(printLevel(1)) (p1.constants()).print();
	  
	  // ----------------------------------
	  // get the nominal beam energy from the run meta data
	  // get the run metadata for real data - d jeans
	  if (_beamMomentum>0)  // first priority: momentum from the steering
	    {
	      nominalBeamEnergy = _beamMomentum;
	    } 
	  else 
	    {
	      if (! lccd::LCConditionsMgr::instance()->getHandler(location+"RunMetaData") ) 
		{
		  cout << "cannot find " << location+"RunMetaData" << " database collection" << endl;
		  cout << " either add to steering file, or enter beam momentum explicitly via beamMomentum parameter" << endl;
		  // make it exit gracefully...
		  assert (lccd::LCConditionsMgr::instance()->getHandler(location+"RunMetaData"));
		} 
	      else 
		{
		  LCCollection *col99 = (lccd::LCConditionsMgr::instance()->getHandler(location+"RunMetaData"))->currentCollection();
		  assert (col99->getNumberOfElements()==1);
		  CALICE::BeamMetaData metadata( (LCGenericObject*) col99->getElementAt(0));
		  cout << "run meta data (pdg, nominal momentum) = " << metadata.getPdgCode() << " " << metadata.getEnergy() << endl;
		  nominalBeamEnergy = metadata.getEnergy();
		} 
	    }
	  
	  streamlog_out(DEBUG)<<"Data collection added in first event" << endl;
	  
	  // ----------------------------------
	  
	} 
      else 
	{ //for MC only
	  streamlog_out(DEBUG)<<"for MC only: " << location << endl;
	  
	  //Fixme should be possible to switch between default and user defined folder
	  LCCollection *col3 = (lccd::LCConditionsMgr::instance()->getHandler(location+"TBTrackSimConstants"))->currentCollection();
	  assert (col3->getNumberOfElements() == 1);

	  LCCollectionVec* c3 = new LCCollectionVec( LCIO::LCGENERICOBJECT );
	  LCPayload<TBTrack::SimConstants> p3(dynamic_cast<LCGenericObject*>(col3->getElementAt(0)));
	  c3->addElement(p3.output()) ;

	  //Fixme: See above ...
	  evt->addCollection(c3,"TBTrackSimConstants");
	  if(printLevel(1)) (p3.constants()).print();
	  
	  // need to decide how to get beam energy for MC
	  // nominalBeamEnergy = ????
	  // get it from the steering file
	  nominalBeamEnergy = _beamMomentum;
	  
	  /* Don't do it this way
	     LCCollection *col4 = (lccd::LCConditionsMgr::instance()->getHandler("mcAlnConstants"))->currentCollection();
	     assert (col4->getNumberOfElements()==1);
	     LCPayload<TBTrack::AlnConstants> p4(dynamic_cast<LCGenericObject*>(col4->getElementAt(0)));
	  */
	  /* Now get them from the SimConstants to ensure consistency
	   */
	  LCPayload<TBTrack::AlnConstants> p4(p3.payload());
	  LCCollectionVec* c4 = new LCCollectionVec( LCIO::LCGENERICOBJECT );
	  c4->addElement(p4.output()) ;
	  evt->addCollection(c4,"TBTrackAlnConstants");
	  if(printLevel(1)) (p4.constants()).print();
	  
	}//for MC only
      
      
      LCCollection *col7 = (lccd::LCConditionsMgr::instance()->getHandler(location+"TBTrackFitConstants"))->currentCollection();
      assert (col7->getNumberOfElements()==1);
      LCCollectionVec* c7 = new LCCollectionVec( LCIO::LCGENERICOBJECT );
      
      LCPayload<TBTrack::FitConstants> p7(dynamic_cast<LCGenericObject*>(col7->getElementAt(0)));
      
      // update the nominal beam energy
      TBTrack::FitConstants bebe = p7.constants();
      
      
      if(printLevel(1)) 
	{
	  cout << "fit constants before momentum scale: " << endl;
	  bebe.print();
	}
      
      cout << "beam momentum for tracking scatter matrix scaling: " << nominalBeamEnergy << endl;
      
      assert ( nominalBeamEnergy>0 );
      
      bebe.pBeamScale(nominalBeamEnergy);
      
      LCPayload<TBTrack::FitConstants> p7a( bebe );
      
      c7->addElement(p7a.output()) ;
      evt->addCollection(c7,"TBTrackFitConstants");

      if(printLevel(1)) 
	{
	  cout << "fit constants after momentum scale" << endl;
	  (p7a.constants()).print();
	}
      
      streamlog_out(DEBUG) << "end of first event DB stuff, added collections to event " << endl;
      
    }//first event 
  
}//processEvt

