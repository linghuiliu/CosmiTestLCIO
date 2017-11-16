/*
 * HcalTrack2RootTree.cpp
 *
 *  Created on: 31.08.2011
 *      Author: weuste
 */

#include "HcalTrack2RootTree.hh"

#include <boost/foreach.hpp>

#include <EVENT/Cluster.h>

#include <CellIndex.hh>


using namespace marlin;
using namespace std;

HcalTrack2RootTree aHcalTrack2RootTree;

HcalTrack2RootTree::HcalTrack2RootTree() : Processor("HcalTrack2RootTree")
{
	_description = "Writes the information of the Tracks into an RootTree";

	registerInputCollection(
			LCIO::CLUSTER,
			"TrackClusterCollection",
			"Name of the LCCollection containing the Clusters of Tracks",
			_lccCluster,
			std::string("AhcTracksNN"));

	registerProcessorParameter(
			"rootOutputFile",
			"Name of the Root Outputfile containing the Tree with the track information",
			_rootFileName,
			std::string("tracks.root")
			);

	registerProcessorParameter(
			"rootTreeName",
			"Name of the Root TTree in the TFile",
			_rootTreeName,
			std::string("tTrack")
			);

	registerInputCollection(
			LCIO::CALORIMETERHIT,
            "EmcCalorimeterHits",
			"EmCal Calorimter Hit Collection",
			_lccEmcCaloHits,
			std::string("EmcCalorimeter_Hits"));

	registerInputCollection(
			LCIO::CALORIMETERHIT,
            "AhcCalorimeterHits",
			"AHCal Calorimter Hit Collection",
			_lccAhcCaloHits,
			std::string("AhcCalorimeter_Hits"));

	registerInputCollection(
			LCIO::CALORIMETERHIT,
            "TcmtCalorimeterHits",
			"TcMt Calorimter Hit Collection",
			_lccTcmtCaloHits,
			std::string("TcmtCalorimeter_Hits"));
}

HcalTrack2RootTree::~HcalTrack2RootTree()
{
	// TODO Auto-generated destructor stub
}


void HcalTrack2RootTree::init()
{
	printParameters();

	_event.idx = 0;

	_rootFile = new TFile(_rootFileName.c_str(), "RECREATE");
	_tree = new TTree(_rootTreeName.c_str(), "List of Tracks");


	_tree->Branch("idx", 				&_event.idx, 		"idx/I");
	_tree->Branch("RunNo", 				&_event.RunNo,	 	"RunNo/I");
	_tree->Branch("EvtNo", 				&_event.EvtNo,	 	"EvtNo/I");
	_tree->Branch("NTracks", 			&_event.NTracks, 	"NTracks/I");

	_tree->Branch("EmcEnergy", 			&_event.EmcEnergy, 	"EmcEnergy/D");
	_tree->Branch("AhcEnergy", 			&_event.AhcEnergy, 	"AhcEnergy/D");
	_tree->Branch("TcmtEnergy", 		&_event.TcmtEnergy,	"TcmtEnergy/D");

	_tree->Branch("t_NHits", 			"vector<int>", 		&_event.t_NHits);
	_tree->Branch("t_NLength", 			"vector<int>", 		&_event.t_NLength);
	_tree->Branch("t_StartLayer", 		"vector<int>", 		&_event.t_StartLayer);
	_tree->Branch("t_StopLayer", 		"vector<int>", 		&_event.t_StopLayer);
	_tree->Branch("t_CosPhi", 			"vector<double>", 	&_event.t_CosPhi);


	_tree->Branch("t_trackHitIndex", 	"vector<int>", 		&_event.t_trackHitIndex);

	_tree->Branch("t_h_Energy", 		"vector<double>", 	&_event.t_h_Energy);
	_tree->Branch("t_h_I", 				"vector<int>", 		&_event.t_h_I);
	_tree->Branch("t_h_J", 				"vector<int>", 		&_event.t_h_J);
	_tree->Branch("t_h_K", 				"vector<int>", 		&_event.t_h_K);


}



void HcalTrack2RootTree::processRunHeader(LCRunHeader* run)
{
	_event.RunNo = run->getRunNumber();
}



void HcalTrack2RootTree::processEvent(LCEvent * evt)
{
	_event.idx++;
	_event.EvtNo = evt->getEventNumber();
    _event.EmcEnergy = getSumOfCaloHits(evt, _lccEmcCaloHits);
    _event.AhcEnergy = getSumOfCaloHits(evt, _lccAhcCaloHits);
    _event.TcmtEnergy = getSumOfCaloHits(evt, _lccTcmtCaloHits);

	try
	{
		LCCollection* lccTracks = evt->getCollection(_lccCluster);

		_event.NTracks = lccTracks->getNumberOfElements();
		int trackIndex = 0;

		_event.t_NHits.resize(_event.NTracks);
		_event.t_NLength.resize(_event.NTracks);
		_event.t_CosPhi.resize(_event.NTracks);
		_event.t_StartLayer.resize(_event.NTracks);
		_event.t_StopLayer.resize(_event.NTracks);

		_event.t_trackHitIndex.resize(_event.NTracks);

		_event.t_h_Energy.clear();
		_event.t_h_I.clear();
		_event.t_h_J.clear();
		_event.t_h_K.clear();

		for (int iTrack = 0; iTrack < lccTracks->getNumberOfElements(); ++iTrack)
		{
			Cluster* lcTrack = dynamic_cast<Cluster*>(lccTracks->getElementAt(iTrack));
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

//		_evt->SetEventNo(_evtNr);
//		_evt->SetRunNo(_runNr);
//
//		for (int iTrack = 0; iTrack < lccTracks->getNumberOfElements(); ++iTrack)
//		{
//			Cluster* lcTrack = dynamic_cast<Cluster*>(lccTracks->getElementAt(iTrack));
//			HCalTrack hcalTrack;
//
//			BOOST_FOREACH(CalorimeterHit* lcHit, lcTrack->getCalorimeterHits())
//			{
//				HCalHit hcalHit;
//
//				hcalHit.SetAmplitudeMIP(lcHit->getEnergy());
//
//				CALICE::CellIndex idx(lcHit->getCellID0());
//				hcalHit.SetIndices(idx.getPadColumn(), idx.getPadRow(), idx.getLayerIndex());
//
//				hcalTrack.AddHit(hcalHit);
//			}
//
//			_evt->AddTrack(hcalTrack);
//		}

		_tree->Fill();
	}
	catch (DataNotAvailableException& e)
	{
		streamlog_out(WARNING) << "No Cluster with name" << _lccCluster << " found" << endl;
	}


}

Double_t HcalTrack2RootTree::getSumOfCaloHits(EVENT::LCEvent* evt, std::string& collectionName)
{
    try
    {
        LCCollection* lcc = evt->getCollection(collectionName);

        Double_t energySum = 0;

        for (int n=0; n<lcc->getNumberOfElements(); n++)
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
		streamlog_out(WARNING0) << "No CalorimterHit collection with name" << collectionName << " found" << endl;
        return 0.0;
    }
}

void HcalTrack2RootTree::check(LCEvent * evt)
{

}



void HcalTrack2RootTree::end()
{
//	_tree->Write();
	_rootFile->WriteTObject(_tree);
	_rootFile->Close();
}

