#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>

#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "EVENT/LCIO.h"
#include "EVENT/LCRunHeader.h"
#include "EVENT/SimTrackerHit.h"
#include "IMPL/LCCollectionVec.h"
#include "EVENT/LCIntVec.h"

#include "TrackProjection.hh"
#include "LCPayload.hh"

#include "TBTrackAligner.hh"
#include "TrackFinder.hh"

using namespace lcio;
using namespace marlin;
using namespace TBTrack;


TBTrackAligner aTBTrackAligner ;

TBTrackAligner::TBTrackAligner() : 
  TBTrackBaseProcessor("TBTrackAligner") {
  _description = "TBTrackAligner" ;
}

void TBTrackAligner::initHists(unsigned p, bool real) {
  assert(p<17);

  std::string sxy[2]={"X","Y"};
  std::string spt[17]={
    "P00","P01","P02","P03","P04","P05","P06","P07",
    "P08","P09","P10","P11","P12","P13","P14","P15",
    "All"};
  
  for(unsigned xy(0);xy<2;xy++) {
    hNumb[xy][p]=new TH1F((sxy[xy]+spt[p]+"Numb").c_str(),
			  (sxy[xy]+" "+spt[p]+" Number of tracks").c_str(),
			  10,0.0,10.0);
    hPatt[xy][p]=new TH1F((sxy[xy]+spt[p]+"Patt").c_str(),
			  (sxy[xy]+" "+spt[p]+" Hit pattern of tracks").c_str(),
			  16,0.0,16.0);
    hPar0[xy][p]=new TH1F((sxy[xy]+spt[p]+"Par0").c_str(),
			  (sxy[xy]+" "+spt[p]+" Parameter 0 of tracks").c_str(),
			  100,-25.0,25.0);
    hPar1[xy][p]=new TH1F((sxy[xy]+spt[p]+"Par1").c_str(),
			  (sxy[xy]+" "+spt[p]+" Parameter 1 of tracks").c_str(),
			  100,-0.005,0.005);
    hErr0[xy][p]=new TH1F((sxy[xy]+spt[p]+"Err0").c_str(),
			  (sxy[xy]+" "+spt[p]+" Error 0 of tracks").c_str(),
			  100,0.0,5.0);
    hErr1[xy][p]=new TH1F((sxy[xy]+spt[p]+"Err1").c_str(),
			  (sxy[xy]+" "+spt[p]+" Error 1 of tracks").c_str(),
			  100,0.0,0.005);
    hCorr[xy][p]=new TH1F((sxy[xy]+spt[p]+"Corr").c_str(),
			  (sxy[xy]+" "+spt[p]+" Correlation of tracks").c_str(),
			  100,-1.0,1.0);
    hProb[xy][p]=new TH1F((sxy[xy]+spt[p]+"Prob").c_str(),
			  (sxy[xy]+" "+spt[p]+" Probability of tracks").c_str(),
			  //100,0.0,1.0);// TEMP!!!!
			  100,0.5,1.0);
    hNmb2[xy][p]=new TH2F((sxy[xy]+spt[p]+"Nmb2").c_str(),
			  (sxy[xy]+" "+spt[p]+" Number vs Par1 and Par2 of tracks").c_str(),
			  100,-25.0,25.0,100,-0.005,0.005);
    hPrb2[xy][p]=new TH2F((sxy[xy]+spt[p]+"Prb2").c_str(),
			  (sxy[xy]+" "+spt[p]+" Probability vs Par1 and Par2 of tracks").c_str(),
			  100,-25.0,25.0,100,-0.005,0.005);

    if(!real) {
      hMPr0[xy][p]=new TH1F((sxy[xy]+spt[p]+"MPr0").c_str(),
			    (sxy[xy]+" "+spt[p]+" MCGeneric Parameter 0 of tracks").c_str(),
			    100,-25.0,25.0);
      hMPr1[xy][p]=new TH1F((sxy[xy]+spt[p]+"MPr1").c_str(),
			    (sxy[xy]+" "+spt[p]+" MCGeneric Parameter 1 of tracks").c_str(),
			    100,-0.005,0.005);
      hMDf0[xy][p]=new TH1F((sxy[xy]+spt[p]+"MDf0").c_str(),
			    (sxy[xy]+" "+spt[p]+" MCGeneric Parameter 0 difference of tracks").c_str(),
			    100,-25.0,25.0);
      hMDf1[xy][p]=new TH1F((sxy[xy]+spt[p]+"MDf1").c_str(),
			    (sxy[xy]+" "+spt[p]+" MCGeneric Parameter 1 difference of tracks").c_str(),
			    100,-0.005,0.005);
    }
  }
}

void TBTrackAligner::Init() {
}

void TBTrackAligner::ProcessRunHeader(LCRunHeader *run) {
  closeHFile();
  openHFile(run);

  _highProb[0][16]=0;
  _highProb[1][16]=0;
  initHists(16,false);

  for(unsigned i(0);i<16;i++) {
    _highProb[0][i]=0;
    _highProb[1][i]=0;

    hPatt[0][i]=0;
    hPatt[1][i]=0;
  }
}

void TBTrackAligner::ProcessEvent(LCEvent *evt) {

  const LCCollection* c(0);

  if((c=getCollection(evt,"TBTrackTdcHits",LCIO::LCINTVEC,false))!=0) {//col in event
    _vEventHits.push_back(EventHits());

    for(int i(0);i<c->getNumberOfElements();i++) {
      const LCIntVec *q=dynamic_cast<const LCIntVec*>(c->getElementAt(i));
      const LCIntVec &v(*q);
      if(printLevel(2)) std::cout << "Element " << i << " has " << v.size()-1
                                  << " hits " << std::endl;

      if(v[0]<8) {
        unsigned xy(v[0]&1);
        unsigned layer(v[0]>>1);
        for(unsigned j(1);j<v.size();j++) {
          if(printLevel(3)) std::cout << "X/Y " << xy << ", layer " << layer
                                      << ", hit " << j-1 << " = " << v[j] << std::endl;
          _vEventHits[_vEventHits.size()-1]._eventHits[xy][layer].push_back(v[j]);
        }
      }
    }
  }

  if( (_mapConstantsValid || _simConstantsValid) &&
     !(_mapConstantsValid && _simConstantsValid) &&
       _alnConstantsValid && _fitConstantsValid) {
  }


  std::string colName[2]={"TBTrackFEX","TBTrackFEY"};

  std::vector<TrackProjection> vTrk[2];

  for(unsigned xy(0);xy<2;xy++) {
    LCCollection* col(0);
    try {
      col = evt->getCollection(colName[xy]);
    } catch ( std::exception ) {
    }

    if(col!=0) {

      assert(col->getTypeName()==LCIO::LCGENERICOBJECT);


      //      std::cout << "XY = " << xy << " Number of elements = "
      //	<< col->getNumberOfElements() << std::endl;


      unsigned n[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

      for(int i(0);i<col->getNumberOfElements();i++) {
	//LCTrackPayload<TBTrackOneD> *pTrk(dynamic_cast<LCTrackPayload<TBTrackOneD>*>(col->getElementAt(i)));
	//const LCPayload<TrackOneD> pTrk(dynamic_cast<LCGenericObject*>(col->getElementAt(i)));
	const TrackProjection trk(dynamic_cast<LCGenericObject*>(col->getElementAt(i)));
	
	/*
	  if(pTrk!=0) {
	  std::cout << "XY = " << xy << " Element = " << i
	  << " returns non-null pointer" << std::endl;	  
	  
	  const int *p((const int*)pTrk);
	  std::cout << p[0] << std::endl;
	*/
	//const TrackOneD &trk(pTrk.constants());
	//trk.print();
	if(printLevel(3)) trk.print();

	  //  std::cout << "XY = " << xy << " Element = " << i
	  //    << " copied track" << std::endl;	  
	
	assert(trk.xy()==xy);

	unsigned hp(trk.hitPattern());
	assert(hp<16);
	n[hp]++;

	vTrk[xy].push_back(trk);

	if(hPatt[xy][hp]==0) initHists(hp,!(_runInformation.isMC()));

	hPatt[xy][16]->Fill(trk.hitPattern());
	hPatt[xy][hp]->Fill(trk.hitPattern());
	
	hPar0[xy][16]->Fill(trk.intercept());
	hPar0[xy][hp]->Fill(trk.intercept());
	hPar1[xy][16]->Fill(trk.gradient());
	hPar1[xy][hp]->Fill(trk.gradient());

	TMatrixDSym e(trk.errorMatrix());
	double corr(e(0,1)/sqrt(e(0,0)*e(1,1)));
	assert(e(0,0)>=0.0);
	assert(e(1,1)>=0.0);
	assert(corr>=-1.0 && corr<=1.0);

	hErr0[xy][16]->Fill(sqrt(e(0,0)));
	hErr0[xy][hp]->Fill(sqrt(e(0,0)));
	hErr1[xy][16]->Fill(sqrt(e(1,1)));
	hErr1[xy][hp]->Fill(sqrt(e(1,1)));
	hCorr[xy][16]->Fill(e(0,1)/sqrt(e(0,0)*e(1,1)));
	hCorr[xy][hp]->Fill(e(0,1)/sqrt(e(0,0)*e(1,1)));
	
	if(trk.probability()>0.5) {
	  _highProb[xy][16]++;
	  _highProb[xy][hp]++;
	}

	hProb[xy][16]->Fill(trk.probability());
	hProb[xy][hp]->Fill(trk.probability());
	
	hNmb2[xy][16]->Fill(trk.intercept(),trk.gradient());
	hNmb2[xy][hp]->Fill(trk.intercept(),trk.gradient());
	hPrb2[xy][16]->Fill(trk.intercept(),trk.gradient(),trk.probability());
	hPrb2[xy][hp]->Fill(trk.intercept(),trk.gradient(),trk.probability());
	
	//std::cout << "XY = " << xy << " Element = " << i
	//    << " filled hist" << std::endl;	  
	/*
	  } else {
	  std::cout << "XY = " << xy << " Element = " << i
	  << " returns null pointer" << std::endl;	  
	  }
	*/
	
      }

      hNumb[xy][16]->Fill(col->getNumberOfElements());
      for(unsigned i(0);i<16;i++) {
	if(hPatt[xy][i]==0) {
	  assert(n[i]==0);
	} else {
	  hNumb[xy][i]->Fill(n[i]);
	}
      }
    }
  }

  if(_runInformation.isMC()) {
    LCCollection* col(0);


    try {
      col = evt->getCollection("MCParticle");
    } catch ( std::exception ) {
    }
    
    if(col!=0) {
      assert(col->getTypeName()==LCIO::MCPARTICLE);

      if(col->getNumberOfElements()>0) {
        const MCParticle *m(dynamic_cast<const MCParticle*>(col->getElementAt(0)));

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

	col=0;
	try {
	  col = evt->getCollection("ProtoDesy0506_ProtoTRKSD03");
	} catch ( std::exception ) {
	}
	
	if(col!=0) {
	  
	  assert(col->getTypeName()==LCIO::SIMTRACKERHIT);
	  
	  const SimTrackerHit *q(0);
	  double zMax(-999999.0);
	  
	  for(int i(0);i<col->getNumberOfElements();i++) {
	    const SimTrackerHit *p(dynamic_cast<const SimTrackerHit*>(col->getElementAt(i)));
	    if(printLevel(3)) {
	      std::cout << "SimTrackerHit found "
			<< std::setw(6)           << p->getCellID()
			<< std::setw(13)          << p->getPosition()[0]
			<< std::setw(13)          << p->getPosition()[1]
			<< std::setw(13)          << p->getPosition()[2]
			<< std::setw(13)          << p->getdEdx()
			<< std::setw(13)          << p->getTime()
			<< std::setw(13)          << p->getMomentum()[0]
			<< std::setw(13)          << p->getMomentum()[1]
			<< std::setw(13)          << p->getMomentum()[2]
			<< std::setw(13)          << p->getPathLength() << std::endl;
	    }
	    if(p->getMomentum()[2]>900.0*m->getMomentum()[2]) {
	      if(p->getPosition()[2]>zMax) {
		zMax=p->getPosition()[2];
		q=p;
	      }	  
	    }
	  }
	  
      
	  if(q!=0) {
	    for(unsigned xy(0);xy<2;xy++) {
	      hMPr0[xy][16]->Fill(q->getPosition()[xy]);
	      hMPr1[xy][16]->Fill(q->getMomentum()[xy]/q->getMomentum()[2]);
	      for(unsigned i(0);i<vTrk[xy].size();i++) {
		hMDf0[xy][                      16]->Fill(vTrk[xy][i].intercept(q->getPosition()[2])-q->getPosition()[xy]);
		hMDf0[xy][vTrk[xy][i].hitPattern()]->Fill(vTrk[xy][i].intercept(q->getPosition()[2])-q->getPosition()[xy]);
		hMDf1[xy][                      16]->Fill(vTrk[xy][i].gradient()-q->getMomentum()[xy]/q->getMomentum()[2]);
		hMDf1[xy][vTrk[xy][i].hitPattern()]->Fill(vTrk[xy][i].gradient()-q->getMomentum()[xy]/q->getMomentum()[2]);
	      }
	    }	
	  }
	}
      }
    }
  }
}


void TBTrackAligner::End() {
  std::cout << "Number of high probability tracks" << std::endl << " All = "
	    << std::setw(10) << _highProb[0][16] << ", "
	    << std::setw(10) << _highProb[1][16] << std::endl;

  for(unsigned i(0);i<16;i++) {
    std::cout << " P" << std::setfill('0') << std::setw(2)
	      << i << std::setfill(' ') << " = "
	      << std::setw(10) << _highProb[0][i] << ", "
	      << std::setw(10) << _highProb[1][i] << std::endl;
  }

  AlnConstants bestAC(_alnConstants);

  unsigned nBest[2]={0,0};
    
  unsigned fb=0;
  unsigned eh=0;
  for(unsigned xy(0);xy<2;xy++) {
    for(int t(-4);t<=4;t++) {
      for(int v(-5);v<=5;v++) {
	AlnConstants ac(_alnConstants);
	if(xy==0) {
	  ac.cTzero(xy,2,_alnConstants.cTzero(xy,2)+0.5*t);
	  ac.vDrift(xy,2,_alnConstants.vDrift(xy,2)+0.001*v);
	} else {
	  ac.cTzero(xy,2,_alnConstants.cTzero(xy,2)+0.5*t);
	  ac.vDrift(xy,2,_alnConstants.vDrift(xy,2)+0.001*v);
	}

	TrackFinder finder;
	finder.alnConstants(ac);
	_fitConstants.probabilityCut(1,0.5);
	_fitConstants.probabilityCut(2,0.5);
	_fitConstants.probabilityCut(3,0.5);
	_fitConstants.probabilityCut(4,0.5);
	finder.fitConstants(_fitConstants);
	
	unsigned ng(0);
	for(unsigned i(0);i<_vEventHits.size();i++) {
	  std::vector<TrackProjection> vTrks[2];
	  
	  vTrks[xy]=(finder.find(fb,eh,xy,
				 _vEventHits[i]._eventHits[xy][0],
				 _vEventHits[i]._eventHits[xy][1],
				 _vEventHits[i]._eventHits[xy][2],
				 _vEventHits[i]._eventHits[xy][3]));
	  
	  ng+=vTrks[xy].size();
	  
	  if(printLevel(2)) {
	    std::cout << "F/B " << fb << ", X/Y " << xy << " found " << vTrks[xy].size() << " tracks" << std::endl;
	    if(printLevel(3)) {
	      for(unsigned i(0);i<vTrks[xy].size();i++) {
		vTrks[xy][i].print(std::cout,"TBTrackProducer: ");
	      }
	    }
	  }
	}
	
	if(ng>nBest[xy]) {
	  std::cout << "t = " << std::setw(2) << t << ", v = " << std::setw(2) << v
		    << ", ng = " << ng << " > nBest = " << nBest[xy]
		    << ", t = " << ac.cTzero(xy,2)
		    << ", v = " << ac.vDrift(xy,2) << std::endl;
	  nBest[xy]=ng;
	  bestAC=ac;
	} else {
	  std::cout << "t = " << std::setw(2) << t << ", v = " << std::setw(2) << v
		    << ", ng = " << ng << " < nBest = " << nBest[xy] << std::endl;
	}
      }
    }
    _alnConstants=bestAC;
  }

  std::cout << "nBest = " << nBest[0] << ", " << nBest[1] << std::endl;
  bestAC.print();

  closeHFile();
}


