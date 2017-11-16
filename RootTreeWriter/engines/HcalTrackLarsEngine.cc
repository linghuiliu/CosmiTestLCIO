#include "HcalTrackLarsEngine.hh"
#include "RootTreeWriter.hh"
#include "CellIndex.hh"

#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/Cluster.h"

using std::cout;
using std::endl;

namespace marlin
{
  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void HcalTrackLarsEngine::registerParameters()
  {
    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_ahcHitsColName",
						   "name of collection containing AHC CalorimeterHits",
						   _ahcHitsColName,
						   std::string("AhcCalorimeter_Hits") );
    
    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_tcmHitsColName",
						   "name of collection containing TCM CalorimeterHits",
						   _tcmHitsColName,
						   std::string("TcmCalorimeter_Hits") );
    
    _hostProcessor.relayRegisterProcessorParameter(_engineName+"_trackColName",
						   "name of collection containing tracks (LCCluster)",
						   _trackColName,
						   std::string("AhcTracksNN") );
    
  }


  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
   void HcalTrackLarsEngine::registerBranches( TTree* hostTree )
  {
    hostTree->Branch("trkLars_NTracks",    &_event.NTracks, 	"NTracks/I");
    hostTree->Branch("trkLars_AhcEnergy",  &_event.AhcEnergy, 	"AhcEnergy/D");
    hostTree->Branch("trkLars_TcmtEnergy", &_event.TcmtEnergy,	"TcmtEnergy/D");
    
    hostTree->Branch("trkLars_NHits",         "vector<int>", 	&_event.t_NHits);
    hostTree->Branch("trkLars_NLength",       "vector<int>", 	&_event.t_NLength);
    hostTree->Branch("trkLars_StartLayer",    "vector<int>", 	&_event.t_StartLayer);
    hostTree->Branch("trkLars_StopLayer",     "vector<int>",    &_event.t_StopLayer);
    hostTree->Branch("trkLars_CosPhi",        "vector<double>", &_event.t_CosPhi);
    hostTree->Branch("trkLars_trackHitIndex", "vector<int>", 	&_event.t_trackHitIndex);
    hostTree->Branch("trkLars_h_Energy",      "vector<double>", &_event.t_h_Energy);
    hostTree->Branch("trkLars_h_I", 	      "vector<int>", 	&_event.t_h_I);
    hostTree->Branch("trkLars_h_J", 	      "vector<int>", 	&_event.t_h_J);
    hostTree->Branch("trkLars_h_K", 	      "vector<int>", 	&_event.t_h_K);
  }

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  void HcalTrackLarsEngine::fillVariables( EVENT::LCEvent* evt ) 
  {
    /*Initialize variables (otherwise variables may keep values from previous events)*/
    _event.NTracks = 0;
    _event.AhcEnergy = 0;
    _event.TcmtEnergy = 0;
    _event.t_NHits.clear();
    _event.t_NLength.clear();
    _event.t_StartLayer.clear();
    _event.t_StopLayer.clear();
    _event.t_CosPhi.clear();
    _event.t_trackHitIndex.clear();
    _event.t_h_Energy.clear();
    _event.t_h_I.clear();
    _event.t_h_J.clear();
    _event.t_h_K.clear();
    

    /*loop over collections*/
    _event.AhcEnergy = getSumOfCaloHits(evt, _ahcHitsColName);
    _event.TcmtEnergy = getSumOfCaloHits(evt, _tcmHitsColName);

    try
      {
	LCCollection* lccTracks = evt->getCollection(_trackColName);
	
	_event.NTracks = lccTracks->getNumberOfElements();
	int trackIndex = 0;
	
	_event.t_NHits.resize(_event.NTracks);
	_event.t_NLength.resize(_event.NTracks);
	_event.t_CosPhi.resize(_event.NTracks);
	_event.t_StartLayer.resize(_event.NTracks);
	_event.t_StopLayer.resize(_event.NTracks);
	
	_event.t_trackHitIndex.resize(_event.NTracks);
	
	int nTracks = lccTracks->getNumberOfElements();
	for (int iTrack = 0; iTrack < nTracks; ++iTrack)
	  {
	    Cluster* lcTrack = dynamic_cast<lcio::Cluster*>(lccTracks->getElementAt(iTrack));
	    CalorimeterHitVec hits = lcTrack->getCalorimeterHits();
	    
	    int NHits = hits.size();
	    
	    _event.t_NHits.at(iTrack) = NHits;
	    _event.t_CosPhi.at(iTrack) = lcTrack->getIPhi();	    
	    _event.t_StartLayer.at(iTrack) = CALICE::CellIndex(hits.front()->getCellID0()).getLayerIndex();
	    _event.t_StopLayer.at(iTrack) = CALICE::CellIndex(hits.back()->getCellID0()).getLayerIndex();
	    _event.t_NLength.at(iTrack) = 1 + _event.t_StopLayer.at(iTrack) - _event.t_StartLayer.at(iTrack);
	    
	    _event.t_trackHitIndex.at(iTrack) = trackIndex;
	    
	    for (int iHit = 0; iHit < NHits; ++iHit)
	      {
		CalorimeterHit* lcHit = hits.at(iHit);
		trackIndex++;
		
		_event.t_h_Energy.push_back(lcHit->getEnergy());

		CALICE::CellIndex idx(lcHit->getCellID0());
		_event.t_h_I.push_back(idx.getPadColumn());
		_event.t_h_J.push_back(idx.getPadRow());
		_event.t_h_K.push_back(idx.getLayerIndex());
	      }
	  }
	
      }
    catch (DataNotAvailableException& e)
      {
	streamlog_out(DEBUG) << "No Cluster with name " << _trackColName << " found" << endl;
      }
    

  }/*fillVariables*/

  /*********************************************************************************/
  /*                                                                               */
  /*                                                                               */
  /*                                                                               */
  /*********************************************************************************/
  Double_t HcalTrackLarsEngine::getSumOfCaloHits(EVENT::LCEvent* evt, std::string collectionName)
  {
    try
      {
        LCCollection* lcc = evt->getCollection(collectionName);
	
        Double_t energySum = 0;
	
        for (int n = 0; n < lcc->getNumberOfElements(); n++)
	  { 
            EVENT::CalorimeterHit* hit = dynamic_cast<EVENT::CalorimeterHit*>(lcc->getElementAt(n));
            if (hit)
	      {
                energySum += hit->getEnergy();
	      }
        }
        
        return energySum;
    }
    catch (DataNotAvailableException& ignored)
    {
      streamlog_out(DEBUG) << "No CalorimeterHit collection with name " << collectionName << " found" << endl;
      return 0.0;
    }
  }

}/*namespace marlin*/


/*********************************************************************************/
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
