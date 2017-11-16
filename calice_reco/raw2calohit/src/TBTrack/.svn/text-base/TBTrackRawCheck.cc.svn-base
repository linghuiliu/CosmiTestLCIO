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
#include "EVENT/LCGenericObject.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/TrackerHitImpl.h"
#include "IMPL/TrackerRawDataImpl.h"
#include "IMPL/TrackImpl.h"

#include "BmlEventData.hh"

#include "TrackProjection.hh"

#include "TBTrackRawCheck.hh"


using namespace lcio ;
using namespace marlin ;
using namespace TBTrack;


TBTrackRawCheck aTBTrackRawCheck ;

TBTrackRawCheck::TBTrackRawCheck() :
  TBTrackBaseProcessor("TBTrackRawCheck") {

  _description = "TBTrackRawCheck" ;

  _cern=true;
}

void TBTrackRawCheck::initHists(bool real) {

  std::string sxy[2]={"TDC0","TDC1"};
  std::string sch[16]={"00","01","02","03","04","05","06","07",
		       "08","09","10","11","12","13","14","15"};

  for(unsigned tdc(0);tdc<2;tdc++) {
    for(unsigned ch(0);ch<8;ch++) {
      if(_cern) {
	hDist[8*tdc+ch]=new TH1F((sxy[tdc]+"Ch"+sch[ch]+"Dist").c_str(),
				  (sxy[tdc]+" Ch"+sch[ch]+": Hit distribution").c_str(),
				  100,500.0,1000.0);
	if((ch%2)==0) {
	  hDi2d[8*tdc+ch]=new TH2F((sxy[tdc]+"Ch"+sch[ch+1]+"VsCh"+sch[ch]).c_str(),
				   (sxy[tdc]+" Ch"+sch[ch+1]+" vs Ch"+sch[ch]).c_str(),
				   100,500.0,1000.0,100,500.0,1000.0);
	  hDi2p[8*tdc+ch]=new TH1F((sxy[tdc]+"Ch"+sch[ch+1]+"PlusCh"+sch[ch]).c_str(),
				   (sxy[tdc]+" Ch"+sch[ch+1]+" plus Ch"+sch[ch]).c_str(),
				   100,1000.0,1500.0);
	  hDi2m[8*tdc+ch]=new TH1F((sxy[tdc]+"Ch"+sch[ch+1]+"MinusCh"+sch[ch]).c_str(),
				   (sxy[tdc]+" Ch"+sch[ch+1]+" minus Ch"+sch[ch]).c_str(),
				   100,-500.0,500.0);
	}




      } else {
	hDist[8*tdc+ch]=new TH1F((sxy[0]+"Ch"+sch[8*tdc+ch]+"Dist").c_str(),
				  (sxy[0]+" Ch"+sch[8*tdc+ch]+": Hit distribution").c_str(),
				  100,0.0,5000.0);
      }
    }
  }
}

void TBTrackRawCheck::Init() {
}

void TBTrackRawCheck::ProcessRunHeader( LCRunHeader* run) {
  closeHFile();
  openHFile(run);
  initHists(true);
}

void TBTrackRawCheck::ProcessEvent( LCEvent * evt ) {


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

  const LCCollection* col(0);
  col=getCollection(evt,_tdcRawDataCollection,LCIO::LCGENERICOBJECT);
  if(col!=0) {
    if(printLevel(2)) std::cout << "Number of elements in collection = "
				<< col->getNumberOfElements() << std::endl;


    assert(col->getNumberOfElements()<=2);
    //hNCol->Fill(col->getNumberOfElements());

    std::vector<int> vHits[16];

    for(int i(0);i<col->getNumberOfElements();i++) {
      const LCGenericObject *q(dynamic_cast<const LCGenericObject*>(col->getElementAt(i)));
      assert(q!=0);

      BmlEventData bmlobj = col->getElementAt(i);
      if(bmlobj.getNumberOfSignalChannels() > 0){
	if (printLevel(3)) std::cout <<" The channels are:" << std::endl;
	TDCChannelContainer_t tdcChannelConteiner = bmlobj.getTDCChannelContainer();
	for (TDCChannelContainer_t::iterator tdcchan_iter = tdcChannelConteiner.begin(); tdcchan_iter != tdcChannelConteiner.end(); tdcchan_iter++){
	  if (printLevel(3)) std::cout << "TDC Channel Number: " << static_cast<unsigned int>((*tdcchan_iter).first) << std::endl;
	  if (printLevel(3)) std::cout << "Number of measured signals: " << (*tdcchan_iter).second.size() <<std::endl;
	  for (std::vector< std::pair<bool,int> >::iterator tdcvec_iter = (*tdcchan_iter).second.begin();  tdcvec_iter != (*tdcchan_iter).second.end(); tdcvec_iter++ ) {
	    std::pair<bool, int> thepair = (*tdcvec_iter);
	    int tdcindex = static_cast<unsigned int>((*tdcchan_iter).first);
	    int index = 6*i + tdcindex;
	    //*********************************************************************
	    if (printLevel(3)) {
	    std::cout << "is StartTime?: " << thepair.first << std::endl;
	    std::cout << "Measured time: " << thepair.second << std::endl;
	    std::cout << "Index in array:" << index << std::endl;
	    }



	    //edges[6*i+static_cast<unsigned int>((*tdcchan_iter).first)] = thepair.second;
	    //*********************************************************************
	    if (index >= 0  && index < 12 && tdcindex < 6) {
	      //edges[6*i+static_cast<unsigned int>((*tdcchan_iter).first)]->push_back(thepair.second);
	      //if (printLevel(3)) std::cout << "In Vector " << edges[index][0] << std::endl;

	      if(thepair.second>0) {
		hDist[8*i+static_cast<unsigned int>((*tdcchan_iter).first)]->Fill(thepair.second);
		vHits[8*i+static_cast<unsigned int>((*tdcchan_iter).first)].push_back(thepair.second);
	      }
	    }
	    else if (printLevel(3)) std::cout << "Out of index: not DC channel" << std::endl;
	  }
	}//tdc channel iteration
      } //num sigchannels
    } //loop over elements

    for(unsigned i(0);i<8;i++) {
      for(unsigned j(0);j<vHits[2*i].size();j++) {
	for(unsigned k(0);k<vHits[2*i+1].size();k++) {
	  hDi2d[2*i]->Fill(vHits[2*i][j],vHits[2*i+1][k]);
	  hDi2p[2*i]->Fill(vHits[2*i][k]+vHits[2*i+1][j]);
	  hDi2m[2*i]->Fill(vHits[2*i][k]-vHits[2*i+1][j]);
	}
      }
    }



    /*
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
	    //if(l==0 && xy==0) col=getCollection(evt,"TBdchX02_dchSDx1",LCIO::SIMTRACKERHIT);
	    //if(l==1 && xy==0) col=getCollection(evt,"TBdchX02_dchSDx2",LCIO::SIMTRACKERHIT);
	    //if(l==2 && xy==0) col=getCollection(evt,"TBdchX02_dchSDx3",LCIO::SIMTRACKERHIT);
	    //if(l==3 && xy==0) col=getCollection(evt,"TBdchX02_dchSDx4",LCIO::SIMTRACKERHIT);
	    //if(l==0 && xy==1) col=getCollection(evt,"TBdchY02_dchSDy1",LCIO::SIMTRACKERHIT);
	    //if(l==1 && xy==1) col=getCollection(evt,"TBdchY02_dchSDy2",LCIO::SIMTRACKERHIT);
	    //if(l==2 && xy==1) col=getCollection(evt,"TBdchY02_dchSDy3",LCIO::SIMTRACKERHIT);
	    //if(l==3 && xy==1) col=getCollection(evt,"TBdchY02_dchSDy4",LCIO::SIMTRACKERHIT);
	    
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
*/
  }
}



void TBTrackRawCheck::End() {
  closeHFile();
}


