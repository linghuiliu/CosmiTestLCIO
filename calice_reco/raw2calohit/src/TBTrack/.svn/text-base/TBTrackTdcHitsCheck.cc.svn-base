#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

#include "TH1F.h"
#include "TH2F.h"

#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"
#include "EVENT/LCIO.h"
#include "EVENT/LCRunHeader.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/LCIntVec.h"
#include "IMPL/LCCollectionVec.h"

#include "TrackProjection.hh"

#include "TBTrackTdcHitsCheck.hh"


using namespace lcio ;
using namespace marlin ;
using namespace TBTrack;


TBTrackTdcHitsCheck aTBTrackTdcHitsCheck ;

TBTrackTdcHitsCheck::TBTrackTdcHitsCheck() :
  TBTrackBaseProcessor("TBTrackTdcHitsCheck") {

  _description = "TBTrackTdcHitsCheck" ;

  _cern=true;
}

void TBTrackTdcHitsCheck::initHists(bool real) {

  std::string sxy[2]={"X","Y"};
  std::string sly[4]={"Layer0","Layer1","Layer2","Layer3"};

  if(real) {
    hNCol=new TH1F("NCol","Number of hit objects",9,0.0,9.0);
    hNEnt=new TH1F("NEnt","Number of events with hit object 4*XY+Layer",8,0.0,8.0);
    
    for(unsigned xy(0);xy<2;xy++) {
      for(unsigned layer(0);layer<4;layer++) {
	hNumb[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Numb").c_str(),
				  (sxy[xy]+" "+sly[layer]+": Number of hits").c_str(),
				  100,0.0,100.0);
	if(_cern) {
	  hDist[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Dist").c_str(),
				    (sxy[xy]+" "+sly[layer]+": Hit distribution").c_str(),
				    200,-1000.0,1000.0);
	} else {
	  hDist[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Dist").c_str(),
				    (sxy[xy]+" "+sly[layer]+": Hit distribution").c_str(),
				    300,0.0,3000.0);
	}
	hSepr[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Sepr").c_str(),
				  (sxy[xy]+" "+sly[layer]+": Hit separation").c_str(),
				  100,0.0,1000.0);
      }
      
      unsigned n(0);
      for(unsigned layerA(0);layerA<3;layerA++) {
	for(unsigned layerB(layerA+1);layerB<4;layerB++) {
	  if((layerA==0 && layerB==2) ||
	     (layerA==1 && layerB==3)) {
	    hLpmL[xy][n]=new TH1F((sxy[xy]+sly[layerB]+"Ms"+sly[layerA]).c_str(),
				  (sxy[xy]+" "+sly[layerB]+" - "+sly[layerA]+
				   ": Hit distribution").c_str(),
				  100,-500.0,500.0);
	  } else {
	    hLpmL[xy][n]=new TH1F((sxy[xy]+sly[layerB]+"Ps"+sly[layerA]).c_str(),
				  (sxy[xy]+" "+sly[layerB]+" + "+sly[layerA]+
				   ": Hit distribution").c_str(),
				  100,1900.0,2900.0);
	  }
	  
	  hLvsL[xy][n]=new TH2F((sxy[xy]+sly[layerB]+"Vs"+sly[layerA]).c_str(),
				(sxy[xy]+" "+sly[layerB]+" vs "+sly[layerA]+
				 ": Hit distributions").c_str(),
				100,800.0,1800.0,100,800.0,1800.0);
	  n++;
	}
      }
    }


  } else {
    for(unsigned xy(0);xy<2;xy++) {
      for(unsigned layer(0);layer<4;layer++) {
	hDiff[xy][layer]=new TH1F((sxy[xy]+sly[layer]+"Diff").c_str(),
				  (sxy[xy]+" "+sly[layer]+": Difference of hit and MC hit").c_str(),
				  100,-2.0,2.0);
      }
    }
  }
}

void TBTrackTdcHitsCheck::Init() {
}

void TBTrackTdcHitsCheck::ProcessRunHeader( LCRunHeader* run) {
  closeHFile();
  openHFile(run);
  initHists(true);
  initHists(false);
}

void TBTrackTdcHitsCheck::ProcessEvent( LCEvent * evt ) {


  if(printLevel(3)) {
    std::cout << "Collections in the event" << std::endl;

    typedef const std::vector<std::string> StringVec ;
    StringVec* strVec = evt->getCollectionNames() ;
    
    for( StringVec::const_iterator name = strVec->begin() ; name != strVec->end() ; name++) {
      std::string sss = name->c_str();
      LCCollection* col = evt->getCollection(*name);
      int nHits =  col->getNumberOfElements() ;
      
      std::cout << "----> Evt " <<evt->getEventNumber()<< ", EXISTING COLLECTION, of the class : "
	   << col->getTypeName().c_str() << " and named : " << sss << " with " << nHits << " elements." << std::endl;
    }
  }

  const LCIntVec *p[2][4]={{0,0,0,0},{0,0,0,0}};

  const LCCollection* col(0);
  col=getCollection(evt, "TBTrackTdcHits", LCIO::LCINTVEC);
  if(col!=0) {
    if(printLevel(2)) std::cout << "Number of elements in collection = "
				<< col->getNumberOfElements() << std::endl;
    
    assert(col->getNumberOfElements()>=0);
    assert(col->getNumberOfElements()<=8);

    hNCol->Fill(col->getNumberOfElements());

    for(int i(0);i<col->getNumberOfElements();i++) {
      const LCIntVec *q(dynamic_cast<const LCIntVec*>(col->getElementAt(i)));
      assert(q!=0);

      // Make it a reference for convenience
      const LCIntVec &v(*q);

      if(printLevel(2))	{
	std::cout << "Element " << i << " size = "
		  << v.size() <<std::endl;
	for(unsigned j(0);j<v.size();j++) {
	  if(printLevel(3)) {
	    std::cout << " Value " << j << " = " << v[j] << std::endl;
	  }
	}
      }

      // If it exists, it should have at least one hit
      assert(v.size()>=1);

      // Check xy and layer values
      assert(v[0]<8);
      unsigned xy(v[0]&1);
      unsigned layer(v[0]>>1);

      // Check the same xy/layer combination has not already appeared
      assert(p[xy][layer]==0);
      p[xy][layer]=q;

      // Fill histograms
      hNEnt->Fill(v[0]);
      hNumb[xy][layer]->Fill(v.size()-2);

      int previous(0);
      for(unsigned j(1);j<v.size();j++) {
	hDist[xy][layer]->Fill(v[j]);

	if(j>1) hSepr[xy][layer]->Fill(v[j]-previous);
	previous=v[j];
      }
    }

    for(unsigned xy(0);xy<2;xy++) {
      unsigned n(0);
      for(unsigned layerA(0);layerA<3;layerA++) {
	if(p[xy][layerA]!=0) {
	  const LCIntVec &vA(*(p[xy][layerA]));

	  for(unsigned layerB(layerA+1);layerB<4;layerB++) {
	    if(p[xy][layerB]!=0) {
	      const LCIntVec &vB(*(p[xy][layerB]));

	      for(unsigned j(1);j<vA.size();j++) {
		for(unsigned k(1);k<vB.size();k++) {
		  if((layerA==0 && layerB==2) ||
		     (layerA==1 && layerB==3)) {
		    hLpmL[xy][n]->Fill(vB[j]-vA[k]);
		  } else {
		    hLpmL[xy][n]->Fill(vB[j]+vA[k]);
		  }
		  hLvsL[xy][n]->Fill(vA[j],vB[k]);
		}
	      }

	      n++;
	    }
	  }
	}
      }
    }
  }		

  if(_runInformation.isMC()) {
    col=getCollection(evt,"MCParticle",LCIO::MCPARTICLE);
    if(col!=0) {
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
	
	for(int l(0);l<4;l++) {
	  if(l==0) col=getCollection(evt,"TBdch02_dchSD2",LCIO::SIMTRACKERHIT);
	  if(l==1) col=getCollection(evt,"TBdch02_dchSD1",LCIO::SIMTRACKERHIT);
	  if(l==2) col=getCollection(evt,"TBdch02_dchSD3",LCIO::SIMTRACKERHIT);
	  if(l==3) col=getCollection(evt,"TBdch02_dchSD4",LCIO::SIMTRACKERHIT);

	  for(unsigned xy(0);xy<2;xy++) {
	    /*
	    if(l==0 && xy==0) col=getCollection(evt,"TBdchX02_dchSDx1",LCIO::SIMTRACKERHIT);
	    if(l==1 && xy==0) col=getCollection(evt,"TBdchX02_dchSDx2",LCIO::SIMTRACKERHIT);
	    if(l==2 && xy==0) col=getCollection(evt,"TBdchX02_dchSDx3",LCIO::SIMTRACKERHIT);
	    if(l==3 && xy==0) col=getCollection(evt,"TBdchX02_dchSDx4",LCIO::SIMTRACKERHIT);
	    if(l==0 && xy==1) col=getCollection(evt,"TBdchY02_dchSDy1",LCIO::SIMTRACKERHIT);
	    if(l==1 && xy==1) col=getCollection(evt,"TBdchY02_dchSDy2",LCIO::SIMTRACKERHIT);
	    if(l==2 && xy==1) col=getCollection(evt,"TBdchY02_dchSDy3",LCIO::SIMTRACKERHIT);
	    if(l==3 && xy==1) col=getCollection(evt,"TBdchY02_dchSDy4",LCIO::SIMTRACKERHIT);
	    */
	    
	    if(col!=0) {
	      bool found(false);
	      double c(0.0);
	      
	      for(int i(0);i<col->getNumberOfElements();i++) {
		const SimTrackerHit *p(dynamic_cast<const SimTrackerHit*>(col->getElementAt(i)));
		assert(p!=0);
		//assert(p->getCellID()==2*l+xy);
		
		if(printLevel(2)) {
		  std::cout << "SimTrackerHit " << l << std::setw(4) << i << " found "
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
		
		if(p->getMomentum()[2]>900.0*m->getMomentum()[2]) {
		  double zLo(p->getPosition()[2]+813.0);
		  double zHi(p->getPosition()[2]+p->getPathLength()+813.0);
		  if(zLo<_fitConstants.zLayer(xy,l) && zHi>_fitConstants.zLayer(xy,l)) {
		    c=p->getPosition()[xy];
		    found=true;
		  }
		}
	      }
	      
	      if(found) {
		if(printLevel(2)) {
		  std::cout << "SimTrackerHit average position = " << c << std::endl;
		}
		
		if(p[xy][l]!=0) {
		  const LCIntVec &v(*(p[xy][l]));
		  for(unsigned j(1);j<v.size();j++) {
		    if(printLevel(3)) {
		      std::cout << "TDC hit position " << std::setw(13) << _simConstants.coordinate(xy,l,v[j]) << std::endl;
		    }
		    hDiff[xy][l]->Fill(c-_simConstants.coordinate(xy,l,v[j]));
		  }		  
		}
	      }
	    }
	  }
	}
      }
    }
  }
}


void TBTrackTdcHitsCheck::End() {
  closeHFile();
}


