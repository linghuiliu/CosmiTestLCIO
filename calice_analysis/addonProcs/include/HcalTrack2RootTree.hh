/*
 * HcalTrack2RootTree.h
 *
 *  Created on: 31.08.2011
 *      Author: weuste
 */

#ifndef HCALTRACK2ROOTTREE_H_
#define HCALTRACK2ROOTTREE_H_

#include "marlin/Processor.h"
#include "IMPL/LCCollectionVec.h"

#include <TFile.h>
#include <TTree.h>
//#include "HCalEvent.hh"

#include <TClonesArray.h>
#include <TObjArray.h>
//#include <TGenericTable.h>
#include <vector>

#include <string>

class HcalTrack2RootTree : marlin::Processor
{
public:
	HcalTrack2RootTree();
	virtual ~HcalTrack2RootTree();

	marlin::Processor* newProcessor() { return new HcalTrack2RootTree(); }

	// the standard Marlin functions

	/** Called at the begin of the job before anything is read.
	 * Use to initialize the processor, e.g. book histograms.
	 */
	virtual void init();

	/** Called for every run.
	 */
	virtual void processRunHeader(LCRunHeader* run);

	/** Called for every event - the working horse.
	 */
	virtual void processEvent(LCEvent * evt);

	virtual void check(LCEvent * evt);

	/** Called after data processing for clean up.
	 */
	virtual void end();

protected:

    virtual Double_t getSumOfCaloHits(EVENT::LCEvent* evt, std::string& collectionName);

	std::string	_lccCluster;
	std::string	_lccEmcCaloHits;
	std::string	_lccAhcCaloHits;
	std::string	_lccTcmtCaloHits;
	std::string _rootFileName;
	std::string _rootTreeName;


	TFile*	_rootFile;
	TTree*	_tree;


	// stats
	struct EventTracks
	{
		Int_t	idx;

		Int_t	RunNo;
		Int_t	EvtNo;

		Int_t	NTracks;

        Double_t    EmcEnergy;
        Double_t    AhcEnergy;
        Double_t    TcmtEnergy;

		std::vector< Int_t >	t_NHits;
		std::vector< Int_t >	t_NLength;
		std::vector< Int_t >	t_StartLayer;
		std::vector< Int_t >	t_StopLayer;
		std::vector< Double_t >	t_CosPhi;

		// The following vectors contain actually hit information for each track
		// I had to use a flat structure, as ROOT does not support a 2 dim (i.e. vector < vector < double > >)
		// without having to create a dictionary, which i want do avoid.
		// Hence: The t_h_xxx variables contain the hit information for all tracks stripped together,
		// and the t_trackHitIndex saves the position of the first hit of each track within these t_h_xxx vars
		std::vector< Int_t >	t_trackHitIndex;
		std::vector< Double_t >	t_h_Energy;
		std::vector< Int_t >	t_h_I;
		std::vector< Int_t >	t_h_J;
		std::vector< Int_t >	t_h_K;

	} _event;

//	struct TrackHit
//	{
//		Double_t	Energy;
//	};

//	HCalEvent*	_evt;

//	// stats
//	struct EventTracks
//	{
//		int		Idx;
//
//		int		RunNr;
//		int		EvtNr;
//		int		NTracks;
//
//		std::vector< int >	t_lengthLayer;
//		std::vector< int >	t_NHits;
//
//	};
};

#endif /* HCALTRACK2ROOTTREE_H_ */
