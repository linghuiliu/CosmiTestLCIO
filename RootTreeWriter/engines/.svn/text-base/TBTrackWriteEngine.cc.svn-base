#include "TBTrackWriteEngine.hh"

#include <cfloat>
#include <cmath>

// #include "EVENT/CalorimeterHit.h"
// #include "UTIL/LCTypedVector.h"
// #include <IMPL/TrackerHitImpl.h>
// #include <IMPL/TrackImpl.h>

#include <TriggerBits.hh>
#include <collection_names.hh>
#include <EVENT/LCCollection.h>
#include <EVENT/LCGenericObject.h>
#include "TBTrackUtil/TrackProjection.hh"

using namespace lcio;
using namespace std;
using namespace CALICE;
using namespace TBTrack;


#define DDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name<<": " << name << std::endl;
#define IDEBUG(name) std::cout << __FILE__ <<","<<__LINE__ << "; " << #name <<" at " << &name << std::endl;

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN


namespace marlin
{
  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void TBTrackWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_colNameX",
						   "name of collection containing x track coordinates",
						   _colXName,
						   string("TBTrackFEX") );

    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_colNameY",
						   "name of collection containing y track coordinates",
						   _colYName,
						   string("TBTrackFEY") );

   _hostProcessor.relayRegisterProcessorParameter(_engineName+"_HcalZ",
						  "Z coordinate of Hcal front face",
						  _z_of_Hcal, float(1716));

   _hostProcessor.relayRegisterProcessorParameter(_engineName+"_rotationAngle",
						  "rotation angle along z-axis",
						  _rotationAngle, float(0.));

  }

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void TBTrackWriteEngine::registerBranches( TTree* hostTree )
  {
    hostTree->Branch("TBTrack_XSlope",       &_hitsFill.XSlope,       "XSlope/F");
    hostTree->Branch("TBTrack_XSlopeError",  &_hitsFill.XSlopeError,  "XSlopeError/F");
    hostTree->Branch("TBTrack_XOffset",      &_hitsFill.XOffset,      "XOffset/F");
    hostTree->Branch("TBTrack_XOffsetError", &_hitsFill.XOffsetError, "XOffsetError/F");
    hostTree->Branch("TBTrack_XChi2",        &_hitsFill.XChi2      ,  "XChi2/F");
    hostTree->Branch("TBTrack_XHcalImpact",  &_hitsFill.XHcalImpact,  "XHcalImpact/F");
    hostTree->Branch("TBTrack_XNoDof",       &_hitsFill.XNoDof,       "XNoDof/I");
    hostTree->Branch("TBTrack_XNoHits",      &_hitsFill.XNoHits,      "XNoHits/I");
    hostTree->Branch("TBTrack_XNumElements", &_hitsFill.XNumElements, "XNumElements/I");
    hostTree->Branch("TBTrack_XProbability", &_hitsFill.XProbability, "XProbability/F");

    hostTree->Branch("TBTrack_YSlope",       &_hitsFill.YSlope,       "YSlope/F");
    hostTree->Branch("TBTrack_YSlopeError",  &_hitsFill.YSlopeError,  "YSlopeError/F");
    hostTree->Branch("TBTrack_YOffset",      &_hitsFill.YOffset,      "YOffset/F");
    hostTree->Branch("TBTrack_YOffsetError", &_hitsFill.YOffsetError, "YOffsetError/F");
    hostTree->Branch("TBTrack_YChi2",        &_hitsFill.YChi2      ,  "YChi2/F");
    hostTree->Branch("TBTrack_YHcalImpact",  &_hitsFill.YHcalImpact,  "YHcalImpact/F");
    hostTree->Branch("TBTrack_YNoDof",       &_hitsFill.YNoDof,       "YNoDof/I");
    hostTree->Branch("TBTrack_YNoHits",      &_hitsFill.YNoHits,      "YNoHits/I");
    hostTree->Branch("TBTrack_YNumElements", &_hitsFill.YNumElements, "YNumElements/I");
    hostTree->Branch("TBTrack_YProbability", &_hitsFill.YProbability, "YProbability/F");

  }

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void TBTrackWriteEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    _hitsFill.XSlope       = -10000;
    _hitsFill.XSlopeError  = -10000;
    _hitsFill.XOffset      = -10000;
    _hitsFill.XOffsetError = -10000;
    _hitsFill.XChi2        = -10000;
    _hitsFill.XHcalImpact  = -10000;
    _hitsFill.XNoDof       = -10000;
    _hitsFill.XNoHits      = 10000;
    _hitsFill.XProbability = -10000;

    _hitsFill.YSlope       = -10000;
    _hitsFill.YSlopeError  = -10000;
    _hitsFill.YOffset      = -10000;
    _hitsFill.YOffsetError = -10000;
    _hitsFill.YChi2        = -10000;
    _hitsFill.YHcalImpact  = -10000;
    _hitsFill.YNoDof       = -10000;
    _hitsFill.YNoHits      = 10000;
    _hitsFill.YProbability = -10000;


    const TriggerBits trigBits(evt->getParameters().getIntVal(PAR_TRIGGER_EVENT));

//     if (trigBits.isBeamTrigger())
//       {
        try
	  {
	    /*------------------ x track -----------------------------*/
	    /* F is sensible, E is your choice, X is the use case*/
	    LCCollection* colX = evt->getCollection( "TBTrackFEX" );
	    int noElemX = colX->getNumberOfElements();

	    /*if there are several tracks, choose the one with the best probability*/
	    TBTrack::TrackProjection *trkX = NULL;

	    if ( noElemX > 0 )
	      {
		for (int iX = 0; iX < noElemX; ++iX)
		  {
		    TBTrack::TrackProjection *currentTrkX = 
		      new TBTrack::TrackProjection(dynamic_cast<LCGenericObject*>(colX->getElementAt(iX)) );

		    if (trkX == NULL)
		      {
			trkX = currentTrkX;
		      }
		    else
		      {
			if (currentTrkX->probability() > trkX->probability())
			  {
			    delete trkX;
			    trkX = currentTrkX;
			  }
			else
			  {
			    delete currentTrkX;
			  }
		      }
		  }/*end of loop over iX*/


		_hitsFill.XSlope       = trkX->gradient(0.);
		_hitsFill.XSlopeError  = trkX->gradientError(0.);
		_hitsFill.XOffset      = trkX->intercept(0., _rotationAngle);
		_hitsFill.XOffsetError = trkX->interceptError(0., _rotationAngle);
		_hitsFill.XChi2        = trkX->chiSquared();
		_hitsFill.XHcalImpact  = trkX->intercept( _z_of_Hcal, _rotationAngle );
		_hitsFill.XNoDof       = trkX->numberOfDof();
		_hitsFill.XNoHits      = trkX->numberOfHits();
		_hitsFill.XNumElements = noElemX;
		_hitsFill.XProbability = trkX->probability();

	      }

	    
	    /*------------------ y track -----------------------------*/
	    /* F is sensible, E is your choice, Y is the use case*/
	    LCCollection* colY = evt->getCollection( "TBTrackFEY" );
	    int noElemY = colY->getNumberOfElements();

	    /*if there are several track, choose the one with the best probability*/
	    TBTrack::TrackProjection *trkY = NULL;

	    if ( noElemY > 0 )
	      {
		for (int iY = 0; iY < noElemY; ++iY)
		  {
		    TBTrack::TrackProjection *currentTrkY = 
		      new TBTrack::TrackProjection(dynamic_cast<LCGenericObject*>(colY->getElementAt(iY)) );

		    if (trkY == NULL)
		      {
			trkY = currentTrkY;
		      }
		    else
		      {
			if (currentTrkY->probability() > trkY->probability())
			  {
			    delete trkY;
			    trkY = currentTrkY;
			  }
			else
			  {
			    delete currentTrkY;
			  }
		      }
		  }/*end of loop over iY*/


		_hitsFill.YSlope       = trkY->gradient(0.);
		_hitsFill.YSlopeError  = trkY->gradientError(0.);
		_hitsFill.YOffset      = trkY->intercept(0., _rotationAngle);
		_hitsFill.YOffsetError = trkY->interceptError(0., _rotationAngle);
		_hitsFill.YChi2        = trkY->chiSquared();
		_hitsFill.YHcalImpact  = trkY->intercept( _z_of_Hcal, _rotationAngle );
		_hitsFill.YNoDof       = trkY->numberOfDof();
		_hitsFill.YNoHits      = trkY->numberOfHits();
		_hitsFill.YNumElements = noElemY;
		_hitsFill.YProbability = trkY->probability();
	      }
	    
	    /*----------------------------------------------------------*/
	  }
	
	catch ( DataNotAvailableException err )
	  {
// 	    cout <<  "WARNING: Collection "<< _colXName<<" or "<<_colYName
// 		 << " not available in event "<< evt->getEventNumber() << endl;
	  }
      }
    
//   }

}/*namespace marlin*/
