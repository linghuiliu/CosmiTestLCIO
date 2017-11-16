#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#ifdef __APPLE__
#include <cmath>
#endif

#include "TApplication.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TVectorD.h"

#include "TH1F.h"
#include "TH2F.h"

#include "TF1.h"
#include "TF2.h"

#include "TCanvas.h"

#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "EVENT/LCIO.h"
#include "EVENT/LCRunHeader.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/LCIntVec.h"
#include "IMPL/LCCollectionVec.h"

#include "TrackProjection.hh"
#include "LCPayload.hh"

#include "TBTrackScatter.hh"

using namespace lcio ;
using namespace marlin ;
using namespace TBTrack;

const float TBTrackScatter::_rotationAngle = TMath::Pi()/4.;

TBTrackScatter aTBTrackScatter ;

TBTrackScatter::TBTrackScatter() :
  TBTrackBaseProcessor("TBTrackScatter") {

  _description = "TBTrackScatter" ;

  _simFakeTrackerHitCollection="ProtoTRKSD03Collection";
  registerProcessorParameter("SimFakeTackerHitCollection",
                             "SimFakeTrackerHit collection name",
			     _simFakeTrackerHitCollection,
			     _simFakeTrackerHitCollection);

}

void TBTrackScatter::initHists() {

  std::string sxy[2]={"X","Y"};

//   const int NPM=14;
//   const int NDM=6;

//   std::string sPosMeas[NPM] = {
//     "MCprod","Fake",
//     "HitL0", "HitL1", "HitL2", "HitL3",
//     "MCExtrapL0", "MCExtrapL1", "MCExtrapL2", "MCExtrapL3",
//     "FakeExtrapL0", "FakeExtrapL1", "FakeExtrapL2", "FakeExtrapL3"
//   };

//   std::string sLayer[NLAYER] = {"HitL0", "HitL1", "HitL2", "HitL3"};
//   std::string sExtrap[NEXTRAP] = {"Raw","MCExtrap","FakeExtrap"};

//   std::string sDirMeas[NDM] = {
//     "MCprod","Fake",
//     "HitL0", "HitL1", "HitL2", "HitL3"
//   };


//   int nbins=300;
//   float hmaxPos=45;
//   float hmaxDir=0.005;

  for(unsigned xy(0);xy<2;xy++) {

//
//    _NEW_hPos1d_fake[xy] = new TH1F((sxy[xy]+"_FakePos").c_str(), (sxy[xy]+"_FakePos").c_str(), nbins, -hmaxPos, hmaxPos);
//    _NEW_hPos1d_mc[xy] = new TH1F((sxy[xy]+"_MCPos").c_str(), (sxy[xy]+"_MCPos").c_str(), nbins, -hmaxPos, hmaxPos);
//
//    for (int iex=0; iex<NEXTRAP; iex++) {
//
//      if (iex<2) hmaxPos=45;
//      else hmaxPos=2;
//
//      for(int il(0);il<NLAYER;il++) {
//
//	if (iex==2) {
//	  switch (il) {
//	  case 0:
//	    hmaxPos=0.2;
//	    break;
//	  case 1:
//	    hmaxPos=0.4;
//	    break;
//	  case 2:
//	    hmaxPos=1.0;
//	    break;
//	  case 3:
//	    hmaxPos=2.0;
//	    break;
//	  }
//	}
//
//	_NEW_hPos1d[xy][iex][il] = new TH1F((sxy[xy]+"_"+sLayer[il]+"_"+sExtrap[iex]).c_str(),
//					    (sxy[xy]+"_"+sLayer[il]+"_"+sExtrap[iex]).c_str(),
//					    nbins, -hmaxPos, hmaxPos);
//      }
//      int n(0);
//      for(int il1(0);il1<NLAYER-1;il1++) {
//	for(int il2(il1+1);il2<NLAYER;il2++) {
//
//	  float hmaxPos1(hmaxPos);
//	  float hmaxPos2(hmaxPos);
//	  
//	  if (iex==2) {
//	    switch (il1) {
//	    case 0:
//	      hmaxPos1=0.2;
//	      break;
//	    case 1:
//	      hmaxPos1=0.4;
//	      break;
//	    case 2:
//	      hmaxPos1=1.0;
//	      break;
//	    case 3:
//	      hmaxPos1=2.0;
//	      break;
//	    }
//	    switch (il2) {
//	    case 0:
//	      hmaxPos2=0.2;
//	      break;
//	    case 1:
//	      hmaxPos2=0.4;
//	      break;
//	    case 2:
//	      hmaxPos2=1.0;
//	      break;
//	    case 3:
//	      hmaxPos2=2.0;
//	      break;
//	    }
//	  }
//
//	  _NEW_hPos2d[xy][iex][n] = new TH2F( (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[iex]).c_str(),
//					      (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[iex]).c_str(),
//					      nbins/3, -hmaxPos1, hmaxPos1, nbins/3, -hmaxPos2, hmaxPos2);
//
//
//	  float hmma = TMath::Max(hmaxPos1, hmaxPos2);
//	  float hmmi = TMath::Min(hmaxPos1, hmaxPos2);
//
//	  if (iex<2) {
//	    hmma*=1.5;
//	    hmmi/=10;
//	  }
//
//	  _NEW_hPos2d_rot[xy][iex][n] = new TH2F( (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[iex]+"_rot").c_str(),
//						  (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[iex]+"_rot").c_str(),
//						  nbins, -3*hmma, 3*hmma, 
//						  nbins, -3*hmmi, 3*hmmi);
//	  n++;
//	}
//      }
//    }
//

    // zpos
    for (int il=0; il<NLAYER+1; il++) {
      TString hn="zpos"+sxy[xy]+"_L";
      if (il<4) hn+=il;
      else hn+="fake";

      float hmin, hmax;
      if (il==0) {
	hmin = -150; hmax = 50;
      } else if (il==1) {
	hmin = -1150; hmax = -950;
      } else if (il==2) {
	hmin = -2200; hmax = -2000;
      } else if (il==3) {
	hmin = -3200; hmax = -3000;
      } else {
	hmin = 1200; hmax = 1400;
      }

      _hZpos[xy][il] = new TH1F(hn, hn, 1000, hmin, hmax);
    }

  } // xy
}

void TBTrackScatter::Init() {
  _oldRun=-9999;

  _rotation.ResizeTo(2,2);
  _rotation(0,0)=TMath::Cos(_rotationAngle);
  _rotation(0,1)=TMath::Sin(_rotationAngle);
  _rotation(1,1)=_rotation(0,0);
  _rotation(1,0)=-_rotation(0,1);

}

void TBTrackScatter::ProcessRunHeader( LCRunHeader* run) {

  if (run->getRunNumber()!=_oldRun) {
    closeHFile();
    openHFile(run);
    initHists();
    _oldRun = run->getRunNumber();
  }

}

void TBTrackScatter::ProcessEvent( LCEvent * evt ) {

  getHitInfo(evt);

}


void TBTrackScatter::getHitInfo( LCEvent * evt ) {

  unsigned tbValid(0);
  TBEvent tbEvent;
  TVectorD tx(4),ty(4);

  const LCCollection *col(0);
  if((col=getCollection(evt,"MCParticle",LCIO::MCPARTICLE))!=0) {
    if(col->getNumberOfElements()>0) {
      const MCParticle *m(dynamic_cast<const MCParticle*>(col->getElementAt(0)));
      tbValid=0x00;
      if(printLevel(2)) {
	std::cout << "Original MCParticle found "
		  << std::setw(6) << m->getPDG()
		  << std::setw(13) << m->getVertex()[0]
		  << std::setw(13) << m->getVertex()[1]
		  << std::setw(13) << m->getVertex()[2]
		  << std::setw(13) << 0.0
		  << std::setw(13) << m->getTime()
		  << std::setw(13) << 1000.0*m->getMomentum()[0] // In GeV!
		  << std::setw(13) << 1000.0*m->getMomentum()[1]
		  << std::setw(13) << 1000.0*m->getMomentum()[2] << std::endl;
      }

      if (m->getVertex()) {
	for (int xy=0; xy<2; xy++) tbEvent.mcProdPos[xy]=m->getVertex()[xy];
	tbValid |= 1<<6;
      }

      if (m->getMomentum()) {
	for (int xy=0; xy<2; xy++) tbEvent.mcProdDir[xy]=m->getMomentum()[xy]/m->getMomentum()[2];
	tbValid |= 1<<7;
      }

      // extrapolate the MC dirn to the layers
      if (m->getVertex() && m->getMomentum()) {
	for (int il=0; il<NLAYER; il++) {
	  for (int xy(0); xy<2; xy++) {
	    tbEvent.mcExtrapPos[xy][il] = tbEvent.mcProdPos[xy] + 
	      tbEvent.mcProdDir[xy]*(_fitConstants.zLayer(xy,il)-m->getVertex()[2]);
	  }
	}
      }


      //      getSimTrackerHits(evt);

      if(printLevel(2)) {
	for (int layer=0; layer<4; layer++) {
	  cout << "fit const zpos for layer " << layer << " = ";
	  cout << _fitConstants.zLayer(0,layer) << " ";
	  cout << _fitConstants.zLayer(1,layer) << endl;
	}
      }

      const SimTrackerHit *pSTH[2][4]={{0,0,0,0},{0,0,0,0}};
      double maxMom[2][4]={{0, 0, 0, 0}, {0, 0, 0, 0}};
//       double zclosest[2][4]={{0,0,0,0},{0,0,0,0}};
      if((col=getCollection(evt,_simTrackerHitCollection,LCIO::SIMTRACKERHIT))!=0) {
	//bool found(false);
	//double c(0.0);
	for(int i(0);i<col->getNumberOfElements();i++) {
	  const SimTrackerHit *p(dynamic_cast<const SimTrackerHit*>(col->getElementAt(i)));
	  assert(p!=0);

	  //assert(p->getCellID()==2*l+xy);

	  unsigned xy(p->getCellID()&1);
	  unsigned layer(p->getCellID()>>1);

	  if(printLevel(2)) {
	    std::cout << "SimTrackerHit found "
		      << std::setw(6)	      << p->getCellID()
		      << std::setw(13) 	      << p->getPosition()[0]
		      << std::setw(13) 	      << p->getPosition()[1]
		      << std::setw(13) 	      << p->getPosition()[2]
		      << std::setw(13) 	      << p->getdEdx()
		      << std::setw(13) 	      << p->getTime()
		      << std::setw(13) 	      << p->getMomentum()[0]
		      << std::setw(13) 	      << p->getMomentum()[1]
		      << std::setw(13) 	      << p->getMomentum()[2]
		      << std::setw(13) 	      << p->getPathLength() 
		      << "  " << layer
		      << std::endl;
	  }


	  _hZpos[xy][layer]->Fill(p->getPosition()[2]);

	  // hacked by daniel...
	  /*
	    if(p->getPosition()[2]<_fitConstants.zLayer(xy,layer)) {
	    if(p->getPosition()[2]>zclosest[0][layer]) {
	    zclosest[0][layer]=p->getPosition()[2];
	    pSTH[0][layer]=p;
	    }
	    }
	    if(p->getPosition()[2]>=_fitConstants.zLayer(xy,layer)) {
	    if(p->getPosition()[2]<zclosest[0][layer]) {
	    zclosest[1][layer]=p->getPosition()[2];
	    pSTH[1][layer]=p;
	    }
	    }
	  */

	  // take highest Z-momentum one in collection...
	  // assumes beam is along z axis
	  float mom = p->getMomentum()[2];
	  if (mom>maxMom[xy][layer]) {
	    pSTH[xy][layer] = p;
	    maxMom[xy][layer]=mom;
	  }

	}


	for(unsigned layer(0);layer<NLAYER;layer++) {
	  if(pSTH[0][layer]!=0 && pSTH[1][layer]!=0) {
	    int bitIndex = layer+2;
	    tbValid |= 1<<bitIndex;

	    for (int xy(0); xy<2; xy++) {
	      tbEvent.hitPos[xy][layer]=pSTH[xy][layer]->getPosition()[xy];
	      tbEvent.hitDir[xy][layer]=pSTH[xy][layer]->getMomentum()[xy]/pSTH[xy][layer]->getMomentum()[2];
	    }

	    tx[layer]=pSTH[1][layer]->getPosition()[0];
	    ty[layer]=pSTH[1][layer]->getPosition()[1];
	  }
	}
	
      }
    }
  }

  if(printLevel(2)) 
    std::cout << "tbValid = " << std::hex << tbValid << std::dec << std::endl;


  const SimTrackerHit *pFakeSTH=NULL;

  float maxP = 0;
  if((col=getCollection(evt,_simFakeTrackerHitCollection,LCIO::SIMTRACKERHIT))!=0) {

    for(int i(0);i<col->getNumberOfElements();i++) {
      const SimTrackerHit *p(dynamic_cast<const SimTrackerHit*>(col->getElementAt(i)));

      if(printLevel(2)) {
	std::cout << "Fake SimTrackerHit found "
		  << std::setw(6)	      << p->getCellID()
		  << std::setw(13) 	      << p->getPosition()[0]
		  << std::setw(13) 	      << p->getPosition()[1]
		  << std::setw(13) 	      << p->getPosition()[2]
		  << std::setw(13) 	      << p->getdEdx()
		  << std::setw(13) 	      << p->getTime()
		  << std::setw(13) 	      << p->getMomentum()[0]
		  << std::setw(13) 	      << p->getMomentum()[1]
		  << std::setw(13) 	      << p->getMomentum()[2]
		  << std::setw(13) 	      << p->getPathLength() << std::endl;
      }

      _hZpos[0][4]->Fill(p->getPosition()[2]);

      if (p->getPosition()) tbValid |= 1;
      if (p->getMomentum()) tbValid |= 1<<1;

      if (p->getMomentum()[2]>maxP) {
	pFakeSTH=p;
	maxP=p->getMomentum()[2];
      }

    }

    if(printLevel(2)) 
      cout << "chosen Fake hit (zmom) = " << maxP << endl;

  }


  if (pFakeSTH) {

    for (int xy(0); xy<2; xy++) {
      tbEvent.fakePos[xy]=pFakeSTH->getPosition()[xy];
      tbEvent.fakeDir[xy]=pFakeSTH->getMomentum()[xy]/pFakeSTH->getMomentum()[2];

      // extrapolate the Fake dirn to the layers
      for (int il=0; il<NLAYER; il++) {
	for (int xy(0); xy<2; xy++) {
	  tbEvent.fakeExtrapPos[xy][il] = tbEvent.fakePos[xy] + 
	    tbEvent.fakeDir[xy]*(_fitConstants.zLayer(xy,il)-pFakeSTH->getPosition()[2]);
	}
      }
    }

  }


  unsigned requiredHits = 0xfc; // for FNAL - 4 drift chambers
  //  unsigned requiredHits = 0xf8; // for CERN - 3 drift chambers

  if(tbValid>=requiredHits) {
    //fillHistograms(&tbEvent);
    _vTBEvent.push_back(tbEvent);
    _vDelta[0].push_back(tx);
    _vDelta[1].push_back(ty);
  }


}

void TBTrackScatter::fillHistograms() {


  // fill histos from vector of tbevent

  std::string sxy[2]={"X","Y"};

//   const int NPM=14;
//   const int NDM=6;

//   std::string sPosMeas[NPM] = {
//     "MCprod","Fake",
//     "HitL0", "HitL1", "HitL2", "HitL3",
//     "MCExtrapL0", "MCExtrapL1", "MCExtrapL2", "MCExtrapL3",
//     "FakeExtrapL0", "FakeExtrapL1", "FakeExtrapL2", "FakeExtrapL3"
//   };

  std::string sLayer[NLAYER] = {"HitL0", "HitL1", "HitL2", "HitL3"};
  std::string sExtrap[NEXTRAP] = {"Raw","MCExtrap","FakeExtrap"};

//   std::string sDirMeas[NDM] = {
//     "MCprod","Fake",
//     "HitL0", "HitL1", "HitL2", "HitL3"
//   };


  int nbins=300;

  float scalerms = 5; // how wide to make histo: this time measured rms


  TMatrixD orig(2,1);

  for (int xy=0; xy<2; xy++) {

    float rms(0);
    int nrms(0);

    for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
      float thisval = fabs(_vTBEvent[iev].fakePos[xy]);
      rms+=thisval*thisval;
      nrms++;
    }
    rms=sqrt(rms/nrms);
    _NEW_hPos1d_fake[xy] = new TH1F((sxy[xy]+"_FakePos").c_str(), (sxy[xy]+"_FakePos").c_str(), nbins, -scalerms*rms, scalerms*rms);
    for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) _NEW_hPos1d_fake[xy]->Fill(_vTBEvent[iev].fakePos[xy]);

    rms=0;
    nrms=0;
    for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
      float thisval = fabs(_vTBEvent[iev].mcProdPos[xy]);
      rms+=thisval*thisval;
      nrms++;
    }
    rms=sqrt(rms/nrms);
    _NEW_hPos1d_mc[xy] = new TH1F((sxy[xy]+"_MCPos").c_str(), (sxy[xy]+"_MCPos").c_str(), nbins, -scalerms*rms, scalerms*rms);
    for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) _NEW_hPos1d_mc[xy]->Fill(_vTBEvent[iev].mcProdPos[xy]);


    for(int il(0);il<NLAYER;il++) {
      rms=0;
      nrms=0;
      for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	float thisval = fabs(_vTBEvent[iev].hitPos[xy][il]);
	rms+=thisval*thisval;
	nrms++;
      }
      rms=sqrt(rms/nrms);
      _NEW_hPos1d[xy][0][il] = new TH1F((sxy[xy]+"_"+sLayer[il]+"_"+sExtrap[0]).c_str(), 
					(sxy[xy]+"_"+sLayer[il]+"_"+sExtrap[0]).c_str(), 
					nbins, -scalerms*rms, scalerms*rms);
      for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) _NEW_hPos1d[xy][0][il]->Fill(_vTBEvent[iev].hitPos[xy][il]);

      rms=0;
      nrms=0;
      for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	float thisval = fabs(_vTBEvent[iev].hitPos[xy][il] - _vTBEvent[iev].mcExtrapPos[xy][il]);
	rms+=thisval*thisval;
	nrms++;
      }

      rms=sqrt(rms/nrms);

      _NEW_hPos1d[xy][1][il] = new TH1F((sxy[xy]+"_"+sLayer[il]+"_"+sExtrap[1]).c_str(), 
					(sxy[xy]+"_"+sLayer[il]+"_"+sExtrap[1]).c_str(), 
					nbins, -scalerms*rms, scalerms*rms);
      for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) _NEW_hPos1d[xy][1][il]->Fill(_vTBEvent[iev].hitPos[xy][il] - _vTBEvent[iev].mcExtrapPos[xy][il]);

      rms=0;
      nrms=0;
      for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	float thisval = fabs(_vTBEvent[iev].hitPos[xy][il] - _vTBEvent[iev].fakeExtrapPos[xy][il]);
	if (thisval<4) {
	  rms+=thisval*thisval;
	  nrms++;
	}
      }

      rms=sqrt(rms/nrms);
      _NEW_hPos1d[xy][2][il] = new TH1F((sxy[xy]+"_"+sLayer[il]+"_"+sExtrap[2]).c_str(), 
					(sxy[xy]+"_"+sLayer[il]+"_"+sExtrap[2]).c_str(), 
					nbins, -scalerms*rms, scalerms*rms);
      for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) _NEW_hPos1d[xy][2][il]->Fill(_vTBEvent[iev].hitPos[xy][il] - _vTBEvent[iev].fakeExtrapPos[xy][il]);
    }



    int n(0);
    for(int il1(0);il1<NLAYER-1;il1++) {
      for(int il2(il1+1);il2<NLAYER;il2++) {

	float rmsX(0);
	float rmsY(0);

	float rmsXr(0);
	float rmsYr(0);

	TH1F* h1 = new TH1F("test1","test1",500,-20,20);
	TH1F* h2 = new TH1F("test2","test2",500,-20,20);
	TH1F* h3 = new TH1F("test3","test3",500,-20,20);
	TH1F* h4 = new TH1F("test4","test4",500,-20,20);

	for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	  orig(0,0) = _vTBEvent[iev].hitPos[xy][il1];
	  orig(1,0) = _vTBEvent[iev].hitPos[xy][il2];

	  h1->Fill(orig(0,0));
	  h2->Fill(orig(1,0));

	  orig = _rotation*orig;

	  h3->Fill(orig(0,0));
	  h4->Fill(orig(1,0));

	}

	rmsX  = getTruncatedRMS(h1);
	rmsY  = getTruncatedRMS(h2);
	rmsXr = getTruncatedRMS(h3);
	rmsYr = getTruncatedRMS(h4);
	
	delete h1;
	delete h2;
	delete h3;
	delete h4;

	_NEW_hPos2d[xy][0][n] = new TH2F( (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[0]).c_str(),
					  (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[0]).c_str(),
					  nbins/3, -scalerms*rmsX, scalerms*rmsX, nbins/3, -scalerms*rmsY, scalerms*rmsY);

	_NEW_hPos2d_rot[xy][0][n] = new TH2F( (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[0]+"_rot").c_str(),
					      (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[0]+"_rot").c_str(),
					      nbins/3, -scalerms*rmsXr, scalerms*rmsXr, nbins/3, -scalerms*rmsYr, scalerms*rmsYr);

	for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	  orig(0,0) = _vTBEvent[iev].hitPos[xy][il1];
	  orig(1,0) = _vTBEvent[iev].hitPos[xy][il2];
	  _NEW_hPos2d[xy][0][n]->Fill(orig(0,0), orig(1,0));
	  orig = _rotation*orig;
	  _NEW_hPos2d_rot[xy][0][n]->Fill(orig(0,0), orig(1,0));
	}






	// offset wrt mc extrapolated position
	h1 = new TH1F("test1","test1",500,-20,20);
	h2 = new TH1F("test2","test2",500,-20,20);
	h3 = new TH1F("test3","test3",500,-20,20);
	h4 = new TH1F("test4","test4",500,-20,20);


	for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	  orig(0,0) = _vTBEvent[iev].hitPos[xy][il1] - _vTBEvent[iev].mcExtrapPos[xy][il1];
	  orig(1,0) = _vTBEvent[iev].hitPos[xy][il2] - _vTBEvent[iev].mcExtrapPos[xy][il2];


	  h1->Fill(orig(0,0));
	  h2->Fill(orig(1,0));

	  orig = _rotation*orig;

	  h3->Fill(orig(0,0));
	  h4->Fill(orig(1,0));

	}
	rmsX  = getTruncatedRMS(h1);
	rmsY  = getTruncatedRMS(h2);
	rmsXr = getTruncatedRMS(h3);
	rmsYr = getTruncatedRMS(h4);
	
	delete h1;
	delete h2;
	delete h3;
	delete h4;

	_NEW_hPos2d[xy][1][n] = new TH2F( (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[1]).c_str(),
					  (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[1]).c_str(),
					  nbins/3, -scalerms*rmsX, scalerms*rmsX, nbins/3, -scalerms*rmsY, scalerms*rmsY);

	_NEW_hPos2d_rot[xy][1][n] = new TH2F( (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[1]+"_rot").c_str(),
					      (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[1]+"_rot").c_str(),
					      nbins/3, -scalerms*rmsXr, scalerms*rmsXr, nbins/3, -scalerms*rmsYr, scalerms*rmsYr);


	for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	  orig(0,0) = _vTBEvent[iev].hitPos[xy][il1] - _vTBEvent[iev].mcExtrapPos[xy][il1];
	  orig(1,0) = _vTBEvent[iev].hitPos[xy][il2] - _vTBEvent[iev].mcExtrapPos[xy][il2];
	  _NEW_hPos2d[xy][1][n]->Fill(orig(0,0), orig(1,0));
	  orig = _rotation*orig;
	  _NEW_hPos2d_rot[xy][1][n]->Fill(orig(0,0), orig(1,0));
	}


	// offset wrt fake-layer extrapolated position

	h1 = new TH1F("test1","test1",500,-20,20);
	h2 = new TH1F("test2","test2",500,-20,20);
	h3 = new TH1F("test3","test3",500,-20,20);
	h4 = new TH1F("test4","test4",500,-20,20);

	for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	  orig(0,0) = _vTBEvent[iev].hitPos[xy][il1] - _vTBEvent[iev].fakeExtrapPos[xy][il1];
	  orig(1,0) = _vTBEvent[iev].hitPos[xy][il2] - _vTBEvent[iev].fakeExtrapPos[xy][il2];
	  h1->Fill(orig(0,0));
	  h2->Fill(orig(1,0));

	  orig = _rotation*orig;

	  h3->Fill(orig(0,0));
	  h4->Fill(orig(1,0));

	}
	rmsX  = getTruncatedRMS(h1);
	rmsY  = getTruncatedRMS(h2);
	rmsXr = getTruncatedRMS(h3);
	rmsYr = getTruncatedRMS(h4);
	
	delete h1;
	delete h2;
	delete h3;
	delete h4;

	_NEW_hPos2d[xy][2][n] = new TH2F( (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[2]).c_str(),
					  (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[2]).c_str(),
					  nbins/3, -scalerms*rmsX, scalerms*rmsX, nbins/3, -scalerms*rmsY, scalerms*rmsY);

	_NEW_hPos2d_rot[xy][2][n] = new TH2F( (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[2]+"_rot").c_str(),
					      (sxy[xy]+"_"+sLayer[il1]+"_"+sLayer[il2]+"_"+sExtrap[2]+"_rot").c_str(),
					      nbins/3, -scalerms*rmsXr, scalerms*rmsXr, nbins/3, -scalerms*rmsYr, scalerms*rmsYr);


	for (unsigned int iev=0; iev<_vTBEvent.size(); iev++) {
	  orig(0,0) = _vTBEvent[iev].hitPos[xy][il1] - _vTBEvent[iev].fakeExtrapPos[xy][il1];
	  orig(1,0) = _vTBEvent[iev].hitPos[xy][il2] - _vTBEvent[iev].fakeExtrapPos[xy][il2];
	  _NEW_hPos2d[xy][2][n]->Fill(orig(0,0), orig(1,0));
	  orig = _rotation*orig;
	  _NEW_hPos2d_rot[xy][2][n]->Fill(orig(0,0), orig(1,0));
	}

	n++;
      }
    }

  }



}



void TBTrackScatter::fillHistograms(TBEvent* tbEvent) {

  TMatrixD orig(2,1);

  for (int xy=0; xy<2; xy++) {

    _NEW_hPos1d_fake[xy]->Fill(tbEvent->fakePos[xy]);
    _NEW_hPos1d_mc[xy]->Fill(tbEvent->mcProdPos[xy]);

    for(int il(0);il<NLAYER;il++) {
      _NEW_hPos1d[xy][0][il]->Fill(tbEvent->hitPos[xy][il]);
      _NEW_hPos1d[xy][1][il]->Fill(tbEvent->hitPos[xy][il] - tbEvent->mcExtrapPos[xy][il]);
      _NEW_hPos1d[xy][2][il]->Fill(tbEvent->hitPos[xy][il] - tbEvent->fakeExtrapPos[xy][il]);
    }


    int n(0);
    for(int il1(0);il1<NLAYER-1;il1++) {
      for(int il2(il1+1);il2<NLAYER;il2++) {

	// raw position
	orig(0,0) = tbEvent->hitPos[xy][il1];
	orig(1,0) = tbEvent->hitPos[xy][il2];
	_NEW_hPos2d[xy][0][n]->Fill(orig(0,0), orig(1,0));

	// rotated variables
        orig = _rotation*orig;
	_NEW_hPos2d_rot[xy][0][n]->Fill(orig(0,0), orig(1,0));

	// offset wrt mc extrapolated position
	orig(0,0) = tbEvent->hitPos[xy][il1] - tbEvent->mcExtrapPos[xy][il1];
	orig(1,0) = tbEvent->hitPos[xy][il2] - tbEvent->mcExtrapPos[xy][il2];
	_NEW_hPos2d[xy][1][n]->Fill(orig(0,0), orig(1,0));

        orig = _rotation*orig;
	_NEW_hPos2d_rot[xy][1][n]->Fill(orig(0,0), orig(1,0));

	// offset wrt fake-layer extrapolated position
	orig(0,0) = tbEvent->hitPos[xy][il1] - tbEvent->fakeExtrapPos[xy][il1];
	orig(1,0) = tbEvent->hitPos[xy][il2] - tbEvent->fakeExtrapPos[xy][il2];
	_NEW_hPos2d[xy][2][n]->Fill(orig(0,0), orig(1,0));

        orig = _rotation*orig;
	_NEW_hPos2d_rot[xy][2][n]->Fill(orig(0,0), orig(1,0));

	n++;
      }
    }

  }

  return;
}



void TBTrackScatter::End() {

  /*
  cout << "**************** finding error in X direction ****************" << endl;
  findError(_vDelta[0]);
  cout << "**************** finding error in Y direction ****************" << endl;
  findError(_vDelta[1]);
  */

  fillHistograms();

  findError2();

  closeHFile();

  std::cout << "Number of events = " << _vDelta[0].size() << std::endl;
}


TMatrixDSym TBTrackScatter::findError(const std::vector<TVectorD> &v, double cutProb, bool mean) {

  // daniel hack to prevent crashing with no hits...
  TMatrixDSym a;
  if (v.size()<=2) return a;

  assert(v.size()>2);

  const unsigned n(v[0].GetNrows());
  std::cout << "findError() "
	    << "Size of TVectorD = " << n << std::endl;

  //double correctionFactor(1.0);
  //double correctionFactor(1.024); // 0.01
  double correctionFactor(1.155); // 0.1
  //double correctionFactor(1.289); // 0.2

  unsigned nNew(0),nOld(0);

  TVectorD m(n),o(n);
  TMatrixDSym e(n),w(n);

  for(unsigned l(0);l<100;l++) {
    std::cout << "findError() "
	      << "Starting iteration = " << l << std::endl;

    nNew=0;
    m.Zero();
    e.Zero();

    for(unsigned i(0);i<v.size();i++) {
      double p(1.0);
      if(l>0) p=TMath::Prob((v[i]-o)*(w*(v[i]-o)),n);
      if(p>=cutProb) {      // remove outliers
	nNew++;
	// calculating mean and variance
	m+=v[i];
	for(unsigned j(0);j<n;j++) {
	  for(unsigned k(0);k<n;k++) {
	    e(j,k)+=v[i](j)*v[i](k);
	  }
	}
      }
    }

    //    if(printLevel(2)) 
    std::cout << "findError() "
	      << "Iteration = " << l << " used " << nNew
	      << " values" << std::endl;

    assert(nNew>1);

    // normalise means and variance matrix
    m*=(1.0/nNew);
    for(unsigned j(0);j<n;j++) {
      for(unsigned k(0);k<n;k++) {
	e(j,k)-=nNew*m(j)*m(k);
      }
    }
    // scale the error matrix...
    e*=(correctionFactor/(nNew-1));

    // make sure it's invertable, then invert
    w=e;
    for(unsigned j(0);j<n;j++) {
      if(w(j,j)<=0.0) {
	for(unsigned k(0);k<n;k++) {
	  w(j,k)=0.0;
	  w(k,j)=0.0;
	}
	w(j,j)=1.0;
      }
    }
    w.Invert();

    //   if(printLevel(2)) {
    std::cout << "findError() "
	      << "Iteration = " << l << std::endl
	      << " Means, probability of being zero = " 
	      << TMath::Prob(m*(w*m)*nNew,n) << std::endl;
    for(unsigned j(0);j<n;j++) {
      std::cout << std::setw(13) << m(j);
    }

    std::cout << std::endl << " Error matrix" << std::endl;
    for(unsigned j(0);j<n;j++) {
      for(unsigned k(0);k<n;k++) {
	std::cout << std::setw(13) << e(j,k);
      }
      std::cout << std::endl;
    }

    std::cout << std::endl << " 1d errors" << std::endl;
    for(unsigned j(0);j<n;j++) {
	std::cout << std::setw(13) << sqrt(e(j,j));
    }

    std::cout << std::endl << " Correlation matrix" << std::endl;
    for(unsigned j(0);j<n;j++) {
      for(unsigned k(0);k<n;k++) {
	std::cout << std::setw(13) << e(j,k)/sqrt(e(j,j)*e(k,k));
      }
      std::cout << std::endl;
    }



    
    if(nNew==nOld) return e; // no more outliers to remove

    if(mean) o=m;
    else o.Zero();
    nOld=nNew;
  }

  return e;
}


void TBTrackScatter::findError2() {

  bool printPlots = true;

  TCanvas* cc = new TCanvas();
  TString plotname = "test.ps";

  if (printPlots) cc->Print(plotname+"[");

  TMatrixD errorMatrix(2,2);

  TF2* f2[NPM*(NPM-1)/2];

  bool constrainGausMeanToZero = true;

  int n=0;
  for (int i=0; i<NPM-1; i++)
    for (int j=i+1; j<NPM; j++) {
      TString name = "twodgausFcn_"; name+=i; name+=j;
      f2[n]= new TF2(name, TBTrackScatter_twodgausFcn,-50,50,-50,50,6);
      f2[n]->SetNpx(100);
      f2[n]->SetNpy(100);
      n++;
    }

//   float oneDfit_mean[2][NPM]={0};
//   float oneDfit_sigma[2][NPM]={0};

//   float twoDfit_mean[2][NPM][NPM-1]={0};
//   float twoDfit_sigma[2][NPM][NPM-1]={0};
//   float twoDfit_correl[2][NPM*(NPM-1)/2]={0};

  for (int ixy=0; ixy<2; ixy++) {

    cout << " ******************** ";
    if (ixy==0) cout << "X";
    else cout << "Y";
    cout << " direction" << endl;

    for (int iex=0; iex<NEXTRAP; iex++) {


      cout << "!!!!!!!!!!!!!!! extrap " << iex << endl;

      //--------------------------------
      // first look at 1-d distributions
      //--------------------------------
      cout << "---------- 1d fits ------------ " << endl;
      cc->Clear();
      cc->Divide(2,2);
      for(int il(0);il<NLAYER;il++) {
	cc->cd(il+1);
	TH1F* hh = _NEW_hPos1d[ixy][iex][il];
	hh->Fit("gaus","q");
	hh->Draw();
	TF1* ff = _NEW_hPos1d[ixy][iex][il]->GetFunction("gaus");
	if (ff) {
	  float mean = ff->GetParameter(1);
	  float sigma = ff->GetParameter(2);

	  if (constrainGausMeanToZero) {
	    mean=0;
	    ff->FixParameter(1,mean);
	  }

	  hh->Fit("gaus","q","",mean-2*sigma, mean+2*sigma);
	  ff = hh->GetFunction("gaus");
	  mean = ff->GetParameter(1);
	  sigma = ff->GetParameter(2);

	  cout << hh->GetName() << " mean=" << mean << " sigma=" << sigma << endl;
	}
      }
      if (printPlots) cc->Print(plotname);



      //--------------------------------
      // then look at the 2d (plots)
      //--------------------------------

      cout << "2d fits " << endl;

      for (int irot=0; irot<2; irot++) { // unrotated, rotated variables

	if (irot==1) cout << "ROTATED variables" << endl;

	TH2F* (*ht)[2][NEXTRAP][NLAYER*(NLAYER-1)/2] = irot==0 ? &_NEW_hPos2d : &_NEW_hPos2d_rot;

	cc->Clear();
	cc->Divide(3,2);
	n=0;
	for(int il1(0);il1<NLAYER-1;il1++) {
	  for(int il2(il1+1);il2<NLAYER;il2++) {
	    cc->cd(n+1);
	    TH2F* hh = (*ht)[ixy][iex][n];
	    hh->Draw("zcol");
	  n++;
	  }
	}
	if (printPlots) cc->Print(plotname);

	// suppress bins with few entries
	n=0;
	for(int il1(0);il1<NLAYER-1;il1++) {
	  for(int il2(il1+1);il2<NLAYER;il2++) {
	    TH2F* hh = (*ht)[ixy][iex][n];
	    float minEntries = hh->GetMaximum()/15.;
	    for (int i=1; i<=hh->GetNbinsX(); i++) 
	      for (int j=1; j<=hh->GetNbinsY(); j++) {
		if (hh->GetBinContent(i,j)<minEntries) hh->SetBinContent(i,j,0);
	      }
	    hh->Draw("zcol");
	    n++;
	  }
	}


	cc->Clear();
	cc->Divide(3,2);
	n=0;
	for(int il1(0);il1<NLAYER-1;il1++) {
	  for(int il2(il1+1);il2<NLAYER;il2++) {
	    cc->cd(n+1);
	    TH2F* hh = (*ht)[ixy][iex][n];
	    hh->Draw("zcol");
	    n++;
	  }
	}
	if (printPlots) cc->Print(plotname);


      // do the fits
      cc->Clear();
      cc->Divide(3,2);
      n=0;
      for(int il1(0);il1<NLAYER-1;il1++) {
        for(int il2(il1+1);il2<NLAYER;il2++) {
	  cc->cd(n+1);
	  TH2F* hh = (*ht)[ixy][iex][n];
	  if ( TMath::Abs( hh->GetCorrelationFactor() ) < 0.98 ) { // if larger, difficult to fit
	    // set initial fit parameters
	    float xmean = hh->GetMean(1);
	    float ymean = hh->GetMean(2);
	    float xrms  = hh->GetRMS(1);
	    float yrms  = hh->GetRMS(2);

	    float xmin = xmean-3*xrms;
	    float xmax = xmean+3*xrms;

	    float ymin = ymean-3*yrms;
	    float ymax = ymean+3*yrms;

	    xmin = hh->GetXaxis()->GetXmin();
	    xmax = hh->GetXaxis()->GetXmax();

	    ymin = hh->GetYaxis()->GetXmin();
	    ymax = hh->GetYaxis()->GetXmax();

	    // set fit range
	    hh->GetXaxis()->SetRangeUser(xmin, xmax);
	    hh->GetYaxis()->SetRangeUser(ymin, ymax);
	    f2[n]->SetRange(xmin,ymin,xmax,ymax);

	    f2[n]->SetParameter(0, hh->GetEntries());

	    if (constrainGausMeanToZero) {
	      f2[n]->FixParameter(1, 0);
	      f2[n]->FixParameter(2, 0);
	    } else {
	      f2[n]->SetParameter(1, xmean);
	      f2[n]->SetParameter(2, ymean);
	    }

	    f2[n]->SetParameter(3, xrms);
	    f2[n]->SetParameter(4, yrms);
	    f2[n]->SetParameter(5, hh->GetCorrelationFactor());

	    hh->Fit(f2[n]->GetName(),"q");

	    f2[n]->Draw("zcol");

	    // extract the error matrix
	    errorMatrix(0,0) = TMath::Power(f2[n]->GetParameter(3),2.);
	    errorMatrix(0,1) = f2[n]->GetParameter(5)*f2[n]->GetParameter(3)*f2[n]->GetParameter(4);
	    errorMatrix(1,0) = errorMatrix(0,1);
	    errorMatrix(1,1) = TMath::Power(f2[n]->GetParameter(4),2.);

	    if (irot==0) {
	      cout << "-s"<<il1<<", s"<<il2<<", corr -----" << 
		TMath::Sqrt(errorMatrix(0,0)) << " " << TMath::Sqrt(errorMatrix(1,1)) << " " <<  
		errorMatrix(1,0)/TMath::Sqrt(errorMatrix(0,0)*errorMatrix(1,1)) << endl;
	    } else {
	      // rotate it back to original reference frame
	      TMatrixD temp(errorMatrix,  TMatrixD::kMult, _rotation);
	      TMatrixD rotatedErrorMatrix(_rotation, TMatrixD::kTransposeMult, temp);
	      
	      cout << "-rotated back s"<<il1<<", s"<<il2<<", corr ---- " << 
		TMath::Sqrt(rotatedErrorMatrix(0,0)) << " " << TMath::Sqrt(rotatedErrorMatrix(1,1)) << " " <<  
		rotatedErrorMatrix(1,0)/TMath::Sqrt(rotatedErrorMatrix(0,0)*rotatedErrorMatrix(1,1)) << endl;

	    }


	  }

	  n++;
	}
      }
      if (printPlots) cc->Print(plotname);


      }

    }

  }

  if (printPlots) cc->Print(plotname+"]");


  return;
}


double TBTrackScatter::getTruncatedRMS(TH1F* h, float factor) {
  if (!h) return -999;
  TH1F* hh = (TH1F*) h->Clone();
  float mm = hh->GetMaximum();
  for (int i=1; i<=hh->GetNbinsX(); i++) 
    if (hh->GetBinContent(i)<mm/factor) 
      hh->SetBinContent(i,0);
  double rms = hh->GetRMS();
  delete hh;
  return rms;
}
