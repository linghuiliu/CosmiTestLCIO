#include "TrackFinderWriteEngine.hh"

#include "EVENT/LCCollection.h"
#include "IMPL/LCGenericObjectImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "EVENT/CalorimeterHit.h"

#include <cfloat>
#include <climits>
#include <stdexcept>

#define INVALIDF (-FLT_MAX)
#define INVALIDI INT_MIN

namespace marlin
{
  void TrackFinderWriteEngine::registerParameters()
  {
    _hostProcessor.relayRegisterProcessorParameter(
						   _engineName+"_prefix",
						   "TrackFinderWriteEngine prefix to tree variables",
						   _enginePrefix,
						   std::string("ptf_") );

    _hostProcessor.relayRegisterProcessorParameter(
						   _engineName+"_showerStartColName",
						   "name of shower start input collection",
						   _showerStartColName,
						   std::string("ShowerStartingLayer") );

   _hostProcessor.relayRegisterProcessorParameter(
						   _engineName+"_trackColName",
						   "name of track input collection",
						   _trackColName,
						   std::string("PrimaryTrackHits") );

    _hostProcessor.relayRegisterProcessorParameter(
						   _engineName+"_numHcalLayers",
						   "number of HCAL layers",
						   _fNumHcalLayers,
						   int(38) );
  }


  void TrackFinderWriteEngine::registerBranches( TTree* hostTree )
  {
    hostTree->Branch( (_enginePrefix+"showerStart").c_str(),
		      & _fShowerStart,
		      (_enginePrefix+"showerStart/I").c_str() );

    hostTree->Branch( (_enginePrefix+"CalorimeterType").c_str(),
		      & _fCalorimeterType,
		      (_enginePrefix+"CalorimeterType/I").c_str() );

    hostTree->Branch( (_enginePrefix+"eSumTrack").c_str(),
		      & _eSumTrack,
		      (_enginePrefix+"eSumTrack/F").c_str() );

    hostTree->Branch( (_enginePrefix+"numTrackHits").c_str(),
		      & _numTrackHits,
		      (_enginePrefix+"numTrackHits/I").c_str() );
    hostTree->Branch( (_enginePrefix+"hitEnergy").c_str(), &_trackHitEnergy, 
		      (_enginePrefix +"hitEnergy[" + _enginePrefix +"numTrackHits]/F").c_str() );
     hostTree->Branch( (_enginePrefix+"hitType").c_str(), &_trackHitType, 
		      (_enginePrefix +"hitType[" + _enginePrefix +"numTrackHits]/I").c_str() );
    


  }


  void TrackFinderWriteEngine::fillVariables( LCEvent* evt )
  {
    LCCollection *inputCol;
    _fShowerStart = -10000;
    _fCalorimeterType = -10000;

    try
      {
	inputCol = evt->getCollection(_showerStartColName);
	int nEntries = inputCol->getNumberOfElements();
   
        for (int i = 0; i < nEntries; ++i)
	  {
	    LCGenericObject *showerStartLayerObj = dynamic_cast<LCGenericObject*>(inputCol->getElementAt(i));

	    /*calorimeter type: 0=ECAL, 1=first 30 layers in HCAL, 2=last coarse part of HCAL*/
	    _fCalorimeterType = showerStartLayerObj->getIntVal(0);
	    _fShowerStart     = showerStartLayerObj->getIntVal(1);

	    // Keep the orginal information from PTF As the user requested.
	    // Please consult the "PrimaryTrackFinder" for the meaning of number.
	  }
	
      }
    catch ( DataNotAvailableException err )
      {
	streamlog_out(DEBUG)<<  "WARNING: Collection " << _showerStartColName << " not available in event " << evt->getEventNumber() << std::endl;
      }

    /*----------------------------------------------------------------------------*/
    
    _eSumTrack = 0;

    streamlog_out(DEBUG)<<"\n\n track col name: "<<_trackColName<<std::endl;

    try
      {
	inputCol = evt->getCollection(_trackColName);
	int nEntries = inputCol->getNumberOfElements();
	_numTrackHits = nEntries;

	streamlog_out(DEBUG)<<"\n\n track entries: "<<nEntries<<std::endl;

	if ( (unsigned) nEntries >  MAXLAYERS ) {
	  streamlog_out(ERROR) << " Please check the error: Number of track entries is larger than MAXLAYERS" <<std::endl;
	  streamlog_out(ERROR) << " Number of track entries :"<< _numTrackHits << " and MAXLAYERS : "<< MAXLAYERS <<std::endl;
	  throw std::range_error(" numTrackHit > MAXLAYERS, index out of bounds!");
	}

  
        for (int i = 0; i < nEntries; ++i)
	  {
	    CalorimeterHit *hit = dynamic_cast<CalorimeterHit*>(inputCol->getElementAt(i));
	    

	    /*calorimeter type: 0=ECAL, 1=first 30 layers in HCAL, 2=last coarse part of HCAL*/
	    int type = hit->getType();
	    _trackHitType[i] = type;
	    
	    float energy = hit->getEnergy();
	    _trackHitEnergy[i] = energy;

	    _eSumTrack += energy;

	  }/*end loop over hits*/
	
      }
    catch ( DataNotAvailableException err )
      {
	streamlog_out(DEBUG)<<  "WARNING: Collection " << _trackColName << " not available in event " << evt->getEventNumber() << std::endl;
      }

    streamlog_out(DEBUG)<<"eSumTrack: "<<_eSumTrack<<std::endl;

  }

} //namespace marlin
